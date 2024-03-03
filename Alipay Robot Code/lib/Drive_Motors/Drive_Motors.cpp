#include "Drive_Motors.h"
#include <Arduino.h>



void Drive_Motors::init_motors() {
    ledcSetup(lMotChannel, motFreq, resolution);
    ledcSetup(rMotChannel, motFreq, resolution);
    ledcAttachPin(lMotPin, lMotChannel);
    ledcAttachPin(rMotPin, rMotChannel);

    l_motor_write(neutralValue);
    r_motor_write(neutralValue);
}

void Drive_Motors::l_motor_write(int value) { 
    l_motor_value = value;
    if (value != 0)
        if (value > 0)
            value += 2;
        else
            value -= 2;
    if (!flip_motors)
        ledcWrite(lMotChannel, neutralValue + value);
    else
        ledcWrite(rMotChannel, neutralValue + value);
}

void Drive_Motors::r_motor_write(int value) { 
    r_motor_value = value;
    if (value != 0)
        if (value > 0)
            value += 2;
        else
            value -= 2;

    if (!flip_motors)
        ledcWrite(rMotChannel, neutralValue + value);
    else
        ledcWrite(lMotChannel, neutralValue + value);
}

void Drive_Motors::set_both_motors(int value) {
    l_motor_write(value);
    r_motor_write(value);
}

bool Drive_Motors::isNeutral() {
    if (l_motor_value == 0 && r_motor_value == 0)
        return true;
    else
        return false;
}