#include <Arduino.h>
#include <DHT.h> //temp and humidity sensors


#if defined(ESP32)
  #include <WiFi.h>
#else
#endif

// firebase
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h" //Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
#include <secretConsts.h> // secret api key 


#include <time.h>

#define DHT11PIN 27
#define FOTO_ANALOG_INPUT_PIN 26



// ugly global var
DHT dht;


FirebaseData FBDataObj;
FirebaseAuth FBauth;
FirebaseConfig FBconfig;
FirebaseJson json;
String DBpath;

unsigned long previous_time = 0;
unsigned long Delay = 300000;
String temperaturePath = "/temperature";
String humidityPath = "/humidity";
String timePath = "/epochTime";
void login(float temperature, int sunShine);
void connectWifi(){
  WiFi.begin(WEB_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

unsigned long Get_Epoch_Time() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return(0);
  }
  time(&now);
  return now;
}

void setup() {
  Serial.begin(115200);
  dht.setup(DHT11PIN);
  connectWifi();
  login(1.0,2);
  
}
void login(float temperature, int sunShine){
  const char* ntpServer = "pool.ntp.org";
  String UID;

  configTime(0, 0, ntpServer);
  FBconfig.api_key = API_KEY;
  FBauth.user.email = USER_EMAIL;
  FBauth.user.password = USER_PASSWORD;
  FBconfig.database_url = DATABASE_URL;
  FBconfig.token_status_callback = tokenStatusCallback;
  FBconfig.max_token_generation_retry = 5;
  Firebase.begin(&FBconfig, &FBauth);
  Serial.println("Getting User UID...");
  while ((FBauth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  UID = FBauth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(UID);

  DBpath = "/Data/" + UID + "/Sensors";
}


void loop(){
  if (Firebase.ready() && (millis() - previous_time > Delay || previous_time == 0)){
    previous_time = millis();

    unsigned long epoch_time = Get_Epoch_Time();
    Serial.print ("time: ");
    Serial.println (epoch_time);

    String parent_path = DBpath + "/" + String(epoch_time);

    json.set(temperaturePath.c_str(), String(3));
    json.set(humidityPath.c_str(), String(5));
    json.set(timePath, String(epoch_time));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&FBDataObj, parent_path.c_str(), &json) ? "ok" : FBDataObj.errorReason().c_str());
  }
}