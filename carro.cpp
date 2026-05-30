#include <math.h>

#include <FS.h>
#include <SD.h>
#include <SPI.h>
#define SD_CS 5



//inicialização wifi
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2oEF18NbX"
#define BLYNK_TEMPLATE_NAME "Telemetria Tesla"
#define BLYNK_AUTH_TOKEN "uukaS9A9TKgGur3neHDQjQSVWYr3XHO-"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// dados da rede
const char* ssid = "M34 de Jao";
const char* senha = "epiphonecasino";

//variaveis para controle de tempo
unsigned long tempoAnterior = 0;
unsigned long intervalo = 1000;


#define led_vermelho 2
#define led_verde 4
#define led_azul 15
#define TERMISTOR1 33
#define BOTAO_RTD 34
#define BOTAO_DOWN 35
#define DIV_TENS 32
#define MOTOR1_H 25
#define MOTOR1_L 13
#define MOTOR2_H 14
#define MOTOR2_L 27
#define HALL_PIN 36
#define buzzer 22




//======== sensor hall ========//
volatile int pulsos = 0;
volatile unsigned long ultimoPulso = 0;

void IRAM_ATTR contarPulso() {
  unsigned long agora = micros();

  if (agora - ultimoPulso > 5000) { // 3 ms
    pulsos++;
    ultimoPulso = agora;
  }
}

//velocidade e grandezas
float circunferencia_roda = 0.14;
float distancia = 0;
float velocidade = 0;
float distancia_total = 0;
int rpm = 0;  


//funcao para ler a tensao
float lerTensao(){

  int leitura = analogRead(DIV_TENS);

  float tensaoESP =
  leitura * (3.3 / 4095.0);

  float tensaoReal =
  tensaoESP * 11.0;

  return tensaoReal;

}


//funcao para ler a temperatura
float lerTemperatura() {

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
    float temperaturaK =
        1.0 / (
            (1.0 / T0) +
            (1.0 / Beta) * log(Rntc / R0)
        );

    // Kelvin -> Celsius
    float temperaturaC = temperaturaK - 273.15;

    return temperaturaC;
}


//elementos da telemetria
float sensorVal1;
int sensorVal2;
int sensorVal3;

BlynkTimer timer; 
float tensao;
float Temperatura;
void myTimer() 
{
  Blynk.virtualWrite(V0, Temperatura);
  Blynk.virtualWrite(V1, sensorVal1);
  Blynk.virtualWrite(V2, sensorVal2);
  Blynk.virtualWrite(V3, sensorVal3);  
  Blynk.virtualWrite(V4, tensao);
}


//micro SD
void salvarSD(){

  File arquivo = SD.open("/telemetria.csv", FILE_APPEND);

  if(arquivo){

    arquivo.print(millis()/1000);
    arquivo.print(",");

    arquivo.print(Temperatura);
    arquivo.print(",");

    arquivo.print(rpm);
    arquivo.print(",");

    arquivo.print(velocidade);
    arquivo.print(",");

    arquivo.print(distancia_total);
    arquivo.print(",");

    arquivo.println(tensao);

    arquivo.close();

    Serial.println("Dados salvos");
  }

  else{
    Serial.println("Erro ao abrir arquivo");
  }
}

//variavel de controle de estados
int estado = 0;
unsigned long tempoEstado = 0;

void setup() {
  Serial.begin(115200);
//conectando a wifi
 Blynk.begin(BLYNK_AUTH_TOKEN, ssid, senha);



  // mostra IP
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
  
 
//definindo pinos
    pinMode(BOTAO_DOWN, INPUT);
    pinMode(BOTAO_RTD, INPUT);
    pinMode(MOTOR1_H, OUTPUT);
    pinMode(MOTOR1_L, OUTPUT);
    pinMode(MOTOR2_H, OUTPUT);
    pinMode(MOTOR2_L, OUTPUT);
    pinMode(led_vermelho, OUTPUT);
    pinMode(led_verde, OUTPUT);
    pinMode(led_azul, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(TERMISTOR1, INPUT);
    pinMode(DIV_TENS, INPUT);
    pinMode(HALL_PIN, INPUT);

	//suavizar a leitura do divisor de tensao
    analogSetPinAttenuation(DIV_TENS, ADC_11db);

	//inicialização sensor hall
attachInterrupt(digitalPinToInterrupt(HALL_PIN), contarPulso, FALLING);
timer.setInterval(1000L, myTimer); 

	//inicialização SD
if(!SD.begin(SD_CS)){
  Serial.println("Erro no cartao SD");
  return;
}

Serial.println("Cartao SD iniciado");

File arquivo = SD.open("/telemetria.csv", FILE_WRITE);

if(arquivo){
  arquivo.println("Tempo,Temperatura,RPM,Velocidade,Distancia,Tensao");
  arquivo.close();
}

}

void loop(){
	//leitura de dados para impressão 
 tensao = lerTensao();
 Temperatura = lerTemperatura();
  unsigned long tempoAtual = millis();


//controle de RTD
if(estado == 0 || estado == 4){

    digitalWrite(led_vermelho, HIGH);
}
    if(digitalRead(BOTAO_RTD) == HIGH){

        digitalWrite(led_vermelho, LOW);

        delay(200);

        estado = 1;
    }



   else if(estado == 1){

    digitalWrite(led_verde, HIGH);
    digitalWrite(led_vermelho, HIGH);
    delay(1000);
    estado = 2;
    }



else if(estado == 2){
    digitalWrite(led_verde, LOW);
    digitalWrite(led_vermelho, LOW);
    digitalWrite(buzzer, HIGH);
    delay(3000);
    digitalWrite(buzzer, LOW);
    estado = 3;
    }


else if(estado == 3){

    digitalWrite(led_verde, HIGH);

    digitalWrite(MOTOR1_H, HIGH);
    digitalWrite(MOTOR1_L, LOW);

    digitalWrite(MOTOR2_H, LOW);
    digitalWrite(MOTOR2_L, HIGH);
  }

if(digitalRead(BOTAO_DOWN) == HIGH || tensao < 5 ){
  estado = 4;
}

if(estado == 4){
  digitalWrite(led_verde, LOW);
  digitalWrite(led_vermelho, HIGH);

    digitalWrite(MOTOR1_H, LOW);
    digitalWrite(MOTOR1_L, LOW);

    digitalWrite(MOTOR2_H, LOW);
    digitalWrite(MOTOR2_L, LOW);
  }



//calculo da velocidade
int pulsosTemp = 0;
int leitura = digitalRead(TERMISTOR1);
 if (tempoAtual - tempoAnterior >= intervalo){
  tempoAnterior = tempoAtual;
    salvarSD();
    noInterrupts();
   pulsosTemp = (pulsos / 48);
   float pulsosTemp1 = pulsos;

	 pulsos = 0;
    interrupts();

	//rpm
	rpm = (pulsosTemp1 * 60);

    // DISTÂNCIA percorrida nesse intervalo
 distancia = pulsosTemp * circunferencia_roda; //a definir circunferencia

    // VELOCIDADE (m/s)
    velocidade = distancia / 1; // 1 segundo
    velocidade = fabs(velocidade);
	
	 //distancia total
  distancia_total += fabs(pulsosTemp * circunferencia_roda);
  


   Serial.print("Pulsos por segundo: ");
  Serial.println(pulsosTemp);
   Serial.print("Temperatura:");
  Serial.println(Temperatura);
  Serial.print("RPM");
  Serial.println(rpm);
  Serial.print("Velocidade:");
  Serial.println(velocidade);
  Serial.print("Distancia:");
  Serial.println(distancia_total);
  Serial.print("Tensao:");
  Serial.println(tensao);
  
 }
  
 
  sensorVal1 = velocidade;
  sensorVal2 = distancia_total;
  sensorVal3 = rpm;


  Blynk.run(); 
  timer.run(); 
  
 
  }
