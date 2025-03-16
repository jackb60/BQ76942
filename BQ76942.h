#include "Arduino.h"

#define DEFAULT_ADDR 0x10;


class BQ76942 {
    public:
        Lora_E5(TwoWire* = &Wire, byte adr = DEFAULT_ADDR);
        bool begin();
        byte secKeys();
    private:
        TwoWire* _Wire;
        byte _adr;
        byte _buf[256];
        byte _bufLen;
        byte _secKeys[8];
        bool _subCmdR(int cmd);
        bool _subCmdW(int cmd, byte* data, byte len);
        void _writeByte(byte data, bool release = true);
        void _writeSubCmdAdr(int data, bool send = false);
};