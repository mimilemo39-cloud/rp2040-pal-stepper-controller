# Schéma de Brochage - RP2040-Zero PAL Stepper Controller

## Vue d'ensemble des connexions

```
┌─────────────────────────────────────────────────────────┐
│                    RP2040-ZERO                          │
│  (Waveshare - Earlephilhower Arduino IDE)              │
└─────────────────────────────────────────────────────────┘
```

## Assignation des GPIO (Pins 0-15 et 26-29)

### Entrées Analogiques
| GPIO | Fonction | Connexion | Notes |
|------|----------|-----------|-------|
| 26 (ADC0) | PAL Video Input | Caméra PAL (signal sync) | Entrée 50Hz |

### Boutons Poussoirs (Pull-up interne)
| GPIO | Fonction | Connexion | État actif |
|------|----------|-----------|-----------|
| 0 | BTN START (Marche) | GND quand appuyé | LOW |
| 1 | BTN STOP (Arrêt) | GND quand appuyé | LOW |
| 2 | BTN EMERGENCY (Arrêt rapide) | GND quand appuyé | LOW |

### Interrupteurs (Pull-up interne)
| GPIO | Fonction | Connexion | État |
|------|----------|-----------|------|
| 3 | SW AUTO/MANUEL | Position 1: AUTO (LOW), Position 2: MANUEL (HIGH) | LOW = AUTO |
| 4 | SW DIRECTION | Position 1: AVANT (LOW), Position 2: ARRIERE (HIGH) | LOW = AVANT |

### Encodeur Rotatif KY-040
| GPIO | Fonction | Connexion | Notes |
|------|----------|-----------|-------|
| 5 | ENCODER CLK | Encodeur CLK | Rotation détection |
| 6 | ENCODER DT | Encodeur DT | Sens rotation |
| 7 | ENCODER SW | Encodeur SW | Bouton: fine/coarse |

### Sortie Moteur Pas à Pas
| GPIO | Fonction | Connexion | Notes |
|------|----------|-----------|-------|
| 8 | STEPPER STEP | Driver DRV8825/TB6600 STEP | Pulse signal |
| 9 | STEPPER DIR | Driver DRV8825/TB6600 DIR | Direction signal |
| 10 | STEPPER ENABLE | Driver DRV8825/TB6600 ENABLE | Active LOW |

### I2C Écran OLED
| GPIO | Fonction | Connexion | Notes |
|------|----------|-----------|-------|
| 12 | I2C SDA | ELEGOO 1315 SDA | Interface données |
| 13 | I2C SCL | ELEGOO 1315 SCL | Interface horloge |

---

## Schéma de Cablage Détaillé

### 1. ALIMENTATION
```
┌─────────────────────────────┐
│  Alimentation +5V / GND     │
├─────────────────────────────┤
│  RP2040-Zero:               │
│  - VBUS/5V (broche)         │ → +5V (Driver, Écran, etc)
│  - GND (broche)             │ → GND (commun)
│                             │
│  Driver Moteur:             │
│  - VCC                      │ ← +5V
│  - GND                      │ ← GND
│  - VMOT (DRV8825)          │ ← +12V (alimentation moteur)
└─────────────────────────────┘
```

### 2. ENTRÉE SIGNAL PAL (GPIO 26)
```
┌──────────────────────────────────────┐
│  Caméra PAL                          │
│  Signal SYNC / COMPOSITE             │
└──────────────────────────────────────┘
           │
           ├─────────────────────────────┐
           │                             │
        [R 10kΩ]                      [C 100nF]
           │                             │
           └───────→ GPIO 26 ← ─────────┤
           (ADC)                        GND
           
           (Filtre passe-bas RC)
```

### 3. BOUTONS POUSSOIRS
```
GPIO 0 (START)     GPIO 1 (STOP)      GPIO 2 (EMERGENCY)
   │                  │                    │
  [━]                [━]                  [━]  (Boutons poussoirs)
   │                  │                    │
   └──→ GND           └──→ GND              └──→ GND
   
   (Pull-ups internes activés)
```

### 4. INTERRUPTEURS
```
GPIO 3 (AUTO/MANUEL)           GPIO 4 (DIRECTION)
      
   Position 1 (LOW)   Position 2 (HIGH)
   ┌────────────┐     ┌─────────────┐
   │   AUTO     │  ↔  │  MANUEL     │
   └────────────┘     └─────────────┘
        ↓                   ↓
      GND                 +3.3V
      
   Position 1 (LOW)   Position 2 (HIGH)
   ┌────────────┐     ┌─────────────┐
   │   AVANT    │  ↔  │  ARRIERE    │
   └────────────┘     └─────────────┘
        ↓                   ↓
      GND                 +3.3V
```

### 5. ENCODEUR ROTATIF KY-040
```
┌────────────────────────────────┐
│  Encodeur Rotatif KY-040       │
├────────────────────────────────┤
│  1. GND          →  GND        │
│  2. + (VCC)      →  +3.3V      │
│  3. SW (Button)  →  GPIO 7     │
│  4. CLK          →  GPIO 5     │
│  5. DT           →  GPIO 6     │
└────────────────────────────────┘

     ┌─────────┐
     │ Encodeur│
     └────┬────┘
          │
    ┌─────┴─────┐
    │  [Rotatif]│
    └───────────┘
    
Ajout de condensateurs de découplage:
    GND ─ [100nF] ─ VCC (près de l'encodeur)
```

### 6. ÉCRAN OLED ELEGOO 1315 (I2C)
```
┌──────────────────────────────┐
│  ELEGOO 1315 SSD1306 I2C     │
│  128x64 Pixels               │
├──────────────────────────────┤
│  Adresse I2C: 0x3C           │
│  Alimentation: 3.3V - 5V     │
├──────────────────────────────┤
│  1. GND      →  GND          │
│  2. VCC      →  +3.3V        │
│  3. SCL      →  GPIO 13      │
│  4. SDA      →  GPIO 12      │
└──────────────────────────────┘

Condensateur de découplage:
    GND ─ [100nF] ─ VCC (près de l'écran)
```

### 7. DRIVER MOTEUR PAS À PAS (DRV8825 ou TB6600)

#### Exemple DRV8825:
```
┌────────────────────────────────┐
│  DRV8825 Stepper Driver        │
├────────────────────────────────┤
│  STEP  (Pin 15)  ←  GPIO 8     │
│  DIR   (Pin 14)  ←  GPIO 9     │
│  EN/SLP (Pin 12) ←  GPIO 10    │
│  GND   (Pin 11)  ←  GND        │
│  GND   (Pin 8)   ←  GND        │
│  FAULT (Pin 6)   → Pas utilisé │
│  GND2  (Pin 5)   ←  GND        │
│  GND1  (Pin 4)   ←  GND        │
│  VMOT  (Pin 9)   ← +12V        │
│  VCP   (Pin 1)   ← +5V         │
│  GND   (Pin 3)   ←  GND        │
│  GND   (Pin 10)  ←  GND        │
└────────────────────────────────┘

Condensateurs de découplage:
    VCP ─ [100nF] ─ GND
    VMOT ─ [100µF] ─ GND (électrolytique)
```

#### Exemple TB6600:
```
┌────────────────────────────────┐
│  TB6600 Stepper Driver         │
├────────────────────────────────┤
│  STEP+ (Pin 1)   ←  GPIO 8     │
│  STEP- (Pin 2)   ←  GND        │
│  DIR+  (Pin 3)   ←  GPIO 9     │
│  DIR-  (Pin 4)   ←  GND        │
│  EN+   (Pin 5)   ←  GPIO 10    │
│  EN-   (Pin 6)   ←  GND        │
│  +5V   (Pin 7)   ← +5V         │
│  GND   (Pin 8)   ←  GND        │
│  A+    (Pin 9)   ← Moteur A    │
│  A-    (Pin 10)  ← Moteur A    │
│  B+    (Pin 11)  ← Moteur B    │
│  B-    (Pin 12)  ← Moteur B    │
└────────────────────────────────┘

Condensateurs de découplage:
    +5V ─ [100nF] ─ GND
```

### 8. MOTEUR PAS À PAS
```
┌─────────────────────────┐
│  Moteur NEMA17/23/34    │
├─────────────────────────┤
│  Coil A+  ← Driver A+   │
│  Coil A-  ← Driver A-   │
│  Coil B+  ← Driver B+   │
│  Coil B-  ← Driver B-   │
└─────────────────────────┘
```

---

## Vue d'ensemble du circuit

```
                    ┌─────────────────────────────────┐
                    │      RP2040-ZERO                │
                    │  (Waveshare - Earlephilhower)  │
                    └──────────────┬──────────────────┘
                                   │
        ┌──────────────────────────┼──────────────────────────┐
        │                          │                          │
        │                          │                          │
    ┌───┴────┐         ┌──────────┴──────────┐          ┌────┴────┐
    │ Caméra │         │   Contrôle          │          │  Écran  │
    │  PAL   │         │  (Boutons/Switch    │          │  OLED   │
    │ 50Hz   │         │   Encodeur)         │          │ I2C     │
    └───┬────┘         └──────────┬──────────┘          └────┬────┘
        │                          │                          │
        │              ┌───────────┼───────────┐              │
        │              │           │           │              │
        │              │     ┌─────┴─────┐     │              │
        │              │     │  DRV8825  │     │              │
        │              │     │ TB6600    │     │              │
        │              │     └─────┬─────┘     │              │
        │              │           │           │              │
        │              │      ┌────┴────┐      │              │
        │              │      │  Moteur │      │              │
        │              │      │ Pas à   │      │              │
        │              │      │ Pas     │      │              │
        └──────────────┴──────┴────┬────┴──────┴──────────────┘
                                   │
                            Synchronisation
                            Commande
                            Affichage
```

---

## Tableau récapitulatif des connexions

| Composant | GPIO | Fonction | Tension | Notes |
|-----------|------|----------|---------|-------|
| Caméra PAL | 26 | Entrée signal | 3.3V | ADC |
| Btn START | 0 | Marche | 3.3V | Pull-up |
| Btn STOP | 1 | Arrêt | 3.3V | Pull-up |
| Btn EMERGENCY | 2 | Arrêt rapide | 3.3V | Pull-up |
| SW AUTO/MANUEL | 3 | Mode | 3.3V | Pull-up |
| SW DIRECTION | 4 | Sens rotation | 3.3V | Pull-up |
| Encoder CLK | 5 | Rotation | 3.3V | Interrupt |
| Encoder DT | 6 | Direction | 3.3V | Lecture |
| Encoder SW | 7 | Fine/Coarse | 3.3V | Pull-up |
| Stepper STEP | 8 | Pulse moteur | 3.3V | Sortie |
| Stepper DIR | 9 | Direction | 3.3V | Sortie |
| Stepper EN | 10 | Enable | 3.3V | Sortie |
| I2C SDA | 12 | Écran données | 3.3V | Open-drain |
| I2C SCL | 13 | Écran horloge | 3.3V | Open-drain |

---

## Recommandations de câblage

1. **Séparation des circuits**
   - Alimentation logique (3.3V/5V) séparée de l'alimentation moteur (12V)
   - Masse commune partout

2. **Condensateurs de découplage**
   - 100nF près de chaque entrée d'alimentation (VCC)
   - 100µF électrolytique pour VMOT du driver

3. **Fil de masse**
   - Utiliser le plus court possible
   - Plusieurs points de connexion à la masse commune

4. **Blindage**
   - Câble blindé pour le signal PAL (entrée analogique)
   - Tenir éloigné des fils d'alimentation moteur

5. **Longueurs de câbles**
   - Pins GPIO vers composants: < 30cm
   - Signal PAL: blindé, < 1m
   - Alimentation: section appropriée à l'intensité

6. **Résistances pull-up additionnelles**
   - Optionnel: 4.7kΩ sur I2C si problèmes de communication
