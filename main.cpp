#include <math.h>
//===== controle micro SD ======//
#include <SPI.h>
#include <SD.h>
#define SD_CS 5
file arquivo;
// ocuparemos também os pinos 18, 19, 23 // 
//=============================//
#define LED_AZUL 2
#define LED_VERDE 13
#define LED_VERMELHO 14
#define TERMISTOR1 21
#define BOTAO_RTD 34
#define BOTAO_DOWN 35
#define DIV_TENS 27
#define MOTOR1_L 4
#define MOTOR1_R 25
#define MOTOR2_L 26
#define MOTOR2_R 16
#define HALL_PIN 36
#define BUZZER 33

//variavel estado
int estado = 0; 


//======== sensor hall ========//
volatile int pulsos = 0;

void contarPulso(){
    pulsos++;
}
volatile unsigned long ultimoPulso = 0;

void IRAM_ATTR hallISR() {
  ultimoPulso = millis();
}

//======= funcao leitura de tensao =======//
float lerTensao(){

  int leitura = analogRead(tensaoPin);

  float tensaoESP =
  leitura * (3.3 / 4095.0);

  float tensaoReal =
  tensaoESP * 3.0;

  return tensaoReal;

}

//===== funcao leitura de temperatura ======//
float lerTemperatura(){

    int leitura = analogRead(NTC_PIN);

    float tensao =
    leitura * (3.3 / 4095.0);

    // resistor fixo
    float Rfixo = 10000.0;

    // resistencia do NTC
    float Rntc =
    Rfixo * ((3.3 / tensao) - 1.0);

    // parametros do NTC
    float Beta = 3950.0;
    float T0 = 298.15; // 25°C em Kelvin
    float R0 = 10000.0;

    // equacao Beta
    float temperaturaK =  1.0 / (  (1.0 / T0)  +  (1.0 / Beta)   *  log(Rntc / R0) );

    // Kelvin -> Celsius
    float temperaturaC =
    temperaturaK - 273.15;

    return temperaturaC;
}


//Controle de tempo
unsigned long tempoAnterior = 0;
const long intervalo = 1000;

//velocidade
float circunferencia_roda;



void setup() {
//====== inicialização suporte SD =======//
Serial.begin(115200);
if(!SD.begin(SD_CS) {
  Serial.println("Erro no SD");
  while(true);
}
Serial.println("SD pronto");

    
//==== definindo pinos ======//
    pinMode(LED_AZUL, OUTPUT);
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);
    pinMode(TERMISTOR1, INPUT);
    pinMode(BOTAO_RTD, INPUT);
    pinMode(BOTAO_DOWN, INPUT);
    pinMode(DIV_TENS, INPUT);
    pinMode(MOTOR1_H, OUTPUT);
    pinMode(MOTOR1_L, OUTPUT);
    pinMode(MOTOR2_H, OUTPUT);
    pinMode(MOTOR2_L, OUTPUT);
    pinMode(HALL_PIN, INPUT);
	pinMode(BUZZER, OUTPUT);
    
  //===== inicialização sensor hall =======//
pinMode(36, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(27), contarPulso, FALLING);
}

void loop(){
  // ======= dados adquiridos =======//
  float temperatura = lerTemperatura();
  float tensao = lerTensao();

  //========= RPM =========//
     pulsos = 0;
     int rpm = pulsos * 60;

  //======= velocidade e distancia em um segundo =======//
if (tempoAtual - tempoAnterior >= intervalo) {
  noInterrupts();
    int pulsosTemp = pulsos;
    pulsos = 0;
    interrupts();

    // DISTÂNCIA percorrida nesse intervalo
    float distancia = pulsosTemp * circunferencia_roda; //a definir circunferencia

    // VELOCIDADE (m/s)
    velocidade = distancia / 1.0; // 1 segundo
}
  //distancia total
  int distancia_total;
  distancia_total += pulsosTemp * circunferencia_roda;

//======== código de todo o sistema a partir daqui ========//

    //leitura botoes
    digitalRead(BOTAO_RTD);
    digitalRead(BOTAO_DOWN);

    //==== sistema shutdown =====//
     bool motorLigado = (millis() - ultimoPulso < 500);
if (temperatura < 0 || temperatura > 45 || tensao < 6 || (motorLigado) || BOTAO_DOWN == HIGH){
    estado = 5
        }

//====== sistema RTD =======//
    if(estado == 0 || estado == 5){
	digitalWrite(LED_VERMELHO,HIGH);
}

 if(digitalRead(BOTAO_RTD) == HIGH){
  digitalWrite(LED_VERMELHO, LOW);
   delay(200); 
  estado = 1;
}

 if(estado == 1){
	 
  	if(temperatura > 0 || temperatura < 45){  
    digitalWrite(vermelho, HIGH);
    digitalWrite(verde,HIGH);
    delay(2000);
    estado = 2;
  	}else{
	estado = 0;
  	}
}

 if (estado == 2){
  
  digitalWrite(LED_VERMELHO,LOW);
  digitalWrite(LED_VERDE,LOW);
  digitalWrite(buzzer,HIGH);
  delay(3000);
  digitalWrite(buzzer,LOW);
  
  estado = 3;
}

if (estado == 3){
	digitalWrite(LED_VERDE, HIGH);
	


  
  
  
  
  
  
  //====== Vamos usar esta aba para alocar os dados requeridos no microSD, repetindo o mesmo cabecalho, e controle tempo =======//
if (tempoAtual - tempoAnterior >= intervalo) {
  
arquivo = SD.open("/dados.txt", FILE_APPEND);

if(arquivo) {
arquivo.print("---: ---);
  arquivo.print(---)

  arquivo.close();
  Serial.print("Dados salvos");
    }
  }
}
