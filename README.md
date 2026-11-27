# Guitarra Clone Hero com ESP32

Guitarra de Guitar Hero que eu montei do zero pra jogar Clone Hero. Ela usa um ESP32 e conecta por Bluetooth, aparecendo no PC como um controle chamado "Guitarra Clone Hero". Os botões são switches de teclado mecânico e a alavanca (whammy) é um potenciômetro.

## Código

O código atual fica em `guitarra-esp32/guitarra-esp32.ino`.

- 9 botões nos pinos 18, 4, 5, 12, 13, 14, 15, 16 e 17
- Whammy no pino 34, calibrado pro curso da minha alavanca: repouso em 575, máximo em 970 e uma zona morta de 15 pra alavanca não ficar tremendo parada
- Se a sua alavanca funcionar ao contrário, é só trocar `INVERT_WHAMMY` pra `true`
- Usa a biblioteca ESP32-BLE-Gamepad com NimBLE, que segura melhor a conexão e evita desconexões no meio da música

Pra gravar no ESP32 pelo Arduino IDE, instale o suporte às placas ESP32 e as bibliotecas ESP32-BLE-Gamepad e NimBLE-Arduino.

Se for montar a sua, ajuste os valores do whammy pra sua alavanca. Abrindo o Monitor Serial (115200) dá pra ver a leitura do potenciômetro em repouso e no fim do curso.

## Versões antigas

Ficam na pasta `versoes-antigas`, guardadas pra histórico:

- `2025-06-19-primeira-versao-bluetooth`: primeira versão por Bluetooth, antes da calibração do whammy
- `2025-06-28-modulo-teclado-e-mouse`: teste de um módulo com joystick e potenciômetro mandando teclado e mouse
