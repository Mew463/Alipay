#include <FastLED.h>

#ifdef IR_BEACON
    #define LEDPIN 5
#else
    #define LEDPIN 4
#endif

void init_led();

void setLeds(CRGB color);

void toggleLeds(CRGB color1, CRGB color2, int delayMS);

void syncToggle();