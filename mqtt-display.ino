/*
BC-Display

works with OLED 128x64 display, connected to I2C

Copy config.h.sample to config.h and make changes you need

Install these libraries:

- https://github.com/tzapu/WiFiManager
- Adafruit SSD1306 (may needs fix in .h file)
- Adafruit GFX
- PubSubClient

MIT license

(C) 2016 Martin Maly, http://retrocip.cz

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "config.h"

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //
#include <PubSubClient.h>



// Update these with values suitable for your network.
IPAddress server SERVER;


#define BUFFER_SIZE 100


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold24pt7b.h>

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

char msg[100];

String smsg;

uint8_t pos;

void callback(const MQTT::Publish& pub) {
  Serial.print(pub.topic());
  Serial.print(" => ");
  smsg = pub.payload_string();
  smsg.toCharArray(msg,100);
  pos = smsg.indexOf('[')+1;
  msg[pos+4] = 0;
    Serial.println(pub.payload_string());
    display.clearDisplay();
   display.setCursor(12,48);
   display.print(msg+pos);
   display.display();
}

WiFiClient wclient;
PubSubClient client(wclient, server);
WiFiManager wifiManager;

void setup() {
  // Setup console
  Serial.begin(115200);
  delay(10);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setFont(&FreeMonoBold24pt7b);
     display.setCursor(12,48);
   display.print("WAIT");
   display.display();

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiManager.autoConnect(WIFIMAN_AP);
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect(MQTT::Connect("mqttdisplay").set_auth(MQTT_AUTH_NAME, MQTT_AUTH_PASS))) {
	      client.set_callback(callback);
	      client.subscribe(TOPIC);
      }
    }

    if (client.connected())
      client.loop();
  }
}
