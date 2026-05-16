// =============================================================================
// FARMTECH SOLUTIONS - Sistema de Irrigação e Monitoramento Automatizado
// FIAP - Fase 3 - Engenharia de Software
// Plataforma: ESP32 (30 pinos) | Simulador: Wokwi
//
// ARQUITETURA DO SISTEMA:
//   - DHT22      → Pino 4  : Leitura de Umidade e Temperatura
//   - Relé Azul  → Pino 26 : Controle da Bomba de Irrigação
//   - Botão N    → Pino 12 : Nitrogênio  (simula aplicação de nutriente)
//   - Botão P    → Pino 14 : Fósforo     (simula aplicação de nutriente)
//   - Botão K    → Pino 27 : Potássio    (CORRIGIDO - era ligado no pino 14)
//   - LDR Azul   → Pino 34 : Sensor ADC auxiliar / pH base inicial
//
// REGRAS DE NEGÓCIO:
//   1. IRRIGAÇÃO: A bomba liga se umidade < 40% E previsão de chuva = FALSO
//   2. IRRIGAÇÃO: A bomba desliga se umidade >= 70% (segurança contra alagamento)
//   3. pH DINÂMICO: O botão K (Potássio) eleva o pH (+0.1 por ciclo) se pH < 7.0
//   4. pH DINÂMICO: O botão N (Nitrogênio) reduz o pH (-0.05 por ciclo) simulando acidificação
//   5. pH DINÂMICO: O botão P (Fósforo) tem efeito neutro no pH (apenas registra aplicação)
//   6. PREVISÃO CHUVA: Controlada via comando Serial ("CHUVA_ON" / "CHUVA_OFF")
//   7. NUTRIENTES: Cada botão incrementa seu contador a cada ciclo enquanto pressionado
// =============================================================================

#include <DHT.h>

// ---------------------------------------------------------------------------
// DEFINIÇÃO DE PINOS
// ---------------------------------------------------------------------------
#define PIN_DHT       4     // DHT22 - Sensor de Umidade/Temperatura
#define PIN_RELAY     26    // Relé  - Bomba de Irrigação (ATIVO em LOW)
#define PIN_BTN_N     12    // Botão Branco - Nitrogênio
#define PIN_BTN_P     14    // Botão Cinza  - Fósforo
#define PIN_BTN_K     27    // Botão Vermelho - Potássio *** PINO CORRIGIDO ***
#define PIN_LDR       34    // LDR   - Leitura ADC para base do pH

// ---------------------------------------------------------------------------
// CONSTANTES DO SISTEMA
// ---------------------------------------------------------------------------
#define DHT_TYPE        DHT22
#define INTERVALO_LOOP  2000    // Ciclo de leitura: 2 segundos

// Faixas de umidade para controle da bomba
#define UMIDADE_MIN_LIGAR   40.0   // Abaixo disso → Liga a bomba
#define UMIDADE_MAX_DESLIGAR 70.0  // Acima disso  → Desliga a bomba (proteção)

// Faixas ideais de pH para o solo (cultura genérica)
#define PH_IDEAL_MIN    6.0
#define PH_IDEAL_MAX    7.5

// Efeito dos botões sobre o pH por ciclo de 2s
#define PH_EFEITO_K    +0.10   // Potássio alcaliniza → sobe o pH
#define PH_EFEITO_N    -0.05   // Nitrogênio acidifica → desce o pH ligeiramente
#define PH_EFEITO_P     0.00   // Fósforo não altera pH diretamente

// Limites físicos do pH (0-14)
#define PH_MINIMO   3.0
#define PH_MAXIMO  10.0

// ---------------------------------------------------------------------------
// VARIÁVEIS GLOBAIS DE ESTADO
// ---------------------------------------------------------------------------
DHT dht(PIN_DHT, DHT_TYPE);

float     g_umidade          = 0.0;
float     g_temperatura      = 0.0;
float     g_ph               = 6.0;    // pH inicial (levemente ácido - solo típico)
bool      g_bomba_ligada     = false;
bool      g_previsao_chuva   = false;  // Controlada pelo Monitor Serial

// Contadores de nutrientes aplicados (acumulados na sessão)
int       g_cnt_nitrogenio   = 0;
int       g_cnt_fosforo      = 0;
int       g_cnt_potassio     = 0;

unsigned long g_ultimo_ciclo = 0;

// ---------------------------------------------------------------------------
// PROTÓTIPOS DE FUNÇÕES
// ---------------------------------------------------------------------------
void lerSensores();
void processarBotoes();
void controlarBomba();
void verificarComandoSerial();
void imprimirRelatorio();
float mapearLDRparaPH(int ldrRaw);

// =============================================================================
// SETUP
// =============================================================================
void setup() {
  Serial.begin(115200);
  dht.begin();

  // Configura pinos dos botões com pull-up interno
  // (Botão conecta o pino ao GND quando pressionado → leitura LOW = pressionado)
  pinMode(PIN_BTN_N, INPUT_PULLUP);
  pinMode(PIN_BTN_P, INPUT_PULLUP);
  pinMode(PIN_BTN_K, INPUT_PULLUP);

  // Relé: inicia DESLIGADO. Relés de módulo azul são ativos em LOW.
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH); // HIGH = Relé desligado (bomba OFF)

  // LDR: Pino 34 é ADC-only no ESP32, não precisa de pinMode

  Serial.println("==============================================");
  Serial.println("  FARMTECH SOLUTIONS - Sistema Inicializado  ");
  Serial.println("==============================================");
  Serial.println("Comandos disponíveis via Monitor Serial:");
  Serial.println("  CHUVA_ON  → Ativa previsao de chuva (bloqueia irrigacao)");
  Serial.println("  CHUVA_OFF → Remove previsao de chuva");
  Serial.println("----------------------------------------------");
  Serial.println("Aguardando primeiro ciclo de leitura...");
  Serial.println();

  delay(2000); // Aguarda DHT22 estabilizar
}

// =============================================================================
// LOOP PRINCIPAL
// =============================================================================
void loop() {
  // Processa comandos do Monitor Serial em qualquer momento (não bloqueia)
  verificarComandoSerial();

  // Executa lógica principal a cada INTERVALO_LOOP milissegundos
  unsigned long agora = millis();
  if (agora - g_ultimo_ciclo >= INTERVALO_LOOP) {
    g_ultimo_ciclo = agora;

    lerSensores();        // 1. Lê DHT22 e LDR
    processarBotoes();    // 2. Processa botões e ajusta pH dinamicamente
    controlarBomba();     // 3. Aplica regra de negócio da bomba
    imprimirRelatorio();  // 4. Imprime estado completo no Serial
  }
}

// =============================================================================
// FUNÇÕES
// =============================================================================

/**
 * lerSensores()
 * Lê o DHT22 (umidade e temperatura) e o LDR (pH base).
 * O pH via LDR serve como "piso" de referência; os botões o ajustam dinamicamente.
 */
void lerSensores() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Valida leitura do DHT22 (pode falhar e retornar NaN)
  if (!isnan(h)) g_umidade     = h;
  if (!isnan(t)) g_temperatura = t;

  // Lê LDR e converte para faixa de pH (apenas como valor de referência visual)
  // O pH dinâmico real é controlado pelos botões (processarBotoes)
  int ldrRaw = analogRead(PIN_LDR); // 0-4095 no ESP32 (12-bit ADC)
  // Nota: não sobrescrevemos g_ph aqui para preservar o efeito dos botões.
  // O LDR é exibido no relatório apenas como valor bruto para referência.
  (void)ldrRaw; // Suprime warning de variável não usada
}

/**
 * processarBotoes()
 * Verifica o estado dos botões e aplica efeitos dinâmicos no pH.
 *
 * REGRA: Botões usam INPUT_PULLUP → LOW = pressionado, HIGH = solto.
 *
 * EFEITOS NO pH:
 *   - Botão K (Potássio, pino 27): pH sobe +0.10 por ciclo → corrige solo ácido
 *   - Botão N (Nitrogênio, pino 12): pH desce -0.05 por ciclo → fertilizante acidificante
 *   - Botão P (Fósforo, pino 14): pH neutro, apenas registra aplicação
 */
void processarBotoes() {
  bool btn_n = (digitalRead(PIN_BTN_N) == LOW);
  bool btn_p = (digitalRead(PIN_BTN_P) == LOW);
  bool btn_k = (digitalRead(PIN_BTN_K) == LOW);

  // Ajuste dinâmico do pH conforme botão pressionado
  if (btn_k) {
    g_ph += PH_EFEITO_K;
    g_cnt_potassio++;
  }
  if (btn_n) {
    g_ph += PH_EFEITO_N;
    g_cnt_nitrogenio++;
  }
  if (btn_p) {
    // Fósforo não altera pH — apenas conta a aplicação
    g_cnt_fosforo++;
  }

  // Garante que o pH permanece dentro de limites físicos realistas
  g_ph = constrain(g_ph, PH_MINIMO, PH_MAXIMO);
}

/**
 * controlarBomba()
 * Aplica as regras de negócio da irrigação:
 *
 *   LIGAR bomba SE:
 *     - Umidade < UMIDADE_MIN_LIGAR (40%)
 *     - E NÃO há previsão de chuva
 *
 *   DESLIGAR bomba SE:
 *     - Umidade >= UMIDADE_MAX_DESLIGAR (70%)  [proteção contra alagamento]
 *     - OU há previsão de chuva               [economia de água]
 *
 * Relé azul de módulo: HIGH = OFF, LOW = ON (lógica invertida)
 */
void controlarBomba() {
  bool deveIrrigar = false;

  if (g_previsao_chuva) {
    // Previsão de chuva: bloqueia irrigação independente da umidade
    deveIrrigar = false;
  } else if (g_umidade < UMIDADE_MIN_LIGAR) {
    // Solo seco e sem chuva prevista → Ligar bomba
    deveIrrigar = true;
  } else if (g_umidade >= UMIDADE_MAX_DESLIGAR) {
    // Solo saturado → Desligar bomba (proteção)
    deveIrrigar = false;
  } else {
    // Zona intermediária: mantém estado atual (evita liga/desliga constante)
    deveIrrigar = g_bomba_ligada;
  }

  // Atualiza estado do relé apenas se houve mudança (evita writes desnecessários)
  if (deveIrrigar != g_bomba_ligada) {
    g_bomba_ligada = deveIrrigar;
    digitalWrite(PIN_RELAY, g_bomba_ligada ? LOW : HIGH);
  }
}

/**
 * verificarComandoSerial()
 * Escuta o Monitor Serial para comandos de controle externo.
 * Permite simular previsão do tempo sem alterar o hardware.
 *
 * Comandos aceitos:
 *   CHUVA_ON  → g_previsao_chuva = true
 *   CHUVA_OFF → g_previsao_chuva = false
 */
void verificarComandoSerial() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd == "CHUVA_ON") {
      g_previsao_chuva = true;
      Serial.println(">>> [SISTEMA] Previsao de CHUVA ATIVADA - Irrigacao BLOQUEADA <<<");
    } else if (cmd == "CHUVA_OFF") {
      g_previsao_chuva = false;
      Serial.println(">>> [SISTEMA] Previsao de chuva REMOVIDA - Irrigacao LIBERADA <<<");
    } else {
      Serial.print(">>> [SISTEMA] Comando desconhecido: ");
      Serial.println(cmd);
    }
  }
}

/**
 * imprimirRelatorio()
 * Imprime o estado completo do sistema no Monitor Serial.
 * Formato otimizado para coleta de dados e geração de histórico.
 */
void imprimirRelatorio() {
  Serial.println("----------------------------------------------");
  Serial.print("[Tempo] ");
  Serial.print(millis() / 1000);
  Serial.println(" s");

  // Sensores
  Serial.print("[DHT22] Umidade: ");
  Serial.print(g_umidade, 1);
  Serial.print("%  |  Temperatura: ");
  Serial.print(g_temperatura, 1);
  Serial.println(" C");

  // pH com diagnóstico
  Serial.print("[pH]    Valor: ");
  Serial.print(g_ph, 2);
  Serial.print("  |  Status: ");
  if (g_ph < PH_IDEAL_MIN)       Serial.println("ACIDO - Aplicar Potassio (Btn K)");
  else if (g_ph > PH_IDEAL_MAX)  Serial.println("ALCALINO - Solo basico demais");
  else                           Serial.println("IDEAL");

  // Bomba e previsão de chuva
  Serial.print("[BOMBA] ");
  Serial.print(g_bomba_ligada ? "LIGADA  " : "DESLIGADA");
  Serial.print("  |  Previsao Chuva: ");
  Serial.println(g_previsao_chuva ? "SIM (bloqueio ativo)" : "NAO");

  // Nutrientes
  Serial.print("[NPK]   N=");
  Serial.print(g_cnt_nitrogenio);
  Serial.print("  P=");
  Serial.print(g_cnt_fosforo);
  Serial.print("  K=");
  Serial.println(g_cnt_potassio);
}
