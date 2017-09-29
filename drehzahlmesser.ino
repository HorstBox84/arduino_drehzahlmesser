// include library for LEDs and configure
#include <NeoPixelBus.h>

const uint8_t PixelPin = 10;
const uint16_t PixelCount = 10;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> LedStrip(PixelCount, PixelPin);

#define maxBrightness 25

RgbColor red(maxBrightness,0,0);
RgbColor yellow(maxBrightness,maxBrightness,0);
RgbColor green(0,maxBrightness,0);
RgbColor black(0);

RgbColor CurrentColor;

// motor specifics
uint16_t CurrentRpm = 0;
#define MaxRpm        10000
#define RedRpm        8001
#define YellowRpm     6001
#define RpmPerLed     (MaxRpm/PixelCount)
#define RpmPin        2
#define PulsesPerRot  2
uint8_t PulseCount = 0;

#define LedTime     100

RgbColor LedArray[PixelCount];

uint8_t i = 0;
unsigned long currentTime = 0;
unsigned long previousTime = 0;

unsigned long currentInterruptTime = 0;
unsigned long previousInterruptTime = 0;

void setup() {
  Serial.begin(57600);
  
  LedStrip.Begin();
  LedStrip.Show();

  InitLedColors();
  
  LedStrip.Show();

  pinMode(RpmPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(RpmPin), RpmInterrupt, RISING);
}

void loop() {
  currentTime = millis();
  
  if (currentTime - previousTime >= LedTime)
  {
    Serial.print("Current RPM: ");
    Serial.println(CurrentRpm);
    
    previousTime = millis();
    
    if (CurrentRpm <= MaxRpm)
    {
      RpmToLed();
    }
    else
    {
      FlashRed();
    }
  }
}

void RpmInterrupt()
{
  PulseCount ++;
  if (PulseCount >= PulsesPerRot)
  {
    currentInterruptTime = micros();
    CurrentRpm = 60000000 / (currentInterruptTime - previousInterruptTime);
    previousInterruptTime = currentInterruptTime;
    PulseCount = 0;
  }
}

void FlashRed()
{
  static uint8_t IsOn = 0;
  
  if (IsOn)
  {
    CurrentColor = black;
  }
  else
  {
    CurrentColor = red;
  }
  
  for (i = 0; i <= PixelCount; i ++)
  {
    LedStrip.SetPixelColor(i, CurrentColor);
  }
  LedStrip.Show();
   
  IsOn ^= 0x01;
}

void RpmToLed()
{
  uint16_t CurrentLedRpm = 0;
  
  for (i = 1; i <= PixelCount; i ++)
  {
    CurrentLedRpm = i * RpmPerLed;
    
    if (CurrentRpm >= CurrentLedRpm)
    {
      CurrentColor = LedArray[i];
    }
    else if (CurrentRpm < CurrentLedRpm)
    {
      if ((CurrentRpm + RpmPerLed) > CurrentLedRpm)
      {
        CurrentColor = LedArray[i];
        CurrentColor.Darken((CurrentLedRpm - CurrentRpm) * (1.0 * maxBrightness / RpmPerLed));
      }
      else
      {
        CurrentColor = black; 
      }
    }
    LedStrip.SetPixelColor(i - 1, CurrentColor);
    LedStrip.Show();
  }
}

void InitLedColors() {
  for (i = 0; i < PixelCount; i ++)
  {
    if (i * RpmPerLed >= RedRpm)
    {
      CurrentColor = red;
    }
    else if (i * RpmPerLed >= YellowRpm)
    {
      CurrentColor = yellow;
    }
    else 
    {
      CurrentColor = green;
    }

    LedArray[i] = CurrentColor;
  }
}

