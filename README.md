# Trabalho Final: Estação Meteorológica

## Integrantes

| Nome                                           | Matrícula |
| ---------------------------------------------- | --------- |
| [Laís Soares](https://github.com/Laisczt)      | 211029512 |
| [Bruna Lima](https://github.com/libruna)       | 211041105 |
| [José Augusto](https://github.com/JAugustoM)   | 231026429 |
| [Ana Catarina](https://github.com/an4catarina) | 211041099 |

## 1. Sobre

Este repositório possui a implementação de uma estação meteorológica pessoal (PWS) com base na solução [Open Weather Station](https://github.com/panchazo/open-weather-station). A implementação foi adaptada de forma a utilizar uma ESP32 e no contexto da disciplina de Fundamentos de Sistemas Operacionais da UnB-FTCE.

PWS são estações de monitoramento meteorológico destinadas ao uso pessoal, com foco em acessibilidade (preço e usabilidade).

Estudo inicial: [Open Weather Station](https://github.com/Laisczt/FSE-OpenWeatherStation).

### 1.1 Funcionalidades

* Sensores atmosféricos
  * Pressão atmosférica
  * Temperatura
  * Luminosidade
  * Humidade
  * Precipitação
* Comunicação sem-fio por WiFi
* Integração com a plataforma WUnderground
* Interface de visualização dos dados em tempo real (MQTT)

## 2. Implementação

### 2.1 Diferenças com relação a Open Weather Station

As principais diferenças da nossa implementação em relação a OWS (excluindo componentes específicos, arquitetura), são:

* Na OWS, o sistema Arduino se comunica através de Bluetooth com um dispositivo móvel, o qual envia os dados ao servidor através do WiFi ou, em especial, à rede móvel.
Como a nossa implementação não faz uso de rede móvel, dispensa também o uso de um dispositivo móvel e conexão Bluetooth, a própria ESP32 é usada para conexão WiFi
* Não possuimos sensores de velocidade e direção de vento/rajada de vento.

### 2.2 Bill of Materials (BoM)

| Qtd   | Componente                  | Conexões no ESP32 (Pinos)                                                | Função / Descrição                                                                                                                               |
| :-----:| :----------------------------| :-------------------------------------------------------------------------| :-------------------------------------------------------------------------------------------------------------------------------------------------|
| **1** | **ESP32 DevKitC-1**         | -                                                                        | Microcontrolador principal responsável pela leitura dos sensores, conexão Wi-Fi, comunicação MQTT e integração com a API do Weather Underground. |
| **1** | **Módulo Sensor BMP280**    | **SDA:** GPIO 21<br>**SCL:** GPIO 22<br>**VCC:** 3.3V<br>**GND:** GND    | Comunicação via I2C. Responsável pela leitura de Temperatura e Pressão Atmosférica.                                                              |
| **1** | **Módulo Sensor DHT11**     | **Data (OUT):** GPIO 4<br>**VCC:** 3.3V<br>**GND:** GND                  | Protocolo proprietário (1-wire). Responsável pela leitura da Umidade Relativa do ar.                                                             |
| **1** | **Módulo Sensor LDR**       | **Sinal Analógico (AO):** GPIO 34<br>**VCC:** 3.3V<br>**GND:** GND       | Leitura analógica. Usado para estimar a Radiação Solar / Luminosidade.                                                                           |
| **1** | **Módulo Sensor de Chuva YL-83**  | **Sinal Digital (DO):** GPIO 18<br>**VCC:** 3.3V (ou 5V)<br>**GND:** GND | Leitura digital (0 ou 1). Placa de detecção de gotas + comparador LM393. Define se está chovendo ou não.                                         |
| **1** | **Protoboard (Breadboard)** | -                                                                        | Base para a montagem e distribuição de energia (3.3V e GND) para todos os módulos. Alternativamente, utilizar 2 protoboards podem facilitar a montagem |
| **1** | **Kit de Fios Jumpers**     | -                                                                        | Fios Macho-Macho e Macho-Fêmea para interligar os módulos à placa ESP32 e às trilhas de alimentação da protoboard.                               |

### 2.3 Montagem

![Esquemático](assets/schematic.png)
Figura 1 - Esquemático de montagem

## 3. Como executar o projeto

### 3.1 Pré-requisitos

Antes de iniciar, certifique-se de ter:

- Todos os componentes de hardware descritos na seção **2.2 Bill of Materials (BoM)**, montados conforme a **Figura 1**.
- O **ESP-IDF** (versão 5.x ou superior) instalado e configurado.
  - Consulte o guia de instalação: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/#installation
- Um **broker MQTT** instalado. Recomendação: *Eclipse Mosquitto*
  - Consulte o guia de instalação: https://mosquitto.org/download/

### 3.2 Criando uma estação no Weather Underground

Para que a ESP32 envie os dados coletados, é necessário criar uma *Personal Weather Station* e obter suas credenciais.

1. Acesse o site do **[Weather Underground](https://www.wunderground.com/)** e crie uma conta.
2. Após o login, acesse **My Profile > My Devices**
3. Clique em **Add New Device**.
4. Selecione a opção **Other** e clique em **Next**.
5. Informe o endereço onde a estação está instalada.

Ao concluir, serão geradas duas credenciais:

- **Station ID:** identificador da estação.
- **Station Key:** chave de autenticação da estação.

> **Aviso:** guarde essas informações, pois elas serão necessárias na etapa de configuração.

### 3.3 Configurando o projeto

### 3.3.1 MQTT

Este projeto utiliza o **Eclipse Mosquitto** como broker MQTT para receber os dados publicados pela ESP32.

#### a) Configurando o broker

Após instalar o Mosquitto, crie um arquivo de configuração chamado `mosquitto.conf` com o seguinte conteúdo:

```conf
allow_anonymous false
password_file password 
```

Em seguida, crie um usuário para autenticação com o seguinte comando:

```bash
mosquitto_passwd -c password [NOMEUSUARIO]
```

Será solicitada uma senha para o usuário. Essa senha deverá ser utilizada também na configuração da ESP32.

Por fim, inicie o broker utilizando o arquivo de configuração criado:

```bash
mosquitto -c mosquitto.conf
```
#### b) Configurando as credenciais da ESP32

As credenciais do MQTT são configuradas por meio do **Kconfig**.

Na raiz do projeto, execute:

```bash
idf.py menuconfig
```

Em seguida, acesse:

```text
MQTT Configuration
```

Preencha as seguintes informações:
- URI do broker (provavelmente mqtt://192.168.x.x:1883)
- Usuário MQTT
- Senha MQTT

Salve a configuração antes de sair do menu.

> **Aviso:** as credenciais definidas em `MQTT_USER` e `MQTT_PASS` devem corresponder exatamente ao usuário e à senha configurados no broker Mosquitto.

### 3.3.2 WiFi e Estação

As credenciais do WiFi e da Estação são configuradas por meio do **Kconfig**.

Na raiz do projeto, execute:

```bash
idf.py menuconfig
```

Em seguida, acesse:

```text
WiFi and Weather Station Configuration
```

Preencha as seguintes informações:

- SSID da rede Wi-Fi;
- Senha da rede Wi-Fi;
- Station ID;
- Station Key.


Salve a configuração antes de sair do menu.

### 3.4 Compilando o projeto

Após configurar as credenciais, compile o projeto com:

```bash
idf.py build
```

### 3.5 Executando o projeto

Execute o comando:

```bash
idf.py flash monitor
```

Durante a execução será possível acompanhar:

- Inicialização da ESP32;
- Conexão com a rede Wi-Fi;
- Leitura dos sensores;
- Comunicação MQTT;
- Requisições HTTP para o Weather Underground;
- Logs gerais da aplicação.

Opcionalmente, para visualizar somente os dados telemétricos em JSON, execute em um novo terminal:

```bash
mosquitto_sub --topic "esp/data" --host localhost --port [PORTA] --username [NOMEUSUARIO] --pw [SENHA]
```
