// ═══════════════════════════════════════════════════════════
//  ESP8266 (NodeMCU) — WiFi Bridge to Adafruit IO
//  Reads events from Arduino Uno via SoftwareSerial
//  Posts them to Adafruit IO feeds
// ═══════════════════════════════════════════════════════════
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// ── WiFi credentials — fill these in ────────────────────────
#define WIFI_SSID     "YOUR_WIFI_NAME"
#define WIFI_PASS     "YOUR_WIFI_PASSWORD"

// ── Adafruit IO credentials — from io.adafruit.com ──────────
// Sign up free at io.adafruit.com → go to My Key (yellow key icon)
#define AIO_SERVER    "io.adafruit.com"
#define AIO_PORT      1883
#define AIO_USERNAME  "YOUR_ADAFRUIT_USERNAME"
#define AIO_KEY       "YOUR_ADAFRUIT_IO_KEY"

// ── Feed names (create these on Adafruit IO dashboard) ───────
// Feed 1: "lock-status"   → shows LOCKED / UNLOCKED
// Feed 2: "lock-events"   → shows all event strings
#define FEED_STATUS   AIO_USERNAME "/feeds/lock-status"
#define FEED_EVENTS   AIO_USERNAME "/feeds/lock-events"

// ── MQTT client setup ────────────────────────────────────────
WiFiClient wifiClient;
Adafruit_MQTT_Client mqtt(&wifiClient, AIO_SERVER, AIO_PORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish feedStatus = Adafruit_MQTT_Publish(&mqtt, FEED_STATUS);
Adafruit_MQTT_Publish feedEvents = Adafruit_MQTT_Publish(&mqtt, FEED_EVENTS);

// ── Serial from Arduino Uno ──────────────────────────────────
// ESP8266 RX (GPIO3 / D9) ← Uno pin 13
// ESP8266 TX (GPIO1 / D10) → Uno pin 12
// NOTE: NodeMCU uses hardware Serial for this bridge
// Use D7 (GPIO13) as RX and D8 (GPIO15) as TX if you want SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial unoSerial(D7, D8); // RX=D7, TX=D8

// ════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);       // USB debug
  unoSerial.begin(9600);      // match Uno's espSerial baud rate

  connectWiFi();
  connectMQTT();
  Serial.println("ESP8266 Bridge Ready");
}

// ════════════════════════════════════════════════════════════
void loop() {
  // Keep MQTT connection alive
  if (!mqtt.connected()) connectMQTT();
  mqtt.processPackets(100);
  mqtt.ping();

  // Read event from Arduino Uno
  if (unoSerial.available()) {
    String event = unoSerial.readStringUntil('\n');
    event.trim();
    if (event.length() > 0) {
      Serial.print("Event received: ");
      Serial.println(event);
      postEvent(event);
    }
  }
}

// ── Post event to both Adafruit IO feeds ─────────────────────
void postEvent(String event) {
  // Post to events log feed (every event)
  feedEvents.publish(event.c_str());

  // Post to status feed (only lock state changes)
  if (event == "UNLOCKED") {
    feedStatus.publish("UNLOCKED");
  } else if (event == "LOCKED") {
    feedStatus.publish("LOCKED");
  } else if (event == "LOCKOUT") {
    feedStatus.publish("LOCKOUT");
  } else if (event == "BOOT") {
    feedStatus.publish("LOCKED");  // assume locked on startup
  }

  Serial.println("Posted to Adafruit IO");
}

// ── Connect to WiFi ──────────────────────────────────────────
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());
}

// ── Connect to Adafruit IO via MQTT ──────────────────────────
void connectMQTT() {
  Serial.print("Connecting to Adafruit IO");
  while (mqtt.connect() != 0) {
    Serial.print(".");
    mqtt.disconnect();
    delay(2000);
  }
  Serial.println("\nAdafruit IO connected!");
}
