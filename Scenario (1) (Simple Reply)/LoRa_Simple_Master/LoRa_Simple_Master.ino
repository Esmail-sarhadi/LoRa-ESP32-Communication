#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 26
#define RFM95_RST 12
#define RFM95_INT 25

//Change to 433.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

char radioPacket[20] = "Group0 Send";                                        //Change this word acording your group number.
char radioAck[20] = "Group0 Ack";
char radioReply[20] = "Group0 ReplyData";
//*******************************************************************************
void setup()
{
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
  rf95.setFrequency(RF95_FREQ);                                              //Setup ISM frequency.

  //The default transmitter power is 13dBm, using PA_BOOST.
  //If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  //you can set transmitter powers from 5 to 23 dBm:
  //Slow+long range. Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on.
  //rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
  //Defaults medium Range after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  rf95.setTxPower(18);
  Serial.println(">> Master Side Start...");
  Serial.println("*******************************************************************");
  delay(1000);
}
//*******************************************************************************
void loop()
{
  int8_t send_ack = 0;                                    

  while (send_ack == 0)
  {
    Serial.println(">> Send Command...");
    rf95.send((uint8_t *)radioPacket, sizeof(radioPacket));
    rf95.waitPacketSent();

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.waitAvailableTimeout(100))
    {
      if (rf95.recv(buf, &len))
      {
        if (strcmp(radioReply, ((char*)buf)) == 0)
        {
          Serial.print(">> Receive: ");
          Serial.println((char*)buf);
          Serial.print(">> RSSI: ");
          Serial.println(rf95.lastRssi(), DEC);
          send_ack = 1;
        }
      }
    } else
    {
      Serial.println(">> Send request again.");
      Serial.println("*******************************************************************");
    }
    delay(1000);
  }

  if (send_ack == 1) {                                                       //Send: ACK.
    delay(1000);
    Serial.println(">> Send ACK...");
    rf95.send((uint8_t *)radioAck, sizeof(radioAck));
    rf95.waitPacketSent();
    Serial.println("*******************************************************************");
    send_ack = 0;
  }
  delay(10000);
}
//***************************************************************************************
