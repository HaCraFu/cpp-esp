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
    delay(20);
  }
}