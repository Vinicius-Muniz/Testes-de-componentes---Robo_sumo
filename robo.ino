#include <Arduino.h>

// ============================================================================
// Definições de pinos e constantes
// ============================================================================
#define TRIG1 8
#define ECHO1 9
#define TRIG2 13
#define ECHO2 12

#define TCRT_LEFT A4
#define TCRT_RIGHT A5

#define MOTOR_LEFT_PWM 10
#define MOTOR_LEFT_DIR 11
#define MOTOR_RIGHT_PWM 5
#define MOTOR_RIGHT_DIR 6

// Parâmetros
#define DIST_OBSTACLE 80     // distância para considerar obstáculo (cm)

// TCRT thresholds based on your tests
#define BACK_LINE_THRESHOLD 440   // TCRT_LEFT reading above 600 means black line
#define FRONT_LINE_THRESHOLD 200   // TCRT_RIGHT reading above 80 means black line

#define SEARCH_SPEED 60           // velocidade ao girar procurando peso/oponente
#define PUSH_SPEED   120          // velocidade ao avançar empurrando

// Ajuste essa velocidade se quiser girar mais rápido ou mais devagar.
#define TURNAROUND_SPEED 80       // velocidade ao fazer o giro de 180º

// Ajuste esse tempo (em ms) para aproximar 180º. Vai depender do seu robô.
// 1000 ms pode ser muito ou pouco, você deve calibrar testando na prática.
#define TURNAROUND_TIME 1000   
#define COOLDOWN_TIME 500   
#define BACKWARD_TIME 500

// ============================================================================
// Enum para a Máquina de Estados
// ============================================================================
enum RobotState {
  SEARCH,    // robô gira procurando alvo
  PUSH,      // robô avança para empurrar alvo
  RETREAT,    // robô faz 180º para não sair da arena
  COOLDOWN   // Pequena espera para não detectar a linha imediatamente de novo
};

RobotState currentState = SEARCH;
unsigned long stateStartTime = 0;

// ============================================================================
// Declarações de Funções
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
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // pinMode(TCRT_LEFT, INPUT_ANALOG); // se necessário
  // pinMode(TCRT_RIGHT, INPUT_ANALOG);

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
  // 1) Ler distância frontal
  long distFrente = medirDistancia(TRIG1, ECHO1);

  // 2) Ler valores analógicos dos sensores de linha
  int linhaTras    = analogRead(TCRT_LEFT);   // TCRT_LEFT -> Back sensor
  int linhaFrente  = analogRead(TCRT_RIGHT);  // TCRT_RIGHT -> Front sensor

  // Debug


  switch (currentState) {

    case SEARCH: {
      // Gira à direita até encontrar algo próximo
      virarDireita(SEARCH_SPEED);

      // Se achar obstáculo à frente (distância < DIST_OBSTACLE), muda para PUSH
      if (distFrente < DIST_OBSTACLE) {
        changeState(PUSH);
      }
      // Se a parte de trás ou da frente ler acima do threshold, faz 180º
      if (linhaTras > BACK_LINE_THRESHOLD || linhaFrente > FRONT_LINE_THRESHOLD) {
          Serial.print("DistFrente: ");
          Serial.print(distFrente);
          Serial.print(" | LinhaTras: ");
          Serial.print(linhaTras);
          Serial.print(" | LinhaFrente: ");
          Serial.println(linhaFrente);
        changeState(RETREAT);
      }
      break;
    }

    case PUSH: {
      // Avança para empurrar o objeto/peso

      // Se detecta a linha ou perde o obstáculo, faz 180º
      bool detectouLinha = (linhaTras > BACK_LINE_THRESHOLD || linhaFrente > FRONT_LINE_THRESHOLD);
      

      if (detectouLinha) {
          Serial.print("DistFrente: ");
          Serial.print(distFrente);
          Serial.print(" | LinhaTras: ");
          Serial.print(linhaTras);
          Serial.print(" | LinhaFrente: ");
          Serial.println(linhaFrente);
        changeState(RETREAT);
      }

      moverFrente(PUSH_SPEED);

      break;
    }

    case RETREAT: {
      unsigned long elapsedTime = millis() - stateStartTime;

      // Step 1: Move back for BACKWARD_TIME milliseconds
      if (elapsedTime < BACKWARD_TIME) {
        moverTras(SEARCH_SPEED);
      } 
      // Step 2: Turn around for TURNAROUND_TIME milliseconds
      else if (elapsedTime < (BACKWARD_TIME + TURNAROUND_TIME)) {
        virarDireita(TURNAROUND_SPEED);
      } 
      // Step 3: Stop and go to COOLDOWN state
      else {
        pararMotores();
        delay(100);
        changeState(COOLDOWN);
      }
      break;
    }

    case COOLDOWN: {
      // Aguarda um tempo antes de voltar a detectar a linha
      unsigned long tempoNoEstado = millis() - stateStartTime;
      if (tempoNoEstado > COOLDOWN_TIME) {
        changeState(SEARCH);  // Após cooldown, volta a procurar pesos
      }
      break;
    }
  }
}

// ============================================================================
// Funções Auxiliares
// ============================================================================
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

// --- Funções de movimento (mantidas, sem alterações) ---
void moverFrente(int velocidade) {
  analogWrite(MOTOR_LEFT_DIR,  velocidade);
  analogWrite(MOTOR_RIGHT_DIR, 0);

  analogWrite(MOTOR_LEFT_PWM,  0);
  analogWrite(MOTOR_RIGHT_PWM, velocidade);
}

void moverTras(int velocidade) {
  analogWrite(MOTOR_LEFT_DIR,  0);
  analogWrite(MOTOR_RIGHT_DIR, velocidade);

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
