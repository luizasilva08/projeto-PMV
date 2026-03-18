#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Definição dos Pinos
#define LDR_PIN A0
#define DHTPIN A1    
#define TRIG_PIN 7
#define ECHO_PIN 8
#define LED_PIN 13

// Configuração do DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Limites de velocidade
#define VELOCIDADE_MAX_DIA 10.0
#define VELOCIDADE_MAX_NOITE 20.0

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variáveis de Distância, Velocidade e Destino
unsigned long tempoAnterior = 0;
const long intervaloMedicao = 500;
float distanciaAnterior = 0;
float velocidadeAtual = 0;

// Variáveis do DHT11
unsigned long tempoAnteriorDHT = 0;
const long intervaloDHT = 2000;
float temperatura = 0;
float umidade = 0;

// Distância total até a próxima cidade (Começa em 5000 metros)
float metrosParaCidade = 5000.0;

// Variáveis de "Memória"
float velocidadeDisplay = 0;
unsigned long tempoUltimaInfracao = 0;
unsigned long tempoPicoVelocidade = 0;

// Controle de Telas (Agora são 5 telas na ordem solicitada)
int telaAtual = 0;
unsigned long tempoTela = 0;
const long intervaloTela = 3000;
bool limparTela = true;
bool estaDeNoite = false;

// Relógio Interno (Simulado)
unsigned long tempoRelogioAnterior = 0;
int dia = 12; int mes = 3; int hora = 12; int minuto = 0; int segundo = 0;

void setup() {
  Serial.begin(9600); // INICIA A COMUNICAÇÃO COM O SERIAL MONITOR

  pinMode(LDR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  dht.begin(); // Inicia o sensor DHT11

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
 
  lcd.print("Iniciando PMV...");
  Serial.println("Iniciando PMV..."); // Envia para o Serial Monitor
 
  delay(2000);
}

float lerDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
 
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duracao == 0) return distanciaAnterior;
  return (duracao * 0.0343) / 2;
}

void atualizarRelogio() {
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoRelogioAnterior >= 1000) {
    tempoRelogioAnterior = tempoAtual;
    segundo++;
    if (segundo >= 60) {
      segundo = 0; minuto++;
      if (minuto >= 60) {
        minuto = 0; hora++;
        if (hora >= 24) { hora = 0; dia++; }
      }
    }
    // Tela 0 é a de Data/Hora, atualiza a cada segundo
    if(telaAtual == 0) atualizarDisplay();
  }
}

void loop() {
  unsigned long tempoAtual = millis();
  atualizarRelogio();

  // Continua lendo a luz para a regra de limite de velocidade, mesmo sem mostrar no display
  estaDeNoite = (analogRead(LDR_PIN) > 512);
  float limiteAtual = estaDeNoite ? VELOCIDADE_MAX_NOITE : VELOCIDADE_MAX_DIA;

  // 1. LEITURA DO DHT11 (A cada 2 segundos)
  if (tempoAtual - tempoAnteriorDHT >= intervaloDHT) {
    tempoAnteriorDHT = tempoAtual;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
   
    if (!isnan(t) && !isnan(h)) {
      temperatura = t;
      umidade = h;
    }
   
    // Atualiza se estiver na tela de Temp/Umidade (Tela 1)
    if(telaAtual == 1) atualizarDisplay();
  }

  // 2. CÁLCULO DE VELOCIDADE E DISTÂNCIA
  if (tempoAtual - tempoAnterior >= intervaloMedicao) {
    float distanciaAtual = lerDistancia();
    float deltaDistancia = abs(distanciaAnterior - distanciaAtual);
    float deltaTempo = intervaloMedicao / 1000.0;
   
    velocidadeAtual = (deltaDistancia / deltaTempo) * 3.6;
   
    if (deltaDistancia > 0) {
      metrosParaCidade = metrosParaCidade - deltaDistancia;
      if (metrosParaCidade < 0) metrosParaCidade = 0;
    }

    if (velocidadeAtual > 5.0) {
      velocidadeDisplay = velocidadeAtual;
      tempoPicoVelocidade = tempoAtual;
    }

    if (velocidadeAtual > limiteAtual) {
      digitalWrite(LED_PIN, HIGH);
      tempoUltimaInfracao = tempoAtual;
    }

    distanciaAnterior = distanciaAtual;
    tempoAnterior = tempoAtual;
   
    // Atualiza se estiver nas telas de sensor(2) ou cidade(3)
    if(telaAtual == 2 || telaAtual == 3) atualizarDisplay();
  }

  // 3. TEMPORIZADORES DE DESLIGAMENTO DO LED E ZERAR VELOCIDADE
  if (digitalRead(LED_PIN) == HIGH && (tempoAtual - tempoUltimaInfracao >= 3000)) {
    digitalWrite(LED_PIN, LOW);
  }
 
  if (velocidadeDisplay > 0 && (tempoAtual - tempoPicoVelocidade >= 3000)) {
    velocidadeDisplay = 0;
    if(telaAtual == 2) atualizarDisplay(); // Atualiza tela de velocidade
  }

  // 4. TROCA DE TELAS (Agora são 5 telas: 0 a 4)
  if (tempoAtual - tempoTela >= intervaloTela) {
    tempoTela = tempoAtual;
    telaAtual = (telaAtual + 1) % 5;
    limparTela = true;
    atualizarDisplay();
  }
}

void atualizarDisplay() {
  if (limparTela) {
    lcd.clear();
    Serial.println("\n--- MUDANCA DE TELA ---"); // Separador no Serial Monitor
    limparTela = false;
  }

  switch (telaAtual) {
    case 0: // 1. DATA E HORA
      // Atualiza o LCD
      lcd.setCursor(0, 0);
      lcd.print("Data: ");
      if(dia < 10) lcd.print('0'); lcd.print(dia); lcd.print('/');
      if(mes < 10) lcd.print('0'); lcd.print(mes);
      lcd.setCursor(0, 1);
      lcd.print("Hora: ");
      if(hora < 10) lcd.print('0'); lcd.print(hora); lcd.print(':');
      if(minuto < 10) lcd.print('0'); lcd.print(minuto); lcd.print(':');
      if(segundo < 10) lcd.print('0'); lcd.print(segundo);

      // Atualiza o Serial Monitor
      Serial.print("[INFO] Data: ");
      if(dia < 10) Serial.print('0'); Serial.print(dia); Serial.print('/');
      if(mes < 10) Serial.print('0'); Serial.print(mes);
      Serial.print(" | Hora: ");
      if(hora < 10) Serial.print('0'); Serial.print(hora); Serial.print(':');
      if(minuto < 10) Serial.print('0'); Serial.print(minuto); Serial.print(':');
      if(segundo < 10) Serial.print('0'); Serial.println(segundo);
      break;

    case 1: // 2. TEMPERATURA E UMIDADE JUNTAS
      // Atualiza o LCD
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperatura, 1);
      lcd.print((char)223);
      lcd.print("C    ");
      lcd.setCursor(0, 1);
      lcd.print("Umid: ");
      lcd.print(umidade, 1);
      lcd.print(" %    ");

      // Atualiza o Serial Monitor
      Serial.print("[INFO] Temp: ");
      Serial.print(temperatura, 1);
      Serial.print(" C | Umid: ");
      Serial.print(umidade, 1);
      Serial.println(" %");
      break;

    case 2: // 3. DISTÂNCIA DO SENSOR E VELOCIDADE
      // Atualiza o LCD
      lcd.setCursor(0, 0);
      lcd.print("Mts Sensor: ");
      lcd.print((int)distanciaAnterior);
      lcd.print("  ");
      lcd.setCursor(0, 1);
      lcd.print("Vel: ");
      lcd.print((int)velocidadeDisplay);
      lcd.print(" km/h    ");

      // Atualiza o Serial Monitor
      Serial.print("[INFO] Mts Sensor: ");
      Serial.print((int)distanciaAnterior);
      Serial.print(" m | Vel: ");
      Serial.print((int)velocidadeDisplay);
      Serial.println(" km/h");
      break;

    case 3: // 4. DISTÂNCIA PARA A PRÓXIMA CIDADE
      // Atualiza o LCD
      lcd.setCursor(0, 0);
      if (metrosParaCidade > 0) {
        lcd.print("Faltam ");
        lcd.print((int)metrosParaCidade);
        lcd.print(" mts  ");
        lcd.setCursor(0, 1);
        lcd.print("proxima cidade  ");
       
        // Atualiza o Serial Monitor
        Serial.print("[INFO] Faltam ");
        Serial.print((int)metrosParaCidade);
        Serial.println(" mts para a proxima cidade");
      } else {
        lcd.print("Voce chegou!    ");
        lcd.setCursor(0, 1);
        lcd.print("                ");

        // Atualiza o Serial Monitor
        Serial.println("[INFO] Voce chegou ao seu destino!");
      }
      break;

    case 4: // 5. MENSAGEM FINAL
      // Atualiza o LCD
      lcd.setCursor(2, 0);
      lcd.print("BOA VIAGEM!   ");
      lcd.setCursor(2, 1);
      lcd.print("Dirija bem.   ");

      // Atualiza o Serial Monitor
      Serial.println("[INFO] BOA VIAGEM! Dirija bem.");
      break;
  }
}