#include <Arduino.h>
#include <EncButton.h>
#include <GyverStepper.h>

#define speed 115200

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

Button btnUSER(BTN, INPUT_PULLUP);
GStepper<STEPPER2WIRE> stepper(1600, PUL, DIR, ENA); // драйвер step-dir + пин enable

String fw = "0.4";

uint8_t state = 0b00000000;
uint8_t count = 0;

enum state
{
  NONE,
  OPEN,
  CLOSE
};

enum ActState
{
  Act_OFF = 0b00000000,
  Act1_UP = 0b00000001,
  Act1_DWN = 0b00000010,
  Act2_UP = 0b00000100,
  Act2_DWN = 0b00001000
};

struct FLAG
{
  bool ACT1 = 0;
  bool ACT2 = 0;
  bool HS1 = 0;
  bool HS2 = 0;
  uint8_t Table = 0;
};

struct FLAG FState;

void SetStateRelay(uint8_t st);
void ButtonHandler();
void TableConroller();
void task_1000();

void setup()
{
  Serial.begin(speed);
  Serial.println("TableConroller");

  pinMode(RL1, OUTPUT);
  digitalWrite(RL1, HIGH);
  pinMode(RL2, OUTPUT);
  digitalWrite(RL2, HIGH);
  pinMode(RL3, OUTPUT);
  digitalWrite(RL3, HIGH);
  pinMode(RL4, OUTPUT);
  digitalWrite(RL4, HIGH);

  pinMode(SEN1, INPUT_PULLUP);
  pinMode(SEN2, INPUT_PULLUP);

  stepper.setRunMode(KEEP_SPEED);

  stepper.disable();
  stepper.stop();
  stepper.reset();

  // stepper.setTarget(360);
}

void loop()
{
  if (digitalRead(SEN1))
  {
    FState.HS1 = 1;
    stepper.setAcceleration(300);
    stepper.setSpeedDeg(600);
    stepper.enable();
    // stepper.setMaxSpeed(800);

    Serial.println("S1: ON");
  }

  while (FState.HS1)
  {
    stepper.tick();

    if (digitalRead(SEN2))
    {
      Serial.println("S2: ON");
      FState.HS1 = 0;
      stepper.disable();
      stepper.stop();
      stepper.reset();
    }
  }

  ButtonHandler();
  TableConroller();
  task_1000();
}

void ButtonHandler()
{
  static uint8_t step_count = 0;
  uint32_t now;

  btnUSER.tick();

  while (btnUSER.busy())
  {
    btnUSER.tick();

    if (btnUSER.hasClicks(1))
    {
      if (FState.Table == NONE || FState.Table == CLOSE)
      {
        FState.Table = OPEN;
      }

      // stepper.setTargetDeg(360);
      // stepper.enable();
      // if (state != 0b00001000)
      // {
      //   state = 0b00001000;
      // }
      // else
      //   state = 0b00000100;

      // now = millis();
      // while (millis() - now < 10000)
      // {
      // }

      // bitClear(state, 0);
      // bitClear(state, 1);
    }

    if (btnUSER.hasClicks(2))
    {
      if (FState.Table == NONE || FState.Table == OPEN)
      {
        FState.Table = CLOSE;
      }
      // Serial.println("RL 1 ON RL 2 OFF");
      // bitSet(state, 0);
      // bitClear(state, 1);
    }

    if (btnUSER.hasClicks(3))
    {
    }
  }
}

void task_1000()
{
  static uint32_t timer_1000 = 0;

  char msg[30];

  if (millis() - timer_1000 > 1000)
  {
    timer_1000 = millis();
    sprintf(msg, "T: 1000");
    Serial.println(msg);
  }
}

void TableConroller()
{
  char msg[30];

  if (FState.Table == OPEN)
  {
    Serial.println("Table Opening");
    delay(3000);
    Serial.println("Driver 1 UP");
    SetStateRelay(Act2_UP);
    delay(10000);

    if (digitalRead(SEN1))
    {
      sprintf(msg, "SENSOR_ 1 Activate");
      Serial.println(msg);

      Serial.println("Motor Enable");
      stepper.setTarget(360);
      stepper.enable();
    }

    if (digitalRead(SEN2))
    {
      sprintf(msg, "SENSOR_ 2 Activate");
      Serial.println(msg);
    }
    Serial.println("Driver 2 UP");
    SetStateRelay(Act1_UP);
    delay(10000);

    FState.Table = NONE;
    Serial.println("Driver OFF");
  }
  else if (FState.Table == CLOSE)
  {
    Serial.println("Table Closing");
    delay(3000);
    Serial.println("Driver 1 DWN");
    SetStateRelay(Act2_DWN);
    delay(10000);

    if (digitalRead(SEN1))
    {
      sprintf(msg, "SENSOR_ 1 Activate");
      Serial.println(msg);

      Serial.println("Motor Enable");
      stepper.setTarget(360);
      stepper.enable();
    }

    if (digitalRead(SEN2))
    {
      sprintf(msg, "SENSOR_ 2 Activate");
      Serial.println(msg);
    }
    Serial.println("Driver 2 DWN");
    SetStateRelay(Act1_DWN);
    delay(10000);

    FState.Table = NONE;
    Serial.println("Driver OFF");
  }
  else
  {
    FState.Table = NONE;

    stepper.disable();
    stepper.stop();
    stepper.reset();

    SetStateRelay(Act_OFF);
  }
}

void SetStateRelay(uint8_t st)
{
  switch (st)
  {
  case 0b00000000:
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, HIGH);
    digitalWrite(RL3, HIGH);
    digitalWrite(RL4, HIGH);
    break;
  case 0b00000001:
    digitalWrite(RL1, LOW);
    digitalWrite(RL2, HIGH);
    break;
  case 0b00000010:
    digitalWrite(RL1, HIGH);
    digitalWrite(RL2, LOW);
    break;
  case 0b00000100:
    stepper.enable();
    digitalWrite(RL3, LOW);
    digitalWrite(RL4, HIGH);
    break;
  case 0b00001000:
    digitalWrite(RL3, HIGH);
    digitalWrite(RL4, LOW);
  default:
    break;
  }
}