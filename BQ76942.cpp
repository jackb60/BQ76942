#include "Arduino.h"

void _writeByte(byte data, bool release = false) {
    _Wire.beginTransmission(_adr);
    _Wire.write(data);
    _Wire.endTransmission(release);
}

void _writeSubCmdAdr(int data, bool send = false) {
    _Wire.beginTransmission(_adr);
    _Wire.write(0x3E);
    _Wire.write((byte) data);
    _Wire.write((byte) (data >> 8));
    if(send) {
        _Wire.endTransmission(false);
    }
}



bool _subCmdR(int cmd) { //See page 13 TRM
    _writeSubCmdAdr(cmd, true); //1. Write lower byte of subcommand to 0x3E, 2. Write upper byte of subcommand to 0x3F.
    
    bool complete = false;
    while(!complete) { //Continue reading 0x3E and 0x3F until it returns what was written originally
        _writeByte(0x3E);
        _Wire.requestFrom(_adr, 2, false) //3. Read 0x3E and 0x3F
        if(Wire.read() == ((byte) data) && Wire.read() == ((byte) (cmd >> 8))) { //If this returns 0xFF, this indicates the subcommand has not completed operation yet
            complete = true;
        }
    }

    _writeByte(0x61); //4. Read the length of response from 0x61.
    _Wire.requestFrom(_adr, 1, false);
    _bufLen = _Wire.read() - 4 //0x61 provides the length of the buffer data plus 4

    _writeByte(0x40); //5. Read buffer starting at 0x40
    _Wire.requestFrom(_adr, 1, false);
    _Wire.readBytes(_buf, _bufLen); 
    
    //6. Read the checksum at 0x60 and verify it matches the data read
    byte checksum = (byte) data + (byte) (cmd >> 8); 
    for(int i = 0; i < _bufLen; i++) {
        checksum += buf[i];
    }
    checksum = ~checksum;
    _writeByte(0x60);
    _Wire.requestFrom(_adr, 1);
    
    return _Wire.read() == checksum;
}

bool _subCmdW(int cmd, byte* data, byte len) {
    byte checksum = (byte) data + (byte) (cmd >> 8);
    for(int i = 0; i < len; i++) {
        checksum += *(data + i);
    }
    checksum = ~checksum;

    _writeSubCmdAdr(cmd);
    _Wire.write(data, len);
    _Wire.endTransmisison(false); //Write data
    
    _Wire.beginTransmission(_adr);
    _Wire.write(0x60);
    _Wire.write(checksum);
    _Wire.write(len + 4);
    _Wire.endTransmission();
}