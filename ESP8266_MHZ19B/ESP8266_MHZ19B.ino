#include <PubSubClient.h>
#include <Wire.h>
#include "EspMQTTClient.h" 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AHT20.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define pwmPin D3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
AHT20 aht20;

int prevVal = LOW;
long th, tl, h, l, ppm;
uint32_t timer_MQQT;

EspMQTTClient client(
  "Begemot12", // Wifi SSID
  "9045557432", // Wifi Password
  "77.232.138.28",  // MQTT Broker server ip
  "pasha",   // Can be omitted if not needed
  "pigin",  // Can be omitted if not needed
  "house", // device name
  1883 // MQTT server port
);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  // client.enableDebuggingMessages();
  pinMode(pwmPin, INPUT);

  // setupAHT20();
  setupDisplay();

}

void loop() {
  client.loop();

  long tt = millis();
  int myVal = digitalRead(pwmPin);

  if (myVal == HIGH) {
    if (myVal != prevVal) {
      h = tt;
      tl = h - l;
      prevVal = myVal;
    }
  }  else {
    if (myVal != prevVal) {
      l = tt;
      th = l - h;
      prevVal = myVal;
      ppm = 5000 * (th - 2) / (th + tl - 4);
      Serial.println("PPM = " + String(ppm));

      display.clearDisplay();

      display.setTextSize(5);
      display.setTextColor(WHITE);
      display.setCursor(0, 20);
      display.println(ppm);
      display.display(); 
    }
  }

  if (millis() - timer_MQQT >= 10000) {
    timer_MQQT = millis();

    // float humidity = aht20.getHumidity();
    // float temperature = aht20.getTemperature();
    
    client.publish("house/eco2", String(ppm));
    // client.publish("house/temp", String(temperature));
    // client.publish("house/hum", String(humidity));
  }
}

void onConnectionEstablished() {
  Serial.println("CONNECTED MQTT");
}

// void setupAHT20() {
//   if (aht20.begin() == false) {
//     Serial.println("AHT20 not detected. Please check wiring. Freezing.");
//     while (1);
//   }

//   Serial.println("AHT20 acknowledged.");
// }

void setupDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  delay(500);
  display.clearDisplay();
  display.setCursor(25, 15);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Aerotrack v2.0");
  display.setCursor(25, 35);
  display.setTextSize(1);
  display.print("Loading...");
  display.display();
}
