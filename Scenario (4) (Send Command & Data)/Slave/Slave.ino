#include <SPI.h>
#include <RH_RF95.h>
#include <DHT.h>
#include <Wire.h>

float RF95_FREQ = 433.0;
#define RFM95_CS 26
#define RFM95_RST 12
#define RFM95_INT 25
RH_RF95 rf95(RFM95_CS, RFM95_INT);
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];                                        //Receive data buffer.
char message[5];

int pinBuzzer = 2;
int pinRelay = 5;

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float numTemperature;                                                        //Change here (1).
float numHumidity;                                                           //Change here (2).
float numValue = 20;

float frequency = 433.0;

int masterCommand;
int numNode;
char nodeCommand[2];
char nodeID[3];
char nodeTemperature[6];
char nodeHumidity[6];
char nodeValue[6];
//************************************************************************************************************************
void writeOutput()
{
  Serial.print(">> Temperature: ");
  Serial.println(numTemperature);
  Serial.print(">> Humidity: ");
  Serial.println(numHumidity);
  Serial.print(">> Value: ");
  Serial.println(numValue);
}
//************************************************************************************************************************
void clearVariables()
{
  memset(buf, 0, RH_RF95_MAX_MESSAGE_LEN);
  memset(message, 0, 5);
  memset(nodeID, 0, 3);
  memset(nodeTemperature, 0, 6);
  memset(nodeHumidity, 0, 6);
  memset(nodeValue, 0, 6);
  memset(nodeCommand, 0, 2);
}
//************************************************************************************************************************
void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode (pinBuzzer, OUTPUT);
  pinMode (pinRelay, OUTPUT);

  dht.begin();

  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);

  Serial.println(">> LoRa Master Test!");
  Serial.println(">> Process begins..");

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
  rf95.setTxPower(20);

  clearVariables();

  Serial.println(">> Setup is done.");
  Serial.println("***********************************************");
  delay(1000);
}
//************************************************************************************************************************
void loop()
{
  char sendData[30] = "11";                                                  //Define Node ID. //Change here (3)

  if (rf95.available())                                                      //Listen Data from LoRa Node.
  {
    int i = 0, j = 0, code[2];
    int m1 = 0, m2 = 0;
    uint8_t len = sizeof(buf);                                               //Data buffer length.
    if (rf95.recv(buf, &len))                                                //Check if there is incoming data.
    {
      strcpy(message, (char *)buf);
      while (i < 5) {
        if (message[i] == ',') {
          code[j] = i;
          j++;
        }
        i++;
      }
      for (int k = 0; k < code[0]; k++) {
        nodeID[m1] = message[k];                                             //Get nodeID.
        m1++;
      }
      for (int k = code[0] + 1; k < code[1]; k++) {
        nodeCommand[m2] = message[k];                                        //Get Command.
        m2++;
      }

      numNode = atoi(nodeID);
      masterCommand = atoi(nodeCommand);

      if (numNode == 11) {                                                   //Change here (4).

        switch (masterCommand)
        {
          case 1: {                                                          //Send Data.
              numTemperature = dht.readTemperature();
              numHumidity = dht.readHumidity();
              numValue += 0.1;
              dtostrf(numTemperature, 5, 2, nodeTemperature);                //Convert float to char.
              dtostrf(numHumidity, 5, 2, nodeHumidity);
              dtostrf(numValue, 5, 2, nodeValue);

              strcat(sendData, ",");
              strcat(sendData, nodeTemperature);
              strcat(sendData, ",");
              strcat(sendData, nodeHumidity);
              strcat(sendData, ",");
              strcat(sendData, nodeValue);
              strcat(sendData, ",");
              Serial.print(">> Message is: ");
              Serial.println(sendData);
              writeOutput();

              rf95.send((uint8_t *)sendData, sizeof(sendData));
              rf95.waitPacketSent();
            }
            break;
          case 2: {
              Serial.println(">> Active Buzzer.");
              digitalWrite (pinBuzzer, HIGH);
              delay(300);
              digitalWrite (pinBuzzer, LOW);
              delay(400);
              digitalWrite (pinBuzzer, HIGH);
              delay(300);
              digitalWrite (pinBuzzer, LOW);
            }
            break;
          case 3: {
              Serial.println(">> Active Relay.");
              digitalWrite (pinRelay, HIGH);
              delay(300);
              digitalWrite (pinRelay, LOW);
              delay(400);
              digitalWrite (pinRelay, HIGH);
              delay(300);
              digitalWrite (pinRelay, LOW);
            }
            break;
          default: {
              Serial.println(">> Command number is not identified.");
              delay(100);
            }
        }
        Serial.println("*********************************************");
        clearVariables();
      }
      else {
        Serial.println(">> Command not recognized..");
        Serial.println("*********************************************");
      }
    }
  }
}
//************************************************************************************************************************
