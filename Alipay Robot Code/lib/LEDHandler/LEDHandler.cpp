#include "LEDHandler.h"

bool ledToggleState = 0;
unsigned long lastdelayToggle = millis();
unsigned long currentDelayToggle = millis();

CRGB lastColor1;
CRGB lastColor2;

CRGB leds[1];

void init_led() {
    FastLED.addLeds<WS2812B, LEDPIN, GRB>(leds, 1).setCorrection(TypicalLEDStrip);
}

void setLeds(CRGB color)
{
    leds[0] = color;
    FastLED.show();
}

void syncToggle() {
    lastdelayToggle = millis();
}

void toggleLeds(CRGB color1, CRGB color2, int delayMS) {
    if (color1 != lastColor1 || color2 != lastColor2 || millis() - lastdelayToggle > delayMS + 50) { // For color syncing purposes between different devices
        lastdelayToggle = millis(); 
        ledToggleState = 0;
    }
    currentDelayToggle = millis();
    
    if (currentDelayToggle - lastdelayToggle > delayMS) {
        ledToggleState = !ledToggleState;
        lastdelayToggle = currentDelayToggle;
    }

    if (ledToggleState)
        setLeds(color1);
    else
        setLeds(color2);

    lastColor1 = color1;
    lastColor2 = color2;
}