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