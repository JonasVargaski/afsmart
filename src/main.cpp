
#include <PortExpander_I2C.h>
#include <SocketIoClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>

#include <utils.h>
#include <formPage.h>

PortExpander_I2C peOUT(0x27);
PortExpander_I2C peIN(0x26);
SocketIoClient socket;
AsyncWebServer server(80);

int interruptPCI_in = 5;

String mac = WiFi.macAddress();
String pwd = "123456";

IPAddress local_IP(192, 168, 0, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

void setState(const char *message, size_t length)
{
  DynamicJsonDocument json(1024);
  deserializeJson(json, message);

  peOUT.digitalWrite(3, !json["0"]); // RL1
  peOUT.digitalWrite(6, !json["1"]); // RL2
  peOUT.digitalWrite(5, !json["2"]); // RL3
  peOUT.digitalWrite(4, !json["3"]); // RL4
  peOUT.digitalWrite(0, !json["4"]); // RL5
  peOUT.digitalWrite(1, !json["5"]); // RL6
  peOUT.digitalWrite(2, !json["6"]); // RL7
  peOUT.digitalWrite(7, !json["7"]); // LED STATUS
}

void sendState(void)
{
  DynamicJsonDocument json(1024);

  json["mac"] = mac;

  JsonObject inputs = json.createNestedObject("inputs");

  inputs["1"] = !peIN.digitalRead(0);
  inputs["2"] = !peIN.digitalRead(1);
  inputs["3"] = !peIN.digitalRead(2);
  inputs["4"] = !peIN.digitalRead(3);
  inputs["5"] = !peIN.digitalRead(4);
  inputs["6"] = !peIN.digitalRead(5);
  inputs["7"] = !peIN.digitalRead(6);
  inputs["8"] = !peIN.digitalRead(7);

  JsonObject outputs = json.createNestedObject("outputs");

  outputs["1"] = !peOUT.digitalRead(3);
  outputs["2"] = !peOUT.digitalRead(6);
  outputs["3"] = !peOUT.digitalRead(5);
  outputs["4"] = !peOUT.digitalRead(4);
  outputs["5"] = !peOUT.digitalRead(0);
  outputs["6"] = !peOUT.digitalRead(1);
  outputs["7"] = !peOUT.digitalRead(2);
  outputs["8"] = !peOUT.digitalRead(7);

  char serializedJson[500];

  serializeJson(json, serializedJson);
  socket.emit("d>s.state", serializedJson);
}
void getState(const char *message, size_t length)
{
  sendState();
}

void setup(void)
{
  EEPROM.begin(200);
  Wire.begin(0, 2);
  peOUT.init();
  peIN.init();

  for (int i = 0; i < 8; i++)
  {
    peIN.pinMode(i, INPUT);
  }
  pinMode(interruptPCI_in, INPUT);

  Serial.begin(115200);
  Serial.println("\r\n\r\nBooting...");

  String ssid = EEPROMReadString(20, 40);
  String password = EEPROMReadString(40, 60);
  String serverAddress = EEPROMReadString(60, 80);
  String serverPort = EEPROMReadString(80, 86);

  //WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);

  // while (WiFi.waitForConnectResult() != WL_CONNECTED)
  // {
  //   Serial.println("Connection Failed! Rebooting...");
  //   delay(5000);
  //   ESP.restart();
  // }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
    EEPROMWriteString(request->getParam(0)->value(), 20, 40);
    EEPROMWriteString(request->getParam(1)->value(), 40, 60);
    EEPROMWriteString(request->getParam(2)->value(), 60, 80);
    EEPROMWriteString(request->getParam(3)->value(), 80, 86);
    request->send(200, "Saved successfully!");
  });

  server.begin();
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("ESPsoftAP @Technow");

  socket.on("s->d.sync", getState);
  socket.on("s->d.state", setState);
  socket.begin(serverAddress, atoi(serverPort));
}

void loop(void)
{
  socket.loop();

  if (!digitalRead(interruptPCI_in))
    sendState();
}
