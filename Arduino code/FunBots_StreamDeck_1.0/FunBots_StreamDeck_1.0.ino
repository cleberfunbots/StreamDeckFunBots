/*
      Programa Stream Deck DIY FunBots
      GitHub: https://github.com/cleberfunbots/StreamDeckFunBots
      
      Stream Deck para enviar comandos para computador, para auxilio de streamings e edição de vídeos
      Customizado para comandos para MacOS. Caso queira trocar para Windows, consultar as teclas conforme site abaixo:
      https://diyusthad.com/list-of-keyboard-modifiers

      Componentes:
        - Arduino Pro Micro;
        - 6 botões tipo "push-button táctil"
        - 6 Displays OLED I2C
        - 1 CI CD4051B
        - 1 Rotary Encoder
        - 1 Led WS2812b

      Versão 1.0 - Versão inicial com comandos para troca de Cenas no OBS MacOS - 30/Mar/2022

 *    * Criado por Cleber Borges - FunBots - @cleber.funbots  *     *

      Instagram: https://www.instagram.com/cleber.funbots/
      Facebook: https://www.facebook.com/cleber.funbots
      YouTube: https://www.youtube.com/channel/UCKs2l5weIqgJQxiLj0A6Atw
      Telegram: https://t.me/cleberfunbots

*/

// Inclusão das Bibliotecas
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <Keyboard.h>
#include <Mouse.h>
#include "logos.h"

// Define e Configura Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definição de Pinos
const int pinAddA = A3;
const int pinAddB = 9;
const int pinAddC = 8;
const int pinBotao1 = 15;
const int pinBotao2 = 16;
const int pinBotao3 = 10;
const int pinBotao4 = 6;
const int pinBotao5 = 5;
const int pinBotao6 = 7;
const int pinLed = 14;
const int pinoEncCLK = A0;
const int pinoEncSW = A1;
const int pinoEncDT = A2;

// Variáveis de teclado (MacOS)
char commandKey = KEY_LEFT_GUI;   // Tecla WIN do Windows
char optionKey = KEY_LEFT_ALT;    // Tecla ALT do Windows
char shiftKey = KEY_LEFT_SHIFT;   // Tecla Shift
char controlKey = KEY_LEFT_CTRL;     // Tecla CTRL
char tabKey = KEY_TAB;              // Tecla Tab

// Cria objeto para acionar Led
Adafruit_NeoPixel led = Adafruit_NeoPixel(1, pinLed, NEO_GRB + NEO_KHZ800);

// Definição de Variáveis Globais
int statusB1 = 0;
int statusB2 = 0;
int statusB3 = 0;
int statusB4 = 0;
int statusB5 = 0;
int statusB6 = 0;
int statusEncSw = 0;

signed char contador = 0;

int CLK = 0;
int DT = 0;

uint32_t blue = led.Color(0,   0,   255);
uint32_t red = led.Color(255,   0,   0);
uint32_t black = led.Color(0,   0,   0);

byte brilho = 0;
int brilhoDir = 0;

/*
   SETUP
*/
void setup() {
  Serial.begin(9600);
  // Inicializa pinos
  pinMode(pinAddA, OUTPUT);
  pinMode(pinAddB, OUTPUT);
  pinMode(pinAddC, OUTPUT);

  pinMode(pinBotao1, INPUT);
  pinMode(pinBotao2, INPUT);
  pinMode(pinBotao3, INPUT);
  pinMode(pinBotao4, INPUT);
  pinMode(pinBotao5, INPUT);
  pinMode(pinBotao6, INPUT);

  pinMode(pinoEncSW, INPUT);
  pinMode(pinoEncCLK, INPUT);
  pinMode(pinoEncDT, INPUT);

  led.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  led.show();            // Turn OFF all pixels ASAP
  led.setBrightness(180);

  

  // Inicializa Displays no endereço 0x3C do I2C
  Wire.begin();
  selDisp1();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  selDisp2();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  selDisp3();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  selDisp4();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  selDisp5();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  selDisp6();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Configura cor e tamanho de texto
  display.setTextColor(WHITE);
  display.setTextSize(3);

  // Atualiza Displays com textos
  refreshDisplays();

}



void loop() {

  // Faz leitura do Encoder e dos botões
  leituraEncoder();
  leituraBotoes();

  // Se alterou o Encoder, envia dado para USB
  if (contador != 0) {
    Mouse.begin();
    Mouse.move(0, 0, contador);
    Mouse.end();
  }

  // Brilho Led Azul pulsante
  if (brilhoDir == 0) {
    brilho = brilho + 5;
    if (brilho >= 255) brilhoDir = 1;
  } else {
    brilho = brilho - 5;
    if (brilho <= 0) brilhoDir = 0;
  }

  // Acende Vermelho se pressionou botão
  if (statusB1 || statusB2 || statusB3 || statusB4 || statusB5 || statusB6 || statusEncSw ) {
    led.setBrightness(250);
    led.setPixelColor(0, red);
    led.show();
  } else {
    led.setBrightness(brilho);
    led.setPixelColor(0, blue);
    led.show();
  }
  delay(50);

}

void leituraBotoes() {
  statusB1 = digitalRead(pinBotao1);
  statusB2 = digitalRead(pinBotao2);
  statusB3 = digitalRead(pinBotao3);
  statusB4 = digitalRead(pinBotao4);
  statusB5 = digitalRead(pinBotao5);
  statusB6 = digitalRead(pinBotao6);
  statusEncSw = !digitalRead(pinoEncSW);

  Keyboard.begin();
  if (statusB1) {
    cmd1();
  } else if (statusB2) {
    cmd2();
  } else if (statusB3) {
    cmd3();
  } else if (statusB4) {
    cmd4();
  } else if (statusB5) {
    cmd5();
  } else if (statusB6) {
    cmd6();
  } else if (statusEncSw) {
    //cmd2();
  }
  Keyboard.end();
}

void leituraEncoder () {
  CLK = digitalRead(pinoEncCLK);
  DT = digitalRead(pinoEncDT);
  if (CLK != DT) {
    if (CLK) {
      contador++;
      if (contador >= 125) contador = 127;
    } else {
      contador--;
      if (contador <= -125) contador = 127;
    }
  } else {
    contador = 0;
  }
}

void cmd1() {
  acionaOBS();
  delay(100);
  Keyboard.press(controlKey);
  Keyboard.press(optionKey);
  Keyboard.press('1');
  delay(50);
  Keyboard.releaseAll();
  delay(100);
  trocaCena();
}

void cmd2() {
  acionaOBS();
  delay(100);
  Keyboard.press(controlKey);
  Keyboard.press(optionKey);
  Keyboard.press('2');
  delay(50);
  Keyboard.releaseAll();
  delay(100);
  trocaCena();
}

void cmd3() {
  acionaOBS();
  delay(100);
  Keyboard.press(controlKey);
  Keyboard.press(optionKey);
  Keyboard.press('3');
  delay(50);
  Keyboard.releaseAll();
  delay(100);
  trocaCena();
}

void cmd4() {
  acionaOBS();
  delay(100);
  Keyboard.press(controlKey);
  Keyboard.press(optionKey);
  Keyboard.press('4');
  delay(50);
  Keyboard.releaseAll();
  delay(100);
  trocaCena();
}

void cmd5() {
  acionaOBS();
  delay(100);
  Keyboard.press(controlKey);
  Keyboard.press(optionKey);
  Keyboard.press('5');
  delay(50);
  Keyboard.releaseAll();
  delay(100);
  trocaCena();
}

void cmd6() {
  acionaOBS();
  delay(100);
  Keyboard.press(controlKey);
  Keyboard.press(optionKey);
  Keyboard.press('6');
  delay(50);
  Keyboard.releaseAll();
  delay(100);
  trocaCena();
}

void trocaCena() {
  Keyboard.press(commandKey);
  Keyboard.press('6');
  delay(50);
  Keyboard.releaseAll();
  delay(100);
  Keyboard.press(commandKey);
  Keyboard.press(tabKey);
  delay(50);
  Keyboard.releaseAll();
}

void acionaOBS() {
  Keyboard.press(controlKey);
  Keyboard.press(optionKey);
  Keyboard.press('0');
  delay(50);
  Keyboard.releaseAll();
}

void refreshDisplays() {
  // Atualiza gráficos dos displays, pode-se colocar imagens se quiser, estão no arquivo logos.h e com comentário aqui embaixo
  selDisp1();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(" Cena 1");
  //display.drawBitmap(0, 0, Normal_allArray[0], 128, 64, SSD1306_WHITE);
  display.display();

  selDisp2();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(" Cena 2");
  //display.drawBitmap(0, 0, Normal_allArray[1], 128, 64, SSD1306_WHITE);
  display.display();

  selDisp3();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(" Cena 3");
  //display.drawBitmap(0, 0, Normal_allArray[2], 128, 64, SSD1306_WHITE);
  display.display();

  selDisp4();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(" Cena 4");
  //display.drawBitmap(0, 0, Normal_allArray[3], 128, 64, SSD1306_WHITE);
  display.display();

  selDisp5();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(" Cena 5");
  //display.drawBitmap(0, 0, Normal_allArray[4], 128, 64, SSD1306_WHITE);
  display.display();

  selDisp6();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(" Cena 6");
  //display.drawBitmap(0, 0, Normal_allArray[5], 128, 64, SSD1306_WHITE);
  display.display();
}

void selDisp1() {
  digitalWrite(pinAddA, LOW);
  digitalWrite(pinAddB, LOW);
  digitalWrite(pinAddC, LOW);
}

void selDisp2() {
  digitalWrite(pinAddA, HIGH);
  digitalWrite(pinAddB, LOW);
  digitalWrite(pinAddC, LOW);
}
void selDisp3() {
  digitalWrite(pinAddA, HIGH);
  digitalWrite(pinAddB, HIGH);
  digitalWrite(pinAddC, LOW);
}
void selDisp4() {
  digitalWrite(pinAddA, LOW);
  digitalWrite(pinAddB, HIGH);
  digitalWrite(pinAddC, LOW);
}
void selDisp5() {
  digitalWrite(pinAddA, HIGH);
  digitalWrite(pinAddB, LOW);
  digitalWrite(pinAddC, HIGH);
}
void selDisp6() {
  digitalWrite(pinAddA, LOW);
  digitalWrite(pinAddB, LOW);
  digitalWrite(pinAddC, HIGH);
}
