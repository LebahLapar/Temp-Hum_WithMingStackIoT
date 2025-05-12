#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>


const char* ssid = "Imalda";
const char* password = "04271908";


const char* mqtt_server = "192.168.100.24";

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
const long interval = 5000;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi terkoneksi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("terhubung");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Gagal baca sensor DHT!");
      return;
    }

    Serial.print("Suhu: ");
    Serial.print(t);
    Serial.print(" °C, Kelembapan: ");
    Serial.print(h);
    Serial.println(" %");

    
    String payload = "{";
    payload += "\"temperature\":";
    payload += String(t, 2);
    payload += ",\"humidity\":";
    payload += String(h, 2);
    payload += "}";

    
    client.publish("iot/ruang1/data", payload.c_str());
  }
}
