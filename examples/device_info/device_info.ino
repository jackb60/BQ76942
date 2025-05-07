#include "BQ76942.h"
#include "Wire.h"

BQ76942 bms(&Wire, 0x08);

void setup() {
  // put your setup code here, to run once:
  Serial.setTx(PA9);
  Serial.setRx(PA10);
  Serial.begin(115200);
  Wire.setSCL(PB6);
  Wire.setSDA(PB7);
  bms.begin();
  delay(1000);
  //bms.enableFet();
  bms.ddsgConfig(0x2A);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print("DEVNUM: ");
  //Serial.println(bms.devNum());
  //Serial.print("Firmware Version: ");
  //Serial.println(bms.fwVersion());
  //bms.fullAccess(); 
  //bms.dfetoffConfig(0x86);
  //bms.cellConfig(6);
  //bms.dPro();
  //bms.ddsgConfig(0x2A);
  Serial.print("FET STATUS: 0x");
  Serial.println(bms.fetStatus(), HEX);
  delay(2000);
}
