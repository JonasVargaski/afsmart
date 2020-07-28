
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <PortExpander_I2C.h>
#include <ArduinoOTA.h>
#include <SocketIoClient.h>

PortExpander_I2C pciOUT(0x27);
PortExpander_I2C pciIN(0x26);
SocketIoClient socket;

int interruptPCI_in = 5;

const char *ssid = "Vargaski";        //  SSID da sua rede
const char *password = "12345678";    // senha da sua rede
const char *server = "192.168.0.104"; // endereço servidor

IPAddress ip(192, 168, 0, 115);    // ip atribuido ao módulo
IPAddress gateway(192, 168, 0, 1); // base ip da sua rede
IPAddress subnet(255, 255, 255, 0);

void event(const char *payload, size_t length)
{
  Serial.printf("got message: %s\n", payload);
}

void socket_Connected(const char *payload, size_t length)
{
  Serial.println("Socket.IO connected!");
}

void setup(void)
{
  Wire.begin(0, 2);
  pciOUT.init();
  pciIN.init();

  for (int i = 0; i < 8; i++)
  {
    pciIN.pinMode(i, INPUT);
  }
  pinMode(interruptPCI_in, INPUT);

  Serial.begin(115200);
  Serial.println("Booting");

  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  socket.on("ping", event);
  socket.on("connect", socket_Connected);
  // socket.setReconnectInterval(5000);
  // socket.enableHeartbeat(15000, 3000, 2);
  socket.begin(server, 3000, "/socket.io/?EIO=3&transport=websocket");
  // ArduinoOTA.begin();
}

void loop(void)
{
  //  ArduinoOTA.handle();
  socket.loop();

  if (!digitalRead(interruptPCI_in))
  {
    pciOUT.digitalWrite(3, pciIN.digitalRead(0)); // Ligado ao CN1_4 pciOUT Acessoria Controla RL1
    pciOUT.digitalWrite(6, pciIN.digitalRead(1)); // Ligado ao CN1_3 pciOUT Acessoria Controla RL2
    pciOUT.digitalWrite(5, pciIN.digitalRead(2)); // Ligado ao CN1_2 pciOUT Acessoria Controla RL3
    pciOUT.digitalWrite(4, pciIN.digitalRead(3)); // Ligado ao CN1_1 pciOUT Acessoria Controla RL4
    pciOUT.digitalWrite(0, pciIN.digitalRead(4)); // Ligado ao CN2_4 pciOUT Acessoria Controla RL5
    pciOUT.digitalWrite(1, pciIN.digitalRead(5)); // Ligado ao CN2_3 pciOUT Acessoria Controla RL6
    pciOUT.digitalWrite(2, pciIN.digitalRead(6)); // Ligado ao CN2_2 pciOUT Acessoria Controla RL7
    pciOUT.digitalWrite(7, pciIN.digitalRead(7)); // Ligado ao CN2_1 pciOUT Acessoria Controla LED STATUS
  }
}
