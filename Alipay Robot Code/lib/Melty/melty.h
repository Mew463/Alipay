#include <Arduino.h>
#include <mpu6050.h>

/*

two uses for ringbuffer is calculating
period_ms
time_seen_beacon 
*/


#define RINGBUFSIZE 5
class ringBuffer{
    public:
        /**
         * @brief Construct a ring buffer object.
         *
         * @param _criteria How close should the new value match the Max value from the ring buffer to be considered legit?
         */
        ringBuffer(float _criteria) {
            criteria = _criteria;
        }

        ringBuffer() {}

        void update(unsigned long val) {
            ringBuf[curIndex++] = val;
            if (curIndex > RINGBUFSIZE)
                curIndex = 0;
        }

        bool isLegit(unsigned long newVal) {
            unsigned long maxVal = getMaxVal();
            if (newVal > maxVal * criteria) 
                return true;
            else
                return false;
        }

        unsigned long getMaxVal() {
            unsigned long maxVal = 0;
            for (int i = 0; i < RINGBUFSIZE; i++)
                if (maxVal < ringBuf[i])
                    maxVal = ringBuf[i];
            return maxVal;
        }

    private:
        unsigned long ringBuf[RINGBUFSIZE];
        int curIndex = 0;
        float criteria = 0;
        String returnArray() {
            String msg = "";
            for (int i = 0; i < RINGBUFSIZE; i++) {
                msg = msg + String(ringBuf[i]) + " ";
            }
            return msg;
        }
};



#define TOP_IR_PIN     10
#define BOTTOM_IR_PIN   9
#define IRLedDataSize   25

class melty {
    public:
        melty();
        void update();
        bool isBeaconSensed(bool currentReading);
        void computeTimings();
        bool translate();
        int RPM = 0;
        int deg = 0;
        float percentageOfRotation = 0;
        bool useTopIr = 1;
    private:
        bool lastSeenIRLed = 0;
        unsigned long period_micros = micros();
        unsigned long currentPulse = micros();
        unsigned long lastPulse = micros();
        unsigned long time_seen_beacon = micros();

        bool IRLedReadings[IRLedDataSize] = {0};
        int IRLedIndex = 0;
        bool lastIRLedReturnValue = 0;
        unsigned long startDrive = micros();
        unsigned long endDrive = micros();

        unsigned long startDrive2 = micros();
        unsigned long endDrive2 = micros();

        bool timingToggle = 0;

        ringBuffer period_micros_calc;
        ringBuffer time_seen_beacon_calc;
};

