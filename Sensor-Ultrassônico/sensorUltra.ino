// Definição de pinos para o sensor ultrassonico 1
#define TRIGGER1 4
#define ECHO1 5

// Definição de pinos para o sensor ultrassonico 2
#define TRIGGER2 6
#define ECHO2 7

#define OUT_OF_RANGE -1  // Valor retornado para "fora de alcance"

// Variáveis para armazenar as distâncias
long distancia1 = 0;
long distancia2 = 0;

void setup() {
    // Configuração dos pinos do sensor 1
    pinMode(TRIGGER1, OUTPUT);
    pinMode(ECHO1, INPUT);

    // Configuração dos pinos do sensor 2
    pinMode(TRIGGER2, OUTPUT);
    pinMode(ECHO2, INPUT);

    // Inicialização da comunicação serial
    Serial.begin(9600);
    Serial.println("Iniciando leitura dos sensores ultrassônicos...");
}

void loop() {
    // Medir a distância do sensor esquerdo
    distancia1 = medirDistancia(TRIGGER1, ECHO1);

    // Medir a distância do sensor direito
    distancia2 = medirDistancia(TRIGGER2, ECHO2);

    // Exibir os valores no monitor serial
    Serial.print("Distância Sensor 1: ");
    if (distancia1 == OUT_OF_RANGE) {
        Serial.println("Fora de alcance");
    } else {
        Serial.print(distancia1);
        Serial.println(" cm");
    }

    Serial.print("Distância Sensor 2: ");
    if (distancia2 == OUT_OF_RANGE) {
        Serial.println("Fora de alcance");
    } else {
        Serial.print(distancia2);
        Serial.println(" cm");
    }

    Serial.println("-----------------------------");

    delay(500); // Aguarde 500ms antes de realizar outra leitura
}

// Função para medir a distância usando HC-SR04
long medirDistancia(int trigPin, int echoPin) {
    // Enviar um pulso no pino TRIG
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Ler o tempo de retorno do pulso no pino ECHO
    long duracao = pulseIn(echoPin, HIGH);

    // Calcular a distância em centímetros
    long distancia = duracao * 0.034 / 2;

    // Limitar a distância para evitar valores inválidos
    if (distancia > 400 || distancia <= 0) {
        return OUT_OF_RANGE; // Retorna -1 para indicar "fora de alcance"
    }

    return distancia;
}
