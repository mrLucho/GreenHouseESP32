#include <Arduino.h>

#include <DHT.h> //temp and humidity sensors

#include <secretConsts.h> // secret api key 

#if defined(ESP32)
  #include <WiFi.h>
#else
  // 
#endif

#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"



#define DHT11PIN 27
#define FOTO_ANALOG_INPUT_PIN 26

//921600
// ugly global var
DHT dht;

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


void setup() {
  Serial.begin(74880);

  dht.setup(DHT11PIN);
  
}

void login(){
  unsigned long sendDataPrevMillis = 0;
  int count = 0;
  bool signupOK = false;
  WiFi.begin(WEB_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

/* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void loop() {
  int NIGHT_VALUE = 2900;
  

  // Serial.println(dht.getTemperature()); // despite wrong borders func returns reasonable feedback
  int analogReadVal = analogRead(FOTO_ANALOG_INPUT_PIN);
  Serial.println(analogReadVal);
  delay(1200);
}