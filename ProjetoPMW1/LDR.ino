void setup_Luz() {
  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop_Luz(unsigned long tempoAtual) {
  // Define se é dia ou noite e atualiza o limite global
  estaDeNoite = (analogRead(LDR_PIN) > 512);
  limiteAtual = estaDeNoite ? VELOCIDADE_MAX_NOITE : VELOCIDADE_MAX_DIA;

  // Temporizador de desligamento do LED de infração
  if (digitalRead(LED_PIN) == HIGH && (tempoAtual - tempoUltimaInfracao >= 3000)) {
    digitalWrite(LED_PIN, LOW);
  }
}
