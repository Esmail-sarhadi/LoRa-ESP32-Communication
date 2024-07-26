#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 26
#define RFM95_RST 12
#define RFM95_INT 25

//Change to 433.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

//Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
//*******************************************************************************
void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(115200);
  delay(100);

  //Manual Reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println(">> LoRa radio initialization failed.");
    while (1);
  }

  //Defaults after init are 433.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println(">> Set frequency failed.");
    while (1);
  }

  //The default transmitter power is 13dBm, using PA_BOOST.
  //If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  //you can set transmitter powers from 5 to 23 dBm:
  //Slow+long range. Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on.
  //rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
  //Defaults medium Range after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  rf95.setTxPower(18);
  Serial.println(">> LoRa radio initialized successfully.");
}

//*******************************************************************************
void loop()
{
  delay(1000);
}
//*******************************************************************************
