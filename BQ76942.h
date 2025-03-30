#include "Arduino.h"

#define DEFAULT_ADDR 0x10

class BQ76942 {
    public:
        BQ76942(TwoWire* = &Wire, byte adr = DEFAULT_ADDR);
        bool begin();
        bool ddsgConfig(byte config);
        int cellVoltage(byte cell);
        int stackVoltage();
        int current();
        float temp();
        void fullAccess();
    private:
        TwoWire* _Wire;
        byte _adr;
        byte _buf[256];
        byte _bufLen;
        void _dirCmdR(byte cmd, byte len);
        bool _subCmdR(int cmd);
        bool _subCmdW(int cmd, byte* data, byte len);
        void _writeByte(byte data, bool release = true);
        void _writeSubCmdAdr(int data, bool send = false);
        bool _OTPcheck();
};