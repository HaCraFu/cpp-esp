/**
 * @file player-sd-a2dp.ino
 * @brief see
 * https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-player/player-sd-a2dp/README.md
 *
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

// set this in AudioConfig.h or here after installing https://github.com/pschatzmann/arduino-libhelix.git
#define USE_HELIX
#define USE_A2DP
//#define USE_SDFAT

#include "AudioCodecs/CodecMP3Helix.h"
#include "AudioTools.h"
#include <SPI.h>
//#include "AudioCopy.h
#include "SPIFFS.h"

// const char *startFilePath = "/";
// const char *ext           = "mp3";
// AudioSourceSdFat source(startFilePath, ext);
A2DPStream out = A2DPStream::instance(); // A2DP input - A2DPStream is a singleton!
// MP3DecoderHelix decoder;
//  AudioPlayer player(source, out, decoder);

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

  // setup output - We send the test signal via A2DP - so we conect to the "LEXON MINO L" Bluetooth Speaker
  auto cfg = out.defaultConfig(TX_MODE);
  cfg.name = "";
  // cfg.auto_reconnect = true;  // if this is use we just quickly connect to the last device ignoring cfg.name
  out.begin(cfg);

  // setup player
  // player.setVolume(0.1);
  // player.begin();

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