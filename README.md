# 🍊 FarmTech Solutions - Sistema de Irrigação Inteligente (Fase 2)

## 📖 Descrição do Projeto
Este projeto compõe a Fase 2 do desenvolvimento do sistema de gestão agrícola da startup **FarmTech Solutions**. O objetivo principal é construir e simular um dispositivo eletrônico IoT baseado no microcontrolador ESP32 para monitoramento de dados em uma fazenda e automatização da irrigação.

Para esta fase, simulada na plataforma Wokwi, criamos um sistema inteligente focado na cultura da **Laranja (Citros)**. O sistema é capaz de ler a umidade do solo, monitorar os níveis químicos (pH) e reagir à inserção de nutrientes (NPK), decidindo de forma autônoma o momento exato de acionar a bomba d'água.

---

## 🛠️ Componentes e Hardware (Simulação no Wokwi)
Devido às limitações de sensores agrícolas nativos na plataforma, foram feitas substituições didáticas conforme as diretrizes do projeto:
* **Microcontrolador:** 1x ESP32.
* **Sensor de Umidade do Solo:** 1x DHT22 (que originalmente mede umidade do ar, adotado aqui para o solo).
* **Sensor de pH:** 1x Sensor LDR (Light Dependent Resistor), mapeado via código de dados analógicos (luminosidade) para a escala de pH de 0.0 a 14.0.
* **Nutrientes (NPK):** 3x Botões Verdes (Pushbuttons) operando em formato digital ("true" ou "false" / pressionado ou não) representando Nitrogênio (N), Fósforo (P) e Potássio (K).
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
O pH ideal para a máxima absorção de nutrientes na laranjeira é levemente ácido, entre **5.5 e 6.5**. Como os níveis de NPK afetam o pH da terra, o sistema fornece alertas interativos no Monitor Serial guiando as ações do agricultor (ou do usuário na simulação):

| Leitura do pH (LDR) | Classificação | Ação Recomendada pelo Sistema |
| :--- | :--- | :--- |
| **Abaixo de 5.5** | Muito Ácido | Acionar o botão **Potássio (K)** para simular a elevação do pH. |
| **Entre 5.5 e 6.5** | **Ideal** | Acionar o botão **Fósforo (P)**, atuando como nutriente de manutenção para raízes. |
| **Acima de 6.5** | Alcalino | Acionar o botão **Nitrogênio (N)**, conhecido por acidificar (baixar) o pH do solo. |

**Mecânica da Simulação (Regra de Ouro):** Como não possuímos um sensor de pH real, a dinâmica da simulação exige interação manual. Quando o sistema pede a inserção de um nutriente (apertar os botões N, P ou K), o usuário deve **alterar manualmente o nível do sensor LDR**, comprovando a tese de que a adição de nutrientes altera o pH da terra.

---

## 🎥 Demonstração de Funcionamento

No vídeo abaixo, demonstramos todo o circuito em funcionamento, provando a eficácia do código na tomada de decisão do relé baseada no DHT22 e a interatividade dos botões de NPK com as mudanças manuais efetuadas no LDR (simulador de pH).

🔗 **Link do Vídeo no YouTube (Até 5 minutos):** *[INSERIR O SEU LINK DO YOUTUBE AQUI]*

🔗 **Link do Projeto no Wokwi:** *[INSERIR O LINK DO SEU PROJETO SALVO NO WOKWI]*

---
*Projeto desenvolvido para o FIAP PBL - Fase 2.*
