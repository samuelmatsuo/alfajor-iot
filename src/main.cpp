#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* qrcodeScript = "https://cdnjs.cloudflare.com/ajax/libs/jquery.qrcode/1.0/jquery.qrcode.min.js";
const char* ssid = "Samuel";
const char* password = "arthur123";
ESP8266WebServer server(80);
const int output5 = 5; // D1
IPAddress local_IP(192, 168, 1, 115); // Troque o IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

bool qrcodeRead = false;

void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles.css\">";
  html += "<script type=\"text/javascript\" src=\"" + String(qrcodeScript) + "\"></script>";
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto;text-align: center;}";
  html += ".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;";
  html += "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
  html += ".button2 {background-color: #77878A;}</style></head>";
  html += "<body><h1>ESP8266 Web Server</h1>";
  html += "<h2>Alfajor Iot</h2>";

  if (qrcodeRead) {
    html += "<button onclick=\"toggleLED('on')\" class=\"button\">O</button>";
    html += "<button onclick=\"toggleLED('off')\" class=\"button button2\">OFF</button>";
  } else {
    html += "<p>Por favor, faça a leitura do QRCode para ativar as tomadas.</p>";
  }

  html += "<div id=\"qrcode\"></div>";
  html += "<script>";
  html += "function toggleLED(state) {";
  html += " if (" + String(qrcodeRead) + ") {";
  html += "   var xhr = new XMLHttpRequest();";
  html += "   xhr.open('GET', '/5/' + state, true);";
  html += "   xhr.send();";
  html += " } else {";
  html += "   alert('Faça a leitura do QRCode primeiro.');";
  html += " }";
  html += "}";
  html += "jQuery('#qrcode').qrcode({text: window.location.href, width: 200, height: 200});"; // Gera o QRCode com a URL atual
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleOn() {
  if (qrcodeRead) {
    digitalWrite(output5, LOW);
    server.send(200, "text/plain", "LED Ligado!");
  } else {
    server.send(200, "text/plain", "Faça a leitura do QRCode primeiro.");
  }
}

void handleOff() {
  if (qrcodeRead) {
    digitalWrite(output5, HIGH);
    server.send(200, "text/plain", "LED Desligado!");
  } else {
    server.send(200, "text/plain", "Faça a leitura do QRCode primeiro.");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(output5, OUTPUT);
  digitalWrite(output5, HIGH);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/5/on", HTTP_GET, handleOn);
  server.on("/5/off", HTTP_GET, handleOff);
  server.begin();
}

void loop() {
  server.handleClient();
}