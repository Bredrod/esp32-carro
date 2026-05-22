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
    float temperaturaK =
    1.0 /
    (
        (1.0 / T0)
        +
        (1.0 / Beta)
        *
        log(Rntc / R0)
    );

    // Kelvin -> Celsius
    float temperaturaC =
    temperaturaK - 273.15;

    return temperaturaC;
}


//Controle de tempo
unsigned long tempoAnterior = 0;
const long intervalo = 1000;



void setup() {
//====== inicialização suporte SD =======//
Serial.begin(115200);
if(!SD.begin(SD_CS) {
  Serial.println("Erro no SD");
  while(true);
}
Serial.println("SD pronto");
}

void loop(){
  // ======= dados adquiridos =======//
  float temperatura = lerTemperatura();
  float tensao = lerTensao();

//======== código de todo o sistema a partir daqui ========//




  
  
  
  
  
  
  //====== Vamos usar esta aba para alocar os dados requeridos no microSD, repetindo o mesmo cabecalho, e controle tempo =======//
if (tempoAtual - tempoAnterior >= intervalo) {
//nesse momento vamos atribuir uma variavel para cada dado a ser guardado//
int ...;

arquivo = SD.open("/dados.txt", FILE_APPEND);

if(arquivo) {
arquivo.print("---: ---);
  arquivo.print(---)

  arquivo.close();
  Serial.print("Dados salvos");
    }
  }
}
