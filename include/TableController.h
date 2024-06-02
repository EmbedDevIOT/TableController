// #ifndef TableConroller_h
// #define TableConroller_h
#pragma once 

#include <Arduino.h>


#define FD 1000
#define PD 2000
#define TimM1 10000
#define TimM2 7000

enum PWR 
{
  OFF = 0,
  ON
};

enum Drive
{
  disable,
  enable,
  forward,
  forw_5RPM,
  back_8RPM,
  back
};

enum state
{
  NONE,
  OPEN,
  CLOSE,
  HOME
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

// class Table{
//     public:
//     Table(byte);
//     SetState(uint8_t state);
//     private:
//     SetRotation();
//     SetSpeed();
//     uint8_t speed = 800;
//     

// }

// #endif 