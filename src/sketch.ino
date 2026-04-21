#include "DHT.h"

// ==========================================
// DEFINIÇÃO DOS PINOS DO ESP32 NO WOKWI
// ==========================================

#define DHTPIN 4          // Pino digital para o sensor DHT22
#define DHTTYPE DHT22     // Define o modelo do sensor de umidade

#define LDR_PIN 34        // Pino Analógico (ADC) para o sensor LDR (pH)
#define BTN_N 12          // Pino Digital para o botão Nitrogênio (BTN BRANCO)
#define BTN_P 14          // Pino Digital para o botão Fósforo (BTN CINZA)
#define BTN_K 27          // Pino Digital para o botão Potássio (BTN VERMELHO)

#define RELE_BOMBA 26     // Pino Digital para o Relé Azul (Bomba)

// Instancia o sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Variável global para manter o estado da previsão do tempo
bool previsao_chuva = false;

void setup() {
  // Inicia o Monitor Serial para visualização dos dados e avisos
  Serial.begin(115200);

  // Inicia o sensor DHT22
  dht.begin();

  // Configuração dos Pinos
  pinMode(LDR_PIN, INPUT);

  // Usamos INPUT_PULLUP para não precisar de resistores físicos nos botões Wokwi
  pinMode(BTN_N, INPUT_PULLUP);
  pinMode(BTN_P, INPUT_PULLUP);
  pinMode(BTN_K, INPUT_PULLUP);

  // Configura o relé e garante que a bomba inicie desligada
  pinMode(RELE_BOMBA, OUTPUT);
  digitalWrite(RELE_BOMBA, LOW);

  Serial.println("--- Sistema FarmTech Solutions Iniciado ---");
  Serial.println("Cultura: Laranja | pH Ideal: 5.5 a 6.5");
}

void loop() {
  // ==========================================
  // 1. PREVISÃO DO TEMPO (Teclado via Serial)
  // ==========================================
  // Verifica se você digitou algo no Monitor Serial
  if (Serial.available() > 0) {
    char comando = Serial.read(); // Lê a letra que você digitou

    if (comando == 'C' || comando == 'c'){
      previsao_chuva = true;
      Serial.println("🌧️ [API TEMPO] Previsao de CHUVA recebida! Sistema preparando para economizar agua");
    }
    else if (comando == 'S' || comando == 's'){
      previsao_chuva = false;
      Serial.println("☀️ [API TEMPO] Tempo SECO. Irrigacao automatica retomada.");
    }
  } 

  // ==========================================
  // 2. LEITURA DE UMIDADE (DHT22)
  // ==========================================
  float umidade = dht.readHumidity();

  // Verifica se houve erro na leitura do DHT22
  if (isnan(umidade)){
    Serial.println("Falha ao ler o sensor DHT22!");
    return;
  }

  // ==========================================
  // 3. LEITURA DOS NUTRIENTES NPK
  // ==========================================
  // Com o resistor PULLUP interno, o botão pressionado envia sinal LOW(0)
  bool n_ativo = !digitalRead(BTN_N);
  bool p_ativo = !digitalRead(BTN_P);
  bool k_ativo = !digitalRead(BTN_K);

  // ==========================================
  // 4. LEITURA DO pH (LDR mapeado)
  // ==========================================
  int leituraADC = analogRead(LDR_PIN);

  // Transforma a leitura analógica (0 a 4095) na escala de pH (0.0 a 14.0)
  float pH = (leituraADC * 14.0) / 4095.0;

  // ==========================================
  // 5. ALGORITMO: SAÚDE DA PLANTA E IRRIGAÇÃO
  // ==========================================
  bool pH_ideal = (pH >= 5.5 && pH <= 6.5);

  Serial.println("----------------------------------------");
  Serial.print("Umidade do solo: "); Serial.print(umidade); Serial.println("%");
  Serial.print("pH atual: "); Serial.println(pH);
  Serial.print("Nutrientes ativos -> N: "); Serial.print(n_ativo);
  Serial.print(" | P: "); Serial.print(p_ativo);
  Serial.print(" | K: "); Serial.println(k_ativo);

  // --- MONITORAMENTO QUÍMICO (Sistema de Avisos para o NPK) ---
  if (pH < 5.5) {
    Serial.println("ALERTA: pH muito BAIXO (Acido)!");
    
    // Verifica se o botão do Potássio foi apertado
    if (!k_ativo) {
      Serial.println("-> ACAO RECOMENDADA: Acione o botao de Potassio (K - Botao Vermelho) para ELEVAR o pH.");
    } else {
      Serial.println("-> STATUS EXCELENTE: Potassio (K - Botao Vermelho) ativado e sendo absorvido perfeitamente pela planta.");
    }
    
  } else if (pH > 6.5) {
    Serial.println("ALERTA: pH muito ALTO (Alcalino)!");
    
    // Verifica se o botão do Nitrogênio foi apertado
    if (!n_ativo) {
      Serial.println("-> ACAO RECOMENDADA: Acione o botao de Nitrogenio (N - Botao Branco) para BAIXAR o pH.");
    } else {
      Serial.println("-> STATUS EXCELENTE: Nitrogenio (N - Botao Branco) ativado e sendo absorvido perfeitamente pela planta.");
    }
    
  } else {
    Serial.println("STATUS: pH IDEAL (5.5 a 6.5) estabilizado!");
    
    // Função do Fósforo (P): Entra em cena quando o pH está perfeito
    if (!p_ativo) {
      Serial.println("-> ACAO RECOMENDADA: pH ideal atingido! Acione o botao de Fosforo (P - Botao Cinza) para estimular as raizes.");
    } else {
      Serial.println("-> STATUS EXCELENTE: Fosforo (P - Botao Cinza) ativado e sendo absorvido perfeitamente pela planta.");
      
      // Verifica se N ou K ficaram ligados "esquecidos", o que tiraria o pH do eixo
      if (n_ativo || k_ativo) {
        Serial.println("-> AVISO: Desligue os botoes N e/ou K para nao desestabilizar o pH novamente.");
      }
    }
  }

  // --- REGRA DE IRRIGAÇÃO (Baseada exclusivamente na umidade) ---
  if (previsao_chuva) {
    // Se a previsão diz que vai chover, bloqueia a bomba para economizar recursos!
    digitalWrite(RELE_BOMBA, LOW);
    Serial.println("Bomba: DESLIGADA (Economizando agua, a chuva fara o trabalho!)");

  } else{

    if (umidade <= 50.0) {
      digitalWrite(RELE_BOMBA, HIGH); // Liga o relé azul
      Serial.println("Bomba: LIGADA (Solo seco. Irrigando...)");

    } else if (umidade >= 80.0) {
      digitalWrite(RELE_BOMBA, LOW); // Desliga o Relé Azul
      Serial.println("Bomba: DESLIGADA (Umidade Alta Demais)");

    } else {
      // Entre 50% e 80%, a planta não precisa de alteração no momento
      digitalWrite(RELE_BOMBA, LOW);
      Serial.println("Bomba: Desligada (Umidade na faixa de seguranca)");
    }
  }

  // Aguarda 2 segundos antes do próximo ciclo de leitura
  delay(2000);
}
