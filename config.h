/*
 * Configuration et constantes pour RP2040 PAL Stepper Controller
 * À inclure dans le projet Arduino
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============ VERSIONS ET IDENTIFICATEURS ============
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define PROJECT_NAME "RP2040 PAL Stepper Controller"

// ============ CONFIGURATION PAL ============
#define PAL_BASE_FREQ        50.0    // Fréquence PAL standard (Hz)
#define PAL_MULTIPLIER       200     // Multiplicateur pour la synchronisation
#define PAL_TARGET_FREQ      (PAL_BASE_FREQ * PAL_MULTIPLIER)  // 10000 Hz
#define PAL_TOLERANCE        0.02    // Tolérance ±2% pour synchronisation

// ============ LIMITES DE FRÉQUENCE ============
#define MIN_FREQ             0       // Fréquence minimale (Hz)
#define MAX_FREQ             20000   // Fréquence maximale (Hz)
#define FREQ_STEP_COARSE     100     // Pas encodeur mode coarse (Hz)
#define FREQ_STEP_FINE       1       // Pas encodeur mode fine (Hz)

// ============ RAMPES D'ACCÉLÉRATION/DÉCÉLÉRATION ============
#define ACCEL_TIME           2000    // Durée accélération maximale (ms)
#define DECEL_TIME           1500    // Durée décélération normale (ms)
#define EMERGENCY_DECEL_TIME 300     // Durée arrêt rapide (ms)

// ============ CONFIGURATION HARDWARE ============

// Entrées analogiques
#define PAL_INPUT_PIN        26      // GPIO26 - Entrée signal PAL (ADC0)

// Boutons poussoirs (actif bas, pull-up interne)
#define BTN_START_PIN        0       // GPIO0 - Bouton MARCHE
#define BTN_STOP_PIN         1       // GPIO1 - Bouton ARRÊT
#define BTN_EMERGENCY_PIN    2       // GPIO2 - Bouton ARRÊT RAPIDE

// Interrupteurs (pull-up interne)
#define SW_AUTO_MANUAL_PIN   3       // GPIO3 - Interrupteur AUTO/MANUEL
#define SW_DIRECTION_PIN     4       // GPIO4 - Interrupteur AVANT/ARRIÈRE

// Encodeur rotatif KY-040 (pull-up interne)
#define ENCODER_CLK_PIN      5       // GPIO5 - CLK encodeur
#define ENCODER_DT_PIN       6       // GPIO6 - DT encodeur
#define ENCODER_SW_PIN       7       // GPIO7 - Bouton encodeur (fine/coarse)

// Sorties moteur pas à pas (actif haut)
#define STEPPER_STEP_PIN     8       // GPIO8 - Signal STEP
#define STEPPER_DIR_PIN      9       // GPIO9 - Signal DIRECTION
#define STEPPER_ENABLE_PIN   10      // GPIO10 - Signal ENABLE (actif bas)

// I2C pour écran OLED (SSD1306)
#define I2C_SDA_PIN          12      // GPIO12 - SDA
#define I2C_SCL_PIN          13      // GPIO13 - SCL
#define OLED_ADDR            0x3C    // Adresse I2C par défaut

// ============ CONFIGURATION ÉCRAN ============
#define SCREEN_WIDTH         128     // Largeur écran (pixels)
#define SCREEN_HEIGHT        64      // Hauteur écran (pixels)
#define REFRESH_RATE_MS      200     // Taux de rafraîchissement écran (ms)

// Zones d'affichage
#define FREQ_ZONE_Y          0
#define FREQ_ZONE_H          20
#define MODE_ZONE_Y          25
#define MODE_ZONE_H          20
#define DIR_ZONE_Y           50
#define DIR_ZONE_H           14

// ============ CONFIGURATION COMMUNICATION ============
#define SERIAL_BAUD          115200  // Vitesse liaison série (baud)
#define I2C_CLOCK            400000  // Vitesse I2C (Hz) - 400kHz standard
#define I2C_TIMEOUT_MS       1000    // Timeout I2C (ms)

// ============ TIMERS ET INTERRUPTIONS ============
#define POLLING_INTERVAL_MS  10      // Intervalle de polling principal (ms)
#define DISPLAY_UPDATE_MS    200     // Intervalle mise à jour écran (ms)
#define ENCODER_DEBOUNCE_US  200     // Délai debounce encodeur (µs)

// ============ CONSTANTES DE CALCUL ============
#define MICROS_PER_SECOND    1000000UL
#define STEP_PULSE_WIDTH_US  5       // Durée pulse STEP (µs)
#define MIN_STEP_INTERVAL_US 5       // Intervalle minimum entre steps (µs)

// ============ MESSAGES SÉRIÉ ============
#define MSG_INIT             "=== RP2040 PAL Stepper Controller v"
#define MSG_SETUP_OK         "Setup completed"
#define MSG_START            "Motor START - Acceleration phase"
#define MSG_ACCEL_DONE       "Acceleration complete - Sync phase"
#define MSG_STOP             "Motor STOP - Deceleration phase"
#define MSG_EMERGENCY        "EMERGENCY STOP - Rapid deceleration"
#define MSG_STOPPED          "Motor stopped - Driver disabled"
#define MSG_FREQ_UPDATE      "Frequency: "
#define MSG_MODE_UPDATE      "Mode: "
#define MSG_DIR_UPDATE       "Direction: "

// ============ ÉNUMÉRATIONS ============
enum MotorState {
  STATE_STOPPED = 0,
  STATE_ACCELERATING = 1,
  STATE_RUNNING = 2,
  STATE_DECELERATING = 3,
  STATE_EMERGENCY_STOP = 4
};

enum OperatingMode {
  MODE_AUTOMATIC = 0,    // Synchronisation PAL
  MODE_MANUAL = 1        // Contrôle encodeur
};

enum Direction {
  DIR_FORWARD = 0,
  DIR_REVERSE = 1
};

enum ButtonState {
  BTN_RELEASED = 0,
  BTN_PRESSED = 1,
  BTN_HELD = 2
};

// ============ STRUCTURES DE DONNÉES ============

typedef struct {
  float current;          // Fréquence actuelle (Hz)
  float target;           // Fréquence cible (Hz)
  float pal;              // Fréquence PAL détectée (Hz)
  float phase;            // Phase PAL (0-360)
} FrequencyData;

typedef struct {
  uint8_t motor_enabled;
  uint8_t is_accelerating;
  uint8_t is_decelerating;
  uint8_t is_emergency_stopping;
  OperatingMode mode;
  Direction direction;
} SystemState;

typedef struct {
  unsigned long pulse_interval;    // Intervalle entre pulses (µs)
  unsigned long last_pulse_time;   // Dernier pulse (µs)
  uint32_t pulse_count;            // Nombre de pulses générés
} StepperData;

typedef struct {
  int position;                    // Position actuelle encodeur
  uint8_t fine_mode;               // Mode fine (1Hz) actif
  uint8_t direction;               // Direction rotation
  unsigned long last_update;       // Dernier changement
} EncoderData;

// ============ MACROS UTILES ============

// Conversion fréquence en intervalle entre steps (µs)
#define FREQ_TO_INTERVAL(freq) ((freq > 0) ? (unsigned long)(MICROS_PER_SECOND / (freq * 2)) : 0)

// Conversion intervalle en fréquence (Hz)
#define INTERVAL_TO_FREQ(interval) ((interval > 0) ? (float)MICROS_PER_SECOND / (interval * 2) : 0)

// Limitation de valeur
#define CONSTRAIN(val, min, max) ((val < min) ? min : (val > max) ? max : val)

// Vérification bit
#define BIT_IS_SET(reg, bit) ((reg & (1 << bit)) != 0)
#define BIT_IS_CLEAR(reg, bit) ((reg & (1 << bit)) == 0)

#endif // CONFIG_H
