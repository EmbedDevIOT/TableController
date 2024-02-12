// #ifndef TableConroller_h
// #define TableConroller_h
#pragma once 

#include <Arduino.h>

#define FD 3000
#define PD 2000
#define TimM1 10000
#define TimM2 7000

enum PWR 
{
  OFF = 0,
  ON
};

enum state
{
  NONE,
  OPEN,
  CLOSE
};

#define ENA 12
#define DIR 14
#define PUL 27

#define RL1 18
#define RL2 5
#define RL3 17
#define RL4 16

#define BTN 33
#define SEN1 32
#define SEN2 35

class Table{
    public:
    Table(byte);
    SetState(uint8_t state);
    private:

}

// #endif 