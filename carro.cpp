#include <math.h>

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2oEF18NbX"
#define BLYNK_TEMPLATE_NAME "Telemetria Tesla"
#define BLYNK_AUTH_TOKEN "31qqdUd7bzKjkoZZPrCm1eL2PgXMry53"

float Temperatura;
float RPM;
float Velocidade;
float Distancia;
float Tensao;



// objeto timer
BlynkTimer timer;



// dados da rede
const char* ssid = "M34 de Jao";
const char* senha = "epiphonecasino";

unsigned long tempoAnterior = 0;
unsigned long intervalo = 1000;


#define led_vermelho 2
#define led_verde 4
#define led_azul 15
#define TERMISTOR1 33
#define BOTAO_RTD 34
#define BOTAO_DOWN 35
#define DIV_TENS 26
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
    pulsos++;
}

//velocidade a definir circunferencia
float circunferencia_roda = 0.14;
float distancia = 0;
float velocidade = 0;
float distancia_total = 0;
int rpm = 0;  

float lerTensao(){

  int leitura = analogRead(DIV_TENS);

  float tensaoESP =
  leitura * (3.3 / 4095.0);

  float tensaoReal =
  tensaoESP * 11.0;

  return tensaoReal;

}



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


// função chamada periodicamente
void myTimer() {

  // leitura analógica
  Temperatura = lerTemperatura();
  RPM = rpm;
  Velocidade = velocidade;
  Distancia = distancia_total;
  Tensao = lerTensao();

  Blynk.virtualWrite(V0, Temperatura);
  Blynk.virtualWrite(V1, RPM);
  Blynk.virtualWrite(V2, Velocidade);
  Blynk.virtualWrite(V3, Distancia);
  Blynk.virtualWrite(V4, Tensao);

  // mostra no serial
  Serial.print("Temperatura:")
  Serial.println(Temperatura);
  Serial.print("RPM")
  Serial.println(RPM);
  Serial.print("Velocidade:")
  Serial.println(Velocidade);
  Serial.print("Distancia:")
  Serial.println(Distancia);
  Serial.print("Tensao:")
  Serial.println(Tensao);
}

int estado = 0;
void setup() {
  Serial.begin(115200);

   Blynk.begin(BLYNK_AUTH_TOKEN, "M34 de Jao", "epiphonecasino");

  // inicia Wi-Fi
  WiFi.begin(ssid, senha);

  Serial.print("Conectando");

  // espera conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");

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

    analogSetPinAttenuation(DIV_TENS, ADC_11db);

attachInterrupt(digitalPinToInterrupt(HALL_PIN), contarPulso, FALLING);
}

void loop(){
   Blynk.run();
  float tensao = lerTensao();
  float temp = lerTemperatura();
  unsigned long tempoAtual = millis();



if(estado == 0 || estado == 4){

    digitalWrite(led_vermelho, HIGH);

    if(digitalRead(BOTAO_RTD) == HIGH){

        digitalWrite(led_vermelho, LOW);

        delay(200);

        estado = 1;
    }
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

    digitalWrite(MOTOR2_H, HIGH);
    digitalWrite(MOTOR2_L, LOW);
  }

if(digitalRead(BOTAO_DOWN) == HIGH || tensao < 6 ){
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




int pulsosTemp = 0;
int leitura = digitalRead(TERMISTOR1);
 if (tempoAtual - tempoAnterior >= intervalo){
  tempoAnterior = tempoAtual;
  
    noInterrupts();
   pulsosTemp = pulsos;
	 pulsos = 0;
    interrupts();

	//rpm
	rpm = pulsosTemp * 60;

    // DISTÂNCIA percorrida nesse intervalo
 distancia = pulsosTemp * circunferencia_roda; //a definir circunferencia

    // VELOCIDADE (m/s)
    velocidade = distancia / 1; // 1 segundo
	
	 //distancia total
  distancia_total += pulsosTemp * circunferencia_roda;
  
 }
 
}
