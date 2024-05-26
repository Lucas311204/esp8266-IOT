#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>

// WiFi - Coloque aqui suas configurações de WI-FI
const char ssid[] = "Luciano";
const char psw[] = "912345678";


// Site remoto - Coloque aqui os dados do site que vai receber a requisição GET
const char http_site[] = "192.168.1.226";
const int http_port = 8081;
const char http_path[] = "/cadastrar";

// Variáveis globais

unsigned long previousMillis = 0;
const long interval = 5000;

IPAddress server(192, 168, 1, 226);  // Endereço IP do servidor - http_site
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
  } else {
    Serial.println("Não está cheio");
  }

  delay(1000); // Espera 1 segundo antes da próxima leitura
}


bool sendDataToServer(boolean cheio) {
  WiFiClientSecure client;
  client.setInsecure();

  String url = "https://" + String(http_site) + http_path + "/" + String(cheio);

  Serial.println("fazendo request");
  Serial.println(url);


  if (client.connect(http_site, http_port)) {
    Serial.println("conectado ao cliente");
    //client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + http_site + "\r\n" + "Connection: close\r\n\r\n");
    client.print(String("GET /cadastrar/") + (cheio) + " HTTP/1.1\r\n" + "Host: " + http_site + "\r\n" + "Connection: close\r\n\r\n");

    while (client.connected()) {
      Serial.println("esperando por resposta do servidor");
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    while (client.available()) {
      Serial.println("esperando resto da resposta");
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }

    Serial.println("concluído");

    client.stop();
    return true;
  } 
    
  Serial.println("Falha na conexão com o servidor");
  return false;
}



