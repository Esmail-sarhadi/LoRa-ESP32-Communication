#include <Process.h>
#include <FileIO.h>
#include <RH_RF95.h>

#define BAUDRATE 115200
RH_RF95 rf95;
String myWriteAPIString = "8R0LAUM2I6SOOQI1";
float frequency = 433.0;
String dataNode = "";
String dataTime = "";

char nodeID[20];
char nodeTemperature[25];
char nodeHumidity[25];
//***************************************************************************************************************
void setup()
{
  Bridge.begin(BAUDRATE);
  rf95.init();
  rf95.setFrequency(frequency);
  rf95.setTxPower(20);
}
//***************************************************************************************************************
void sendDataNode() {
  String myUrl = "https://api.thingspeak.com/update?api_key=";
  myUrl += myWriteAPIString;
  myUrl += "&";
  myUrl += dataNode;

  delay(100);

  Process p;
  p.begin("curl");
  p.addParameter("-k");
  p.addParameter(myUrl);
  p.run();
  p.close();
}
//************************************************************************************************************************
void loop()
{
  int tempNode = 0;
  float tempHumidity = 0;
  float tempTemperature = 0;

  if (rf95.available())                                                                 //Listen Data from LoRa Node.
  {
    int i = 0, j = 0, code[5];
    int m1 = 0, m2 = 0, m3 = 0, m4 = 0, m5 = 0;
    char message[40] = "\0";
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];                                               //Receive data buffer.
    uint8_t len = sizeof(buf);                                                          //Data buffer length.
    uint8_t serverReply[] = "   Server ACK.";
    dataNode = "";
    
    if (rf95.recv(buf, &len))                                                           //Check if there is incoming data.
    {
      strcpy(message, (char *)buf);
      while (i < 40) {
        if (message[i] == ',') {
          code[j] = i;
          j++;
        }
        i++;
      }
      for (int k = 0; k < code[0]; k++) {
        nodeID[m1] = message[k];                                                        //Get nodeID.
        m1++;
      }
      for (int k = code[0] + 1; k < code[1]; k++) {
        nodeTemperature[m2] = message[k];                                               //Get Temperature.
        m2++;
      }
      for (int k = code[1] + 1; k < code[2]; k++) {
        nodeHumidity[m3] = message[k];                                                  //Get Humidity.
        m3++;
      }

      tempNode = atoi(nodeID);
      tempHumidity = atof(nodeHumidity);
      tempTemperature = atof(nodeTemperature);

      dataNode = String("&field1=") + tempTemperature + "&field2=" + tempHumidity;
      sendDataNode();                                                                   //Send Node data to internet.

      switch (tempNode)
      {
        case 11: {                                                                      //Check if the ID match the LoRa Node ID.
            serverReply[0] = '1';
            serverReply[1] = '1';
            rf95.send(serverReply, sizeof(serverReply));
            rf95.waitPacketSent();
          }
          break;

        case 12: {
            serverReply[0] = '1';
            serverReply[1] = '2';
            rf95.send(serverReply, sizeof(serverReply));
            rf95.waitPacketSent();
          }
          break;

        case 13: {
            serverReply[0] = '1';
            serverReply[1] = '3';
            rf95.send(serverReply, sizeof(serverReply));
            rf95.waitPacketSent();
          }
          break;

        case 14: {
            serverReply[0] = '1';
            serverReply[1] = '4';
            rf95.send(serverReply, sizeof(serverReply));
            rf95.waitPacketSent();
          }
          break;

        case 15: {
            serverReply[0] = '1';
            serverReply[1] = '5';
            rf95.send(serverReply, sizeof(serverReply));
            rf95.waitPacketSent();
          }
          break;

        default:
          delay(100);
      }
    }
  }
}
//***************************************************************************************************************
