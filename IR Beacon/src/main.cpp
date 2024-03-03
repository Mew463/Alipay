#include <Arduino.h>
#include <WiFiUdp.h>
#include <LEDHandler.h>
#include <Battery_Monitor.h>
#include <BLE_Uart.h>
#include <esp_now_txrx.h>

#define TARGETCMD '1' // Change based on which IR_Beacon working on 

const int packSize = 6;
char laptop_packetBuffer[packSize] = {'0', '0', '0', '0', '0', '0'};

const int IRLedPin = 4;
const int freq = 38000;
const int ledChannel = 0;
const int resolution = 8;

int dutycycle = 100;
const int maxdutycycle = 180;

BLE_Uart laptop = BLE_Uart(laptop_packetBuffer, packSize);

void setup(){
  init_led();
  setLeds(CRGB::Green); 
  USBSerial.begin(115200);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(IRLedPin, ledChannel);
  laptop.init_ble("IR Beacon");
  setLeds(CRGB::Black);
}
 
void loop(){
  if (laptop.isConnected()) {
    switch (laptop_packetBuffer[0]) {
      case '0': // Disabled 
        toggleLeds(CRGB::Red, CRGB::Green, 500);
        ledcWrite(ledChannel, 0);
        EVERY_N_SECONDS(10) 
          laptop.send(get1sVoltage());
        break;
      case '1': // Enable one of the IR Beacons
        if (laptop_packetBuffer[1] == TARGETCMD) {
          toggleLeds(CRGB::Blue, CRGB::Purple, 500);
          ledcWrite(ledChannel, dutycycle);
        } else {
          setLeds(CRGB::Black);
          ledcWrite(ledChannel, 0);
        }
        break;
      default: // Don't enable current IR Beacon if in another mode
        setLeds(CRGB::Black);
        ledcWrite(ledChannel, 0);
        break;
    }
    EVERY_N_MILLIS(100) {
      if (laptop_packetBuffer[1] == TARGETCMD) {
        switch (laptop_packetBuffer[2]) {
          case '1':
            dutycycle+=3;
            if (dutycycle > maxdutycycle)
              dutycycle = maxdutycycle;
            break;
          case '2':
            dutycycle-=3;
            if (dutycycle < 0)
              dutycycle = 0;
            break;
        }
        if (laptop_packetBuffer[2] == '1' || laptop_packetBuffer[2] == '2') {
          String msg = "dutycycle : " + String(dutycycle);
          laptop.send(msg);
        }
      }
    }
  } else {
    toggleLeds(CRGB::Red, CRGB::Black, 500);
    ledcWrite(ledChannel, 0);
  }
}