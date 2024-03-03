#include <Arduino.h>
#include "/Users/mingweiyeoh/Documents/GitHub/Arduino-Projects/libraries/Custom/USBSerialHandler.h"

SerialHandler myComputer = SerialHandler(); 

const int motpin = 8; // 7 is right and 8 is left
const int motchannel = 0;
const int neutralVal = 189;

void autoMotorTune() {
  delay(1000);
  USBSerial.println("Start with motor off!!");
  ledcWrite(motchannel, 220);
  delay(3000);
  USBSerial.println("Turn motor on!!!");
  delay(5000);
  ledcWrite(motchannel, 40);
  USBSerial.println("Tuning low value!!!");
  delay(2500);
  ledcWrite(motchannel, neutralVal);
  USBSerial.println("Tuning neutral value!!!");
  delay(2500);
  USBSerial.println("Done!!!");
}


void setup() {
  USBSerial.begin(115200);  
  ledcSetup(motchannel, 500, 8);
  ledcAttachPin(motpin, motchannel);
  ledcWrite(motchannel, neutralVal);


  autoMotorTune();

  while (myComputer.getInt(0) == 0) {
    delay(10);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  ledcWrite(motchannel, myComputer.getInt(0));
  delay(10);
}

