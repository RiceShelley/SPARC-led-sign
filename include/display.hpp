#ifndef DISPLAY_H
#define DISPLAY_H

#include <WS2812.h>
#include <Wire.h>
#include "Adafruit_MCP23008.h"

class display
{
public:
    display();
    void draw();
    int write(int x, int y, u8 r, u8 g, u8 b);
    void clear();
    void hardClear();
};

#endif