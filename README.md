# 🍊 FarmTech Solutions - Sistema de Irrigação Inteligente (Fase 3)

## 📖 Descrição do Projeto
Este projeto compõe a **Fase 3** do desenvolvimento do sistema de gestão agrícola da startup **FarmTech Solutions**. O objetivo principal é evoluir o dispositivo IoT baseado no microcontrolador ESP32 para monitoramento de dados agrícolas, simulando não apenas as tomadas de decisões automatizadas, mas também a persistência e carga histórica dessas informações em um banco de dados relacional.

O sistema é focado na cultura da **Laranja (Citros)**, sendo capaz de ler a umidade do solo, monitorar os níveis químicos (pH), reagir à inserção de nutrientes (NPK) e enviar esses registros estruturados para análise.

---

## 🛠️ Componentes e Hardware (Simulação no Wokwi)
Devido às limitações de sensores agrícolas nativos na plataforma, foram feitas substituições didáticas conforme as diretrizes do projeto:
* **Microcontrolador:** 1x ESP32.
* **Sensor de Umidade do Solo:** 1x DHT22 (adotado aqui para simular a umidade do solo).
* **Sensor de pH:** 1x Sensor LDR, mapeado via código de dados analógicos para a escala de pH de 0.0 a 14.0.
* **Nutrientes (NPK):** 3x Botões operando em formato digital representando Nitrogênio (N - Botão Branco), Fósforo (P - Botão Cinza) e Potássio (K - Botão Vermelho).
* **Bomba de Irrigação:** 1x Relé Módulo Azul.

### 📸 Imagem do Circuito Corrigido

<img width="502" height="532" alt="Circuito Wokwi" src="[https://github.com/user-attachments/assets/82b1db2e-6b1c-4c3e-b0ee-8573ee2404a1](https://github.com/user-attachments/assets/82b1db2e-6b1c-4c3e-b0ee-8573ee2404a1)" />

---

## 🧠 Lógica de Negócio e Regras de Decisão

### 1. Irrigação Automatizada (Umidade)
* **Umidade <= 50%:** Solo seco. O sistema **LIGA** a bomba de irrigação.
* **Umidade >= 80%:** Solo úmido. O sistema **DESLIGA** a bomba para evitar fungos.
* **Entre 50% e 80%:** Estado mantido (zona de segurança).

### 2. Monitoramento Químico (pH e NPK)
O pH ideal para a laranjeira é entre **5.5 e 6.5**. O sistema simula de forma dinâmica o impacto dos nutrientes no solo:
* **Abaixo de 5.5 (Ácido):** Recomenda acionar o **Potássio (K - Botão Vermelho)** para elevar o pH.
* **Entre 5.5 e 6.5 (Ideal):** Permite acionar o **Fósforo (P - Botão Cinza)** para manutenção das raízes.
* **Acima de 6.5 (Alcalino):** Recomenda acionar o **Nitrogênio (N - Botão Branco)** para baixar o pH.

---

## 🗄️ Integração com Banco de Dados (Oracle SQL Developer)

Uma das grandes evoluções da Fase 3 foi a modelagem da persistência de dados. Os logs estruturados gerados pelo Monitor Serial do ESP32 foram convertidos e mapeados para um arquivo estruturado `.csv`.

Utilizando o assistente de importação de dados locais do **Oracle SQL Developer**, a carga foi realizada com sucesso na tabela de monitoramento, criando um histórico real de comportamento dos sensores.

### 📊 Evidência da Importação no Banco de Dados

<img width="1907" height="1101" alt="image" src="https://github.com/user-attachments/assets/8d721657-aa09-4015-b34a-05e87d42856f" />


---

## 📁 Estrutura do Repositório
* `src/main.cpp`: Código-fonte C/C++ rodando no ESP32 com simulação dinâmica de pH.
* `database/dados_farmtech.csv`: Arquivo de dados gerado para a importação no SQL Developer.
* `database/cdu_scripts.sql`: Script contendo a estrutura da tabela utilizada.
* `README.md`: Documentação completa do projeto.

---

## 🎥 Demonstração de Funcionamento

No vídeo abaixo, demonstramos todo o circuito funcionando de ponta a ponta: as tomadas de decisão do Relé baseadas no DHT22, o acionamento corrigido do botão vermelho de Potássio e o processo de importação visual do arquivo CSV gerado diretamente para dentro do Oracle SQL Developer.

🔗 **Link do Vídeo no YouTube:** *[https://youtu.be/yDUCwO61Y7g](https://youtu.be/yDUCwO61Y7g)*

🔗 **Link do Projeto no Wokwi:** *[https://wokwi.com/projects/464135313164062721](https://wokwi.com/projects/464135313164062721)*

---
*Projeto desenvolvido para a FIAP - Fase 3 por integrantes da Startup FarmTech Solutions.*
