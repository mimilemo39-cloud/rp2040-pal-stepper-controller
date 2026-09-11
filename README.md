# RP2040-Zero PAL Stepper Motor Controller

Contrôleur de moteur pas à pas synchronisé avec signal PAL 50Hz pour Waveshare RP2040-Zero avec Arduino IDE (Earlephilhower).

## Fonctionnalités

✅ **Synchronisation signal PAL**
- Capture du signal de synchronisation 50Hz d'une caméra PAL
- Multiplication par facteur 200 (10000 Hz)
- Alignement de phase absolue

✅ **Contrôle du moteur**
- Rampe d'accélération progressive (max 2 secondes)
- Rampe de décélération progressive
- Arrêt rapide d'urgence (décelération ultra rapide)
- Mode automatique (synchronisé PAL) et manuel (encodeur)

✅ **Interface utilisateur**
- 3 boutons poussoirs: Marche, Arrêt, Arrêt rapide
- 2 interrupteurs: Auto/Manuel, Direction (Avant/Arrière)
- Encodeur rotatif KY-040 avec 2 modes:
  - Mode coarse: pas de 100 Hz
  - Mode fine: pas de 1 Hz (bouton enfoncé)
- Plage de fréquence: 0 - 20000 Hz

✅ **Affichage OLED**
- Écran ELEGOO 1315 (SSD1306 I2C)
- Affichage de la fréquence actuelle (Hz) en haut
- Mode AUTO/MANUEL au centre
- Direction AVANT/ARRIÈRE en bas

✅ **Architecture matérielle**
- RP2040-Zero (Waveshare)
- Driver moteur: DRV8825 ou TB6600
- Connexions sur GPIO 0-15 et 26-29 uniquement

---

## Configuration matérielle requise

### Composants principaux
- **1x** Waveshare RP2040-Zero
- **1x** ELEGOO 1315 Écran OLED I2C (128x64)
- **1x** Encodeur rotatif KY-040
- **1x** Driver moteur pas à pas (DRV8825 ou TB6600)
- **1x** Moteur pas à pas NEMA17/23/34
- **3x** Boutons poussoirs momentanés
- **2x** Interrupteurs à bascule
- **1x** Caméra PAL ou générateur signal 50Hz

### Alimentation
- 5V pour le RP2040 et le driver logique
- 12V pour le moteur (selon le driver utilisé)
- GND commune pour tous les circuits

### Condensateurs et résistances
- **100nF** (x4) condensateurs de découplage
- **100µF** condensateur électrolytique (alimentation moteur)
- **10kΩ** résistance (filtre signal PAL)

---

## Installation et configuration

### 1. Préparation de l'environnement Arduino

#### Installer Arduino IDE 2.0+
Télécharger depuis: https://www.arduino.cc/en/software

#### Ajouter le support RP2040 (Earlephilhower)
1. Ouvrir Arduino IDE
2. Aller à `Fichier → Préférences`
3. Dans "URLs de gestionnaire de cartes supplémentaires", ajouter:
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
4. Aller à `Outils → Carte → Gestionnaire de cartes`
5. Chercher "Raspberry Pi Pico" (par Earle F. Philhower)
6. Cliquer "Installer"

### 2. Installer les bibliothèques Arduino requises

#### Adafruit SSD1306
1. `Sketch → Inclure une bibliothèque → Gérer les bibliothèques`
2. Chercher "Adafruit SSD1306"
3. Installer la version la plus récente

### 3. Compilation et programmation

1. Ouvrir `rp2040_pal_stepper_controller.ino`
2. Sélectionner `Outils → Carte → Waveshare RP2040-Zero`
3. Cliquer `Sketch → Vérifier` (test de compilation)
4. Connecter le RP2040-Zero
5. Cliquer `Sketch → Téléverser`

---

## Utilisation

### Démarrage

1. Allumer le système
2. L'écran affiche la fréquence actuelle
3. Le moteur est prêt

### Mode Automatique (PAL)
- Mettre l'interrupteur **AUTO/MANUEL** en **AUTO**
- Appuyer sur **MARCHE**
- Le moteur se synchronise avec le signal PAL

### Mode Manuel (Encodeur)
- Mettre l'interrupteur **AUTO/MANUEL** en **MANUEL**
- Tourner l'encodeur (pas de 100 Hz)
- Bouton enfoncé + rotation: pas de 1 Hz
- Appuyer sur **MARCHE** pour démarrer

### Contrôle direction
- Ajuster l'interrupteur **DIRECTION** (moteur arrêté)

### Arrêt
- **Arrêt normal**: Appuyer sur **ARRÊT**
- **Arrêt rapide**: Appuyer sur **ARRÊT RAPIDE**

---

## Fichiers du projet

```
├── rp2040_pal_stepper_controller.ino  # Code principal
├── BROCHAGE.md                        # Schéma de connexions détaillé
└── README.md                          # Ce fichier
```

---

## Ressources

- **RP2040 Datasheet**: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf
- **Waveshare RP2040-Zero**: https://www.waveshare.com/wiki/RP2040-Zero
- **Adafruit SSD1306**: https://github.com/adafruit/Adafruit_SSD1306
- **Earlephilhower Arduino-Pico**: https://github.com/earlephilhower/arduino-pico

---

**Version**: 1.0.0 | **Date**: Septembre 2026
