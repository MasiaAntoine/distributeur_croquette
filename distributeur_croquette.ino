#include "HX711.h"

HX711 scale(5, 4);

// Pins du moteur sur le L298N
const int in1 = 27;
const int in2 = 26;

float calibration_factor = -209;
float g;

// Variables de configuration
float grammageCible = 30.0;  // Quantité de croquettes désirée en grammes
float facteurCorrection = 0.3;  // Facteur de correction réduit à 30% pour fermer bien plus tôt

// Variables pour surveiller la vitesse de distribution
float dernierPoids = 0.0;
unsigned long dernierTemps = 0;
float vitesseDist = 0.0; // grammes par seconde

// Variable pour contrôler la distribution des croquettes
boolean distributionEnCours = true;
boolean trappeOuverte = false;
float poidsFinal = 0.0;

void setup() {
  Serial.begin(115200);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  scale.set_scale();
  scale.tare();
  
  Serial.println("Distributeur de croquettes prêt");
  Serial.println("Commandes: r=Redémarrage, gXX=Changer grammage cible");
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
    String commande = Serial.readStringUntil('\n');
    
    if (commande.equals("r")) {
      distributionEnCours = true;
      trappeOuverte = false;
      Serial.println("Réinitialisation de la distribution");
      scale.tare();
    }
    else if (commande.startsWith("g")) {
      float nouveauGrammage = commande.substring(1).toFloat();
      if (nouveauGrammage > 0) {
        grammageCible = nouveauGrammage;
        Serial.print("Nouveau grammage cible: ");
        Serial.print(grammageCible);
        Serial.println("g");
      }
    }
  }

  scale.set_scale(calibration_factor);
  g = scale.get_units(10);

  Serial.print("Poids: ");
  Serial.print(g);
  Serial.println(" grams");
  
  // Calcul de la vitesse de distribution
  unsigned long tempsActuel = millis();
  if (tempsActuel - dernierTemps > 500) { // Toutes les 500ms
    vitesseDist = (g - dernierPoids) * 1000 / (tempsActuel - dernierTemps); // g/s
    dernierPoids = g;
    dernierTemps = tempsActuel;
  }

  if (distributionEnCours) {
    // Ouvrir la trappe une seule fois au début de la distribution
    if (!trappeOuverte) {
      rotateMotor(15);
      trappeOuverte = true;
      Serial.println("Trappe ouverte");
      dernierTemps = millis();
      dernierPoids = 0;
    }

    // Calculer le seuil de fermeture avec facteur variable
    float seuilFermeture = grammageCible * facteurCorrection;
    
    // Fermer plus tôt si la vitesse de distribution est élevée
    if (vitesseDist > 10) { // Si plus de 10g/s
      seuilFermeture *= 0.8; // Réduire encore de 20%
    }

    // Vérifier si on a atteint le seuil de fermeture
    if (g >= seuilFermeture) {
      distributionEnCours = false;
      Serial.print("Fermeture à: ");
      Serial.print(g);
      Serial.println("g (seuil de fermeture atteint)");
      
      // Fermer le moteur
      rotateMotor(-15);
      
      // Attendre un moment pour que les croquettes finissent de tomber
      delay(1000);
      
      // Mesurer le poids final
      poidsFinal = scale.get_units(10);
      
      Serial.println("Distribution terminée");
      Serial.print("Poids final visé: ");
      Serial.print(grammageCible);
      Serial.print("g - Poids obtenu: ");
      Serial.print(poidsFinal);
      Serial.print("g - Différence: ");
      Serial.print(poidsFinal - grammageCible);
      Serial.println("g");
    }
  }
  
  delay(10);
}
