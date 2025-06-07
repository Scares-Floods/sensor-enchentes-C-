Nome e RM
Bruno Scuciato RM:562159  
João Basta RM:565383 
Kelwin Silva RM: 566348

# 🌧️ Sistema de Monitoramento de Enchentes com Arduino

Este projeto é um **Sistema de Alerta de Enchentes** baseado em Arduino, utilizando sensores para detectar o nível da água, intensidade da chuva, umidade e temperatura. Ele fornece **alertas visuais e sonoros** para diferentes níveis de risco (Seguro, Atenção e Perigo).

## 🔧 Funcionalidades

- Medição do nível de água com sensor ultrassônico (HC-SR04)
- Simulação de chuva com potenciômetro (ou sensor de chuva real)
- Leitura de temperatura e umidade com DHT22
- Alertas com LEDs e buzzer
- Exibição de status no LCD I2C
- Envio de dados via Serial Monitor

## 📦 Componentes Utilizados

| Componente        | Descrição                      |
|-------------------|-------------------------------|
| Arduino UNO       | Microcontrolador principal     |
| HC-SR04           | Sensor de distância ultrassônico |
| DHT22             | Sensor de temperatura e umidade |
| Potenciômetro     | Simulação de sensor de chuva   |
| LCD I2C (16x2)    | Exibição de informações        |
| Buzzer            | Alarme sonoro                  |
| LEDs (Verde, Amarelo, Vermelho) | Indicadores visuais de risco |
| Resistores        | Para os LEDs (220Ω sugerido)   |
| Jumpers, Protoboard | Conexões e montagem           |

## ⚙️ Como Funciona

- O **HC-SR04** mede a distância até a superfície da água. Quanto menor a distância, maior o risco.
- O **potenciômetro** simula a intensidade da chuva (pode ser substituído por um sensor de chuva real).
- O **DHT22** fornece leituras ambientais que ajudam na avaliação das condições.
- O sistema muda de estado conforme os sensores indicam:
  - `SEGURO`: LED verde aceso
  - `ATENÇÃO`: LED amarelo + buzzer lento
  - `PERIGO`: LED vermelho + buzzer rápido

## 🖥️ Exibição no LCD

A cada segundo, o sistema atualiza o display com:
- Estado do sistema e nível da água (cm)
- Intensidade da chuva e umidade do ar (%)

## 📡 Dados no Serial Monitor

O sistema imprime todos os dados lidos no Serial Monitor, útil para testes e depuração.

## 🛠️ Instalação

1. Instale as bibliotecas necessárias no Arduino IDE:
   - `LiquidCrystal_I2C`
   - `DHT sensor library` (da Adafruit)
   - `Adafruit Unified Sensor`

2. Faça as conexões conforme o esquema de hardware (veja abaixo).

3. Carregue o código no Arduino.

4. Abra o Serial Monitor com baud rate de **9600**.

## 📐 Esquema de Conexões (resumo)

| Componente     | Pino Arduino |
|----------------|--------------|
| Trigger HC-SR04 | D7           |
| Echo HC-SR04    | D6           |
| DHT22           | D2           |
| Potenciômetro   | A0           |
| LED Verde       | D13          |
| LED Amarelo     | D12          |
| LED Vermelho    | D11          |
| Buzzer          | D10          |
| LCD I2C         | SDA -> A4, SCL -> A5 (UNO) |

## 📸 Imagens (opcional)

Adicione aqui imagens do projeto montado, do circuito e do display em funcionamento.

## 🧪 Testes

Gire o potenciômetro no sentido horário para simular chuva forte e observe a mudança no estado do sistema.

## 📄 Licença

Este projeto está sob a licença MIT. Sinta-se livre para usar, modificar e compartilhar.

---

👨‍💻 Projeto desenvolvido com fins educacionais. Ideal para ensino de eletrônica, programação embarcada e IoT.
