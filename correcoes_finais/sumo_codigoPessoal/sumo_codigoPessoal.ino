/*
Esquema de ligação dos módulos e sensores no arduino UNO
(pino do sensor/módulo) => (pino no arduino)
motor 1:
        L => D10
        R => D11

motor 2:
        L => D5
        R => D6

Sonar:
      trigger => D8
      echo => D9

sensor de linha (frente):
                        D => A5
sensor de linha (trás):
                        D => A4
*/

//pinos do motor 1
#define Lmot1 10
#define Rmot1 11

//pinos do motor 2
#define Lmot2 5
#define Rmot2 6

//pinos do sonar
#define SonarTrigger 8
#define SonarEcho 9


float distancia, dist1, dist2, temp_curva, tempo = 0; 

float tempo_volta = 900; //tempo necessário para o robo dar uma volta completa com a velocidade de 255 ou 100%

float tempo_dete = 5000; //tempo em que o robo permanece girando até detectar algo 

float dist_obstaculo = 50; 

int vel = 200; //velocidade de locomoção 255 -- 100% | 0 -- 0%

int infra_frente, infra_tras = 0; //variaveis que amazenam o valor referente a absorção de luz no piso da arena



//função para medir a distancia do objeto com o sonar
float dist(){
  digitalWrite(SonarTrigger,LOW);
  delayMicroseconds(2);
  
  digitalWrite(SonarTrigger, HIGH);
  delayMicroseconds(10); 
  digitalWrite(SonarTrigger,LOW);
  
  tempo = pulseIn(SonarEcho, HIGH);
  distancia = tempo/58.2;

  return distancia;
}



//função que faz o robo andar para frente com a velocidade definida
void frente(){
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, 255);
  analogWrite(Lmot2, 255);
  analogWrite(Rmot2, 0);
}

//função que faz o robo andar para trás, a entrada t representa o tempo que o robo se move, isso está diretamente ligado com a distancia percorrida 
void re(int t){
  analogWrite(Lmot1, vel);
  analogWrite(Rmot1, 0);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, vel);
  delay(t);
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, 0);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, 0);
}

//para o robo
void para(){
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, 0);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, 0);
}

// essa função faz o robo rotacionar para a direita em um valor de ang°
void viraR(float ang){
  temp_curva = tempo_volta * ang/360; //calcula o tempo que o robo demora para rotacionar ang°
         
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, vel);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, vel);
  
  delay(temp_curva);
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, 0);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, 0);
}

void viraL(float ang){
  temp_curva = tempo_volta * ang/360;
         
  analogWrite(Lmot1, vel);
  analogWrite(Rmot1, 0);
  analogWrite(Lmot2, vel);
  analogWrite(Rmot2, 0);
  
  delay(temp_curva);
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, 0);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, 0);
}

//faz o robo virar para a direita de forma indefinida, essa função é utilizada para buscar o objeto
void vira(){
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, 10);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, 10);
}

//funcao que faz o robo virar e detectar ao mesmo tempo
void viraDete(){
  
  unsigned long tempo_inicial = millis(); // Marca o tempo de início do giro

  // Faz o robô começar a girar para a direita
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, 10);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, 10);

  // Enquanto o tempo de giro não for atingido, verifica a distância
  while (millis() - tempo_inicial < tempo_dete) {
    // Verifica se há um obstáculo e se encontrar um, ele empurra o obstaculo
    delay(900); 
    if (dist() <= dist_obstaculo) {
      para();  // Para o robô se encontrar um obstáculo
      frente(); // empurra o obstaculo
    
    }else{
      para(); 
      viraR(180); 
      frente();
    }
  }

  // Depois que o tempo do giro ou a detecção do obstáculo ocorrer, o robô para de girar
  analogWrite(Lmot1, 0);
  analogWrite(Rmot1, 0);
  analogWrite(Lmot2, 0);
  analogWrite(Rmot2, 0);

}

void setup() {
  //define os pinos de entrada e saida da ponte H e do sonar
  pinMode(Lmot1, OUTPUT);
  pinMode(Rmot1, OUTPUT);
  pinMode(Lmot2, OUTPUT);
  pinMode(Rmot2, OUTPUT);
  pinMode(SonarTrigger, OUTPUT);
  pinMode(SonarEcho, INPUT);

  //inicia a comunicação serial
  Serial.begin(9600);

  //tempo que o robo demora para começar a funcionar depois de ligado
  delay(500);

  //variaveis utilizadas como referencia para a cor do piso
  infra_frente = analogRead(A5);
  infra_tras = analogRead(A4);
}

void loop() {
  //no primeiro if o robo verifica se existe um objeto a menos de 50cm de distancia, caso exista ele se move para frente, caso contrario ele vira para a direita até encontrar um objeto que satisfaça a condição
  if(dist() <= dist_obstaculo){
    para();
    frente();
  }else{
    unsigned long tempo_inicial = millis();
    
    // Caso não haja obstáculo, o robô gira indefinidamente para a direta
    vira(); // Realiza o giro indefinido para a direita, ao mesmo tempo em que detecta obstaculo

    if(tempo_inicial > 4000){
      para(); 
      frente(); 
    }
    
    /*Após o giro, caso o robo nao encontre nada, o robô verifica novamente a distância
    if(dist() > dist_obstaculo){
      // Se não houver obstáculo, ele vira a direita e se move para frente
      para(); 
      viraR(180); 
      frente();
    }*/
    
    
  }

  //as duas funções a seguir utilizam o valor de 2 vezes o valor de referencia, para compensar possíveis ocilações de leitura 
  //quanto menos reflexivo o material, maior é o valor analogico retornado pelo o sensor, sendo o valor de 1023 o caso onde toda a onda emitida é absorvida pelo material

  //verifica se o infravermelho traseiro detectou a linha, caso tenha detectado o robo vira 90° para a esquerda e segue em frente
  if(analogRead(A4) > (infra_tras*15)){ 
    re(600);
    para();
    viraR(90);
  }

//verifica se o infrevermelho da frente detectou a linha, caso tenha detectado o robo da re por 300ms e depois vira 90° para a direita
  if(analogRead(A5) > (infra_frente*15)){
    para();
    delay(150);
    re(600);
    viraR(90);
  }

/*
  Serial.print(infra_frente);
  Serial.print(" Leitura F: ");
  Serial.print(analogRead(A5));
  Serial.print("    ");
  Serial.print(infra_tras);
  Serial.print(" Leitura T: ");
  Serial.println(analogRead(A4));
  */

}
