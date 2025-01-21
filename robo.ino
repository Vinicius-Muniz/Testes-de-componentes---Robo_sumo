// ============================================================================
// Bibliotecas e Definições
// ============================================================================
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

#define DIST_OBSTACLE 20  // Distância limite (em cm) para considerar algo como obstáculo

// ============================================================================
// Declarações de Funções
// ============================================================================
long medirDistancia(int trigPin, int echoPin);
void moverFrente(int velocidade);
void pararMotores();
void virarEsquerda(int velocidade);
void virarDireita(int velocidade);

// ============================================================================
// Função setup()
// ============================================================================
void setup() {
  // Configuração dos sensores ultrassônicos
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // Configuração dos sensores TCRT5000 (módulos digitais)
  pinMode(TCRT_LEFT, INPUT);
  pinMode(TCRT_RIGHT, INPUT);

  // Configuração dos motores
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);

  Serial.begin(9600);
}

// ============================================================================
// Função loop()
// ============================================================================
void loop() {
  // Leitura dos sensores ultrassônicos
  long distLeft = medirDistancia(TRIG1, ECHO1);
  long distRight = medirDistancia(TRIG2, ECHO2);

  // Leitura dos sensores de linha (TCRT)
  // Se o TCRT estiver configurado para dar LOW quando detecta preto, então:
  int linhaEsquerda = digitalRead(TCRT_LEFT);
  int linhaDireita  = digitalRead(TCRT_RIGHT);

  Serial.print("distLeft: ");
  Serial.print(distLeft);
  Serial.print(" cm, distRight: ");
  Serial.print(distRight);
  Serial.print(" cm, linhaEsquerda: ");
  Serial.print(linhaEsquerda);
  Serial.print(", linhaDireita: ");
  Serial.println(linhaDireita);

  // --------------------------------------------------------------------------
  // Lógica de movimentação:
  // 1) Se detectar preto em qualquer TCRT, girar para o lado oposto, evitando
  //    sair da arena.
  // 2) Senão, se detectar obstáculo à frente (em uma das leituras < DIST_OBSTACLE),
  //    girar para a direção contrária do lado que está mais próximo do obstáculo.
  // 3) Caso contrário, andar para frente.
  // --------------------------------------------------------------------------

  if (linhaEsquerda == LOW || linhaDireita == LOW) {
    // Se o sensor retorna LOW, significa que foi detectada a linha preta
    pararMotores();
    
    if (linhaEsquerda == LOW && linhaDireita == LOW) {
      // Ambos detectaram a linha simultaneamente (caso raro):
      // Girar para trás ou fazer uma manobra simples. Aqui, vamos girar à direita.
      virarDireita(150);
    }
    else if (linhaEsquerda == LOW) {
      // Linha à esquerda -> vira à direita
      virarDireita(150);
    } 
    else {
      // Linha à direita -> vira à esquerda
      virarEsquerda(150);
    }
  }
  else if (distLeft < DIST_OBSTACLE || distRight < DIST_OBSTACLE) {
    // Obstáculo próximo detectado por um dos sensores ultrassônicos
    pararMotores();
    if (distLeft < distRight) {
      // Obstáculo mais próximo à esquerda -> girar à direita
      virarDireita(150);
    } else {
      // Obstáculo mais próximo à direita -> girar à esquerda
      virarEsquerda(150);
    }
  }
  else {
    // Caso não esteja na linha preta e nenhum obstáculo esteja muito perto,
    // avançar
    moverFrente(200);
  }

  delay(100); // Pequena pausa para estabilizar leituras e movimento
}

// ============================================================================
// Função para medir distância com HC-SR04
// ============================================================================
long medirDistancia(int trigPin, int echoPin) {
  // Disparo do pulso
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // medição do tempo em que o echoPin fica HIGH
  long duracao = pulseIn(echoPin, HIGH);

  // Cálculo de distância em cm (velocidade do som ~340 m/s)
  long distancia = duracao * 0.034 / 2;

  // Limita a leitura caso seja muito alta ou inválida
  if (distancia > 400 || distancia <= 0) {
    distancia = 400; 
  }

  return distancia;
}

// ============================================================================
// Funções de controle de motores
// ============================================================================

void moverFrente(int velocidade) {
  // Definir sentido: HIGH = frente (ajuste conforme seu driver)
  digitalWrite(MOTOR_LEFT_DIR,  HIGH);
  digitalWrite(MOTOR_RIGHT_DIR, HIGH);

  // Definir PWM
  analogWrite(MOTOR_LEFT_PWM,  velocidade);
  analogWrite(MOTOR_RIGHT_PWM, velocidade);
}

void pararMotores() {
  analogWrite(MOTOR_LEFT_PWM, 0);
  analogWrite(MOTOR_RIGHT_PWM, 0);
}

// Girar para a esquerda: motor esquerdo parado, motor direito para frente
void virarEsquerda(int velocidade) {
  // Motor esquerdo parado
  analogWrite(MOTOR_LEFT_PWM, 0);
  // Opcional: definir a direção do motor esquerdo como LOW, só para manter coerência
  digitalWrite(MOTOR_LEFT_DIR, LOW);

  // Motor direito gira para frente
  digitalWrite(MOTOR_RIGHT_DIR, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, velocidade);
}

// Girar para a direita: motor direito parado, motor esquerdo para frente
void virarDireita(int velocidade) {
  // Motor direito parado
  analogWrite(MOTOR_RIGHT_PWM, 0);
  // Opcional: definir a direção do motor direito como LOW, só para manter coerência
  digitalWrite(MOTOR_RIGHT_DIR, LOW);

  // Motor esquerdo gira para frente
  digitalWrite(MOTOR_LEFT_DIR, HIGH);
  analogWrite(MOTOR_LEFT_PWM, velocidade);
}
