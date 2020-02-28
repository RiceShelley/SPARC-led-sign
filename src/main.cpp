#include <Arduino.h>
#include <display.hpp>

//14 15 16 17
int poll_time = 1;

typedef struct vec
{
  float x;
  float y;
  float z;
} vec;

typedef struct rot
{
  float x;
  float y;
  float z;
} rot;

vec cam = {
    0,
    0,
    0};

rot camr = {
    0,
    0,
    0};

int fnum = 24;

display *sc;
void setup()
{
  sc = new display();
  sc->hardClear();
  cam.z = 5.5;
  pinMode(14, INPUT);
  pinMode(15, INPUT);
  pinMode(16, INPUT);
  pinMode(17, INPUT);
}

vec translate(vec v)
{
  vec rtn;
  float x = v.x + cam.x;
  float y = v.y + cam.y;
  float z = v.z + cam.z;

  float nz = z * cos(camr.y) + x * sin(camr.y);
  float nx = x * cos(camr.y) - z * sin(camr.y);
  z = nz;
  x = nx;

  nz = z * cos(camr.x) + y * sin(camr.x);
  float ny = y * cos(camr.x) - z * sin(camr.x);
  z = nz;
  y = ny;

  double f = fnum / z;
  rtn.x = (x * f) + 12;
  rtn.y = (y * f) + 12;
  return rtn;
}

void writep(int x, int y, int dx, int dy, int x1, int x2, int y1, int y2, u8 r, u8 g, u8 b)
{
  sc->write((x2 > x1) ? x : (abs(x - 24) - (2 * (12 - x1))), (y2 > y1) ? y : (abs(y - 24) - (2 * (12 - y1))), r, g, b);
}

void line(int x1, int y1, int x2, int y2, u8 r, u8 g, u8 b)
{

  if (x1 == x2)
  {
    int x = x1;
    if (y2 > y1)
    {
      for (int y = y1; y <= y2; y++)
      {
        sc->write(x, y, r, g, b);
      }
    }
    else
    {
      for (int y = y2; y <= y1; y++)
      {
        sc->write(x, y, r, g, b);
      }
    }
    return;
  }
  else if (y1 == y2)
  {
    int y = y1;
    if (x2 > x1)
    {
      for (int x = x1; x <= x2; x++)
      {
        sc->write(x, y, r, g, b);
      }
    }
    else
    {
      for (int x = x2; x <= x1; x++)
      {
        sc->write(x, y, r, g, b);
      }
    }
    return;
  }

  //calculating range for line between start and end point
  int dx = x2 - x1;
  int dy = y2 - y1;

  int x = x1;
  int y = y1;

  sc->write(x, y, r, g, b);
  //this is the case when slope(m) < 1
  if (abs(dx) > abs(dy))
  {
    writep(x, y, dx, dy, x1, x2, y1, y2, r, g, b);
    int pk = (2 * abs(dy)) - abs(dx);

    for (int i = 0; i < abs(dx); i++)
    {
      x = x + 1;
      if (pk < 0)
        pk = pk + (2 * abs(dy));
      else
      {
        y = y + 1;
        pk = pk + (2 * abs(dy)) - (2 * abs(dx));
      }
      writep(x, y, dx, dy, x1, x2, y1, y2, r, g, b);
    }
    writep(x, y, dx, dy, x1, x2, y1, y2, r, g, b);
  }
  else
  {
    //this is the case when slope is greater than or equal to 1  i.e: m>=1
    writep(x, y, dx, dy, x1, x2, y1, y2, r, g, b);
    int pk = (2 * abs(dx)) - abs(dy);

    for (int i = 0; i < abs(dy); i++)
    {
      y = y + 1;
      if (pk < 0)
        pk = pk + (2 * abs(dx));
      else
      {
        x = x + 1;
        pk = pk + (2 * abs(dx)) - (2 * abs(dy));
      }
      writep(x, y, dx, dy, x1, x2, y1, y2, r, g, b);
    }
    writep(x, y, dx, dy, x1, x2, y1, y2, r, g, b);
  }
}

vec cube[] = {
    {1, 1, 1},
    {-1, 1, 1},
    {1, -1, 1},
    {-1, -1, 1},
    {1, 1, -1},
    {-1, 1, -1},
    {1, -1, -1},
    {-1, -1, -1}};

vec edges[] = {
    {0, 1, 0},
    {0, 2, 0},
    {2, 3, 0},
    {1, 3, 0},

    {4, 5, 0},
    {4, 6, 0},
    {6, 7, 0},
    {5, 7, 0},

    {0, 4, 0},
    {1, 5, 0},
    {2, 6, 0},
    {3, 7, 0}

};

vec rot_x(float ang, vec v)
{
  vec r;
  r.x = v.x;
  r.y = v.y * cos(ang) + v.z * sin(ang);
  r.z = v.y * sin(ang) * -1 + v.z * cos(ang);
  return r;
}

vec rot_y(float ang, vec v)
{
  vec r;
  r.x = v.x * cos(ang) - v.z * sin(ang);
  r.y = v.y;
  r.z = v.x * sin(ang) + v.z * cos(ang);
  return r;
}

vec rot_z(float ang, vec v)
{
  vec r;
  r.x = v.x * cos(ang) + v.y * sin(ang);
  r.y = v.x * sin(ang) * -1 + v.y * cos(ang);
  r.z = v.z;
  return r;
}

void r_cube() {
  poll_time = 1;
  sc->clear();
  for (int i = 0; i < 12; i++)
  {
    vec p1 = translate(cube[(int)edges[i].x]);
    vec p2 = translate(cube[(int)edges[i].y]);
    line(p1.x, p1.y, p2.x, p2.y, 0, 50, 0);
  }
  for (int i = 0; i < 8; i++)
  {
    //cube[i] = rot_y(.001, cube[i]);
    //cube[i] = rot_y(.001, cube[i]);
    vec v = translate(cube[i]);
    sc->write(v.x, v.y, 50, 50, 0);
    cube[i] = rot_z(.04, cube[i]);
    cube[i] = rot_y(.02, cube[i]);
  }
}

float sec = 0;
int c = 11;
float radi = 8;

void clock() {
  poll_time = 1;
  sec -= .5;
  sc->clear();
  line(c, c, c - (int)(cos(sec / 360.0) * 5), c - (int)(sin(sec / 360.0) * 5), 0, 50, 0);
  line(c, c, c - (int)(cos(sec / 60.0) * 10), c - (int)(sin(sec / 60.0) * 10), 50, 0, 0);
  line(c, c, c - (int)(cos(sec) * 12), c - (int)(sin(sec) * 12), 0, 0, 50);

  for (float i = 0; i < 2 * PI; i+=.05) {
    sc->write(c - (int)(cos(i) * 12), c - (int)(sin(i) * 12), (((int)(i * 100.0)) % 4) ? 50 : 0, (((int)(i * 100.0)) % 3) ? 50 : 0, (((int)(i * 100.0)) % 2) ? 50 : 0);
  }
}

enum dir {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

dir sdir = LEFT;
int sx = 11;
int sy = 11;

int snake_len = 1;

typedef struct snb {
  u8 x;
  u8 y;
  u8 r;
  u8 b;
  u8 g;
} snb;

snb sn[200];

int fx[] = {10, 6, 14, 18};
int fy[] = {15, 10, 8, 2};

void snake() {
  poll_time = 300;
  sc->clear();

  int old_sx = sx;
  int old_sy = sy;

  switch(sdir) {
    case UP:
      sx--;
      break;
    case DOWN:
      sy--;
      break;
    case LEFT:
      sy++;
      break;
    case RIGHT:
      sx++;
      break;
  }

  // make sure snake does not hit self
  for (int i = 0; i < snake_len; i++) {
    if (sx == sn[i].x && sy == sn[i].y) {
      sx = 11;
      sy = 11;
      snake_len = 1;
      sn[0].x = 11;
      sn[0].y = 11;
      poll_time = 300;
    }
  }

  if (sc->write(sx, sy, 0, 50, 50) != 0) {
    sx = 11;
    sy = 11;
    snake_len = 1;
    sn[0].x = 11;
    sn[0].y = 11;
    poll_time = 300;
  } else if (sx != old_sx || sy != old_sy) {
    for (int i = 0; i < 4; i++) {
      if (fx[i] == sx && fy[i] == sy) {
        fx[i] = random(1, 23);
        fy[i] = random(1, 23);
        snake_len++;
        poll_time -= 10;
      }
    }
  }
  if (poll_time < 20) {
    poll_time = 20;
  }

  for(int i = snake_len - 1; i > 0; i--) {
    sn[i].x = sn[i - 1].x;
    sn[i].y = sn[i - 1].y;
    sn[i].r = sn[i - 1].r;
    sn[i].g = sn[i - 1].g;
    sn[i].b = sn[i - 1].b;
  }
  sn[0].x = sx;
  sn[0].y = sy;
  sn[0].r = random(0, 30);
  sn[0].g = random(40, 70);
  sn[0].b = random(0, 5);

  for (int i = 0; i < 4; i++) {
    sc->write(fx[i], fy[i], 50, 0, 50);
  }
  sc->write(sn[0].x, sn[0].y, 50, 30, 0);
  for (int i = 1; i < snake_len; i++) {
    sc->write(sn[i].x, sn[i].y, sn[i].r, sn[i].g, sn[i].b);
  }

}

enum mode {
  CUBE,
  CLOCK,
  SNAKE
};

void (*fn_mode[]) (void) = {r_cube, clock, snake};

mode curMode = CLOCK;

void loop()
{

  fn_mode[curMode]();

  sc->draw();
  for (int i = 0; i < poll_time; i++) {
    if (digitalRead(14) == HIGH) {
      sdir = UP;
      sc->write(0, 0, 50, 0, 0);
    } else if (digitalRead(15) == HIGH) {
      sdir = DOWN;
      sc->write(0, 0, 50, 0, 0);
      sc->write(1, 0, 50, 0, 0);
    } else if (digitalRead(16) == HIGH) {
      sdir = LEFT;
      sc->write(0, 0, 50, 0, 0);
      sc->write(2, 0, 50, 0, 0);
    } else if (digitalRead(17) == HIGH) {
      sdir = RIGHT;
      sc->write(0, 0, 50, 0, 0);
      sc->write(3, 0, 50, 0, 0);
    }
    if (digitalRead(14) == HIGH && digitalRead(15) == HIGH && digitalRead(16) == HIGH && digitalRead(17 == HIGH)) {
      curMode = (mode) (((int) curMode) + 1);
      if (curMode > 2) {
        curMode = (mode) 0;
      }
      delay(1000);
    }
    delay(1);
  }
}