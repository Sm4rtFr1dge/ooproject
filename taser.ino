#include <Servo.h>

Servo p1;
Servo p2;

int p1Pin = 6;
int p2Pin = 7;
int taserPin = 5; // Choose the pin you want to control
char in;

void setup() {
  Serial.begin(9600);
  pinMode(taserPin, OUTPUT);
  p1.attach(p1Pin);
  p2.attach(p2Pin);
  p1.write(90);
  p2.write(90);
}

void loop() {
  if (Serial.available() > 0) {
    in = Serial.read();

    if (in == '1') {
      digitalWrite(taserPin, HIGH);
      delay(500);
      digitalWrite(taserPin, LOW);
      p1.write(180);
      delay(50);
      p1.write(90);
    }
    else if (in == '2'){
      digitalWrite(taserPin, HIGH);
      delay(500);
      digitalWrite(taserPin, LOW);
      p2.write(180);
      delay(50);
      p2.write(90);
    }
  }
}