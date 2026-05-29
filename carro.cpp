#include <math.h>


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

int estado = 0;
void setup() {
  Serial.begin(9600);
 
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
  

  Serial.print("velocidade:");
  Serial.println(velocidade);
  Serial.print("distancia:");
  Serial.println(distancia);
  Serial.print("distancia total");
  Serial.println(distancia_total);
  Serial.print("temperatura:");
  Serial.println(temp);
  Serial.print("tensao:");
  Serial.println(tensao);
    }

}
