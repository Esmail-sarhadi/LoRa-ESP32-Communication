#include <RH_RF95.h>
#include <SPI.h>

float RF95_FREQ = 433.0;
#define RFM95_CS 26
#define RFM95_RST 12
#define RFM95_INT 25
RH_RF95 rf95(RFM95_CS, RFM95_INT);

char nodeID[20];
char nodeTemperature[25];
char nodeHumidity[25];
char nodeValue[25];

int numNode = 0;
float numTemperature = 0;
float numHumidity = 0;
float numValue = 0;
//************************************************************************************************************************
void writeOutput()
{
  Serial.print(">> Temperature: ");
  Serial.println(numTemperature);
  Serial.print(">> Humidity: ");
  Serial.println(numHumidity);
  Serial.print(">> Value: ");
  Serial.println(numValue);
  Serial.println("*********************************************");
}
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

  Serial.println(">> LoRa Server RX Test!");
  Serial.println(">> Process begins...");

  digitalWrite(RFM95_RST, LOW);                                                //Manual reset.
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init())
    Serial.println(">> LoRa initialization is failed!");
  else
    Serial.println(">> LoRa initialized successfully.");


  if (!rf95.setFrequency(RF95_FREQ)) {                                         //Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM.
    Serial.println(">> SetFrequency failed!");
    while (1);
  }
  Serial.print(">> Set Freq to: ");
  Serial.println(RF95_FREQ);

  //Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //The default transmitter power is 13dBm, using PA_BOOST.
  //If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then you can set transmitter powers from 2 to 20 dBm:
  rf95.setTxPower(20, false);

  Serial.println(">> Stup is done.");
  Serial.println("***********************************************");
  delay(1000);
}
//************************************************************************************************************************
void loop()
{ numNode = 0;

  if (rf95.available())                                                        //Listen Data from LoRa Node.
  {
    int i = 0, j = 0, code[5];
    int m1 = 0, m2 = 0, m3 = 0, m4 = 0, m5 = 0;
    char message[40] = "\0";
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];                                      //Receive data buffer.
    uint8_t len = sizeof(buf);                                                 //Data buffer length.
    uint8_t nodeReply[] = "   Server ACK.";
    if (rf95.recv(buf, &len))                                                  //Check if there is incoming data.
    {
      strcpy(message, (char *)buf);
      Serial.print(">> Message is: ");
      Serial.println(message);
      while (i < 40) {
        if (message[i] == ',') {
          code[j] = i;
          j++;
        }
        i++;
      }
      for (int k = 0; k < code[0]; k++) {
        nodeID[m1] = message[k];                                               //Get nodeID.
        m1++;
      }
      for (int k = code[0] + 1; k < code[1]; k++) {
        nodeTemperature[m2] = message[k];                                      //Get Temperature.
        m2++;
      }
      for (int k = code[1] + 1; k < code[2]; k++) {
        nodeHumidity[m3] = message[k];                                         //Get Humidity.
        m3++;
      }
      for (int k = code[2] + 1; k < code[3]; k++) {
        nodeValue[m4] = message[k];                                            //Get Value.
        m4++;
      }

      numHumidity = atof(nodeHumidity);
      numTemperature = atof(nodeTemperature);
      numValue = atof(nodeValue);
    
      numNode = atoi(nodeID);
      switch (numNode)
      {
        case 11: {                                                                                      
            Serial.println(">> Server got Node 11's message.");              
            nodeReply[0] = '1';                                                  
            nodeReply[1] = '1';                                                  
            rf95.send(nodeReply, sizeof(nodeReply));
            rf95.waitPacketSent();
            writeOutput();
          }
          break;
        case 12: {
            Serial.println(">> Server got Node 12's message.");
            nodeReply[0] = '1';
            nodeReply[1] = '2';
            rf95.send(nodeReply, sizeof(nodeReply));
            rf95.waitPacketSent();
            writeOutput();
          }
          break;
        case 13: {
            Serial.println(">> Server got Node 13's message.");
            nodeReply[0] = '1';
            nodeReply[1] = '3';
            rf95.send(nodeReply, sizeof(nodeReply));
            rf95.waitPacketSent();
            writeOutput();
          }
        case 14: {                                                             
            Serial.println(">> Server got Node 14's message.");              
            nodeReply[0] = '1';                                                  
            nodeReply[1] = '4';                                                  
            rf95.send(nodeReply, sizeof(nodeReply));
            rf95.waitPacketSent();
            writeOutput();
          }
          break;
        case 15: {
            Serial.println(">> Server got Node 15's message.");
            nodeReply[0] = '1';
            nodeReply[1] = '5';
            rf95.send(nodeReply, sizeof(nodeReply));
            rf95.waitPacketSent();
            writeOutput();
          }
          break;
        default:
          Serial.println("*********************************************");
          delay(100);
      }
      nodeID[0] = 0;
      nodeID[1] = 0;
    }
  }
}
//************************************************************************************************************************
