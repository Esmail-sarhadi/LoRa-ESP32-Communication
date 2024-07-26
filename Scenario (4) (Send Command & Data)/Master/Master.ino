#include <RH_RF95.h>
#include <SPI.h>

float RF95_FREQ = 433.0;
#define RFM95_CS 26
#define RFM95_RST 12
#define RFM95_INT 25
RH_RF95 rf95(RFM95_CS, RFM95_INT);
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];                                       //Receive data buffer.
char message[30];

char nodeID[3];
char nodeTemperature[6];
char nodeHumidity[6];
char nodeValue[6];

int numNode = 0;
float numTemperature = 0;
float numHumidity = 0;
float numValue = 0;
//************************************************************************************************************************
void writeOutput()
{
  Serial.print(">> Node: ");
  Serial.println(numNode);
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
  memset(message, 0, 30);
  memset(nodeID, 0, 3);
  memset(nodeTemperature, 0, 6);
  memset(nodeHumidity, 0, 6);
  memset(nodeValue, 0, 6);
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
  char nodeCommand[5] = "11";                                               //Define Node ID. Change here (1).
  char nodeRequest[1];
  int numRequest = 0;

  while (Serial.available() > 0) {                                          //Important for clearing the buffer. (Quiz)
    Serial.read();
  }

  Serial.println();
  Serial.println("****************************************************************");
  Serial.println("****************************************************************");
  Serial.println("##########################################");
  Serial.println("#                  Menu                  #");
  Serial.println("##########################################");
  Serial.println("#  1. Send Data.                         #");
  Serial.println("#  2. Active Buzzer.                     #");
  Serial.println("#  3. Active Relay.                      #");
  Serial.println("#                                        #");
  Serial.println("#  Type the number and press \"Enter\":    #");
  Serial.println("##########################################");

  while (!Serial.available()) {}
  int i = Serial.parseInt();

  switch (i) {
    case 1:
      numRequest = 1;
      break;
    case 2:
      numRequest = 2;
      break;
    case 3:
      numRequest = 3;
      break;
  }

  Serial.println(">> Transmitting...");

  dtostrf(numRequest, 1, 0, nodeRequest);

  strcat(nodeCommand, ",");
  strcat(nodeCommand, nodeRequest);
  strcat(nodeCommand, ",");
  Serial.print(">> Command is: ");
  Serial.println(nodeCommand);

  rf95.send((uint8_t *) nodeCommand, sizeof(nodeCommand));                  //Send LoRa Data.
  rf95.waitPacketSent();

  if (rf95.waitAvailableTimeout(1000))                                      //Listen Data from LoRa Node.
  {
    int i = 0, j = 0, code[4];
    int m1 = 0, m2 = 0, m3 = 0, m4 = 0;
    uint8_t len = sizeof(buf);                                              //Data buffer length.
    if (rf95.recv(buf, &len))                                               //Check if there is incoming data.
    {
      strcpy(message, (char *)buf);
      Serial.print(">> Message is: ");
      Serial.println(message);
      while (i < 30) {
        if (message[i] == ',') {
          code[j] = i;
          j++;
        }
        i++;
      }
      for (int k = 0; k < code[0]; k++) {
        nodeID[m1] = message[k];                                            //Get nodeID.
        m1++;
      }
      for (int k = code[0] + 1; k < code[1]; k++) {
        nodeTemperature[m2] = message[k];                                   //Get Temperature.
        m2++;
      }
      for (int k = code[1] + 1; k < code[2]; k++) {
        nodeHumidity[m3] = message[k];                                      //Get Humidity.
        m3++;
      }
      for (int k = code[2] + 1; k < code[3]; k++) {
        nodeValue[m4] = message[k];                                         //Get Value.
        m4++;
      }

      numNode = atoi(nodeID);
      numHumidity = atof(nodeHumidity);
      numTemperature = atof(nodeTemperature);
      numValue = atof(nodeValue);
      if (numNode == 11) writeOutput();                                     //Change here (2).
      else {
        Serial.println(">> Message not recognized.");
        Serial.println("*********************************************");
      }
      clearVariables();
    }
  }
}
//************************************************************************************************************************
