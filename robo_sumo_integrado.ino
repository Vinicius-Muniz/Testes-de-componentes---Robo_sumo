// Inclusão de bibliotecas
#define TRIG1 4
#define ECHO1 5
#define TRIG2 6
#define ECHO2 7
#define TCRT_LEFT 8
#define TCRT_RIGHT 9
#define MOTOR_LEFT_PWM 10
#define MOTOR_LEFT_DIR 11
#define MOTOR_RIGHT_PWM 12
#define MOTOR_RIGHT_DIR 13

#define DIST_OBSTACLE 20 // Distância limite para obstáculos (em cm)

void setup() {
  // Configuração dos sensores ultrassônicos
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // Configuração dos sensores TCRT5000
  pinMode(TCRT_LEFT, INPUT);
  pinMode(TCRT_RIGHT, INPUT);

  // Configuração dos motores
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Leitura dos sensores ultrassônicos
  long distLeft = medirDistancia(TRIG1, ECHO1);
  long distRight = medirDistancia(TRIG2, ECHO2);

  // Leitura dos sensores de linha
  int linhaEsquerda = digitalRead(TCRT_LEFT);
  int linhaDireita = digitalRead(TCRT_RIGHT);

  // Lógica de movimentação
  if (linhaEsquerda == LOW || linhaDireita == LOW) { // Preto detectado
    pararMotores();
    if (linhaEsquerda == LOW) {
      virarDireita(150);
    } else {
      virarEsquerda(150);
    }
  } else if (distLeft < DIST_OBSTACLE || distRight < DIST_OBSTACLE) { // Obstáculo detectado
    pararMotores();
    if (distLeft < distRight) {
      virarDireita(150);
    } else {
      virarEsquerda(150);
    }
  } else {
    moverFrente(200);
  }

  delay(100);
}

// Função para medir distância com HC-SR04
long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH);
  long distancia = duracao * 0.034 / 2;
  return (distancia > 400 || distancia <= 0) ? 400 : distancia; // Limite de 400 cm
}

// Funções para controle de motores
void moverFrente(int velocidade) {
  analogWrite(MOTOR_LEFT_PWM, velocidade);
  digitalWrite(MOTOR_LEFT_DIR, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, velocidade);
  digitalWrite(MOTOR_RIGHT_DIR, HIGH);
}

void pararMotores() {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
}

void virarEsquerda(int velocidade) {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, velocidade);
  digitalWrite(MOTOR_RIGHT_DIR, HIGH);
}

void virarDireita(int velocidade) {
  analogWrite(MOTOR_RIGHT_PWM, 0);
  analogWrite(MOTOR_LEFT_PWM, velocidade);
  digitalWrite(MOTOR_LEFT_DIR, HIGH);
}
