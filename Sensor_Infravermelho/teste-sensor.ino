// definição de pinos do sensor de linha 1
const int pinOUT1 = 8;
const int ECHO1 = 9;

// definicao de pinos do sensor de linha 2
const int pinOUT2 = 10;
const int ECHO1 = 11;

// dados de leitura dos sensores
int IRvalue1 = 0;
int IRvalue2 = 0;


void setup()
{
  // configuração dos pinos do sensor 1
  pinMode(pinOUT1,INPUT);
  pinMode(ECHO1,OUTPUT);
  
  // configuração dos pinos do sensor 2
  pinMode(pinOUT2,INPUT);
  pinMode(ECHO2,OUTPUT);
  
  // Inicialização
  Serial.begin(9600);
  Serial.println("Iniciando leitura dos sensores de Infravermelho...");


}

void loop()
{
  Serial.print("Leitura sensor 1: ");
  Serial.println(IRvalue1);
  Serial.print("Leitura sensor 2: ");
  Serial.println(IRvalue2);
  delay(500);
  
  IRvalue1 = digitalRead(pinOUT1);
  IRvalue2 = digitalRead(pinOUT2);

}
