// #include "DHT.h"
// #include <SoftwareSerial.h>
// #include <SdsDustSensor.h>

// const int DHT_PIN = A3;
// const int SDS_RX_PIN = 10;
// const int SDS_TX_PIN = 11;
// const int WAKEUP_WORKING_TIME = 5000;

// DHT dht(DHT_PIN, DHT11);
// SoftwareSerial softwareSerial(SDS_RX_PIN, SDS_TX_PIN);
// SdsDustSensor sds(softwareSerial);
// String Data;

// #define BUZZER_PIN 2


// void initializeSensors() {
//   pinMode(BUZZER_PIN, OUTPUT);
//   dht.begin();
//   sds.begin();
// }

// void readAndSendData() {
//   float humidity = dht.readHumidity();
//   float temperature = dht.readTemperature();

//   if (isnan(humidity) || isnan(temperature)) {
//     Serial.println(F("Failed to read from DHT sensor!"));
//     return;
//   }

//   float hic = dht.computeHeatIndex(temperature, humidity, false);

//   sds.wakeup();                // Wake up SDS011
//   delay(WAKEUP_WORKING_TIME);  // Allow sensor to stabilize

//   PmResult pm = sds.queryPm();

//   // if (pm.isOk()) {
//   Data = "Humidity " + String(humidity) + "% " + "Temperature " + String(temperature) + "°C " + "HeatIndex " + String(hic) + "°C " + "PM2.5 " + String(pm.pm25) + "µg/m³ " + "PM10 " + String(pm.pm10) + "µg/m³";
//   Serial.println(Data.c_str());  // Print with newline



//   if (hic > 40) {
//     Serial.println("High health risk - Heatstroke may occur soon");
//     tone(BUZZER_PIN, 300);
//     delay(5000);
//     noTone(BUZZER_PIN);
//   } else if (hic >= 33 && hic <= 39) {
//     Serial.println("Prolonged exposure and activity may lead to heat-related illnesses");
//     tone(BUZZER_PIN, 300);
//     delay(5000);
//     noTone(BUZZER_PIN);
//   } else if (hic >= 27 && hic <= 32) {
//     Serial.println("Prolonged exposure and activity may lead to fatigue");
//     tone(BUZZER_PIN, 300);
//     delay(5000);
//     noTone(BUZZER_PIN);
//   }
//   // } else {
//   //   Serial.print("Could not read values from sensor, reason: ");
//   //   Serial.println(pm.statusToString());
//   // }

//   // WorkingStateResult state = sds.sleep();  // Put SDS011 to sleep

//   // if (state.isWorking()) {
//   //   Serial.println("Problem with sleeping the SDS011 sensor.");
//   // } else {
//   //   Serial.println("SDS011 sensor is sleeping");
//   // }
// }

// void setup() {
//   Serial.begin(115200);
//   initializeSensors();
// }

// void loop() {
//   readAndSendData();
// }







#include <ESP8266WiFi.h>
#include <time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Booklab";
const char* password = "ccsadmin";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClient client;
unsigned long lastTime = 0;
unsigned long timerDelay = 300000;
const int ledGreenPin = D4;
const int ledRedPin = D6;
String data;

void setup() {
  Serial.begin(115200);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  timeClient.setTimeOffset(25200);
}

void loop() {
  if (Serial.available() > 0) {
    digitalWrite(ledRedPin, LOW);
    data = Serial.readStringUntil('\n');
    Serial.println(data);

    timeClient.update();
    time_t epochTime = timeClient.getEpochTime();
    String formattedTime = timeClient.getFormattedTime();

    struct tm* ptm = gmtime((time_t*)&epochTime);
    int currentDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon + 1;
    int currentYear = ptm->tm_year + 1900;

    String currentDate = String(currentDay) + "-" + String(currentMonth) + "-" + String(currentYear) + " " + String(formattedTime);

    // Parse the received data
    float humidity, temperature, HeatIndex, pm25, pm10;
    if (sscanf(data.c_str(), "Humidity %f%% Temperature %f°C HeatIndex %f°C PM2.5 %fµg/m³ PM10 %fµg/m³", &humidity, &temperature, &HeatIndex, &pm25, &pm10) == 5) {


      StaticJsonDocument<200> jsonDocument;
      jsonDocument["Temp"] = temperature;
      jsonDocument["Humid"] = humidity;
      jsonDocument["HeatIndex"] = HeatIndex;
      jsonDocument["PM25"] = pm25;
      jsonDocument["PM10"] = pm10;
      jsonDocument["timestamp"] = currentDate;

      String jsonData;
      serializeJson(jsonDocument, jsonData);

      HTTPClient http;
      http.begin(client, "http://192.168.0.178:80/ESP8266");
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        String payload = http.getString();
        Serial.println("Returned payload:");
        Serial.println(payload);

        // Turn on green LED to indicate successful POST
        digitalWrite(ledGreenPin, HIGH);
        delay(1000);
        digitalWrite(ledGreenPin, LOW);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));

        // Turn on red LED to indicate error
        digitalWrite(ledRedPin, HIGH);
        delay(1000);
        digitalWrite(ledRedPin, LOW);
      }

      http.end();
    } else {
      Serial.println("Error parsing data.");
    }
  } else {
    Serial.println("Not Serial.available");
    digitalWrite(ledRedPin, HIGH);
  }
  delay(3000);
}
