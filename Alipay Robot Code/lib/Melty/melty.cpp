#include <melty.h>
#include <LEDHandler.h>

melty::melty() {
    period_micros_calc = ringBuffer(1);
    time_seen_beacon_calc = ringBuffer(0.5); 
}

void melty::update() {
    bool curSeenIRLed;
    if (useTopIr)
        curSeenIRLed = isBeaconSensed(!digitalRead(TOP_IR_PIN));
    else
        curSeenIRLed = isBeaconSensed(!digitalRead(BOTTOM_IR_PIN));

    if (curSeenIRLed != lastSeenIRLed)
        if (curSeenIRLed) { // Activates on the rising edge of seeing the IR LED
            setLeds(CRGB::Green);
            currentPulse = micros();
            period_micros = currentPulse - lastPulse; // How long it takes to complete one revolution
            period_micros_calc.update(period_micros);
            lastPulse = currentPulse;
        }
        else { // Activates on the falling edge of seeing the IR LED
            setLeds(CRGB::Red);
            time_seen_beacon = micros() - currentPulse;
            time_seen_beacon_calc.update(time_seen_beacon);
            
            if (time_seen_beacon_calc.isLegit(time_seen_beacon)) { // && period_micros_calc.isLegit(period_micros) 
                computeTimings();
            }
        }

    
    lastSeenIRLed = curSeenIRLed;
}

void melty::computeTimings() {
    RPM = (60000*1000)/(period_micros_calc.getMaxVal());
    unsigned long center_of_beacon = currentPulse + time_seen_beacon/2; // This should ideally be centered on the beacon 
    unsigned long centerOfDrivePulse =  center_of_beacon + (float(deg)/360)*period_micros_calc.getMaxVal(); // Direction that we should be driving towards
    unsigned long deltaDriveTiming = (percentageOfRotation * period_micros_calc.getMaxVal())/2;

    unsigned long offset = 0;
    if (centerOfDrivePulse - deltaDriveTiming < micros()) { // If we're supposed to start translating before we have calculated those values, offset by one rotation
        offset = period_micros_calc.getMaxVal();
    }

    if (timingToggle) { // Swap variables that we use so we don't interfere (in the case that we're currently translating whilst computing)
        startDrive = centerOfDrivePulse - deltaDriveTiming + offset;
        endDrive = centerOfDrivePulse + deltaDriveTiming + offset;
    } else { 
        startDrive2  = centerOfDrivePulse - deltaDriveTiming + offset;
        endDrive2 = centerOfDrivePulse + deltaDriveTiming + offset;
    } 

    timingToggle = !timingToggle; // Toggle it so that next iteration uses different variables 

}

bool melty::translate() { // Returns whether or not robot should translate now
    unsigned long currentTime = micros();
    if (percentageOfRotation != 0)
        return (currentTime > startDrive && currentTime < endDrive || currentTime > startDrive2 && currentTime < endDrive2);
    else
        return 0;
}

bool melty::isBeaconSensed(bool currentReading) {
    // for (int i = 0; i < IRLedDataSize; i++)
    //     USBSerial.print(IRLedReadings[i]);
    // USBSerial.println();

    IRLedReadings[IRLedIndex++] = currentReading; // This is just code for a ring buffer
    if (IRLedIndex == IRLedDataSize)
        IRLedIndex = 0;

    if (lastIRLedReturnValue) {
        for (int i = 0; i < IRLedDataSize; i++)
            if (IRLedReadings[i] == lastIRLedReturnValue) 
                return true;
        lastIRLedReturnValue = !lastIRLedReturnValue;
    } else {
        for (int i = 0; i < IRLedDataSize; i++)
            if (IRLedReadings[i] == lastIRLedReturnValue) 
                return false;
        lastIRLedReturnValue = !lastIRLedReturnValue;
    }
    return lastIRLedReturnValue;
}