#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"

// Konfigurasi pin sensor dan relay
#define DHTPIN 2        // Pin data DHT22 (GPIO2 / D4 pada ESP8266)
#define DHTTYPE DHT22   // Jenis sensor DHT
const int relayPin = 14; // Pin untuk relay (GPIO14 / D5)

// Inisialisasi sensor
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

// NTP Declaration
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // +7 jam dari UTC

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastPublishTime = 0;

// Fungsi untuk menangani pesan MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Pesan diterima di topik: ");
    Serial.println(topic);

    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.print("Payload: ");
    Serial.println(message);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
        return;
    }

    const char* command = doc["command"];
    const char* action = doc["action"];

    if (strcmp(command, "power") == 0) {
        if (strcmp(action, "on") == 0) {
            digitalWrite(relayPin, HIGH);
            Serial.println("Relay ON");
        } else if (strcmp(action, "off") == 0) {
            digitalWrite(relayPin, LOW);
            Serial.println("Relay OFF");
        }
    }
}

// Fungsi untuk menyambungkan kembali ke MQTT jika terputus
void reconnect() {
    while (!client.connected()) {
        Serial.print("Menghubungkan ke MQTT...");
        if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
            Serial.println("Terhubung!");
            client.subscribe(mqttTopicControl);
            Serial.println("Berlangganan ke topik: " + String(mqttTopicControl));
        } else {
            Serial.print("Gagal, kode: ");
            Serial.print(client.state());
            Serial.println(" Coba lagi dalam 5 detik...");
            delay(5000);
        }
    }
}

// Fungsi untuk mendapatkan timestamp
String getFormattedTimestamp() {
    time_t rawTime = timeClient.getEpochTime();
    struct tm *timeinfo = localtime(&rawTime);

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
             timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
    return String(buffer);
}

// Fungsi untuk mengirim data sensor ke MQTT
void publishSensorData() {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    float lux = lightMeter.readLightLevel();
    int timeActive = 20;

    if (isnan(humidity) || isnan(temperature)) {
        Serial.println(F("Gagal membaca sensor DHT22!"));
        return;
    }

    // Ambil timestamp
    String timestamp = getFormattedTimestamp();

    // Membuat JSON payload
    StaticJsonDocument<256> doc;
    doc["timestamp"] = timestamp;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["light_intensity"] = lux;
    doc["time_active"] = timeActive;

    char payload[256];
    serializeJson(doc, payload);

    Serial.print("Mengirim data sensor: ");
    Serial.println(payload);

    if (client.publish(mqttTopicSensor, payload)) {
        Serial.println("Data sensor berhasil dikirim.");
    } else {
        Serial.println("Gagal mengirim data sensor.");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(relayPin, OUTPUT);

    // Inisialisasi sensor
    dht.begin();
    Wire.begin();
    lightMeter.begin();

    // Koneksi ke WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");

    // Koneksi ke MQTT
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    reconnect();

    // Inisialisasi NTP Client
    timeClient.begin();
    timeClient.update();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    timeClient.update();  // Update waktu dari NTP server

    // Kirim data sensor setiap 20 detik
    if (millis() - lastPublishTime > 20000) {
        publishSensorData();
        lastPublishTime = millis();
    }
}
