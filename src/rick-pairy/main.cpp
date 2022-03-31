#define USE_HELIX
#define USE_A2DP

#include "AudioCodecs/CodecMP3Helix.h"
#include "AudioTools.h"
#include "SPIFFS.h"
#include <SPI.h>

A2DPStream out = A2DPStream::instance(); // A2DP input - A2DPStream is a singleton!

StreamCopy copier;
EncodedAudioStream decoder(&out, new MP3DecoderHelix()); // Decoding stream

File *audioFile = nullptr;

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin(false))
  {
    Serial.printf("Error starting SPIFFS\n");
    while (true)
    {
    }
  }
  audioFile = new File{SPIFFS.open("/rick.mp3", "r")};
  if (!audioFile)
  {
    Serial.printf("Error opening file\n");
    while (true)
    {
    }
  }
  Serial.printf("File size: %d\n", audioFile->available());

  AudioLogger::instance().begin(Serial, AudioLogger::Warning);

  auto cfg = out.defaultConfig(TX_MODE);
  cfg.name = ""; // connect _any_ speaker - you may specify the name of your bluetooth speakers here
  // cfg.auto_reconnect = true;  // if this is use we just quickly connect to the last device ignoring cfg.name
  out.begin(cfg);

  decoder.setNotifyAudioChange(out);
  decoder.begin();

  // begin copy
  copier.begin(decoder, *audioFile);
}

void loop()
{
  if (!copier.copy())
  {
    audioFile->seek(0);
    Serial.println("reeewind");
    // stop();
  }
}