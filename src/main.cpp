#include <Arduino.h>
#include <EncButton.h>
#include <GyverStepper.h>
#include "TableController.h"

#define speed 115200

Button btnUSER(BTN, INPUT_PULLUP);
GStepper<STEPPER2WIRE> stepper(1600, PUL, DIR, ENA); // драйвер step-dir + пин enable

String fw = "0.7";

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
  bool HS1 = 0;            // State Hall Sensor 1
  bool HS2 = 0;            // State Hall Sensor 2
  bool Block = false;      // Block User button
  uint8_t TableNS = 0;     // New State
  uint8_t TableOS = CLOSE; // Old State
} FState;
// struct FLAG FState;

uint8_t state = Act_OFF;
uint8_t count = 0;

void SetStateRelay(uint8_t st);
void SetStateDrive(uint8_t state);
void ButtonHandler();
void TableConroller();
void task_1000();

void setup()
{
  Serial.begin(speed);

  Serial.println("TableConroller");
  Serial.printf("firmware: %s", fw);
  Serial.println();

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

  SetStateDrive(disable);
}

void loop()
{
  ButtonHandler();
  TableConroller();
  task_1000();
}

void ButtonHandler()
{
  btnUSER.tick();

  while (btnUSER.busy())
  {
    btnUSER.tick();
    // Block - protect by random clicks in works
    if (btnUSER.hasClicks(1) && (FState.TableNS == NONE) && FState.Block == false)
    {
      // Change State Old and New State
      if (digitalRead(SEN2))
      {
        FState.TableNS = CLOSE;
        FState.TableOS = CLOSE;
        Serial.println("New State: CLOSE");
      }
      else if (digitalRead(SEN1))
      {
        FState.TableNS = OPEN;
        FState.TableOS = OPEN;
        Serial.println("New State: OPEN");
      }
    }

    if (btnUSER.hasClicks(3))
    {
      Serial.println("ACT Down");
      SetStateRelay(Act2_DWN);
      SetStateRelay(Act1_DWN);
      delay(TimM1);
      SetStateRelay(Act_OFF);
    }
  }
}

void TableConroller()
{
  uint32_t now;
  // Opening
  if (FState.TableNS == OPEN && digitalRead(SEN1))
  {
    FState.Block = true;
    Serial.println("Table Opening Starting..");
    // First delay waiting (3s) item 2
    delay(FD);
    Serial.println("Driver 1-3 UP");
    // Three drivers moving Forward (item 3)
    SetStateRelay(Act2_UP);
    delay(TimM1);
    SetStateRelay(Act_OFF);

    // If Hall sensor HS1 in HOME position, Start Servo (item 4)
    if (digitalRead(SEN1))
    {
      FState.HS1 = 1;
      SetStateDrive(forward);
      Serial.println("S1: ON");
    }

    // while Hall Sensor is not HOME position
    while (FState.HS1)
    {
      stepper.tick();

      if (digitalRead(SEN2))
      {
        Serial.println("S2: ON");
        FState.HS1 = 0;
        FState.HS2 = 1;
        SetStateDrive(disable);
      }
    }
    // Started, if 2 Hall sensor in Home Position
    if (FState.HS2)
    {
      SetStateDrive(forward);

      delay(PD);
      now = millis();
      // Moved slightfly to forward (item 5)
      while (millis() - now < 3000)
      {
        stepper.tick();
      }
      SetStateDrive(disable);

      // Moved Four Driver (Item 4)
      delay(PD);
      Serial.println("Driver 4 UP");
      SetStateRelay(Act1_UP);
      delay(TimM2);
      SetStateRelay(Act_OFF);

      SetStateDrive(back);

      // while Hall Sensor is not HOME position (item 7)
      while (!digitalRead(SEN2))
      {
        stepper.tick();
      }
      FState.HS2 = 0;
      SetStateDrive(disable);
    }

    FState.TableNS = NONE;
    Serial.println("Driver OFF");
    Serial.println("Table Opening Compleated.");
  }
  // Closing
  else if (FState.TableNS == CLOSE && digitalRead(SEN2)) 
  {
    FState.Block = true; // Blocked User Btn
    Serial.println("Table Closing Starting");
    // Item 2. First delay waiting (3s)
    delay(FD);
    // If Hall sensor HS2 in HOME position, Start Servo
    if (digitalRead(SEN2))
    {
      Serial.println("S2: ON");
      FState.HS2 = 1;
      SetStateDrive(forward);
    }
    if (FState.HS2)
    {
      now = millis();
      // Item 3. The motor Nema 23 moves slightly forward
      while (millis() - now < 3000)
      {
        stepper.tick();
      }
      FState.HS2 = 0;
      SetStateDrive(disable);
    }

    delay(PD);

    // Item 4. Moved Four Driver.
    Serial.println("Driver 4 Down");
    SetStateRelay(Act1_DWN);
    delay(TimM2);
    SetStateRelay(Act_OFF);
    // Item 5. The driver Nema 23 moves slightly backward
    SetStateDrive(back);
    while (!digitalRead(SEN2))
    {
      stepper.tick();
    }
    // Item 6. Driver Nema 23 moves slightly backward to HAll sensor 1
    while (!digitalRead(SEN1))
    {
      stepper.tick();
    }
    SetStateDrive(disable);
    delay(PD);
    // Item 7. Three Drivers run
    Serial.println("Driver 1-3 DWN");
    // Three drivers moving Forward (item 3)
    SetStateRelay(Act2_DWN);
    delay(TimM1);
    SetStateRelay(Act_OFF);

    FState.TableNS = NONE;
    Serial.println("Driver OFF");
    Serial.println("Table Closing Compleated.");
  }
  // Disable
  else
  {
    FState.TableNS = NONE;
    FState.Block = false;
    SetStateDrive(disable);
    SetStateRelay(Act_OFF);
  }
}

void SetStateDrive(uint8_t state)
{
  switch (state)
  {
  case disable:
    stepper.disable();
    stepper.stop();
    stepper.reset();
    break;
  case enable:
    /* code */
    break;
  case back:
    stepper.setAcceleration(3000);
    stepper.setSpeedDeg(-800);
    stepper.enable();
    break;
  case forward:
    stepper.setAcceleration(3000);
    stepper.setSpeedDeg(800);
    stepper.enable();
    break;
  default:
    break;
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