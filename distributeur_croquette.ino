#include "HX711.h"

HX711 scale(5, 4);

// Pins du moteur sur le L298N
const int in1 = 27;
const int in2 = 26;

float calibration_factor = -209;
float g;

// Variables de configuration
float grammageCible = 30.0;  // Quantité de croquettes désirée en grammes
float facteurCorrection = 0.30;  // Facteur de correction du poids

// Variables pour surveiller la vitesse de distribution
float dernierPoids = 0.0;
float poidsAvantDernier = 0.0;
unsigned long dernierTemps = 0;
float vitesseDist = 0.0; // grammes par seconde
float acceleration = 0.0; // accélération de distribution

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
  delay(abs(duration));
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

  // Calcul de la vitesse de distribution et accélération
  unsigned long tempsActuel = millis();
  if (tempsActuel - dernierTemps > 250) { // Mesure plus fréquente (toutes les 250ms)
    float deltaTemps = (tempsActuel - dernierTemps) / 1000.0; // en secondes
    float ancienneVitesse = vitesseDist;
    vitesseDist = (g - dernierPoids) / deltaTemps; // g/s
    acceleration = (vitesseDist - ancienneVitesse) / deltaTemps; // g/s²

    poidsAvantDernier = dernierPoids;
    dernierPoids = g;
    dernierTemps = tempsActuel;

    // Afficher la vitesse uniquement pendant la distribution
    if (distributionEnCours && trappeOuverte) {
      Serial.print("Vitesse: ");
      Serial.print(vitesseDist);
      Serial.println(" g/s");
    }
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

    // Calculer le seuil de fermeture avec prédiction avancée
    float seuilBase = grammageCible * facteurCorrection;

    // Prédiction du dépassement basée sur la vitesse et l'accélération
    float tempsEstimeFermeture = 0.8f; // temps estimé pour fermer la trappe (secondes)
    float depassementPredit = vitesseDist * tempsEstimeFermeture + 0.5f * acceleration * tempsEstimeFermeture * tempsEstimeFermeture;

    // Ajuster dynamiquement le seuil selon la vitesse de distribution et le dépassement prédit
    float seuilFermeture = seuilBase;
    if (vitesseDist > 0) {
      // Plus la vitesse est élevée, plus on ferme tôt
      // Correction: utiliser des float explicites pour éviter les erreurs de type
      float valMin = 1.0f;
      float difference = grammageCible - depassementPredit;
      seuilFermeture = (difference > valMin) ? difference : valMin;

      // Ne jamais dépasser 20% du grammage cible comme seuil max
      float seuilMax = grammageCible * 0.2f;
      seuilFermeture = (seuilFermeture < seuilMax) ? seuilFermeture : seuilMax;
    }

    Serial.print("Seuil: ");
    Serial.print(seuilFermeture);
    Serial.println("g");

    // Vérifier si on a atteint le seuil de fermeture
    if (g >= seuilFermeture) {
      distributionEnCours = false;
      Serial.print("Fermeture à: ");
      Serial.print(g);
      Serial.println("g (seuil de fermeture atteint)");

      // Fermer le moteur
      rotateMotor(-14);

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
