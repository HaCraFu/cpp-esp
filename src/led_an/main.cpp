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