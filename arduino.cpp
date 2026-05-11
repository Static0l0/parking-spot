#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "Guest";
const char* password = "trainingroom";
String serverName = "https://arduino-parking.vercel.app/api/parking";

int trig = 13;
int echo = 18;
int trig2 = 14;
int echo2 = 21;
int r1 = 22;
int r2 = 32;

long back = 0;
int distance = 0;
long back2 = 0;
int distance2 = 0;

unsigned long previousMillis = 0;
const long interval = 1000;

String lastStatus1 = "";
String lastStatus2 = "";

void setup() {
  Serial.begin(115200);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected!");
}

void ping() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  back = pulseIn(echo, HIGH, 30000);
  distance = back / 58.0;
}

void ping2() {
  digitalWrite(trig2, LOW);
  delayMicroseconds(2);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2, LOW);
  back2 = pulseIn(echo2, HIGH, 30000);
  distance2 = back2 / 58.0;
}

void senditbro(int sensor, String status) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected! Skipping send.");
    return;
  }

  WiFiClientSecure client;  
  client.setInsecure();     

  HTTPClient http;
  String url = String(serverName) + "?sensor=" + sensor + "&status=" + status;

  Serial.println("Sending: " + url);

  http.begin(client, url);
  http.setTimeout(5000);

  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("HTTP " + String(httpCode) + " | Response: " + payload);
  } else {
    Serial.println("Error: " + http.errorToString(httpCode));
  }
  http.end();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost, reconnecting...");
    WiFi.begin(ssid, password);
    delay(3000);
    return;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    ping();
    ping2();

    String status1 = distance <= 5 ? "PARK" : "nothing";
    String status2 = distance2 <= 5 ? "PARK" : "nothing";
    digitalWrite(r1, distance <= 5 ? HIGH : LOW);
    digitalWrite(r2, distance2 <= 5 ? HIGH : LOW);

    if (status1 != lastStatus1) {
      lastStatus1 = status1;
      senditbro(1, status1);
    }
    if (status2 != lastStatus2) {
      lastStatus2 = status2;
      senditbro(2, status2);
    }

    Serial.print("Sensor 1: ");
    Serial.print(distance);
    Serial.print(" cm  | ");
    Serial.println(status1);

    Serial.print("Sensor 2: ");
    Serial.print(distance2);
    Serial.print(" cm  | ");
    Serial.println(status2);

    Serial.println("-----------------------------");
  }
}
