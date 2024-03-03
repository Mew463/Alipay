#include <Arduino.h>
#include <LEDHandler.h>
#include <mpu6050.h>
#include <Drive_Motors.h>
#include <Battery_Monitor.h>
#include <melty.h> 
#include <BLE_Uart.h>

const int packSize = 6;
char laptop_packetBuffer[packSize] = {'0', '0', '0', '0', '0', '0'};
bool wasMeltying = false;
int slowDownSpeed = 5;

BLE_Uart laptop = BLE_Uart(laptop_packetBuffer, packSize);
Drive_Motors driveMotors = Drive_Motors();

melty alipay = melty();
struct melty_parameters {
  int rot = 10;
  int tra = 6;
  float per = 0.5;
  int invert = 1;
} melty_parameters;

struct tank_drive_parameters {
  int drive = 2;
  int turn = 2;
} tank_drive_parameters;

void setup()
{
  init_led();
  setLeds(CRGB::Green);
  USBSerial.begin(115200);
  init_mpu6050();
  driveMotors.init_motors();
  laptop.init_ble("Alipay");
  setLeds(CRGB::Black); 
}
 
void loop()
{
  if (laptop.isConnected()) {
    if (driveMotors.isNeutral()) {
      if (getAccelZ() > 7) {
        alipay.useTopIr = 1;
        driveMotors.flip_motors = 0;
        melty_parameters.invert = 1;
      }
      if (getAccelZ() < -7) {
        alipay.useTopIr = 0;
        driveMotors.flip_motors = 1;
        melty_parameters.invert = -1;
      }
    }

    if (laptop_packetBuffer[0] == '1') { // Currently enabled and meltybraining!!!
      alipay.update();
      if (alipay.translate()) {
        driveMotors.l_motor_write(melty_parameters.invert * (melty_parameters.rot-melty_parameters.tra));
        driveMotors.r_motor_write(melty_parameters.invert *(melty_parameters.rot+melty_parameters.tra));
      } else {
        driveMotors.set_both_motors(melty_parameters.invert * melty_parameters.rot);
      }

      switch (laptop_packetBuffer[1]) { // Check the drive cmd
      case '8':
        alipay.deg = 0; 
        break;
      case '7':
        alipay.deg = 45; 
        break;
      case '6':
        alipay.deg = 90; 
        break;
      case '5':
        alipay.deg = 135; 
        break;
      case '4':
        alipay.deg = 180; 
        break;
      case '3':
        alipay.deg = 225; 
        break;
      case '2':
        alipay.deg = 270; 
        break;
      case '1':
        alipay.deg = 315; 
        break;
      }

      if (laptop_packetBuffer[1] != '0') {// Check drive cmd for setting the "neutral" state
        alipay.percentageOfRotation = melty_parameters.per;
      } else {
        alipay.percentageOfRotation = 0;
      }

      EVERY_N_SECONDS(1) { // DEBUGGIN!!!!
        String msg = "rpm : " + String(alipay.RPM);
        laptop.send(msg);
      }

      EVERY_N_MILLIS(100) {
        switch (laptop_packetBuffer[2]) {
          case '1':
            melty_parameters.rot++;
            break;
          case '2':
            melty_parameters.rot--;
            break;
          case '3':
            melty_parameters.tra++;
            break;
          case '4':
            melty_parameters.tra--;
            break;
          case '5':
            melty_parameters.per = melty_parameters.per + .03;
            break;
          case '6':
            melty_parameters.per = melty_parameters.per - .03;
            break;
        }

        if (laptop_packetBuffer[2] != '0') {
          String msg = "rotpwr : " + String(melty_parameters.rot) + " tranpwr : " + String(melty_parameters.tra) + " perc : " + String(melty_parameters.per);
          laptop.send(msg);
        }
      }

      wasMeltying = 1;

    } else if (laptop_packetBuffer[0] == '2') { // Tank driving mode!
      if (wasMeltying) { // Was previously meltybraining, we need to slowdown
        unsigned long timeout = millis();
        while (abs(getAccelY()) > .3 && millis() - timeout < 2000) {
          driveMotors.set_both_motors(-slowDownSpeed * melty_parameters.invert);
          toggleLeds(CRGB::White, CRGB::Red, 150);
        }
        wasMeltying = 0;
      }
      
      int lmotorpwr = 0;
      int rmotorpwr = 0;
      switch (laptop_packetBuffer[1]) { // Check the drive cmd
      case '0':
        lmotorpwr = 0;
        rmotorpwr = 0;
        break;
      case '1':
        lmotorpwr = tank_drive_parameters.drive;
        rmotorpwr = tank_drive_parameters.drive;
        break;
      case '2':
        lmotorpwr = tank_drive_parameters.drive + tank_drive_parameters.turn;
        rmotorpwr = tank_drive_parameters.drive; 
        break;
      case '3':
        lmotorpwr = tank_drive_parameters.turn;
        rmotorpwr = -tank_drive_parameters.turn; 
        break;
      case '4':
        lmotorpwr = -tank_drive_parameters.drive - tank_drive_parameters.turn;
        rmotorpwr = -tank_drive_parameters.drive;
        break;
      case '5':
        lmotorpwr = -tank_drive_parameters.drive;
        rmotorpwr = -tank_drive_parameters.drive;
        break;
      case '6':
        lmotorpwr = -tank_drive_parameters.drive;
        rmotorpwr = -tank_drive_parameters.drive - tank_drive_parameters.turn; 
        break;
      case '7':
        lmotorpwr = -tank_drive_parameters.turn;
        rmotorpwr = tank_drive_parameters.turn;  
        break;
      case '8':
        lmotorpwr = tank_drive_parameters.drive;
        rmotorpwr = tank_drive_parameters.drive + tank_drive_parameters.turn; 
        break;
      }

      EVERY_N_MILLIS(100) {
        switch (laptop_packetBuffer[2]) {
          case '1':
            tank_drive_parameters.drive++;
            break;
          case '2':
            tank_drive_parameters.drive--;
            break;
          case '3':
            tank_drive_parameters.turn++;
            break;
          case '4':
            tank_drive_parameters.turn--;
            break;
        }

        if (laptop_packetBuffer[2] != '0') {
          String msg = "drivpwr : " + String(tank_drive_parameters.drive) + " turnpwr : " + String(tank_drive_parameters.turn);
          laptop.send(msg);
        }
      }

      driveMotors.l_motor_write(-lmotorpwr);
      driveMotors.r_motor_write(rmotorpwr);
      toggleLeds(CRGB::White, CRGB::Black, 500);
    
    } else { // Currently disabled
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      driveMotors.set_both_motors(0); 
        EVERY_N_SECONDS(10) {
          laptop.send(get3sVoltage());
        }
    }
  } else {
    driveMotors.set_both_motors(0);
    toggleLeds(CRGB::Red, CRGB::Black, 500);
  }
}
