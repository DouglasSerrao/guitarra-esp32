#define USE_NIMBLE // Biblioteca otimizada para evitar desconexões
#include <Arduino.h>
#include <BleGamepad.h>
#include <math.h>

#define NUM_BUTTONS 9

// Pinos dos botões da guitarra
#define BUTTON_PIN_0 18
#define BUTTON_PIN_1 4
#define BUTTON_PIN_2 5
#define BUTTON_PIN_3 12
#define BUTTON_PIN_4 13
#define BUTTON_PIN_5 14
#define BUTTON_PIN_6 15
#define BUTTON_PIN_7 16
#define BUTTON_PIN_8 17

// Pino do Whammy Bar
#define WHAMMY_PIN 34

// --- CALIBRAÇÃO MECÂNICA DA IMPRESSÃO 3D ---
#define WHAMMY_RESTING 575 // Média do estado de repouso físico
#define WHAMMY_FULL 970    // Limite mecânico máximo da estrutura
#define WHAMMY_DEADZONE 15 // Filtro para oscilações (570-580 não ativam a alavanca)

#define INVERT_WHAMMY false

int buttons[NUM_BUTTONS] = {
  BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5,
  BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9
};

int buttonPins[NUM_BUTTONS] = {
  BUTTON_PIN_0, BUTTON_PIN_1, BUTTON_PIN_2,
  BUTTON_PIN_3, BUTTON_PIN_4, BUTTON_PIN_5,
  BUTTON_PIN_6, BUTTON_PIN_7, BUTTON_PIN_8
};

bool buttonState[NUM_BUTTONS] = {false};
int lastWhammyFinal = 0; 

BleGamepad bleGamepad("Guitarra Clone Hero", "Custom", 100);

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  bleGamepad.begin();
  Serial.println("Firmware Calibrado Iniciado.");
}

void loop() {
  if (bleGamepad.isConnected()) {
    int raw = analogRead(WHAMMY_PIN);
    int whammyFinal = 0;
    bool isPressed = false;
    
    // Verifica se a alavanca saiu da zona morta
    if (WHAMMY_RESTING < WHAMMY_FULL) {
        isPressed = raw > (WHAMMY_RESTING + WHAMMY_DEADZONE);
    } else {
        isPressed = raw < (WHAMMY_RESTING - WHAMMY_DEADZONE);
    }

    if (isPressed) {
      int constrainedRaw = raw;
      
      // Trava a leitura nos limites físicos estipulados
      if (WHAMMY_RESTING < WHAMMY_FULL) {
          constrainedRaw = constrain(raw, WHAMMY_RESTING, WHAMMY_FULL);
      } else {
          constrainedRaw = constrain(raw, WHAMMY_FULL, WHAMMY_RESTING);
      }

      // Normaliza o pequeno curso mecânico (575-970) para uma escala de 0.0 a 1.0
      float norm = abs(float(constrainedRaw - WHAMMY_RESTING)) / abs(float(WHAMMY_FULL - WHAMMY_RESTING));
      norm = constrain(norm, 0.0, 1.0);
      
      // Aplica uma curva exponencial para suavidade no jogo
      float curved = pow(norm, 1.7);
      whammyFinal = int(curved * 32767.0);

      if (INVERT_WHAMMY) {
        whammyFinal = 32767 - whammyFinal;
      }
    } else {
      whammyFinal = INVERT_WHAMMY ? 32767 : 0;
    }

    // Controle de tráfego (evita a saturação do Bluetooth que causava as quedas)
    if (whammyFinal != lastWhammyFinal) {
      if (abs(whammyFinal - lastWhammyFinal) > 300 || whammyFinal == 0 || whammyFinal == 32767) {
        bleGamepad.setAxes(0, 0, 0, whammyFinal, 0, 0);
        lastWhammyFinal = whammyFinal;
      }
    }

    // Processamento dos botões
    for (int i = 0; i < NUM_BUTTONS; i++) {
      bool buttonPressed = !digitalRead(buttonPins[i]);
      if (buttonPressed != buttonState[i]) {
        buttonState[i] = buttonPressed;
        if (buttonPressed) bleGamepad.press(buttons[i]);
        else bleGamepad.release(buttons[i]);
      }
    }

    delay(5);
  }
}