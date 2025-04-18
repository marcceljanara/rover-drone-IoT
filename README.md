# **ESP8266 Sensor & Relay Control dengan MQTT**
Proyek ini menggunakan **ESP8266** untuk membaca sensor suhu, kelembaban, intensitas cahaya, serta mengontrol relay melalui **MQTT**. Konfigurasi WiFi dan MQTT disimpan di file `config.h` untuk keamanan lebih baik.

## **📌 Fitur**
- Menggunakan **DHT22** untuk membaca suhu & kelembaban.
- Menggunakan **BH1750** untuk membaca intensitas cahaya.
- Mengontrol **relay** melalui pesan **MQTT**.
- Menggunakan **NTP Client** untuk mendapatkan timestamp.
- Menyimpan konfigurasi di `config.h` untuk menjaga keamanan informasi sensitif.

---

## **🛠 Persiapan**
### **1. Instalasi Library yang Diperlukan**
Sebelum menjalankan proyek ini, pastikan Arduino IDE memiliki library berikut:
- **ESP8266WiFi** (bawaan ESP8266)
- **PubSubClient** (untuk MQTT)
- **ArduinoJson** (untuk parsing JSON)
- **DHT sensor library** (untuk membaca DHT22)
- **BH1750** (untuk sensor cahaya)
- **NTPClient** (untuk waktu)

Cara install:
1. **Buka Arduino IDE**
2. **Pergi ke Sketch → Include Library → Manage Libraries**
3. **Cari dan Install**:
   - "PubSubClient"
   - "ArduinoJson"
   - "DHT sensor library"
   - "BH1750"
   - "NTPClient"

---

### **2. Menyimpan Kredensial di `config.h`**
Karena proyek ini menggunakan **WiFi dan MQTT**, informasi sensitif disimpan di file **`config.h`** agar lebih aman.

#### **🔹 Buat File `config.h`**
1. **Buka Arduino IDE**.
2. **Buat file baru** dengan nama `config.h`.
3. **Tambahkan kode berikut di dalamnya:**

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// WiFi Credentials
const char *ssid = "NAMA_WIFI";
const char *password = "PASSWORD_WIFI";

// MQTT Credentials
const char *mqttServer = "BROKER_IP";
const int mqttPort = 1883;
const char *mqttUser = "MQTT_USER";
const char *mqttPassword = "MQTT_PASSWORD";

// MQTT Topics
const char *mqttTopicControl = "controltopic";
const char *mqttTopicSensor = "sensortopic";

#endif
