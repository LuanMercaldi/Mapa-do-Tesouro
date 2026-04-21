# 🍊 FarmTech Solutions - Sistema de Irrigação Inteligente (Fase 2)

## 📖 Descrição do Projeto
Este projeto compõe a Fase 2 do desenvolvimento do sistema de gestão agrícola da startup **FarmTech Solutions**. O objetivo principal é construir e simular um dispositivo eletrônico IoT baseado no microcontrolador ESP32 para monitoramento de dados em uma fazenda e automatização da irrigação.

Para esta fase, simulada na plataforma Wokwi, criamos um sistema inteligente focado na cultura da **Laranja (Citros)**. O sistema é capaz de ler a umidade do solo, monitorar os níveis químicos (pH) e reagir à inserção de nutrientes (NPK), decidindo de forma autônoma o momento exato de acionar a bomba d'água.

---

## 🛠️ Componentes e Hardware (Simulação no Wokwi)
Devido às limitações de sensores agrícolas nativos na plataforma, foram feitas substituições didáticas conforme as diretrizes do projeto:
* **Microcontrolador:** 1x ESP32.
* **Sensor de Umidade do Solo:** 1x DHT22 (que originalmente mede umidade do ar, adotado aqui para o solo).
* **Sensor de pH:** 1x Sensor LDR, mapeado via código de dados analógicos (luminosidade) para a escala de pH de 0.0 a 14.0.
* **Nutrientes (NPK):** 3x Botões operando em formato digital ("true" ou "false" / pressionado ou não) representando Nitrogênio (N - Botão Branco), Fósforo (P - Botão Cinza) e Potássio (K - Botão Vermelho).
* **Bomba de Irrigação:** 1x Relé Módulo Azul.

### 📸 Imagem do Circuito

<img width="502" height="532" alt="image" src="https://github.com/user-attachments/assets/82b1db2e-6b1c-4c3e-b0ee-8573ee2404a1" />

---

## 🧠 Lógica de Negócio e Regras de Decisão

A cultura escolhida foi a **Laranja**, que possui necessidades muito específicas para evitar apodrecimento de raízes e garantir a absorção de nutrientes. O código em C/C++ divide a automação em duas vertentes:

### 1. Irrigação Automatizada (Umidade)
A laranjeira é sensível tanto ao estresse hídrico quanto ao encharcamento. O sensor DHT22 controla de forma totalmente autônoma o Relé Azul (Bomba):
* **Umidade <= 50%:** Solo seco. O sistema **LIGA** a bomba de irrigação.
* **Umidade >= 80%:** Solo atingiu o limite de segurança. O sistema **DESLIGA** a bomba para evitar fungos.
* **Entre 50% e 80%:** Estado mantido, sem necessidade de alterações.

### 2. Monitoramento Químico (pH e NPK)
O pH ideal para a máxima absorção de nutrientes na laranjeira é levemente ácido, entre **5.5 e 6.5**. Como os níveis de NPK afetam o pH da terra, o sistema fornece alertas interativos no Monitor Serial guiando as ações do agricultor:

| Leitura do pH (LDR) | Classificação | Ação Recomendada pelo Sistema |
| :--- | :--- | :--- |
| **Abaixo de 5.5** | Muito Ácido | Acionar o botão **Potássio (K)** para simular a elevação do pH. |
| **Entre 5.5 e 6.5** | **Ideal** | Acionar o botão **Fósforo (P)**, atuando como nutriente de manutenção para raízes. |
| **Acima de 6.5** | Alcalino | Acionar o botão **Nitrogênio (N)**, conhecido por acidificar (baixar) o pH do solo. |

**Mecânica da Simulação:** Como não possuímos um sensor de pH real, a dinâmica da simulação exige interação manual. Quando o sistema pede a inserção de um nutriente (apertar os botões N, P ou K), o usuário deve **alterar manualmente o nível do sensor LDR**, comprovando a tese de que a adição de nutrientes altera o pH da terra.

---

## 🚀 Programa "Ir Além" (Data Science e IoT)
Para trazer uma visão prática do uso da IoT e da IA na otimização de recursos agrícolas, nosso grupo desenvolveu de forma total os dois itens opcionais do projeto:

### 🌩️ Ir Além 1: Integração Python com API Pública
Integramos dados meteorológicos obtidos da API pública **OpenWeather** usando Python para prever condições climáticas adversas e ajustar a irrigação automaticamente.
* **Objetivo:** Se houver previsão de chuva, o sistema suspende a irrigação para economizar recursos (água e energia).
* **Solução Técnica:** Como a integração automática direta não é trivial no plano gratuito do Wokwi, implementamos a transferência de dados via Monitor Serial do simulador ESP32. O código lê a previsão gerada pelo Python e, utilizando as funções `Serial.available()` e `Serial.read()`, recebe comandos do usuário via teclado ('C' para chuva, 'S' para sol) para intervir na bomba em tempo real.

### 📊 Ir Além 2: Análise Estatística em R (Data Science)
Implementamos uma análise estatística em linguagem **R** para decidir matematicamente se a bomba de irrigação deve ser ligada, agregando conhecimentos de Data Science ao projeto IoT.
* **Base de Dados:** Simulamos um banco de dados histórico com informações de fontes públicas do Agronegócio (como EMBRAPA e MAPA).
* **Modelo Utilizado:** Desenvolvemos um algoritmo de **Regressão Logística** que cruza variáveis atuais de umidade do solo e temperatura para calcular a probabilidade exata da necessidade de acionamento do relé azul, embasando a lógica do nosso hardware com inteligência de dados.

---

## 📁 Estrutura do Repositório e Entregáveis
Os arquivos deste projeto estão organizados da seguinte forma no repositório:
* `sketch.ino`: Código-fonte C/C++ desenvolvido e rodando no ESP32.
* `previsao_tempo.py`: Script do Programa Ir Além 1 (Python + Consulta de API OpenWeather).
* `analise_estatistica.R`: Script do Programa Ir Além 2 (Regressão Logística em R).
* `README.md`: Documentação completa do projeto.

---

## 🎥 Demonstração de Funcionamento

No vídeo abaixo, demonstramos todo o circuito em funcionamento, provando a eficácia do código na tomada de decisão do relé baseada no DHT22, a interatividade dos botões de NPK com as mudanças manuais efetuadas no LDR (simulador de pH) e a atuação do programa de Previsão do Tempo via teclado.

🔗 **Link do Vídeo no YouTube (Até 5 minutos):** *https://youtu.be/yDUCwO61Y7g*

🔗 **Link do Projeto no Wokwi:** *https://wokwi.com/projects/461237221191174145*

---
*Projeto desenvolvido para a FIAP - Fase 2 por integrantes da Startup FarmTech Solutions.*
```
