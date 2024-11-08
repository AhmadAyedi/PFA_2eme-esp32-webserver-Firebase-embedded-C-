#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>

/* WiFi Credentials */
#define WIFI_SSID "TOPNET4F57F07B"
#define WIFI_PASSWORD "CF0D1A748E"

/* Firebase Configuration */
#define API_KEY "AIzaSyA0_votMsZ3r0KhS4_1L5FrFzEpTaNqdqU"
#define DATABASE_URL "https://dht11-new1-default-rtdb.firebaseio.com/"

/* Firebase Auth */
#define USER_EMAIL "ahmadayedi@gmail.com"
#define USER_PASSWORD "123456"

/* DHT11 Setup */
#define DHT_PIN 4 // Connect DHT11 data pin to GPIO4 on Wemos Lolin32
#define DHT_TYPE DHT11

/* Firebase Data Object */
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

/* DHT11 Sensor Initialization */
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long lastDataSend = 0; // Track the last time data was sent
const unsigned long dataInterval = 5000; // 5 seconds interval for sending data

void setup() {
  Serial.begin(115200);

  // WiFi Connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected to Wi-Fi");

  // Firebase Initialization
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);

  dht.begin(); // Start the DHT11 sensor
}

void loop() {
  if (Firebase.ready() && (millis() - lastDataSend > dataInterval || lastDataSend == 0)) {
    lastDataSend = millis();

    // Read temperature and humidity from DHT11
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      // Send temperature and humidity data to Firebase
      if (Firebase.RTDB.setFloat(&fbdo, "/sensor_data/temperature", temperature)) {
        Serial.println("Temperature data sent to Firebase.");
      } else {
        Serial.println("Failed to send temperature data to Firebase: " + String(fbdo.errorReason()));
      }

      if (Firebase.RTDB.setFloat(&fbdo, "/sensor_data/humidity", humidity)) {
        Serial.println("Humidity data sent to Firebase.");
      } else {
        Serial.println("Failed to send humidity data to Firebase: " + String(fbdo.errorReason()));
      }
    }
  }
}
