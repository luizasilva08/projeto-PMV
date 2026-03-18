void setup_Ultra() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
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

void loop_Ultra(unsigned long tempoAtual) {
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
    
    if(telaAtual == 2 || telaAtual == 3) atualizarDisplay();
  }

  // Zerar velocidade da memória (display)
  if (velocidadeDisplay > 0 && (tempoAtual - tempoPicoVelocidade >= 3000)) {
    velocidadeDisplay = 0;
    if(telaAtual == 2) atualizarDisplay(); 
  }
}
