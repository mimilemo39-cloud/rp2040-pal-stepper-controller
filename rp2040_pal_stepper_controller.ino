/*
 * RP2040-Zero PAL Stepper Motor Controller
 * Waveshare RP2040-Zero avec Arduino IDE (Earlephilhower)
 * 
 * Fonctionnalités:
 * - Synchronisation signal PAL 50Hz multiplié par 200
 * - Rampe d'accélération progressive (max 2s)
 * - Rampe de décélération progressive
 * - Arrêt rapide
 * - Encodeur rotatif KY-040 pour contrôle manuel
 * - Interrupteur Auto/Manuel
 * - Interrupteur Avant/Arrière (moteur à l'arrêt)
 * - Écran ELEGOO I2C 128x64 (SSD1315)
 */

#include <Wire.h>
#include <Adafruit_SSD1306.h>

// ============ CONFIGURATION DES PINS ============
// Entrée signal PAL (ADC)
#define PAL_INPUT_PIN     26

// Boutons poussoirs
#define BTN_START_PIN     0   // Marche
#define BTN_STOP_PIN      1   // Arrêt
#define BTN_EMERGENCY_PIN 2   // Arrêt rapide

// Interrupteurs
#define SW_AUTO_MANUAL_PIN   3   // Auto/Manuel
#define SW_DIRECTION_PIN     4   // Avant/Arrière

// Encodeur rotatif KY-040
#define ENCODER_CLK_PIN   5
#define ENCODER_DT_PIN    6
#define ENCODER_SW_PIN    7   // Bouton encodeur (fine/coarse)

// Sortie moteur pas à pas
#define STEPPER_STEP_PIN  8   // Step signal
#define STEPPER_DIR_PIN   9   // Direction signal
#define STEPPER_ENABLE_PIN 10 // Enable signal

// Écran OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

// ============ CONSTANTES ============
#define PAL_BASE_FREQ        50.0  // Hz
#define PAL_MULTIPLIER       200
#define TARGET_FREQ          (PAL_BASE_FREQ * PAL_MULTIPLIER)  // 10000 Hz
#define MAX_FREQ             20000  // Hz
#define ENCODER_COARSE_STEP  100   // Hz
#define ENCODER_FINE_STEP    1     // Hz
#define ACCEL_TIME           2000  // ms
#define DECEL_TIME           1500  // ms
#define EMERGENCY_DECEL_TIME 300   // ms

// ============ VARIABLES GLOBALES ============
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// État du système
volatile float currentFreq = 0;
volatile float targetFreq = 0;
float displayFreq = 0;
unsigned long accelStartTime = 0;
unsigned long decelStartTime = 0;
bool isAccelerating = false;
bool isDecelerating = false;
bool isEmergencyStopping = false;
bool motorEnabled = false;

// Encodeur
volatile int encoderPos = 0;
volatile int lastEncoderCLK = 0;
int lastEncoderDT = 0;
bool encoderButtonPressed = false;

// État des entrées
bool lastStartBtn = false;
bool lastStopBtn = false;
bool lastEmergencyBtn = false;
bool autoMode = true;
bool directionForward = true;
bool lastDirectionSw = true;
bool lastAutoSw = true;

// Génération de fréquence
volatile unsigned long stepInterval = 0;
volatile unsigned long lastStepTime = 0;

// Signal PAL
volatile float palFreq = 0;
volatile float palPhase = 0;
volatile unsigned long palSampleTime = 0;
int palDetectCounter = 0;
unsigned long lastPalTime = 0;

// ============ INTERRUPTIONS ============

// ISR Encodeur rotatif
void encoderISR() {
  int currentCLK = digitalRead(ENCODER_CLK_PIN);
  int currentDT = digitalRead(ENCODER_DT_PIN);
  
  if (currentCLK != lastEncoderCLK) {
    lastEncoderCLK = currentCLK;
    
    if (currentCLK == LOW) {
      bool fineMode = (digitalRead(ENCODER_SW_PIN) == LOW);
      int step = fineMode ? ENCODER_FINE_STEP : ENCODER_COARSE_STEP;
      
      if (currentDT == LOW) {
        encoderPos += step;
      } else {
        encoderPos -= step;
      }
      
      // Limiter entre 0 et MAX_FREQ
      if (encoderPos < 0) encoderPos = 0;
      if (encoderPos > MAX_FREQ) encoderPos = MAX_FREQ;
    }
  }
}

// ISR pour la détection PAL
void palDetectISR() {
  unsigned long now = micros();
  
  if (lastPalTime > 0) {
    unsigned long interval = now - lastPalTime;
    // Filtrer les valeurs aberrantes (PAL = 20ms ± 10%)
    if (interval > 18000 && interval < 22000) {
      palFreq = 1000000.0 / interval;
    }
  }
  lastPalTime = now;
}

// ISR Timer pour génération de steps
void stepGeneratorISR() {
  unsigned long now = micros();
  
  if (motorEnabled && (now - lastStepTime) >= stepInterval) {
    digitalWrite(STEPPER_STEP_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(STEPPER_STEP_PIN, LOW);
    lastStepTime = now;
  }
}

// ============ INITIALISATION ============

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== RP2040 PAL Stepper Controller ===");
  
  // Configuration des pins
  pinMode(PAL_INPUT_PIN, INPUT);
  pinMode(BTN_START_PIN, INPUT_PULLUP);
  pinMode(BTN_STOP_PIN, INPUT_PULLUP);
  pinMode(BTN_EMERGENCY_PIN, INPUT_PULLUP);
  pinMode(SW_AUTO_MANUAL_PIN, INPUT_PULLUP);
  pinMode(SW_DIRECTION_PIN, INPUT_PULLUP);
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  
  pinMode(STEPPER_STEP_PIN, OUTPUT);
  pinMode(STEPPER_DIR_PIN, OUTPUT);
  pinMode(STEPPER_ENABLE_PIN, OUTPUT);
  
  // Initialiser moteur désactivé
  digitalWrite(STEPPER_STEP_PIN, LOW);
  digitalWrite(STEPPER_DIR_PIN, LOW);
  digitalWrite(STEPPER_ENABLE_PIN, HIGH);  // Active LOW
  
  // Initialiser écran OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Initialisation..."));
  display.display();
  
  // Configuration des interruptions
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PAL_INPUT_PIN), palDetectISR, RISING);
  
  // Timer pour génération de steps (tous les 10us)
  set_sys_clock_khz(125000, false);  // Assurer horloge stable
  
  Serial.println("Setup completed");
  delay(1000);
}

// ============ BOUCLE PRINCIPALE ============

void loop() {
  // Lecture des entrées
  readInputs();
  
  // Traiter les boutons
  handleButtons();
  
  // Traiter l'encodeur en mode manuel
  if (!autoMode) {
    handleManualEncoder();
  }
  
  // Mettre à jour la fréquence cible
  updateTargetFrequency();
  
  // Traiter accélération/décélération
  handleAcceleration();
  
  // Générer les steps
  generateSteps();
  
  // Mettre à jour l'affichage
  updateDisplay();
  
  delay(10);  // Polling principal tous les 10ms
}

// ============ FONCTIONS DE GESTION DES ENTRÉES ============

void readInputs() {
  lastStartBtn = (digitalRead(BTN_START_PIN) == LOW);
  lastStopBtn = (digitalRead(BTN_STOP_PIN) == LOW);
  lastEmergencyBtn = (digitalRead(BTN_EMERGENCY_PIN) == LOW);
  autoMode = (digitalRead(SW_AUTO_MANUAL_PIN) == LOW);
  
  // Direction: lire seulement si moteur arrêté
  if (!motorEnabled) {
    directionForward = (digitalRead(SW_DIRECTION_PIN) == LOW);
  }
  
  // Appliquer la direction
  digitalWrite(STEPPER_DIR_PIN, directionForward ? LOW : HIGH);
}

void handleButtons() {
  static bool startPressed = false;
  static bool stopPressed = false;
  static bool emergencyPressed = false;
  
  // Bouton MARCHE
  if (lastStartBtn && !startPressed) {
    startPressed = true;
    Serial.println("START button pressed");
    if (!motorEnabled) {
      startMotor();
    }
  } else if (!lastStartBtn && startPressed) {
    startPressed = false;
  }
  
  // Bouton ARRÊT normal
  if (lastStopBtn && !stopPressed) {
    stopPressed = true;
    Serial.println("STOP button pressed");
    if (motorEnabled && !isDecelerating) {
      stopMotor();
    }
  } else if (!lastStopBtn && stopPressed) {
    stopPressed = false;
  }
  
  // Bouton ARRÊT RAPIDE
  if (lastEmergencyBtn && !emergencyPressed) {
    emergencyPressed = true;
    Serial.println("EMERGENCY STOP pressed");
    emergencyStop();
  } else if (!lastEmergencyBtn && emergencyPressed) {
    emergencyPressed = false;
  }
}

void handleManualEncoder() {
  static int lastEncoderPos = 0;
  
  if (encoderPos != lastEncoderPos) {
    lastEncoderPos = encoderPos;
    targetFreq = encoderPos;
    
    // Si moteur en marche, ajuster la fréquence
    if (motorEnabled && !isAccelerating && !isDecelerating) {
      currentFreq = targetFreq;
    }
    
    Serial.print("Manual freq: ");
    Serial.println(targetFreq);
  }
}

// ============ GESTION DU MOTEUR ============

void startMotor() {
  if (motorEnabled) return;
  
  motorEnabled = true;
  isAccelerating = true;
  isDecelerating = false;
  isEmergencyStopping = false;
  accelStartTime = millis();
  currentFreq = 0;
  
  if (autoMode) {
    targetFreq = TARGET_FREQ;
  } else {
    targetFreq = encoderPos;
  }
  
  // Activer le driver
  digitalWrite(STEPPER_ENABLE_PIN, LOW);
  
  Serial.println("Motor START - Acceleration phase");
}

void stopMotor() {
  if (!motorEnabled) return;
  
  motorEnabled = false;
  isDecelerating = true;
  isAccelerating = false;
  isEmergencyStopping = false;
  decelStartTime = millis();
  targetFreq = 0;
  
  Serial.println("Motor STOP - Deceleration phase");
}

void emergencyStop() {
  motorEnabled = false;
  isEmergencyStopping = true;
  isAccelerating = false;
  isDecelerating = false;
  decelStartTime = millis();
  targetFreq = 0;
  
  Serial.println("EMERGENCY STOP - Rapid deceleration");
}

void updateTargetFrequency() {
  if (!autoMode) {
    // Mode manuel: encodeur gère la fréquence
    targetFreq = encoderPos;
  } else if (motorEnabled && !isAccelerating && !isDecelerating) {
    // Mode auto: synchroniser avec PAL
    if (palFreq > 0) {
      targetFreq = palFreq * PAL_MULTIPLIER;
      // Maintenir phase alignée
      maintainPhaseAlignment();
    }
  }
}

void maintainPhaseAlignment() {
  // Ajustement fin pour maintenir alignement de phase
  // Variation max ±2% de la fréquence cible
  float syncTolerance = targetFreq * 0.02;
  
  if (currentFreq < (targetFreq - syncTolerance)) {
    currentFreq += 10;  // Petit ajustement ascendant
  } else if (currentFreq > (targetFreq + syncTolerance)) {
    currentFreq -= 10;  // Petit ajustement descendant
  }
}

void handleAcceleration() {
  unsigned long now = millis();
  
  if (isAccelerating) {
    unsigned long elapsed = now - accelStartTime;
    
    if (elapsed >= ACCEL_TIME) {
      // Accélération terminée
      isAccelerating = false;
      currentFreq = targetFreq;
      Serial.println("Acceleration complete - Sync phase");
    } else {
      // Rampe d'accélération linéaire
      float progress = (float)elapsed / ACCEL_TIME;
      currentFreq = targetFreq * progress;
    }
  }
  else if (isDecelerating) {
    unsigned long decelDuration = isEmergencyStopping ? EMERGENCY_DECEL_TIME : DECEL_TIME;
    unsigned long elapsed = now - decelStartTime;
    
    if (elapsed >= decelDuration) {
      // Décélération terminée
      isDecelerating = false;
      isEmergencyStopping = false;
      currentFreq = 0;
      digitalWrite(STEPPER_ENABLE_PIN, HIGH);  // Désactiver le driver
      Serial.println("Motor stopped - Driver disabled");
    } else {
      // Rampe de décélération linéaire
      float progress = 1.0 - ((float)elapsed / decelDuration);
      currentFreq = targetFreq * progress;
    }
  }
}

void generateSteps() {
  if (currentFreq <= 0) {
    stepInterval = 0;
    return;
  }
  
  // Calcul de l'intervalle entre les steps (en microsecondes)
  // Fréquence = 2 steps par cycle (HIGH et LOW)
  stepInterval = (unsigned long)(1000000.0 / (currentFreq * 2));
  
  // Génération des steps via polling
  if (motorEnabled) {
    unsigned long now = micros();
    if ((now - lastStepTime) >= stepInterval) {
      digitalWrite(STEPPER_STEP_PIN, HIGH);
      delayMicroseconds(5);
      digitalWrite(STEPPER_STEP_PIN, LOW);
      lastStepTime = now;
    }
  }
}

// ============ AFFICHAGE OLED ============

void updateDisplay() {
  static unsigned long lastDisplayUpdate = 0;
  unsigned long now = millis();
  
  // Mettre à jour à 5 Hz
  if ((now - lastDisplayUpdate) < 200) return;
  lastDisplayUpdate = now;
  
  display.clearDisplay();
  
  // Zone haute: Fréquence en Hz (bleu = inverse)
  display.fillRect(0, 0, 128, 20, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(0, 2);
  
  char freqStr[20];
  dtostrf(currentFreq, 5, 0, freqStr);
  strcat(freqStr, " Hz");
  
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(freqStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 2);
  display.println(freqStr);
  
  // Zone milieu: Mode AUTO/MANUEL
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 32);
  
  const char* modeStr = autoMode ? "AUTO" : "MANUEL";
  display.getTextBounds(modeStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 32);
  display.println(modeStr);
  
  // Zone basse: Direction
  display.setTextSize(1);
  const char* dirStr = directionForward ? "AVANT" : "ARRIERE";
  display.getTextBounds(dirStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 56);
  display.println(dirStr);
  
  // Statut moteur
  if (motorEnabled) {
    display.setCursor(0, 56);
    display.println("[ON]");
  } else {
    display.setCursor(0, 56);
    display.println("[OFF]");
  }
  
  display.display();
}

// ============ FONCTIONS UTILITAIRES ============

void printStatus() {
  Serial.print("Freq: ");
  Serial.print(currentFreq);
  Serial.print(" Hz | Target: ");
  Serial.print(targetFreq);
  Serial.print(" Hz | PAL: ");
  Serial.print(palFreq);
  Serial.print(" Hz | Mode: ");
  Serial.print(autoMode ? "AUTO" : "MANUAL");
  Serial.print(" | Dir: ");
  Serial.print(directionForward ? "FWD" : "REV");
  Serial.print(" | Motor: ");
  Serial.println(motorEnabled ? "ON" : "OFF");
}
