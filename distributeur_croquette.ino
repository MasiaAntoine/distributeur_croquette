#include "HX711.h"

HX711 scale(5, 4);

// Pins du moteur sur le L298N
const int in1 = 27;
const int in2 = 26;

float calibration_factor = -209;
float g;

// Variables de configuration
float grammageCible = 30.0;  // Quantité de croquettes désirée en grammes
float facteurCorrection = 0.7;  // Facteur de correction (70% du poids cible pour la fermeture anticipée)

// Variable pour contrôler la distribution des croquettes
boolean distributionEnCours = true;
boolean trappeOuverte = false; // Nouvelle variable pour suivre l'état de la trappe

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
  // Vérifier si des données sont disponibles sur le port série
  if (Serial.available() > 0) {
    char commande = Serial.read();
    if (commande == 'r') {
      distributionEnCours = true;
      trappeOuverte = false;
      Serial.println("Réinitialisation de la distribution");
    }
  }

  scale.set_scale(calibration_factor);
  g = scale.get_units(10);

  Serial.print("Poids: ");
  Serial.print(g);
  Serial.println(" grams");
  delay(10);

  if (distributionEnCours) {
    // Ouvrir la trappe une seule fois au début de la distribution
    if (!trappeOuverte) {
      rotateMotor(15);
      trappeOuverte = true;
      Serial.println("Trappe ouverte");
    }

    // Calculer le seuil de fermeture avec le facteur de correction
    float seuilFermeture = grammageCible * facteurCorrection;

    // Vérifier si on a atteint le seuil de fermeture
    if (g >= seuilFermeture) {
      distributionEnCours = false;
      // Fermer le moteur
      rotateMotor(-15);
      Serial.println("Distribution terminée");
      Serial.print("Poids final visé: ");
      Serial.print(grammageCible);
      Serial.print("g - Seuil de fermeture: ");
      Serial.print(seuilFermeture);
      Serial.println("g");
    }
  }
}
