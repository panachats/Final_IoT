#include "DHT.h"
#include <SoftwareSerial.h>
#include <SdsDustSensor.h>


const int DHT_PIN = A3;
const int SDS_RX_PIN = 10;
const int SDS_TX_PIN = 11;
const int WAKEUP_WORKING_TIME = 5000;

DHT dht(DHT_PIN, DHT11);
SoftwareSerial softwareSerial(SDS_RX_PIN, SDS_TX_PIN);
SdsDustSensor sds(softwareSerial);
String Data;


void initializeSensors() {
  dht.begin();
  sds.begin();
}

void readAndSendData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  String Status;

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hic = dht.computeHeatIndex(temperature, humidity, false);

  sds.wakeup();                // Wake up SDS011
  delay(WAKEUP_WORKING_TIME);  // Allow sensor to stabilize

  PmResult pm = sds.queryPm();

  if (hic >= 40 || hic <= 18 || pm.pm25 >= 25) {
    Status = "Uncomfortable";
  } else if (hic >= 30 || pm.pm25 >= 12) {
    Status = "Warning";
  } else {
    Status = "Comfortable";
  }

  // if (pm.isOk()) {
  Data = "Humidity " + String(humidity) + "% " + "Temperature " + String(temperature) + "°C " + "HeatIndex " + String(hic) + "°C " + "PM2.5 " + String(pm.pm25) + "µg/m³ " + "PM10 " + String(pm.pm10) + "µg/m³ " + "Status " + Status;
  Serial.println(Data.c_str()); 

  // } else {
  //   Serial.print("Could not read values from sensor, reason: ");
  //   Serial.println(pm.statusToString());
  // }

  // WorkingStateResult state = sds.sleep();  // Put SDS011 to sleep

  // if (state.isWorking()) {
  //   Serial.println("Problem with sleeping the SDS011 sensor.");
  // } else {
  //   Serial.println("SDS011 sensor is sleeping");
  // }
}

void setup() {
  Serial.begin(115200);
  initializeSensors();
}

void loop() {
  readAndSendData();
}
