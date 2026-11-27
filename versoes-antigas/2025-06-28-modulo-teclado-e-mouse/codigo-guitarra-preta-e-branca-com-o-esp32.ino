#include <Keyboard.h>
#include <Mouse.h>

// Pinos
const int joyXPin = A0;
const int joyYPin = A1;
const int potPin  = A2;
const int buttonPin = 15;

// Configurações
const int deadzone = 150;        // AUMENTAMOS a zona morta
const int potDeadzone = 150;

// Estados anteriores
int prevXDir = 0;
int prevYDir = 0;
bool prevButtonState = HIGH;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  // Lê joystick
  int xRaw = analogRead(joyXPin) - 512;
  int yRaw = analogRead(joyYPin) - 512;

  int xDir = 0;
  int yDir = 0;

  // Processa eixo X
  if (xRaw > deadzone) {
    xDir = 1; // direita
  } else if (xRaw < -deadzone) {
    xDir = -1; // esquerda
  }

  // Processa eixo Y (inverso caso teu módulo seja invertido)
  if (yRaw > deadzone) {
    yDir = -1; // para baixo (inverter se necessário)
  } else if (yRaw < -deadzone) {
    yDir = 1; // para cima
  }

  // Se houve mudança no X
  if (xDir != prevXDir) {
    if (xDir == 1) {
      sendCtrlArrow(KEY_RIGHT_ARROW);
    } else if (xDir == -1) {
      sendCtrlArrow(KEY_LEFT_ARROW);
    }
  }
  prevXDir = xDir;

  // Se houve mudança no Y
  if (yDir != prevYDir) {
    if (yDir == 1) {
      sendCtrlArrow(KEY_UP_ARROW);
    } else if (yDir == -1) {
      sendCtrlArrow(KEY_DOWN_ARROW);
    }
  }
  prevYDir = yDir;

  // Potenciômetro → zoom
  int potValue = analogRead(potPin) - 512;

  if (potValue > potDeadzone) {
    Mouse.move(0, 0, 1);
    delay(50);
  } else if (potValue < -potDeadzone) {
    Mouse.move(0, 0, -1);
    delay(50);
  }

  // Botão físico → Ctrl+0
  bool buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && prevButtonState == HIGH) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('0');
    delay(50);
    Keyboard.releaseAll();
  }
  prevButtonState = buttonState;

  delay(10);
}

void sendCtrlArrow(byte arrowKey) {
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(arrowKey);
  delay(50);
  Keyboard.releaseAll();
}
