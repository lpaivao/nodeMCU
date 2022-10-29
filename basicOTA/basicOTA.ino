#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <gpio.h>
#include <uart.h>

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
  READ_DIGITAL = 0x5,
  READ_ANALOG = 0x4,
  NODE_STATUS = 0x3
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
    D0,
    // setDigitalSensor,
};
sensor DS1 = {
    DIGITAL,
    readDigitalSensor,
    0,
    D5,
    // setDigitalSensor,
};
sensor AS0 = {
    ANALOG,
    readAnalogSensor,
    0,
    A0,
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
  for (int i = 0; i < analog_sensors.total; i++)
  {
    analog_sensors.sensors[i] = &AS0;
  }
}

void setup()
{
  // Serial.begin(BAUDUART0);
  uart0 = uart_init(UART0, BAUDUART0, UART_8N1, 0, 1, 10, 0);
  Serial.println("Booting");
  // ota_startup();
  setupSensorMaps();
  Serial.println("Ready");
}

char *recByte = (char *)malloc(sizeof(char) * 2);
int addr = 0;

void loop()
{
  // ArduinoOTA.handle();
  // if (Serial.available() > 0) {
  //   Serial.print(Serial.read());
  // }
  while ((int)uart_rx_available(uart0) > 0)
  {
    Serial.readBytes(recByte, 2);
    switch (recByte[0])
    {
    case NODE_STATUS:
      Serial.printf("[STATUS] NodeMCU status report code recieved\n");
      break;
    case READ_ANALOG:
      addr = recByte[1];
      if (addr >= analog_sensors.installed)
      {
        break;
      }
      Serial.printf("[ READ ] A0: %d\n", analogRead(analog_sensors.sensors[addr]->pin));
      break;
    case READ_DIGITAL:
      addr = recByte[1];
      if (addr >= digital_sensors.installed)
      {
        break;
      }
      Serial.printf("[ READ ] D: %d\n", digitalRead(digital_sensors.sensors[addr]->pin));
      break;
    default:
      Serial.printf("[ NONE ] Skipping ...\n");
      break;
    }
  }
  // delay(5);
}