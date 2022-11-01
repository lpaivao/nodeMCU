#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <gpio.h>
#include <uart.h>

// COMANDOS DE REQUISIÇÃO
#define SITUACAO_ATUAL_NODE 0x03
#define SOLICITA_ENTRADA_ANALOGICA 0x04
#define SOLICITA_ENTRADA_DIGITAL 0x05
#define ACENDE_LED 0x06

// COMANDOS DE RESPOSTA
#define NODE_COM_PROBLEMA 0x1F
#define NODE_FUNCIONANDO 0x00
#define MEDIDA_ENTRADA_ANALOGICA 0x01
#define ESTADO_ENTRADA_DIGITAL 0x02

#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK "Pbl-Sistemas-Digitais"
#endif

#define BAUDUART0 115200
#define BAUDUART1 115200

const char *ssid = STASSID;
const char *password = STAPSK;

// Nome do ESP na rede
const char *host = "ESP-10.0.0.109";

// Definições de rede
IPAddress local_IP(10, 0, 0, 109);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 0, 0);

bool lstate = LOW;

enum sensor_type
{
  DIGITAL,
  ANALOG
};

enum codes
{
  READ_DIGITAL = 'A',
  READ_ANALOG = 'B',
  NODE_STATUS = 'C'
};

// Estrutura contendo as referencias para uso de um sensor, tipo, funcão de leitura e configuração
typedef struct
{
  enum sensor_type type;
  int (*read)(int), last_read, pin;
  // bool (*set)(int);
} sensor;

// Estrutura de mapeamento de sensores
typedef struct
{
  int total = 16, installed;
  enum sensor_type type;
  sensor *sensors[16];
} sensor_map;

uart_t *uart0;

sensor_map digital_sensors, analog_sensors;

int readDigitalSensor(int pin)
{
  return GPIO_INPUT_GET(pin);
}

int readAnalogSensor(int pin)
{
  return analogRead(pin);
}

sensor DS0 = {
    DIGITAL,
    readDigitalSensor,
    0,
    GPIO_PIN_ADDR(16),
    // setDigitalSensor,
};
sensor DS1 = {
    DIGITAL,
    readDigitalSensor,
    0,
    GPIO_PIN_ADDR(5),
    // setDigitalSensor,
};
sensor AS0 = {
    ANALOG,
    readAnalogSensor,
    0,
    GPIO_PIN_ADDR(17),
    // setDigitalSensor,
};

void ota_startup()
{
  // Configuração do IP fixo no roteador, se não conectado, imprime mensagem de falha
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]()
                     {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    } });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupSensorMaps()
{
  digital_sensors.type = DIGITAL;
  digital_sensors.installed = 2;
  digital_sensors.sensors[0] = &DS0;
  digital_sensors.sensors[1] = &DS1;
  for (int i = 2; i < digital_sensors.total; i++)
  {
    digital_sensors.sensors[i] = &DS0;
  } 
  analog_sensors.type = ANALOG;
  analog_sensors.installed = 1;
  analog_sensors.sensors[0] = &AS0;
 // for (int i = 0; i < analog_sensors.total; i++)
 // {
}

void gpioSetup()
{
  // pinMode(D0, OUTPUT);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, GPIO_IN_ADDRESS);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, GPIO_IN_ADDRESS);
  gpio_init();
}

void setup()
{
  // Serial.begin(BAUDUART0);
  uart0 = uart_init(UART0, BAUDUART0, UART_8N1, 0, 1, 10, 0);

  ets_uart_printf("\nBooting\r\n");
  // ota_startup();
  setupSensorMaps();
  ets_uart_printf("Ready\r\n");
  ets_install_uart_printf();
}

char *recByte = (char *)malloc(sizeof(char) * 2);
char *sendByte = (char *)malloc(sizeof(char) * 1);
int addr = 0;

void loop()
{
  // ArduinoOTA.handle();
  // if (Serial.available() > 0) {
  //   Serial.print(Serial.read());
  // }
  while ((int)uart_rx_available(uart0) > 0)
  {

    uart_read(uart0, recByte, 2);
         
    switch (recByte[0])
    {
    case NODE_STATUS:
      ets_uart_printf("Status Request:\n");
      ets_putc('\n');
      break;
    case READ_ANALOG:
      addr = recByte[1] - '0';
      if (addr >= analog_sensors.installed)
      {
      ets_uart_printf("[ERROR] Analog Sensor\n");
      ets_putc('\n');
        break;
      }
      ets_uart_printf("Analog Sensor:\n");
      ets_putc('\n');
      break;
    case READ_DIGITAL:
      addr = recByte[1];
      if (addr >= digital_sensors.installed)
      {
        ets_uart_printf("[ERROR] Digital Sensor\n");
        ets_putc('\n');
        break;
      }
      if (addr == GPIO_PIN_ADDR(16))
      {
        ets_uart_printf("Digital Sensor (1):\nLevel: ");
        uart_write_char(uart0, GPIO_INPUT_GET(16) + '0');
      }
      else if (addr == GPIO_PIN_ADDR(5))
      {
        ets_uart_printf("Digital Sensor (2):\nLevel: ");
        uart_write_char(uart0, GPIO_INPUT_GET(5) + '0');
      }

      ets_putc('\n');
      ets_putc('\n');

      break;
    default:
      ets_uart_printf("None\n");
      ets_putc('\n');
      break;
    }
    uart_flush(uart0);
  }
}