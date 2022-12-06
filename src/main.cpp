// private includes

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

#define MOISTURE_ANALOG_INPUT_PIN 34
#define DIGITAL_MOISTURE_SIGNAL 35
#define DHT11PIN 27
#define FOTO_ANALOG_INPUT_PIN 36
#define WATER_LEVEL_ANALOG_INPUT_PIN 39

struct SensorReadings
{
  float temperature;
  int humidity;
  uint16_t sunShine;
  uint16_t moisture;
  uint16_t waterLevel;
};

// ugly global var
DHT dht;

FirebaseData FBDataObj;
FirebaseAuth FBauth;
FirebaseConfig FBconfig;
FirebaseJson json;
String DBpath;

const char* temperaturePath = "/temperature";
const char* humidityPath = "/humidity";
const char* sunPath = "/sunshine";
const char* moisturePath = "/moisture";
const char* waterLevel = "/waterLevel";
const char* timePath = "/epochTime";



// here change
uint8_t debugFlag = 1;


long int duration = 300000;



unsigned long previous_time = 0;


// fun prototypes----
SensorReadings getSensorReadings();
void login();
void connectWifi();
void sendJsonToDB(SensorReadings sensors);
unsigned long Get_Epoch_Time();
// ------


void setup() {
  Serial.begin(115200);
  dht.setup(DHT11PIN);
  connectWifi();
  login();

  
}

void loop(){
  // if debug shorten time to wait
  if (debugFlag == 0) duration = 300000;
  else duration = 300000/10;

  if (Firebase.ready() && (millis() - previous_time > duration || previous_time == 0)){ //300000 is 5 min delay
    previous_time = millis();
    SensorReadings s = getSensorReadings();
    sendJsonToDB(s);
  } 
}

// rest of fun
void sendJsonToDB(SensorReadings sensors){
  unsigned long epoch_time = Get_Epoch_Time();
  Serial.print ("time: ");
  Serial.println (epoch_time);

  String parent_path = DBpath + "/" + String(epoch_time);

  json.set(temperaturePath, String(sensors.temperature));
  json.set(humidityPath, String(sensors.humidity));
  json.set(sunPath, String(sensors.sunShine));
  json.set(moisturePath, String(sensors.moisture));
  json.set(waterLevel, String(sensors.waterLevel));

  json.set(timePath, String(epoch_time));
  Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&FBDataObj, parent_path.c_str(), &json) ? "ok" : FBDataObj.errorReason().c_str());
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

SensorReadings getSensorReadings(){
  // if (dht.getStatusString() == "OK"){ dht.getMinimumSamplingPeriod() might be useful
  float temp = dht.getTemperature();
  int humidity = dht.getHumidity();
  uint16_t sun = analogRead(FOTO_ANALOG_INPUT_PIN);
  uint16_t water = analogRead(WATER_LEVEL_ANALOG_INPUT_PIN);
  uint16_t moist = analogRead(MOISTURE_ANALOG_INPUT_PIN);

  SensorReadings s = {temp, humidity, sun,moist ,water};

  return s;
}

void login(){
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