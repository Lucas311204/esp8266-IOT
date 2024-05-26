#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>

// WiFi - Coloque aqui suas configurações de WI-FI
const char ssid[] = "Luciano";
const char psw[] = "912345678";


// Site remoto - Coloque aqui os dados do site que vai receber a requisição GET
const char http_site[] = "192.168.1.119";
const int http_port = 8081;
const char http_path[] = "/cadastrar";

// Variáveis globais

unsigned long previousMillis = 0;
const long interval = 5000;

IPAddress server(192, 168, 1, 119);  // Endereço IP do servidor - http_site
#define trigPin D7
#define echoPin D6

void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(trigPin, LOW);


  delay(5000);
  Serial.begin(9600);
  Serial.println("NodeMCU - Gravando dados no BD via GET");
  Serial.println("Aguardando conexão");

  // Tenta conexão com Wi-Fi
  WiFi.begin(ssid, psw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("\nWi-Fi conectado com sucesso: ");
  Serial.println(ssid);
}

long duration;
int distance;

void loop() {


  // Gera um pulso ultrassônico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Mede o tempo do pulso de eco
  duration = pulseIn(echoPin, HIGH, 10000); // Tempo limite de 10ms para esperar pelo eco
  distance = duration * 0.034 / 2; // Converte o tempo em distância (cm)
  
  // Imprime a distância e o estado no monitor serial

  if (distance <= 5 && distance >= 1) {
    Serial.println("Está cheio");
    sendDataToServer("1");
  } else {
    Serial.println("Não está cheio");
  }

  delay(1000); // Espera 1 segundo antes da próxima leitura
}


bool sendDataToServer( String cheio) {
  HTTPClient http;
  WiFiClient client;

  String url = "http://" + String(http_site) + ":" + String(http_port) + http_path + "/" + String(cheio);

  Serial.println("fazendo request");
  Serial.println(url);

  http.begin(client, url);

  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
  } else {
    Serial.println("Falha na requisição");
  }

  http.end();
  return httpCode > 0;
}