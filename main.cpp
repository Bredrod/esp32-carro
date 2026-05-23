#include <math.h>
//===== controle micro SD ======//
#include <SPI.h>
#include <SD.h>
#define SD_CS 5
File arquivo;
// ocuparemos também os pinos 18, 19, 23 // 

//===== iniciando bluetooth ====//
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

//=============================//
#define LED_AZUL 2
#define LED_VERDE 13
#define LED_VERMELHO 14
#define TERMISTOR1 39
#define BOTAO_RTD 34
#define BOTAO_DOWN 35
#define DIV_TENS 27
#define MOTOR1_H 4
#define MOTOR1_L 25
#define MOTOR2_H 26
#define MOTOR2_L 16
#define HALL_PIN 36
#define BUZZER 33

//variavel estado
int estado = 0; 


//======== sensor hall ========//
volatile int pulsos = 0;
volatile unsigned long ultimoPulso = 0;

void IRAM_ATTR contarPulso() {
    pulsos++;
    ultimoPulso = millis();
}

//======= funcao leitura de tensao =======//
float lerTensao(){

  int leitura = analogRead(DIV_TENS);

  float tensaoESP =
  leitura * (3.3 / 4095.0);

  float tensaoReal =
  tensaoESP * 3.0;

  return tensaoReal;

}

//===== funcao leitura de temperatura ======//
float lerTemperatura(){

    int leitura = analogRead(TERMISTOR1);

    float tensao = leitura * (3.3 / 4095.0);

    // resistor fixo
    float Rfixo = 10000.0;

    // resistencia do NTC
    float Rntc = Rfixo * ((3.3 / tensao) - 1.0);

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

//velocidade a definir circunferencia
float circunferencia_roda = 0;
float distancia = 0;
float velocidade = 0;



void setup() {
//====== inicialização suporte SD =======//
Serial.begin(115200);
if(!SD.begin(SD_CS)) {
  Serial.println("Erro no SD");
  while(true);
}
Serial.println("SD pronto");

//==== iniciando bluetooth ===//
	SerialBT.begin("ESP32");
	
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
attachInterrupt(digitalPinToInterrupt(HALL_PIN), contarPulso, FALLING);
}

void loop(){

  // ======= dados adquiridos =======//
  float temperatura = lerTemperatura();
  float tensao = lerTensao();


  //======= RPM, velocidade e distancia em um segundo =======//
int pulsosTemp = 0;
int rpm = 0;  

if (tempoAtual - tempoAnterior >= intervalo) {
  noInterrupts();
   pulsosTemp = pulsos;
	 pulsos = 0;
    interrupts();

    // DISTÂNCIA percorrida nesse intervalo
 distancia = pulsosTemp * circunferencia_roda; //a definir circunferencia

    // VELOCIDADE (m/s)
    velocidade = distancia / 1.0; // 1 segundo
	
	 //distancia total
  int distancia_total = 0;
  distancia_total += pulsosTemp * circunferencia_roda;
}
 

//======== código de todo o sistema a partir daqui ========//

    //leitura botoes
    digitalRead(BOTAO_RTD);
    digitalRead(BOTAO_DOWN);

    //==== sistema shutdown =====//
     bool motorLigado = (millis() - ultimoPulso < 500);
if (temperatura < 0 || temperatura > 45 || tensao < 6 || (!motorLigado) || BOTAO_DOWN == HIGH){
    estado = 5;
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
	 
  	if(temperatura > 0 && temperatura < 45){  
    digitalWrite(LED_VERMELHO, HIGH);
    digitalWrite(LED_VERDE,HIGH);
    delay(2000);
    estado = 2;
  	}else{
	estado = 0;
  	}
}

 if (estado == 2){
  
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(BUZZER, HIGH);
  delay(3000);
  digitalWrite(BUZZER, LOW);
  
  estado = 3;
}

if (estado == 3){
	digitalWrite(LED_VERDE, HIGH);
	digitalWrite(MOTOR1_H, HIGH);
  	digitalWrite(MOTOR1_L, LOW);
	digitalWrite(MOTOR2_H, HIGH);
  	digitalWrite(motor2_L, LOW);

	//====== Vamos usar esta aba para alocar os dados requeridos no microSD, repetindo o mesmo cabecalho, e controle tempo =======//
if (tempoAtual - tempoAnterior >= intervalo) {
  
arquivo = SD.open("/dados.txt", FILE_APPEND);

if(arquivo) {
arquivo.print(" temperatura em celsius: ");
arquivo.println(temperatura);
arquivo.print(" tensao em volts: ");
arquivo.println(tensao);
arquivo.print(" RPM: ");
arquivo.print(rpm);
arquivo.print(" velocidade m/s: ");
arquivo.println(velocidade);
arquivo.print(" distancia percorrida: ");
arquivo.println(distancia);	
	
arquivo.close();
Serial.print("Dados salvos");
SerialBT.print("Dados salvos");	
    }
  }
//==== imprimindo informações no painel serial ====//
Serial.print(" temperatura em celsius: ");
Serial.println(temperatura);
Serial.print(" tensao em volts: ");
Serial.println(tensao);
Serial.print(" RPM: ");
Serial.print(rpm);
Serial.print(" velocidade m/s: ");
Serial.println(velocidade);
Serial.print(" distancia percorrida: ");
Serial.println(distancia);	
	
// imprimindo no serial bluetooth
SerialBT.print(" temperatura em celsius: ");
SerialBT.println(temperatura);
SerialBT.print(" tensao em volts: ");
SerialBT.println(tensao);
SerialBT.print(" RPM: ");
SerialBT.print(rpm);
SerialBT.print(" velocidade m/s: ");
SerialBT.println(velocidade);
SerialBT.print(" distancia percorrida: ");
SerialBT.println(distancia);	
}
