//Desenvolvido por Lucas Santos 2018
#include<EEPROM.h>

//Config. Componentes
#define LDR A0
#define LED_R 3
#define LED_G 4
#define LED_B 5

const uint8_t LED[] = {LED_R, LED_G, LED_B};

#define BOTAO_COR 11
#define BOTAO_CEDULA 12

//#define BUZZER 13

//Parâmetros de leitura
#define qtdLeituras 10
#define tempoLED 100 //100ms
#define tolerancia 20 //10%
#define limSup (255 * 100) / (100 + (2 * tolerancia)) //212 para tolerancia = 10%
#define limInf (255 * (2 * tolerancia)) / 100 //51 para tolerancia = 10%

//Definição das variáveis
int leitura[3];
int brancoCalibracao[] = {0, 0, 0},
    pretoCalibracao[] = {1023, 1023, 1023};

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando DICOD v2.0 ...");
  
  for (int i = 0; i < 3; i++) {
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], LOW);
  }

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  pinMode(BOTAO_COR, INPUT_PULLUP);
  pinMode(BOTAO_CEDULA, INPUT_PULLUP);
    
  Serial.flush();
  while(!Serial.available()); 

  if (EEPROM[0] != 'b' && EEPROM[4] != 'p') {
    calibrarSensor();
  } else {
    Serial.println("Recuperando dados da EEPROM...");
    for (int i = 1; i < 4; i++) {
      brancoCalibracao[i] = EEPROM[i];
      pretoCalibracao[i] = EEPROM[i + 4];
    }
    Serial.println("Pronto para começar!");
  }
}

void loop() {
  int btCor = digitalRead(BOTAO_COR),
      btCedula = digitalRead(BOTAO_CEDULA);
  if (btCor == LOW && btCedula == LOW) {
    calibrarSensor();
  } else if (btCor == LOW) {
    lerCor();
  } else if (btCedula == LOW) {
    lerCedula();
  }
}

int mediaLeituras() { //Método que faz n leituras e retorna a média
  int leituras = 0;

  for (int i = 0; i < qtdLeituras; i++) {
    leituras += analogRead(LDR);
    delay(tempoLED / qtdLeituras);
  }

  return leituras / qtdLeituras;
}

/*
void toque(int num, int freq, int tempo) { //Método de saída sonora
  for (int i = 0; i < num; i++) {
    tone(BUZZER, freq);
    delay(tempo);
    noTone(BUZZER);
    delay(tempo);
  }
}
*/

void lerSensor() { //Método que faz o procedimento de leitura de cada canal RGB da cor
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED[i], HIGH);
    //delay(tempoLED);

    leitura[i] = mediaLeituras();

    digitalWrite(LED[i], LOW);
    delay(tempoLED);
  }
}

void calibrarSensor() { //Método que faz 10 médias de cada leitura do sensor para calibração da cor branca e cor preta
  //toque(3, 33, 500);//Inicio da calibração de cor branca
  Serial.println("Iniciando Calibração...");
  delay(1000);
  Serial.println("Coloque um objeto branco na frente do sensor e pressione algum botão para calibrar");
  while (digitalRead(BOTAO_COR) == HIGH && digitalRead(BOTAO_CEDULA) == HIGH); //Aguarda pressionar algum botão
  Serial.println("Calibrando...");
  for (int i = 0; i < 10; i++) {
    lerSensor();

    for (int j = 0; j < 3; j++) {
      if (leitura[j] > brancoCalibracao[j]) {
        brancoCalibracao[j] = leitura[j];
      }
    }
  }

  EEPROM[0] = 'b';
  for (int i = 0; i < 3; i++) {
    if (brancoCalibracao[i] > EEPROM[i + 1]) {
      EEPROM[i + 1] = leitura[i];
    }
  }

  //toque(3, 98, 300);
  Serial.println("Calibração branca concluída!");
  delay(1000);
  Serial.println("Coloque um objeto preto na frente do sensor e pressione algum botão para calibrar");
  while (digitalRead(BOTAO_COR) == HIGH && digitalRead(BOTAO_CEDULA) == HIGH); //Aguarda pressionar algum botão
  Serial.println("Calibrando...");
  //toque(3, 33, 500); //Inicio da calibração de cor preta

  for (int i = 0; i < 10; i++) {
    lerSensor();

    for (int j = 0; j < 3; j++) {
      if (leitura[j] < pretoCalibracao[j]) {
        pretoCalibracao[j] = leitura[j];
      }
    }
  }

  EEPROM[4] = 'p';
  for (int i = 0; i < 3; i++) {
    if (pretoCalibracao[i] < EEPROM[i + 5]) {
      EEPROM[i + 5] = leitura[i];
    }
  }
  Serial.println("Calibração concluída!");
  delay(1000);
  Serial.println("Pronto para começar!");
  //toque(3, 98, 300); //Fim da calibração
}

void converteCores() {
  for (int i = 0; i < 3; i++) {
    leitura[i] = constrain(map(leitura[i], pretoCalibracao[i], brancoCalibracao[i], 0, 255), 0, 255);
  }
}

void lerCor() {
  Serial.println("Lendo cor...");
  delay(500);
  lerSensor();
  converteCores();
  String cor = "cor_desconhecida";
  if(branco())
    cor = "branco";
  if(preto())
    cor = "preto";
  if(vermelho())
    cor = "vermelho";
  if(verde())
    cor = "verde";
  if(azul())
    cor = "azul";
  if(ciano())
    cor = "azul_claro";
  if(amarelo())
    cor = "amarelo";
  if(magenta())
    cor = "rosa";
  
  Serial.print("(cor;");
  Serial.print(leitura[0]);
  Serial.print(",");
  Serial.print(leitura[1]);
  Serial.print(",");
  Serial.print(leitura[2]);
  Serial.print(",");
  Serial.print(cor);
  Serial.println(")");
}

boolean branco(){
  for(int i = 0; i < 3; i++){
    if(leitura[i] < limSup)
      return false;
  }
  return true;
}

boolean preto(){
  for(int i = 0; i < 3; i++){
    if(leitura[i] > limInf)
      return false;
  }
  return true;
}

boolean vermelho(){
  if(leitura[0] < limSup || leitura[1] > limInf || leitura[2] > limInf)
      return false;
  return true;
}

boolean verde(){
  if(leitura[0] > limInf || leitura[1] < limSup || leitura[2] > limInf)
      return false;
  return true;
}

boolean azul(){
  if(leitura[0] > limInf || leitura[1] > limInf || leitura[2] < limSup)
      return false;
  return true;
}

boolean magenta(){
  if(leitura[0] < limSup || leitura[1] > limInf || leitura[2] < limSup)
      return false;
  return true;
}

boolean ciano(){
  if(leitura[0] > limInf || leitura[1] < limSup || leitura[2] < limSup)
      return false;
  return true;
}

boolean amarelo(){
  if(leitura[0] < limSup || leitura[1] < limSup || leitura[2] > limInf)
      return false;
  return true;
}

void lerCedula() {
  Serial.println("Lendo cédula...");
  delay(500);

  String cedula =  "5";
  if(azul()){
    //pode ser 2 reais
    cedula = "2";
  }

  Serial.print("(cedula;");
  Serial.print(cedula);
  Serial.println(")");
}

