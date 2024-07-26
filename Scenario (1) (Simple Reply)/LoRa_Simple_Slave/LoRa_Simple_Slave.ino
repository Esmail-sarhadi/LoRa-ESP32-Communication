#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 26
#define RFM95_RST 12
#define RFM95_INT 25

//Change to 433.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

char radioPacket[20] = "Group0 Send";                                     //Change this word acording your group number.
char radioAck[20] = "Group0 Ack";
uint8_t data[] = "Group0 ReplyData";
String replayMessage = ">> Reply from Group0.";
int pinBuzzer = 2;
//*******************************************************************************
void setup()
{
  pinMode (pinBuzzer, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);

  while (!Serial);
  Serial.begin(115200);
  delay(100);

  //Manual Reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init())
    Serial.println(">> Initialize failed.");
  else Serial.println(">> LoRa radio initialized successfully.");
  rf95.setFrequency(RF95_FREQ);                                           //Setup ISM frequency

  //The default transmitter power is 13dBm, using PA_BOOST.
  //If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  //you can set transmitter powers from 5 to 23 dBm:
  //Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on.
  //Slow+long range.
  //rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
  //Defaults after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //Medium Range

  rf95.setTxPower(18);
  Serial.println(">> Slave Side Start...");
  Serial.println("*******************************************************************");
  delay(1000);
}
//*******************************************************************************
void loop()
{
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      Serial.print(">> Received: ");
      Serial.println((char*)buf);
      Serial.print(">> RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

      if (strcmp(radioPacket, ((char*)buf)) == 0) {
        Serial.println(">> Received data request from server...");
        Serial.println(replayMessage);
        rf95.send(data, sizeof(data));                                           //Sizeof(data)
        rf95.waitPacketSent();
        
        digitalWrite (pinBuzzer, HIGH);
        delay(100);
        digitalWrite (pinBuzzer, LOW);
        delay(200);
        digitalWrite (pinBuzzer, HIGH);
        delay(100);
        digitalWrite (pinBuzzer, LOW);
      }

      else if (strcmp(radioAck, ((char*)buf)) == 0) {
        Serial.println(">> Received ack from server.");
        Serial.println("*******************************************************************");
      }
      else {
        Serial.println(">> Messegae is not recognized.");
        Serial.println("*******************************************************************");
      }
    }
  }
}
//*******************************************************************************
