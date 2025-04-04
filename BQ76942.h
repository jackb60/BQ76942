#include "Arduino.h"
#include "Wire.h"

#define DEFAULT_ADDR 0x10

class BQ76942 {
    public:
        BQ76942(TwoWire* wire = &Wire, byte adr = DEFAULT_ADDR);
        void begin();
        int cellVoltage(byte cell);
        int stackVoltage();
        int current();
        float temp();
        void fullAccess();
        unsigned int devNum();
        unsigned int fwVersion();
        bool dfetoffConfig(byte config);
        bool ddsgConfig(byte config);
        void daConfig(byte config = 0x0A);
    private:
        TwoWire* _Wire;
        byte _adr;
        byte _buf[256];
        byte _bufLen;
        void _dirCmdR(byte cmd, byte len);
        bool _subCmdR(int cmd);
        void _subCmdW(int cmd, byte* data, byte len);
        void _writeByte(byte data, bool release = false);
        void _writeSubCmdAdr(int data, bool send = false);
        bool _OTPcheck();
        bool _writeMem(int cmd, byte* data, byte len);
        bool cellConfig(byte numCells);

};