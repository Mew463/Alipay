#include "mpu6050.h"

Adafruit_MPU6050 mpu;

void init_mpu6050() {
    Wire.begin(5,6);
    if (!mpu.begin()) {
        USBSerial.println("Failed to find MPU6050 chip");
        while (1) {
        delay(10);
        }
    }
    USBSerial.println("MPU6050 Found!");

    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
}

float getAccelY() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    return (a.acceleration.y);
}

float getAccelZ() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    return (a.acceleration.z);
}

float getTemp() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    return (temp.temperature);
}