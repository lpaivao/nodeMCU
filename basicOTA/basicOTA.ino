#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "NANET_24"
#define STAPSK "254790bi"
#endif

#define BAUDUART0 115200
#define BAUDUART1 115200

const char *ssid = STASSID;
const char *password = STAPSK;

bool lstate = LOW;

enum sensor_type { DIGITAL,
                   ANALOG };

enum codes {
  READ_DIGITAL = 0x41,
  READ_ANALOG = 0x42,
  NODE_STATUS = 0x43
};


// Estrutura contendo as referencias para uso de um sensor, tipo, funcão de leitura e configuração
typedef struct {
  enum sensor_type type;
  int (*read)(int), last_read, pin;
  bool (*set)(int);
} sensor;

// Estrutura de mapeamento de sensores
typedef struct {
  int total = 16, installed;
  enum sensor_type type;
  sensor *sensors[16];
} sensor_map;

sensor_map digital_sensors, analog_sensors;

int readDigitalSensor(int pin) {
  return digitalRead(pin);
}

int readAnalogSensor(int pin) {
  return analogRead(pin);
}

bool setDigitalSensor(int pin) {
  digitalWrite(pin, HIGH);
  return false;
}


sensor DS0 = {
  DIGITAL,
  readDigitalSensor,
  0,
  D0,
  setDigitalSensor,
};
sensor DS1 = {
  DIGITAL,
  readDigitalSensor,
  0,
  D5,
  setDigitalSensor,
};
sensor AS0 = {
  ANALOG,
  readAnalogSensor,
  0,
  A0,
  setDigitalSensor,
};



void ota_startup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
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
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupSensorMaps() {
  digital_sensors.type = DIGITAL;
  digital_sensors.installed = 2;
  digital_sensors.sensors[0] = &DS0;
  digital_sensors.sensors[1] = &DS1;
  for (int i = 2; i < digital_sensors.total; i++) {
    digital_sensors.sensors[i] = &DS0;
  }
  analog_sensors.type = ANALOG;
  analog_sensors.installed = 1;
  for (int i = 0; i < analog_sensors.total; i++) {
    analog_sensors.sensors[i] = &AS0;
  }
}



void setup() {
  Serial.begin(BAUDUART0);
  Serial.println("Booting");
  // ota_startup();
  setupSensorMaps();
  Serial.println("Ready");
}

char *recByte = (char *)malloc(sizeof(char) * 2);
int addr = 0;

void loop() {
  // ArduinoOTA.handle();
  // if (Serial.available() > 0) {
  //   Serial.print(Serial.read());
  // }
  while (Serial.available() > 0) {
    Serial.readBytes(recByte, 2);
    switch (recByte[0]) {
      case NODE_STATUS:
        Serial.printf("[STATUS] NodeMCU status report code recieved\n");
        break;
      case READ_ANALOG:
        addr = recByte[1] - 0x30;
        if (addr >= analog_sensors.installed) {
          break;
        }
        Serial.printf("[ READ ] A0: %d\n", analogRead(analog_sensors.sensors[addr]->pin));
        break;
      case READ_DIGITAL:
        addr = recByte[1] - 0x30;
        if (addr >= digital_sensors.installed) {
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