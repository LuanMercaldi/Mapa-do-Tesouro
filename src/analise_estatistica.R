# ==============================================================================
# FARMTECH SOLUTIONS - FASE 2: Análise Estatística para Irrigação Inteligente
# Cultura: Laranja (Citros)
# Modelo: Regressão Logística (Data Science)
# ==============================================================================

# 1. Criação do Banco de Dados Histórico (Simulando dados da EMBRAPA/MAPA)
# O dataset relaciona a Umidade do Solo (%), Temperatura (°C) e a Decisão Histórica da Bomba (1 = Ligada, 0 = Desligada)
dados_historicos <- data.frame(
  umidade_solo = c(30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90),
  temperatura = c(32, 30, 29, 31, 28, 25, 26, 24, 22, 21, 20, 19, 18),
  bomba_ligada = c(1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0)
  
)

print("Dados Históricos Carregados:")
print(dados_historicos)

# 2. Treinamento do Modelo Estatístico (Regressão Logística)
# Vamos ensinar o R a prever quando ligar a bomba com base na umidade e temperatura
modelo_irrigacao <- glm(bomba_ligada ~ umidade_solo + temperatura,
                        data = dados_historicos,
                        family = binomial)

# Resumo estatístico do modelo gerado
print("Resumo Estatístico do Modelo (Regressão Logística):")
summary(modelo_irrigacao)

# 3. Testando o Modelo com os dados do ESP32 (Wokwi)
# Vamos simular que o ESP32 enviou uma leitura atual da fazenda: Umidade em 48% e Temperatura em 28°C
leitura_atual <- data.frame(umidade_solo = 48, temperatura = 28)

# 4. Cálculo de Probabilidade (A Análise Estatística em ação)
probabilidade <- predict(modelo_irrigacao, newdata = leitura_atual, type = "response")

cat("\n======================================================\n")
cat("RESULTADO DA ANÁLISE ESTATÍSTICA PARA O RLÉ AZUL\n")
cat("======================================================\n")
cat(sprintf("Leitura Atual -> umidade: %d%% | Temperatura: %dºC\n", leitura_atual$umidade_solo, leitura_atual$temperatura))
cat(sprintf("Probabilidade estatística de precisar de irrigação: %.2f%%\n", probabilidade*100))

# 5. Tomada de Decisão Automatizada
limite_decisao <- 0.50 # 50% de probabilidade

if(probabilidade > limite_decisao){
  cat("DECISÃO DATA SCIENCE: LIGAR A BOMBA DE IRRIGAÇÃO (Relé Azul = HIGH)\n")
} else{
  cat("DECISÃO DATA SCIENCE: MANTER A BOMBA DESLIGADA (Relé Azul = LOW)\n")
}
