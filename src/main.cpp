
#include <PortExpander_I2C.h>
#include <SocketIoClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

PortExpander_I2C peOUT(0x27);
PortExpander_I2C peIN(0x26);
SocketIoClient socket;

int interruptPCI_in = 5;

const char *ssid = "Vargaski";
const char *password = "12345678";

const char *server = "192.168.0.104";
const int serverPort = 3000;

String mac = WiFi.macAddress();
String pwd = "123456";

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

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  socket.begin(server, serverPort);
  socket.on("s->d.sync", getState);
  socket.on("s->d.state", setState);
}

void loop(void)
{
  socket.loop();

  if (!digitalRead(interruptPCI_in))
    sendState();
}
