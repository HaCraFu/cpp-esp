# Einleitung

Der ESP32 lässt sich in der Programmiersprache C++ ähnlich wie ein Arduino programmieren. Die Sprache kann zwar etwas schwieriger als Python sein, ist dafür aber deutlich schneller in der Ausführung. Und sie wird kompiliert, wodurch sich vor der Ausführung die meisten Fehler vermeiden lassen.

## Platformio

In diesem Beispielprojekt verwenden wir zur Programmierung das Werkzeug [Platformio](https://platformio.org/). 
Dieses ermöglicht die Beschreibung des Mikrocontrollerprojektes durch die Datei `platformio.ini`. Hier werden festgelegt, welcher Mikrocontroller programmiert werden soll, welches Entwicklungsboard man hat, wie geflasht wird und welche Bibliotheken man verwendet.

Z.B.:
```ini
[env:azesp32]
platform = espressif32          ;ESP32 Mikrocontroller
board = az-delivery-devkit-v4   ;az-delivery-Board mit den entsprechenden Pins und Bezeichnungen
framework = arduino             ;Arduino-Framework verwenden (setup(), loop(), digitalWrite(), ...)
monitor_speed = 115200          ;Baudrate für serielle Konsole
lib_deps = adafruit/Adafruit NeoPixel@^1.10.4   ;Verwendung der Adafruit NeoPixel Bibliothek
```

Das Projekt lässt sich nun durch den Aufruf
```bash
pio run -e azesp32 -t upload -t monitor
```
kompilieren, aus den Mikrocontroller flashen und danach per seriellem Monitor die Ausgaben ansehen.

# Beispielprogramme

## Hello world
Dieses Programm gibt jede Sekunde die Anzahl Sekunden seit dem Start auf der seriellen Konsole aus.
Flashen mit 
```
pio run -e hello_world -t upload -t monitor
```

Code:
```cpp
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.print("Hello world ");
    Serial.println(millis()/1000.);
    delay(1000);
}
```

## Anschalten einer LED

Flashen mit 
```
pio run -e led_an -t upload
```
Code:
```cpp
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel np{10, 4};

void setup()
{
  np.begin();
  np.setPixelColor(1, 20, 30, 0);
  np.show();
}

void loop()
{
}
```

## LED Lauflicht

Flashen mit 
```
pio run -e led_lauflicht -t upload
```
Code:
```cpp
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel np{10, 4};

int iLed = 0;

void setup()
{
  np.begin();
}

void loop()
{
  for (int iLed = 0; iLed < np.numPixels(); ++iLed)
  {
    np.clear();
    np.setPixelColor(iLed, 20, 30, 0);
    np.show();
    delay(50);
  }
}
```

## LED API

Dieses Programm verbindet sich mit dem WLAN und lässt den LED-Streifen per HTTP-API steuern.
Beispielaufruf Abruf des LED Zustands:
```bash
> curl http://IP-Adresse/leds
[{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0},{"r":0,"g":0,"b":0}]
```

Beispielaufruf Anschalten der ersten beiden LEDs:
```bash
> curl -X POST http://IP-Adresse/leds -H 'Content-Type: application/json' -d '[{"r":0,"g":50,"b":10},{"r":30,"g":30,"b":10}]'
```
Flashen mit 
```
pio run -e led_api -t upload -t monitor
```
Code:
```cpp
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

AsyncWebServer server{80};

Adafruit_NeoPixel np{10, 4};

const char *ssid     = "MY_SSID";
const char *password = "MY_PASSWORD";

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send a GET request to <IP>/get?message=<message>
  server.on("/leds", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse(true);
    response->addHeader("Server", "ESP Async Web Server");
    auto root = response->getRoot().as<JsonArray>();
    for (int iLed = 0; iLed < np.numPixels(); ++iLed)
    {
      auto color      = np.getPixelColor(iLed);
      root[iLed]["r"] = (color >> 16) & 0xFF;
      root[iLed]["g"] = (color >> 8) & 0xFF;
      root[iLed]["b"] = (color >> 0) & 0xFF;
    }
    response->setLength();
    request->send(response);
  });

  auto postLedsHandler =
      new AsyncCallbackJsonWebHandler("/leds", [](AsyncWebServerRequest *request, JsonVariant &json) {
        auto root = json.as<JsonArray>();
        for (int iLed = 0; iLed < np.numPixels(); ++iLed)
        {
          if (iLed < root.size())
          {
            auto led = root[iLed];
            np.setPixelColor(iLed, led["r"], led["g"], led["b"]);
          }
        }
        np.show();
        request->send(200);
      });
  postLedsHandler->setMethod(HTTP_POST);
  server.addHandler(postLedsHandler);

  server.begin();
  np.begin();
}

void loop()
{
}
```

## Hackertyper

Flashen mit 
```
pio run -e hackertyper -t upload -t monitor
```

## Rick Pairy

Download von rick.mp3 nach `data/rick.mp3`

Flashen mit 
```
pio run -e rick-pairy -t uploadfs
pio run -e rick-pairy -t upload -t monitor
```
