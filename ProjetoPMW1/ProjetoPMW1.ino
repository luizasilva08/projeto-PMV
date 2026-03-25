#include <Wire.h>
#include <LiquidCrystal_I2C.h>  //precisa baixar biblioteca LiquidCrystal_I2C
#include <DHT.h> // precisa baixar biblioteca dht sensor library

// --- PINOS ---
#define LDR_PIN A0
#define DHTPIN A1    
#define TRIG_PIN 7
#define ECHO_PIN 8
#define LED_PIN 13

// --- CONFIGURAÇÕES DE OBJETOS ---
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- LIMITES ---
#define VELOCIDADE_MAX_DIA 10.0
#define VELOCIDADE_MAX_NOITE 20.0

// --- VARIÁVEIS GLOBAIS (Compartilhadas entre as abas) ---
// Ultrassônico
unsigned long tempoAnterior = 0;
const long intervaloMedicao = 500;
float distanciaAnterior = 0;
float velocidadeAtual = 0;
float metrosParaCidade = 5000.0;
float velocidadeDisplay = 0;
unsigned long tempoPicoVelocidade = 0;

// DHT11
unsigned long tempoAnteriorDHT = 0;
const long intervaloDHT = 2000;
float temperatura = 0;
float umidade = 0;

// LDR e LED
bool estaDeNoite = false;
float limiteAtual = VELOCIDADE_MAX_DIA;
unsigned long tempoUltimaInfracao = 0;

// Display e Relógio
int telaAtual = 0;
unsigned long tempoTela = 0;
const long intervaloTela = 3000;
bool limparTela = true;
unsigned long tempoRelogioAnterior = 0;
int dia = 12; int mes = 3; int hora = 12; int minuto = 0; int segundo = 0;

void setup() {
  Serial.begin(9600);
  
  // Chama as configurações de cada aba
  setup_DHT();
  setup_Ultra();
  setup_Luz();
  setup_Display();

  Serial.println("Iniciando PMV...");
  delay(2000);
}

void loop() {
  unsigned long tempoAtual = millis();

  // Chama o "loop" de cada aba (cada pessoa cuida do seu)
  loop_Luz(tempoAtual);
  loop_DHT(tempoAtual);
  loop_Ultra(tempoAtual);
  loop_Display(tempoAtual);
}
