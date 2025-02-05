# 🐾 Distributeur de Croquettes

Ce projet consiste à fabriquer un distributeur de croquettes en système embarqué.

## Composants Utilisés

- **ESP32** : Utilisé pour sa connectivité WiFi, ce qui permettra de se connecter à une future API pour régler les horaires et le dosage des croquettes.
- **Module HX711** : Utilisé avec un capteur de poids de 5 kg pour obtenir le poids en grammes.
- **Module moteur L298N** : Utilisé pour contrôler le moteur.
- **Moteur TT DC 3-6V avec Boîte de Vitesses** : Moteur avec un ratio de 1:48 et 200 RPM, utilisé pour ouvrir et fermer la trappe du réservoir de croquettes.
- **Adaptateur d'alimentation universel, 12V, 1A** : Adaptateur d'alimentation dénudé et soudé dans le système pour fournir l'énergie nécessaire.

## ⚙️ Fonctionnement

Le système utilise un ESP32 pour contrôler un moteur via le module L298N. Le moteur ouvre et ferme la trappe du réservoir de croquettes. Le poids des croquettes est mesuré à l'aide du module HX711 et d'un capteur de poids de 5 kg. Les données de poids sont envoyées à l'ESP32, qui les affiche sur le moniteur série.

## 🌐 Avantages de l'ESP32

L'ESP32 a été choisi pour ce projet en raison de sa connectivité WiFi, qui permettra de se connecter à une future API. Cette API permettra de régler les horaires et le dosage des croquettes, offrant ainsi une solution flexible et connectée pour nourrir les animaux de compagnie.

## 🔌 Schéma de Connexion

- **ESP32** :
  - GPIO 5 : DT du module HX711
  - GPIO 4 : SCK du module HX711
  - GPIO 27 : IN1 du module L298N
  - GPIO 26 : IN2 du module L298N

- **Module HX711** :
  - DT : GPIO 5 de l'ESP32
  - SCK : GPIO 4 de l'ESP32

- **Module L298N** :
  - IN1 : GPIO 27 de l'ESP32
  - IN2 : GPIO 26 de l'ESP32

- **Moteur TT DC** :
  - Connecté aux sorties du module L298N

- **Adaptateur d'alimentation** :
  - 12V, 1A, dénudé et soudé pour alimenter le système

## 💻 Code

Le code source du projet se trouve dans le fichier `distributeur_croquette.ino`.
