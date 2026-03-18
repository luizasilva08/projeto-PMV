void setup_DHT() {
  dht.begin(); // Inicia o sensor DHT11
}

void loop_DHT(unsigned long tempoAtual) {
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
}
