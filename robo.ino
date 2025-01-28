#include <Arduino.h>

// ============================================================================
// Definições de pinos e constantes
// ============================================================================
#define TRIG1 8
#define ECHO1 9
#define TRIG2 13
#define ECHO2 12

#define TCRT_LEFT A4
#define TCRT_RIGHT 5A

#define MOTOR_LEFT_PWM 10
#define MOTOR_LEFT_DIR 11
#define MOTOR_RIGHT_PWM 5
#define MOTOR_RIGHT_DIR 6

// Parâmetros
#define DIST_OBSTACLE 80    // distância para considerar obstáculo (cm)
#define SEARCH_SPEED 60     // velocidade ao girar procurando peso/oponente
#define PUSH_SPEED   200    // velocidade ao avançar empurrando
#define RETREAT_SPEED 80    // velocidade ao recuar

// ============================================================================
// Enum para a Máquina de Estados
// ============================================================================
enum RobotState {
  SEARCH,    // robô gira procurando alvo
  PUSH,      // robô avança para empurrar alvo
  RETREAT    // robô recua para não sair da arena
};

// Variável global de estado
RobotState currentState = SEARCH;

// Variável para armazenar o momento que entrou no estado (usada em RETREAT)
unsigned long stateStartTime = 0;

// ============================================================================
// Declarações de Funções (mantemos as originais, sem mexer na lógica interna)
// ============================================================================
long medirDistancia(int trigPin, int echoPin);

void moverFrente(int velocidade);
void moverTras(int velocidade);
void virarEsquerda(int velocidade);
void virarDireita(int velocidade);
void pararMotores();

// Função auxiliar para trocar de estado
void changeState(RobotState newState) {
  currentState = newState;
  stateStartTime = millis(); // salva o horário de entrada nesse novo estado
}

// ============================================================================
// setup()
// ============================================================================
void setup() {
  // Configuração dos pinos ultrassônicos
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // Configuração dos sensores TCRT
  pinMode(TCRT_LEFT, INPUT);
  pinMode(TCRT_RIGHT, INPUT);

  // Configuração dos motores
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);

  Serial.begin(9600);
  delay(1000);
}

// ============================================================================
// loop() - Máquina de Estados
// ============================================================================
void loop() {
  // Faz a leitura de distância do sensor frontal (ou use TRIG2/ECHO2 se preferir)
  long distFrente = medirDistancia(TRIG1, ECHO1);

  // Leitura dos sensores de linha
  // (Verifique como seu TCRT está configurado: HIGH pode indicar linha ou não)
  int linhaEsq = digitalRead(TCRT_LEFT);
  int linhaDir = digitalRead(TCRT_RIGHT);

  // Debug
  Serial.print("distFrente: ");
  Serial.print(distFrente);
  Serial.print(" cm | linhaEsq: ");
  Serial.print(linhaEsq);
  Serial.print(" | linhaDir: ");
  Serial.println(linhaDir);

  switch (currentState) {

    case SEARCH: {
      // Fica girando para a direita (ou esquerda) até encontrar algo
      virarDireita(SEARCH_SPEED);

      // Se achar obstáculo à frente (distância menor que limite), muda para PUSH
      if (distFrente < DIST_OBSTACLE) {
        changeState(PUSH);
      }
      break;
    }

    case PUSH: {
      // Avança para empurrar o peso (ou adversário)
      moverFrente(PUSH_SPEED);

      // Se detecta a linha (bordas) ou perde o obstáculo, recua
      if ((linhaEsq == HIGH || linhaDir == HIGH) || (distFrente > DIST_OBSTACLE + 10)) {
        changeState(RETREAT);
      }
      break;
    }

    case RETREAT: {
      // Dá ré para não sair da arena
      moverTras(RETREAT_SPEED);

      // Ficamos nesse estado por um tempo determinado (p.ex. 1 segundo)
      unsigned long tempoNoEstado = millis() - stateStartTime;
      if (tempoNoEstado > 1000) {
        // Já recuou o suficiente, pode parar e voltar a procurar
        pararMotores();
        delay(100);  // pequena pausa
        changeState(SEARCH);
      }
      break;
    }
  }
}

// ============================================================================
// Funções Auxiliares
// ============================================================================

// (Mesma lógica que você já tinha, só tome cuidado se \"HIGH\" realmente é frente)
long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH);
  long distancia = duracao * 0.034 / 2;
  if (distancia <= 0 || distancia > 400) {
    distancia = 400;  // limite
  }
  return distancia;
}

// ============================================================================
// MANTENDO as funções que você já tinha, SEM ALTERAÇÕES
// ============================================================================
void moverFrente(int velocidade) {
  // Definir sentido: HIGH = frente (ajuste conforme seu driver)
  analogWrite(MOTOR_LEFT_DIR,  velocidade);
  analogWrite(MOTOR_RIGHT_DIR, 0);

  // Definir PWM
  analogWrite(MOTOR_LEFT_PWM,  0);
  analogWrite(MOTOR_RIGHT_PWM, velocidade);
}

void moverTras(int velocidade) {
  // Definir sentido (inverso do moverFrente)
  analogWrite(MOTOR_LEFT_DIR,  0);
  analogWrite(MOTOR_RIGHT_DIR, velocidade);

  // Definir PWM
  analogWrite(MOTOR_LEFT_PWM,  velocidade);
  analogWrite(MOTOR_RIGHT_PWM, 0);
}

void virarEsquerda(int velocidade) {
  analogWrite(MOTOR_LEFT_PWM, velocidade);
  analogWrite(MOTOR_LEFT_DIR, 0);
  analogWrite(MOTOR_RIGHT_PWM, velocidade);
  analogWrite(MOTOR_RIGHT_DIR, 0);
}

void virarDireita(int velocidade) {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_LEFT_DIR, velocidade);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  analogWrite(MOTOR_RIGHT_DIR, velocidade);
}

void pararMotores() {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  analogWrite(MOTOR_LEFT_DIR, 0);
  analogWrite(MOTOR_RIGHT_DIR, 0);
}
