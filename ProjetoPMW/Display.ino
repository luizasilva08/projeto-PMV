void setup_Display() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando PMV...");
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
    if(telaAtual == 0) atualizarDisplay();
  }
}

void loop_Display(unsigned long tempoAtual) {
  atualizarRelogio();

  // Temporizador para trocar a tela
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
    Serial.println("\n--- MUDANCA DE TELA ---");
    limparTela = false;
  }

  switch (telaAtual) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Data: ");
      if(dia < 10) lcd.print('0'); lcd.print(dia); lcd.print('/');
      if(mes < 10) lcd.print('0'); lcd.print(mes);
      lcd.setCursor(0, 1);
      lcd.print("Hora: ");
      if(hora < 10) lcd.print('0'); lcd.print(hora); lcd.print(':');
      if(minuto < 10) lcd.print('0'); lcd.print(minuto); lcd.print(':');
      if(segundo < 10) lcd.print('0'); lcd.print(segundo);
      break;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Temp: "); lcd.print(temperatura, 1); lcd.print((char)223); lcd.print("C    ");
      lcd.setCursor(0, 1);
      lcd.print("Umid: "); lcd.print(umidade, 1); lcd.print(" %    ");
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Mts Sensor: "); lcd.print((int)distanciaAnterior); lcd.print("  ");
      lcd.setCursor(0, 1);
      lcd.print("Vel: "); lcd.print((int)velocidadeDisplay); lcd.print(" km/h    ");
      break;

    case 3:
      lcd.setCursor(0, 0);
      if (metrosParaCidade > 0) {
        lcd.print("Faltam "); lcd.print((int)metrosParaCidade); lcd.print(" mts  ");
        lcd.setCursor(0, 1);
        lcd.print("proxima cidade  ");
      } else {
        lcd.print("Voce chegou!    ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
      }
      break;

    case 4:
      lcd.setCursor(2, 0); lcd.print("BOA VIAGEM!   ");
      lcd.setCursor(2, 1); lcd.print("Dirija bem.   ");
      break;
  }
}
