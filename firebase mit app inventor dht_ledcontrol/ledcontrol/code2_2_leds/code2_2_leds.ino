#include <WiFi.h>
#include <Firebase_ESP_Client.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "TOPNET4F57F07B"
#define WIFI_PASSWORD "CF0D1A748E"

/* 2. Define the API Key */
#define API_KEY "AIzaSyCZR1HehRNWtXhijDuopPe-9ZlnuDGl5WM"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://ledcontrolyes-default-rtdb.firebaseio.com/" 

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "ahmad.ayadi2020@gmail.com"
#define USER_PASSWORD "123456"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

const int bleuLED = 2;
const int RedLED = 4;

void setup()
{
  pinMode(bleuLED, OUTPUT);
  digitalWrite(bleuLED, LOW);
  pinMode(RedLED, OUTPUT);
  digitalWrite(RedLED, LOW);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

   /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h


  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
}

void loop()
{
  // Firebase.ready() should be called repeatedly to handle authentication tasks.
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

   if(Firebase.RTDB.getString(&fbdo, "/MITLedControl/blue/state")){
    digitalWrite(bleuLED, atoi(fbdo.to<const char *>()));
   }else{
    Serial.println(fbdo.errorReason().c_str());
   }

   if(Firebase.RTDB.getString(&fbdo, "/MITLedControl/red/state")){
    digitalWrite(RedLED, atoi(fbdo.to<const char *>()));
   }else{
    Serial.println(fbdo.errorReason().c_str());
   }
  }
}
