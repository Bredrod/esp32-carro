
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
