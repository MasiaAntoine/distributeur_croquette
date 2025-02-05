#include "HX711.h"

HX711 scale(5, 4);

// Pins du moteur sur le L298N
const int in1 = 27;
const int in2 = 26;

float calibration_factor = -200;
float g;

void setup() {
  Serial.begin(115200);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  scale.set_scale();
  scale.tare();
}

void rotateMotor(int angle) {
  int duration = (angle * 166) / 20; // Approximation basée sur 200 RPM
  if (angle > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  delay(abs(duration)); // Temps d'activation proportionnel à l'angle
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void loop() {
  scale.set_scale(calibration_factor);
  g = scale.get_units(10);

  Serial.print("Poids: ");
  Serial.print(g);
  Serial.println(" grams");
  delay(10);

  // rotateMotor(15);
  // delay(500);
  // rotateMotor(-15);
  // delay(500);
}
