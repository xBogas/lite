#include <Arduino.h>
#include <STM32Ethernet.h>

// Local headers
#include "parsers.h"
#include "IMC_GENERATED/Blob.hpp"

static const IPAddress ip(10, 0, 2, 83);

static EthernetUDP sock;

static const int port = 8080;

static uint8_t packetBuffer[100];  //buffer to hold incoming packet,

void readUDP()
{
  int packetSize = sock.parsePacket();
  if (packetSize) 
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = sock.remoteIP();
    
    for (int i = 0; i < 4; i++) 
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
        Serial.print(".");
    }

    Serial.println();
    memset(packetBuffer, 0, packetSize);
    int bfr_len = sock.read(packetBuffer, packetSize);

    IMC::Message* msg = IMC::parser(packetBuffer, bfr_len);
    if (msg)
    {
      Serial.println("Created IMC msg: ");
      Serial.println(msg->getName());
      delete msg;
    }
  }
}


void readSerial()
{
  while(Serial.available())
  {
    //Reading only 8 bits to test double conversion
    int size = Serial.readBytes(packetBuffer, 8);
    Serial.printf("Read %d bytes\n", size);

    uint64_t data = 0x8000000000000000;
    memcpy(&data, packetBuffer, 8);
    Serial.print("uint64_t: ");
    Serial.println(data, 10);

    float rv = to_float(data);
    Serial.print("Converted to float ");
    Serial.println(rv);
  }
}


void setup() 
{
  Serial.begin(9600);
  delay(500);

  Ethernet.begin(ip);
  if(sock.begin(port))
    Serial.println("sock ready");

  sock.begin(ip);
  Serial.println("Setup completed");
}


void loop() 
{
  readUDP();
}