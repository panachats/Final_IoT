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

// // #define BUZZER_PIN 2


// void initializeSensors() {
//   // pinMode(BUZZER_PIN, OUTPUT);
//   dht.begin();
//   sds.begin();
// }

// void readAndSendData() {
//   float humidity = dht.readHumidity();
//   float temperature = dht.readTemperature();
//   String Status;

//   if (isnan(humidity) || isnan(temperature)) {
//     Serial.println(F("Failed to read from DHT sensor!"));
//     return;
//   }

//   float hic = dht.computeHeatIndex(temperature, humidity, false);

//   sds.wakeup();                // Wake up SDS011
//   delay(WAKEUP_WORKING_TIME);  // Allow sensor to stabilize

//   PmResult pm = sds.queryPm();

//   if (hic >= 40 || hic <= 18 || pm.pm25 >= 25) {
//     Status = "Uncomfortable";
//     // Serial.println(Status);
//     // tone(BUZZER_PIN, 500);
//     // delay(5000);
//     // noTone(BUZZER_PIN);
//   } else if (hic >= 28 || pm.pm25 >= 12) {
//     Status = "Warning";
//     // Serial.println(Status);
//     // tone(BUZZER_PIN, 1000);
//     // delay(5000);
//     // noTone(BUZZER_PIN);
//   } else {
//     Status = "Comfortable";
//     // Serial.println(Status);
//   }

//   // if (pm.isOk()) {
//   Data = "Humidity " + String(humidity) + "% " + "Temperature " + String(temperature) + "°C " + "HeatIndex " + String(hic) + "°C " + "PM2.5 " + String(pm.pm25) + "µg/m³ " + "PM10 " + String(pm.pm10) + "µg/m³ " + "Status " + Status;
//   Serial.println(Data.c_str());  // Print with newline




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
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "_4most";
const char* password = "55556666";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastTime = 0;
unsigned long timerDelay = 300000;
const int ledGreenPin = D4;
const int ledRedPin = D6;
String data;
const int buzzer = D3;


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("Buzzer");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}





void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '2') {
    tone(buzzer, 500);
    delay(3000);
    Serial.println("Buzzer Status2 turned on");
  } else if ((char)payload[0] == '3') {
    tone(buzzer, 1000);
    delay(3000);
    Serial.println("Buzzer Status3 turned on");
  }
  else {
    noTone(buzzer);
    delay(3000);
    Serial.println("Buzzer turned off");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(buzzer, OUTPUT);

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
  client.setServer("10.1.1.113", 1883);
  client.setCallback(callback);
}







void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
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
    String Status;
    if (sscanf(data.c_str(), "Humidity %f%% Temperature %f°C HeatIndex %f°C PM2.5 %fµg/m³ PM10 %fµg/m³ Status %s", &humidity, &temperature, &HeatIndex, &pm25, &pm10, Status.c_str()) == 6) {



      StaticJsonDocument<200> jsonDocument;
      jsonDocument["Temp"] = temperature;
      jsonDocument["Humid"] = humidity;
      jsonDocument["HeatIndex"] = HeatIndex;
      jsonDocument["PM25"] = pm25;
      jsonDocument["PM10"] = pm10;
      jsonDocument["Status"] = Status.c_str();
      jsonDocument["timestamp"] = currentDate;

      String jsonData;
      serializeJson(jsonDocument, jsonData);



      HTTPClient http;
      http.begin(espClient, "http://10.1.1.113:80/ESP8266");
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
  // delay(3000);
}
