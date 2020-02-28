#include "display.hpp"

#define NLEDS 24

static WS2812 seg[] = {WS2812(NLEDS), WS2812(NLEDS), WS2812(NLEDS)};
static Adafruit_MCP23008 mcp;

static cRGB screen[24][24];
static cRGB realScreen[24][24];

display::display()
{
  seg[0].setOutput(9);
  seg[1].setOutput(8);
  seg[2].setOutput(10);

  mcp.begin();
  mcp.pinMode(0, OUTPUT);
  mcp.pinMode(1, OUTPUT);
  mcp.pinMode(2, OUTPUT);
  mcp.pinMode(3, OUTPUT);
  mcp.pinMode(4, OUTPUT);
  mcp.pinMode(5, OUTPUT);

  mcp.pinMode(6, OUTPUT);
  mcp.pinMode(7, OUTPUT);
  pinMode(11, OUTPUT);

  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 24; j++)
    {
      screen[i][j].r = 0;
      screen[i][j].g = 0;
      screen[i][j].b = 0;

      realScreen[i][j].r = 0;
      realScreen[i][j].g = 0;
      realScreen[i][j].b = 0;
    }
  }
}

static void decode(uint8_t curStrip)
{
  if (curStrip < 8)
  {
    switch (curStrip)
    {
    case 0:
      mcp.digitalWrite(0, LOW);
      mcp.digitalWrite(1, LOW);
      mcp.digitalWrite(2, LOW);
      break;
    case 1:
      mcp.digitalWrite(0, HIGH);
      mcp.digitalWrite(1, LOW);
      mcp.digitalWrite(2, LOW);
      break;
    case 2:
      mcp.digitalWrite(0, LOW);
      mcp.digitalWrite(1, HIGH);
      mcp.digitalWrite(2, LOW);
      break;
    case 3:
      mcp.digitalWrite(0, HIGH);
      mcp.digitalWrite(1, HIGH);
      mcp.digitalWrite(2, LOW);
      break;
    case 4:
      mcp.digitalWrite(0, LOW);
      mcp.digitalWrite(1, LOW);
      mcp.digitalWrite(2, HIGH);
      break;
    case 5:
      mcp.digitalWrite(0, HIGH);
      mcp.digitalWrite(1, LOW);
      mcp.digitalWrite(2, HIGH);
      break;
    case 6:
      mcp.digitalWrite(0, LOW);
      mcp.digitalWrite(1, HIGH);
      mcp.digitalWrite(2, HIGH);
      break;
    case 7:
      mcp.digitalWrite(0, HIGH);
      mcp.digitalWrite(1, HIGH);
      mcp.digitalWrite(2, HIGH);
      break;
    }
  }
  else if (curStrip < 16)
  {
    switch (curStrip - 8)
    {
    case 0:
      mcp.digitalWrite(3, LOW);
      mcp.digitalWrite(4, LOW);
      mcp.digitalWrite(5, LOW);
      break;
    case 1:
      mcp.digitalWrite(3, HIGH);
      mcp.digitalWrite(4, LOW);
      mcp.digitalWrite(5, LOW);
      break;
    case 2:
      mcp.digitalWrite(3, LOW);
      mcp.digitalWrite(4, HIGH);
      mcp.digitalWrite(5, LOW);
      break;
    case 3:
      mcp.digitalWrite(3, HIGH);
      mcp.digitalWrite(4, HIGH);
      mcp.digitalWrite(5, LOW);
      break;
    case 4:
      mcp.digitalWrite(3, LOW);
      mcp.digitalWrite(4, LOW);
      mcp.digitalWrite(5, HIGH);
      break;
    case 5:
      mcp.digitalWrite(3, HIGH);
      mcp.digitalWrite(4, LOW);
      mcp.digitalWrite(5, HIGH);
      break;
    case 6:
      mcp.digitalWrite(3, LOW);
      mcp.digitalWrite(4, HIGH);
      mcp.digitalWrite(5, HIGH);
      break;
    case 7:
      mcp.digitalWrite(3, HIGH);
      mcp.digitalWrite(4, HIGH);
      mcp.digitalWrite(5, HIGH);
      break;
    }
  }
  else
  {
    switch (curStrip - 16)
    {
    case 0:
      mcp.digitalWrite(6, LOW);
      mcp.digitalWrite(7, LOW);
      digitalWrite(11, LOW);
      break;
    case 1:
      mcp.digitalWrite(6, HIGH);
      mcp.digitalWrite(7, LOW);
      digitalWrite(11, LOW);
      break;
    case 2:
      mcp.digitalWrite(6, LOW);
      mcp.digitalWrite(7, HIGH);
      digitalWrite(11, LOW);
      break;
    case 3:
      mcp.digitalWrite(6, HIGH);
      mcp.digitalWrite(7, HIGH);
      digitalWrite(11, LOW);
      break;
    case 4:
      mcp.digitalWrite(6, LOW);
      mcp.digitalWrite(7, LOW);
      digitalWrite(11, HIGH);
      break;
    case 5:
      mcp.digitalWrite(6, HIGH);
      mcp.digitalWrite(7, LOW);
      digitalWrite(11, HIGH);
      break;
    case 6:
      mcp.digitalWrite(6, LOW);
      mcp.digitalWrite(7, HIGH);
      digitalWrite(11, HIGH);
      break;
    case 7:
      mcp.digitalWrite(6, HIGH);
      mcp.digitalWrite(7, HIGH);
      digitalWrite(11, HIGH);
      break;
    }
  }
}
int color_scale = 3;
int display::write(int x, int y, u8 r, u8 g, u8 b)
{
  if (y > NLEDS - 1 || x > NLEDS - 1 || x < 0 || y < 0)
  {
    return -1;
  }
  screen[y][x].r = r * color_scale;
  screen[y][x].g = g * color_scale;
  screen[y][x].b = b * color_scale;
  return 0;
}

u8 v = 0;
void display::draw()
{
  for (uint8_t curStrip = 0; curStrip < 24; curStrip++)
  {
    /*screen[4][0].r = v;
    screen[20][0].r = v;
    v += 10;*/
    int i = 0;
    if (curStrip < 8)
    {
      i = 0;
    }
    else if (curStrip < 16)
    {
      i = 1;
    }
    else
    {
      i = 2;
    }
    bool changed = false;
    for (int j = 0; j < NLEDS; j++)
    {
      if (screen[curStrip][j].r != realScreen[curStrip][j].r ||
          screen[curStrip][j].g != realScreen[curStrip][j].g ||
          screen[curStrip][j].b != realScreen[curStrip][j].b)
      {
        changed = true;
        for (j = 0; j < NLEDS; j++)
        {
          realScreen[curStrip][j].r = screen[curStrip][j].r;
          realScreen[curStrip][j].g = screen[curStrip][j].g;
          realScreen[curStrip][j].b = screen[curStrip][j].b;
          seg[i].set_crgb_at(j, screen[curStrip][j]);
        }
        j = NLEDS + 1;
      }
    }
    if (changed == true)
    {
      decode(curStrip);
      seg[i].sync();
    }
  }
}

void display::clear()
{
  for (int i = 0; i < NLEDS; i++)
  {
    for (int j = 0; j < NLEDS; j++)
    {
      write(j, i, 0, 0, 0);
    }
  }
}

void display::hardClear()
{
  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 24; j++)
    {
      screen[i][j].r = 1;
      screen[i][j].g = 0;
      screen[i][j].b = 0;

      realScreen[i][j].r = 0;
      realScreen[i][j].g = 0;
      realScreen[i][j].b = 0;
    }
  }
  draw();
  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 24; j++)
    {
      screen[i][j].r = 0;
      screen[i][j].g = 0;
      screen[i][j].b = 0;
    }
  }
  draw();
}