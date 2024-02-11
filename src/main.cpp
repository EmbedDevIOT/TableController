#include <Arduino.h>
#include <GyverButton.h>
#include <GyverStepper.h>

#define speed 115200

#define ENA 12
#define DIR 14
#define PUL 27

#define RL1 18
#define RL2 5
#define RL3 17
#define RL4 16

#define BTN 34
#define SEN1 32
#define SEN2 35

GButton button(BTN);

char fw = {0.1};

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

  pinMode(SEN1, INPUT);
  pinMode(SEN2, INPUT);

}

void loop()
{
}
