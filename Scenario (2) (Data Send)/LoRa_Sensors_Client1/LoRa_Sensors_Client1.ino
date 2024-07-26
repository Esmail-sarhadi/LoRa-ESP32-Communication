#include <RH_RF95.h>
#include <SPI.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float RF95_FREQ = 433.0;
#define RFM95_CS 26
#define RFM95_RST 12
#define RFM95_INT 25
RH_RF95 rf95(RFM95_CS, RFM95_INT);

char nodeMessage[50];                                                       //Define Node ID. Change here (1).
uint8_t numNode1 = '1';                                                     //Change here (2).
uint8_t numNode2 = '1';                                                     //Change here (3).
int replyNum = 0;
int reTry = 0;
char nodeTemperature[10];
char nodeHumidity[10];
char nodeValue[10];

uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];                                       //Reply data array.
uint8_t len = sizeof(buf);                                                  //Reply data length.

float numTemperature;
float numHumidity;
float numValue = 11.11;
//************************************************************************************************************************
void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);

  Serial.println(">> LoRa Client TX Test!");
  Serial.println(">> Process begins..");
  dht.begin();
  delay(500);

  digitalWrite(RFM95_RST, LOW);                                             //Manual reset.
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init())
    Serial.println(">> LoRa initialization is failed!");
  else
    Serial.println(">> LoRa initialized successfully.");


  if (!rf95.setFrequency(RF95_FREQ)) {                                      //Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM.
    Serial.println(">> SetFrequency failed!");
    while (1);
  }
  Serial.print(">> Set Freq to: ");
  Serial.println(RF95_FREQ);

  //Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //The default transmitter power is 13dBm, using PA_BOOST.
  //If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then you can set transmitter powers from 2 to 20 dBm:
  rf95.setTxPower(20, false);

  Serial.println(">> Setup is done.");
  Serial.println("***********************************************");
  delay(1000);                                                              //Make all things stable.
}
//************************************************************************************************************************
void loop()
{
  replyNum = 0;
  reTry = 0;

  numTemperature = dht.readTemperature();
  numHumidity = dht.readHumidity();
  delay(500);

  memset(nodeMessage, 0, 50);
  memset(nodeTemperature, 0, 10);
  memset(nodeHumidity, 0, 10);
  memset(nodeValue, 0, 10);
  strcpy(nodeMessage, "11");                                                //Change here (4).

  Serial.println(">> Transmitting...");

  dtostrf(numTemperature, 5, 2, nodeTemperature);                           //Convert float to char.
  dtostrf(numHumidity, 5, 2, nodeHumidity);
  dtostrf(numValue, 5, 2, nodeValue);

  strcat(nodeMessage, ",");
  strcat(nodeMessage, nodeTemperature);
  strcat(nodeMessage, ",");
  strcat(nodeMessage, nodeHumidity);
  strcat(nodeMessage, ",");
  strcat(nodeMessage, nodeValue);
  strcat(nodeMessage, ",");
  Serial.print(">> Message is: ");
  Serial.println(nodeMessage);

  do
  {
    reTry += 1;

    rf95.send((uint8_t *) nodeMessage, strlen((char*)nodeMessage));         //Send LoRa Data.
    rf95.waitPacketSent();

    if (rf95.waitAvailableTimeout(1000))                                    //Should be a reply message for us now.
    {
      if (rf95.recv(buf, &len))                                             //Check if reply message is correct.
      {
        if ((buf[0] == numNode1) && (buf[1] == numNode2))                   //Check if reply message has the our node ID.
        {
          Serial.print(">> Got Reply: ");                                   //Print reply.
          Serial.println((char*)buf);
          Serial.print(">> RSSI: ");
          Serial.println(rf95.lastRssi(), DEC);
          Serial.println("***********************************************");
          replyNum = 1;
          delay(500);
        }
        else
        {
          Serial.println(">> No reply from server...");
          replyNum = 0;
        }
      }
    }
    else
    {
      Serial.println(">> No reply, Is there a listener around?");           //No signal reply.
      replyNum = 0;
      delay(500);
    }
    memset(buf, 0, RH_RF95_MAX_MESSAGE_LEN);
  } while (!replyNum && reTry < 3);

  if (reTry == 3) Serial.println("***********************************************");
  delay(10000);
}
//************************************************************************************************************************
