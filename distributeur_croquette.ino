// Pins du moteur sur le L298N
const int in1 = 27;
const int in2 = 26;

void setup() {
  Serial.begin(115200);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
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
  rotateMotor(15);
  delay(1500);
  rotateMotor(-15);
  delay(1500);
}
