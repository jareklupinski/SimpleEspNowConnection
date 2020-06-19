/*
  SimpleEspNowConnectionServer

  Basic EspNowConnection Server implementation

  HOWTO Arduino IDE:
  - Prepare two ESP8266 or ESP32 based devices (eg. WeMos)
  - Start two separate instances of Arduino IDE and load 
    on the first one the 'SimpleEspNowConnectionServer.ino' and
    on the second one the 'SimpleEspNowConnectionClient.ino' sketch and upload 
    these to the two ESP devices.
  - Start the 'Serial Monitor' in both instances and set baud rate to 9600
  - Type 'startpair' into the edit box of both 'Serial Monitors' and hit Enter key (or press 'Send' button)
  - After devices are paired, type 'sendtest' into the edit box 
    of the 'Serial Monitor' and hit Enter key (or press 'Send' button)

  - You can use multiple clients which can be connected to one server

  Created 04 Mai 2020
  By Erich O. Pintar
  Modified 14 Jun 2020
  By Erich O. Pintar

  https://github.com/saghonfly/SimpleEspNowConnection

*/

#include "SimpleEspNowConnection.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);

String inputString;
String clientAddress;

void OnSendError(uint8_t* ad)
{
  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE!");
}

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  char output[len+1];
  output[len] = 0;

  memcpy(output, message, len);
  Serial.printf("MESSAGE:[%d]%s from %s\n", len, output, simpleEspConnection.macToStr(ad).c_str());

  Serial.println(ESP.getFreeHeap(),DEC);
  
//  clientAddress = simpleEspConnection.macToStr(ad);  

//  simpleEspConnection.sendMessage("Message at OnMessage from Server", clientAddress);  
}

void OnPaired(uint8_t *ga, String ad)
{
  Serial.println("EspNowConnection : Client '"+ad+"' paired! ");

  clientAddress = ad;
}

void OnConnected(uint8_t *ga, String ad)
{
  Serial.println("EspNowConnection : Client '"+ad+"' connected! ");

  simpleEspConnection.sendMessage((uint8_t *)"Message at OnConnected from Server", 34, ad);
}

void setup() 
{
  Serial.begin(9600);
  Serial.println();
  // clientAddress = "ECFABC0CE7A2"; // Test if you know the client
  clientAddress = "CC50E35B56B1";

  simpleEspConnection.begin(true);
   simpleEspConnection.setPairingBlinkPort(2);
  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
  simpleEspConnection.onSendError(&OnSendError);
  simpleEspConnection.onConnected(&OnConnected);  

  Serial.println(WiFi.macAddress());    
}

void loop() 
{
  simpleEspConnection.loop();
  
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n') 
    {
      Serial.println(inputString);

      if(inputString == "startpair")
      {
        simpleEspConnection.startPairing(30);
      }
      else if(inputString == "endpair")
      {
        simpleEspConnection.endPairing();
      }
      else if(inputString == "changepairingmac")
      {
        uint8_t np[] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};
        
        simpleEspConnection.setPairingMac(np);
      }      
      else if(inputString == "sendtest")
      {
        if(!simpleEspConnection.sendMessage((uint8_t *)"This comes from the server", 26, clientAddress))
        {
          Serial.println("SENDING TO '"+clientAddress+"' WAS NOT POSSIBLE!");
        }
      }
      
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}
