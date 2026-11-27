#include <Arduino.h>
#include <BleGamepad.h>
#include <math.h>

// Total de botões
#define NUM_BUTTONS 9

// Pinos dos botões
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

// Faixa física real (ajuste para seu potenciômetro)
#define WHAMMY_MIN 1000
#define WHAMMY_MAX 3000

// Zona morta para desabilitar envio (mais baixo = mais sensível)
#define WHAMMY_DEADZONE 150   // em valor bruto (4095 scale)

// Inversão opcional
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

BleGamepad bleGamepad("Guitarra Clone Hero", "SeuNome", 100);

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  bleGamepad.begin();
  Serial.println("Whammy Bar com ativação automática.");
}

void loop() {
  if (bleGamepad.isConnected()) {
    // Leitura bruta
    int raw = analogRead(WHAMMY_PIN);

    // Valor de referência no repouso (centro da faixa)
    int whammyCenter = (WHAMMY_MIN + WHAMMY_MAX) / 2;

    // Detecta se está fora da zona morta
    if (abs(raw - whammyCenter) > WHAMMY_DEADZONE) {
      // Fora da zona morta = ativa whammy
      raw = constrain(raw, WHAMMY_MIN, WHAMMY_MAX);
      float norm = float(raw - WHAMMY_MIN) / float(WHAMMY_MAX - WHAMMY_MIN);
      norm = constrain(norm, 0.0, 1.0);
      float curved = pow(norm, 1.7);
      int whammyFinal = int(curved * 32767.0);
      if (INVERT_WHAMMY) {
        whammyFinal = 32767 - whammyFinal;
      }

      bleGamepad.setAxes(0, 0, 0, whammyFinal, 0, 0);

      Serial.print("Whammy ativo: ");
      Serial.println(whammyFinal);
    } else {
      // Zona morta = desativa whammy
      bleGamepad.setAxes(0, 0, 0, 0, 0, 0);
      Serial.println("Whammy parado.");
    }

    // Botões
    for (int i = 0; i < NUM_BUTTONS; i++) {
      bool isPressed = !digitalRead(buttonPins[i]);
      if (isPressed != buttonState[i]) {
        buttonState[i] = isPressed;
        if (isPressed) bleGamepad.press(buttons[i]);
        else bleGamepad.release(buttons[i]);
      }
    }

    delay(5);
  }
}
