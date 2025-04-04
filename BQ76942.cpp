#include "Arduino.h"
#include "BQ76942.h"

BQ76942::BQ76942(TwoWire* wire = &Wire, byte adr) {
    _Wire = wire;
    _adr = adr;
}

void BQ76942::begin() {
    _Wire->begin();
}

void BQ76942::_writeByte(byte data, bool release = false) {
    _Wire->beginTransmission(_adr);
    _Wire->write(data);
    _Wire->endTransmission(release);
}

void BQ76942::_writeSubCmdAdr(int data, bool send = false) {
    _Wire->beginTransmission(_adr);
    _Wire->write(0x3E);
    _Wire->write((byte) data);
    _Wire->write((byte) (data >> 8));
    if(send) {
        _Wire->endTransmission(false);
    }
}

void BQ76942::_dirCmdR(byte cmd, byte len) {
    _Wire->beginTransmission(_adr);
    _Wire->write(cmd);
    _Wire->endTransmission(false);
    _Wire->requestFrom(_adr, len);
    _Wire->readBytes(_buf, len);
}

bool BQ76942::_subCmdR(int cmd) { //See page 13 TRM
    _writeSubCmdAdr(cmd, true); //1. Write lower byte of subcommand to 0x3E, 2. Write upper byte of subcommand to 0x3F.
    
    bool complete = false;
    while(!complete) { //Continue reading 0x3E and 0x3F until it returns what was written originally
        _writeByte(0x3E);
        _Wire->requestFrom(_adr, 2, false) //3. Read 0x3E and 0x3F
        if(Wire.read() == ((byte) data) && Wire.read() == ((byte) (cmd >> 8))) { //If this returns 0xFF, this indicates the subcommand has not completed operation yet
            complete = true;
        }
    }

    _writeByte(0x61); //4. Read the length of response from 0x61.
    _Wire->requestFrom(_adr, 1, false);
    _bufLen = _Wire->read() - 4 //0x61 provides the length of the buffer data plus 4

    _writeByte(0x40); //5. Read buffer starting at 0x40
    _Wire->requestFrom(_adr, 1, false);
    _Wire->readBytes(_buf, _bufLen); 
    
    //6. Read the checksum at 0x60 and verify it matches the data read
    byte checksum = (byte) data + (byte) (cmd >> 8); 
    for(int i = 0; i < _bufLen; i++) {
        checksum += _buf[i];
    }
    checksum = ~checksum;
    _writeByte(0x60);
    _Wire->requestFrom(_adr, 1);
    
    return _Wire->read() == checksum;
}

void BQ76942::_subCmdW(int cmd, byte* data, byte len) {
    byte checksum = (byte) data + (byte) (cmd >> 8);
    for(int i = 0; i < len; i++) {
        checksum += *(data + i);
    }
    checksum = ~checksum;

    _writeSubCmdAdr(cmd);
    _Wire->write(data, len);
    _Wire->endTransmisison(false); //Write data
    
    _Wire->beginTransmission(_adr);
    _Wire->write(0x60);
    _Wire->write(checksum);
    _Wire->write(len + 4);
    _Wire->endTransmission();
}

bool BQ76942::_writeMem(int cmd, byte* data, byte len) {
    _subCmdW(cmd, data, len);
    if(!_subCmdR(cmd)) {
        return false;
    }
    for(byte i = 0; i < len; i++) {
        if(*(data + i) != *(_buf + i)) {
            return false;
        }
    }
    return true;
}


bool BQ76942::ddsgConfig(byte config) {
    _writeMem(0x9302, (byte*) &config, 1);
}

bool BQ76942::dfetoffConfig(byte config) {
    _writeMem(0x92FB, (byte*) &config, 1);
}

void BQ76942::daConfig(byte config = 0x0A) { //0x0A: userV = mV, userA = cA, use die temp for cell temp protections
    _writeMem(0x9303, (byte*) &config, 1);
}

int BQ76942::cellVoltage(byte cell) {
    _dirCmdR(0x14 + 2 * (cell - 1), 2);
    return (buf[1] >> 8) + buf[0];
}

int BQ76942::stackVoltage() {
    _dirCmdR(0x34, 2);
    return (buf[1] >> 8) + buf[0];
}

int BQ76942::current() {
    _dirCmdR(0x3A, 2);
    return (buf[1] >> 8) + buf[0];
}

float BQ76942::temp() {
    _dirCmdR(0x68, 2);
    return (((buf[1] >> 8) + buf[0]) * 10) - 273.15;
}

void BQ76942::fullAccess() {
    _Wire->beginTransmission(_adr);
    _Wire->write(0x3E);
    _Wire->write(0xFF);
    _Wire->write(0xFF);
    _Wire->endTransmission(false);
    _Wire->beginTransmission(_adr);
    _Wire->write(0x3E);
    _Wire->write(0xFF);
    _Wire->write(0xFF);
    _Wire->endTransmission();
}

bool BQ76942::_OTPCheck() {
    _subCmdR(0x00A0);
    return buf[0] & (1 << 7);
}

unsigned int BQ76942::devNum() {
    _subCmdR(0x0002);
    return *((unsigned int*) &buf);
}

unsigned int BQ76942::fwVersion() {
    _subCmdR(0x0002);
    return *(((unsigned int*) &buf) + 1);
}

bool BQ76942::cellConfig(byte numCells) {
    unsigned int config;
    if(numCells == 4) {
        config = 0x0702;
    } else if(numCells == 6) {
        config = 0x1F02;
    }
    _writeMem(0x9304, (byte*) &config, 2);
}
