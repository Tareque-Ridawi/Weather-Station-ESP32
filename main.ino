#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

// ====== WiFi Networks ======
const char* ssidList[] = {"Tareque", "Ataullah (LINK4)", "PU_GUESTS"};
const char* passList[] = {"Fatik173", "Arabi298052", "PuC123456@#"};
const int wifiCount = 3;

// ====== Server URLs ======
String serverName = "https://venjex-coding.atwebpages.com/";
String logURL = serverName + "log.php";
String controlURL = serverName + "get_controls.php";
String pingURL = serverName + "ping.php";

// ====== Components ======
Adafruit_BMP085 bmp;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo windowServo;

// Relay pins
const int relayFan = 26;        // Fan relay
const int relayBuzzer = 25;     // Buzzer relay

// LED pins
const int ledYellow = 33;
const int ledGreen = 32;

// Servo pin
const int servoPin = 27;

// ====== Variables ======
float temp_threshold = 32.5;
float press_threshold = 1005.0;
int fan_state = 0;
int window_state = 0;

bool fanOn = false;
bool windowOpen = false;

unsigned long lastLog = 0;
unsigned long lastPing = 0;
unsigned long lastFetch = 0;

// ====== Setup ======
void setup() {
  Serial.begin(115200);

  // I/O setup
  pinMode(relayFan, OUTPUT);
  pinMode(relayBuzzer, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  // Default safe states
  digitalWrite(relayFan, HIGH);      // Fan OFF
  digitalWrite(relayBuzzer, HIGH);   // Buzzer OFF
  digitalWrite(ledYellow, HIGH);     // ON before WiFi
  digitalWrite(ledGreen, LOW);       // OFF at startup

  delay(500);
  windowServo.attach(servoPin);
  windowServo.write(0); // Window closed

  // LCD setup
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Weather Station");

  // BMP180
  if (!bmp.begin()) {
    Serial.println("BMP180 not found!");
    while (1);
  }

  // Connect WiFi
  connectWiFi();

  // Once connected
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledGreen, LOW);   // ensure off after connect
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connected");
  delay(1500);
}

// ====== WiFi Connection Function ======
void connectWiFi() {
  for (int i = 0; i < wifiCount; i++) {
    Serial.print("Trying WiFi: ");
    Serial.println(ssidList[i]);

    WiFi.begin(ssidList[i], passList[i]);
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected!");
      return;
    }
    Serial.println("\nFailed, trying next...");
  }
  Serial.println("No WiFi network found, restarting...");
  delay(2000);
  ESP.restart();
}

// ====== Buzzer Beep ======
void beepBuzzer() {
  digitalWrite(relayBuzzer, LOW);  // ON
  delay(250);
  digitalWrite(relayBuzzer, HIGH); // OFF
}

// ====== Main Loop ======
void loop() {
  float t = bmp.readTemperature();
  float p = bmp.readPressure() / 100.0;
  float a = bmp.readAltitude();

  // Display sensor data
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);
  lcd.print(" P:");
  lcd.print(p);
  lcd.setCursor(0, 1);
  lcd.print("Alt:");
  lcd.print(a);

  // Yellow LED blink for 300 ms every time data sent
  if (millis() - lastLog > 5000) {
    digitalWrite(ledYellow, HIGH);
    sendLog(t, p, a);
    lastLog = millis();
    delay(300);
    digitalWrite(ledYellow, LOW);
  }

  // Ping server
  if (millis() - lastPing > 5000) {
    sendPing();
    lastPing = millis();
  }

  // Fetch controls
  if (millis() - lastFetch > 5000) {
    fetchControls();
    lastFetch = millis();
  }

  // Apply control logic
  applyControls(t, p);

  delay(1000);
}

// ====== Logging ======
void sendLog(float t, float p, float a) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(logURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "temp=" + String(t) + "&press=" + String(p) + "&alt=" + String(a);
    int httpCode = http.POST(postData);

    if (httpCode == 200) {
      Serial.println("Log OK: " + postData);
    } else {
      Serial.print("Log failed: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

// ====== Ping ======
void sendPing() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(pingURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("id=1");

    if (httpCode == 200) {
      Serial.println("Ping success");
    } else {
      Serial.print("Ping failed: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

// ====== Fetch Controls ======
void fetchControls() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(controlURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("id=1");

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println("Controls JSON: " + payload);

      DynamicJsonDocument doc(512);
      if (!deserializeJson(doc, payload)) {
        temp_threshold = doc["temp_threshold"];
        press_threshold = doc["press_threshold"];
        fan_state = doc["fan_state"];
        window_state = doc["window_state"];
      }
    } else {
      Serial.print("Fetch failed: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

// ====== Apply Controls ======
void applyControls(float t, float p) {
  bool autoFan = (t > temp_threshold);
  bool autoWindow = (t > temp_threshold);

  bool manualFan = (fan_state == 1);
  bool manualWindow = (window_state == 1);

  bool shouldFan = (manualFan || autoFan);
  bool shouldWindow = (manualWindow || autoWindow);

  // --- Beep when turning ON
  if ((shouldFan && !fanOn) || (shouldWindow && !windowOpen)) {
    beepBuzzer();
  }

  // --- Window
  if (shouldWindow && !windowOpen) {
    windowServo.write(90);
    windowOpen = true;
    delay(500);
  } else if (!shouldWindow && windowOpen) {
    windowServo.write(0);
    windowOpen = false;
  }

  // --- Fan
  if (shouldFan && !fanOn) {
    digitalWrite(relayFan, LOW);
    fanOn = true;
  } else if (!shouldFan && fanOn) {
    digitalWrite(relayFan, HIGH);
    fanOn = false;
  }

  // ✅ Green LED ON only when manually turned ON via web
  if (manualFan || manualWindow) {
    digitalWrite(ledGreen, HIGH);
  } else {
    digitalWrite(ledGreen, LOW);
  }
}
