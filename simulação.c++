#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Definição dos pinos
#define TRIG_PIN 7        // Pino Trigger do HC-SR04
#define ECHO_PIN 6        // Pino Echo do HC-SR04
#define RAIN_SENSOR A0    // Pino analógico do potenciômetro (sensor de chuva)
#define DHT_PIN 2         // Pino do sensor DHT22
#define DHT_TYPE DHT22    // Tipo do sensor DHT

#define LED_VERDE 13      // LED indicador seguro
#define LED_AMARELO 12    // LED indicador atenção
#define LED_VERMELHO 11   // LED indicador perigo
#define BUZZER 10         // Buzzer para alarme

// Inicialização dos componentes
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Display LCD I2C
DHT dht(DHT_PIN, DHT_TYPE);          // Sensor DHT22

// Variáveis globais
float distancia_agua = 0;     // Distância medida pelo ultrassônico (cm)
int nivel_chuva = 0;          // Valor do potenciômetro (0-1023)
float umidade = 0;            // Umidade do ar (%)
float temperatura = 0;        // Temperatura (°C)

// Constantes para calibração
const float ALTURA_MAXIMA = 200.0;    // Altura máxima do sensor ao solo (cm)
const float NIVEL_ATENCAO = 150.0;    // Nível de atenção (cm do sensor)
const float NIVEL_PERIGO = 100.0;     // Nível de perigo (cm do sensor)

// Calibração do potenciômetro como sensor de chuva
// Valores MAIORES = MAIS chuva (girando o potenciômetro no sentido horário)
const int CHUVA_INTENSA = 700;        // Valor para chuva intensa (potenciômetro > 700)
const int CHUVA_MODERADA = 400;       // Valor para chuva moderada (potenciômetro > 400)

// Estados do sistema
enum EstadoSistema {
  SEGURO,
  ATENCAO,
  PERIGO
};

EstadoSistema estado_atual = SEGURO;
unsigned long ultimo_alerta = 0;
const unsigned long INTERVALO_ALERTA = 2000; // Intervalo entre alertas (ms)

void setup() {
  Serial.begin(9600);
  
  // Inicialização do LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Anti-");
  lcd.setCursor(0, 1);
  lcd.print("Enchente v1.0");
  delay(2000);
  lcd.clear();
  
  // Inicialização do DHT22
  dht.begin();
  
  // Configuração dos pinos
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RAIN_SENSOR, INPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  // Estado inicial - seguro
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(BUZZER, LOW);
  
  Serial.println("=== Sistema de Monitoramento de Enchentes Iniciado ===");
  Serial.println("Monitorando: Nível de Água | Chuva | Umidade | Temperatura");
  Serial.println("POTENCIÔMETRO: Gire no sentido horário para simular mais chuva");
  Serial.println("==================================================");
}

void loop() {
  // Leitura dos sensores
  lerSensores();
  
  // Análise dos dados e determinação do estado
  analisarCondicoes();
  
  // Atualização dos indicadores visuais e sonoros
  atualizarIndicadores();
  
  // Exibição no display LCD
  atualizarDisplay();
  
  // Envio de dados para Serial Monitor
  enviarDadosSerial();
  
  delay(1000); // Atualização a cada segundo
}

void lerSensores() {
  // Leitura do sensor ultrassônico (nível de água)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duracao = pulseIn(ECHO_PIN, HIGH);
  distancia_agua = duracao * 0.034 / 2; // Conversão para cm
  
  // Leitura do potenciômetro (simulando sensor de chuva)
  nivel_chuva = analogRead(RAIN_SENSOR);
  
  // Leitura do sensor DHT22
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();
  
  // Verificação de leituras válidas do DHT22
  if (isnan(umidade) || isnan(temperatura)) {
    umidade = 0;
    temperatura = 0;
  }
}

void analisarCondicoes() {
  EstadoSistema novo_estado = SEGURO;
  
  // Análise do nível de água (quanto menor a distância, maior o nível)
  if (distancia_agua <= NIVEL_PERIGO) {
    novo_estado = PERIGO;
  } else if (distancia_agua <= NIVEL_ATENCAO) {
    if (novo_estado < ATENCAO) novo_estado = ATENCAO;
  }
  
  // Análise da intensidade da chuva (potenciômetro)
  // Valores MAIORES indicam MAIS chuva
  if (nivel_chuva >= CHUVA_INTENSA) {
    novo_estado = PERIGO;
  } else if (nivel_chuva >= CHUVA_MODERADA) {
    if (novo_estado < ATENCAO) novo_estado = ATENCAO;
  }
  
  // Análise da umidade (umidade muito alta pode indicar chuva intensa)
  if (umidade >= 90) {
    if (novo_estado < ATENCAO) novo_estado = ATENCAO;
  }
  
  // Combinação de fatores para estado de perigo
  if ((distancia_agua <= NIVEL_ATENCAO) && (nivel_chuva >= CHUVA_MODERADA) && (umidade >= 85)) {
    novo_estado = PERIGO;
  }
  
  estado_atual = novo_estado;
}

void atualizarIndicadores() {
  // Reset de todos os LEDs
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(BUZZER, LOW);
  
  unsigned long agora = millis();
  
  switch (estado_atual) {
    case SEGURO:
      digitalWrite(LED_VERDE, HIGH);
      break;
      
    case ATENCAO:
      digitalWrite(LED_AMARELO, HIGH);
      // Buzzer intermitente lento
      if ((agora - ultimo_alerta) >= INTERVALO_ALERTA) {
        digitalWrite(BUZZER, HIGH);
        delay(200);
        digitalWrite(BUZZER, LOW);
        ultimo_alerta = agora;
      }
      break;
      
    case PERIGO:
      digitalWrite(LED_VERMELHO, HIGH);
      // Buzzer intermitente rápido
      if ((agora - ultimo_alerta) >= (INTERVALO_ALERTA / 4)) {
        digitalWrite(BUZZER, HIGH);
        delay(100);
        digitalWrite(BUZZER, LOW);
        ultimo_alerta = agora;
      }
      break;
  }
}

void atualizarDisplay() {
  lcd.clear();
  
  // Primeira linha - Status e nível de água
  lcd.setCursor(0, 0);
  switch (estado_atual) {
    case SEGURO:
      lcd.print("SEGURO ");
      break;
    case ATENCAO:
      lcd.print("ATENCAO ");
      break;
    case PERIGO:
      lcd.print("PERIGO! ");
      break;
  }
  
  float nivel_agua = ALTURA_MAXIMA - distancia_agua;
  lcd.print(nivel_agua, 1);
  lcd.print("cm");
  
  // Segunda linha - Chuva e umidade
  lcd.setCursor(0, 1);
  if (nivel_chuva >= CHUVA_INTENSA) {
    lcd.print("Chuva:FORTE ");
  } else if (nivel_chuva >= CHUVA_MODERADA) {
    lcd.print("Chuva:MOD ");
  } else {
    lcd.print("Chuva:FRACA ");
  }
  
  lcd.print(umidade, 0);
  lcd.print("%");
}

void enviarDadosSerial() {
  Serial.println("==================== DADOS ATUAIS ====================");
  Serial.print("Status do Sistema: ");
  
  switch (estado_atual) {
    case SEGURO:
      Serial.println("SEGURO ✓");
      break;
    case ATENCAO:
      Serial.println("ATENÇÃO ⚠️");
      break;
    case PERIGO:
      Serial.println("PERIGO! 🚨");
      break;
  }
  
  float nivel_agua = ALTURA_MAXIMA - distancia_agua;
  Serial.print("Nível de Água: ");
  Serial.print(nivel_agua);
  Serial.println(" cm do solo");
  
  Serial.print("Distância do Sensor: ");
  Serial.print(distancia_agua);
  Serial.println(" cm");
  
  Serial.print("Potenciômetro (Chuva): ");
  Serial.print(nivel_chuva);
  Serial.print("/1023 ");
  if (nivel_chuva >= CHUVA_INTENSA) {
    Serial.println("(Chuva Forte)");
  } else if (nivel_chuva >= CHUVA_MODERADA) {
    Serial.println("(Chuva Moderada)");
  } else {
    Serial.println("(Sem chuva/Chuva fraca)");
  }
  
  Serial.print("Umidade do Ar: ");
  Serial.print(umidade);
  Serial.println("%");
  
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");
  
  Serial.println("=====================================================");
  Serial.println();
}