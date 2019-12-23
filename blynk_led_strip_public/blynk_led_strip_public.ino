#include <ESP8266_Lib.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "<din blynk app auth>";

char ssid[] = "<ssid>";
char pass[] = "<passord>";

////////////////////////////////////
// Blynk Virtual Variable Mapping //
//////////////////////////////////// //
#define BLYNK_PRINT Serial
#define DEFAULT_MAX_BRIGHTNESS 32

//////////////////////////
// Hardware Definitions //
//////////////////////////
#define NUM_LEDS 60
#define DATA_PIN D6
Adafruit_NeoPixel rgb = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
void setLED(uint8_t red, uint8_t green, uint8_t blue);

//////////////////////////
// LIGHT VALUES
//////////////////////////
byte blynkRed = 0;
byte blynkGreen = 123;
byte blynkBlue = 0;
unsigned int rgbMaxBrightness = DEFAULT_MAX_BRIGHTNESS;
byte blynkRedSaved = 255;
byte blynkGreenSaved = 222;
byte blynkBlueSaved = 173;
unsigned int rgbMaxBrightnessSaved = DEFAULT_MAX_BRIGHTNESS;


//////////////////////////
// LOGICAL VARS
//////////////////////////
int TRANSITION_ENUM = 0; // 0 imidiate, 1 snake, 2 middle snake

void setup()
{
  // Debug console
  Serial.begin(9600);

  Serial.print("Initializing ...");
  rgb.begin(); // Set up WS2812
  Blynk.begin(auth, ssid, pass);
  setRGBValues(0, 123, 0);
  writeSavedValues();
}

void loop()
{
  Blynk.run();
}

void setRGBValues(uint8_t red, uint8_t green, uint8_t blue) {
  Serial.print("Updating RGB values: (");

  Serial.print("R: ");
  Serial.print(blynkRed);
  Serial.print(", ");

  Serial.print("G: ");
  Serial.print(blynkGreen);
  Serial.print(", ");

  Serial.print("B: ");
  Serial.print(blynkBlue);

  Serial.println(")");

  blynkRed = red;
  blynkGreen = green;
  blynkBlue = blue;

  updateRGB();
}

void updateRGB(void) {
  writeBlynkValues();
  updateLedStrip();
}

void writeBlynkValues() {
  Blynk.virtualWrite(V1, TRANSITION_ENUM);
  Blynk.virtualWrite(V5, blynkRed);
  Blynk.virtualWrite(V6, blynkGreen);
  Blynk.virtualWrite(V7, blynkBlue);
  Blynk.virtualWrite(V8, rgbMaxBrightness);

}

void saveRGBValues(void) {
  blynkRedSaved = blynkRed;
  blynkGreenSaved = blynkGreen;
  blynkBlueSaved = blynkBlue;
  rgbMaxBrightnessSaved = rgbMaxBrightness;

  Serial.print("Saving RGB values: (");

  Serial.print("R: ");
  Serial.print(blynkRed);
  Serial.print(", ");

  Serial.print("G: ");
  Serial.print(blynkGreen);
  Serial.print(", ");

  Serial.print("B: ");
  Serial.print(blynkBlue);

  Serial.print("). Saved values are: (");

  Serial.print("R: ");
  Serial.print(blynkRedSaved);
  Serial.print(", ");

  Serial.print("G: ");
  Serial.print(blynkGreenSaved);
  Serial.print(", ");

  Serial.print("B: ");
  Serial.print(blynkBlueSaved);

  Serial.println(")");

  writeSavedValues();
}

void writeSavedValues(void) {
  Blynk.virtualWrite(V11, blynkRedSaved);
  Blynk.virtualWrite(V12, blynkGreenSaved);
  Blynk.virtualWrite(V13, blynkBlueSaved);
}

void loadRGBValues(void) {
  blynkRed = blynkRedSaved;
  blynkGreen = blynkGreenSaved;
  blynkBlue = blynkBlueSaved;
  rgbMaxBrightness = rgbMaxBrightnessSaved;

  updateRGB();
}

void updateLedStrip(void) {
  if (TRANSITION_ENUM == 0) {
    updateLedStrip_imidiate();
    return;
  }
  if (TRANSITION_ENUM == 1) {
    updateLedStrip_snake();
    return;
  }
  if (TRANSITION_ENUM == 2) {
    updateLedStrip_fromMiddle();
    return;
  }
}

void updateLedStrip_imidiate(void) {
  for (int i = 0; i < rgb.numPixels() ; i++)
    setAdafruitPixelColor(i);
  rgb.show();
}

void updateLedStrip_snake(void) {
  for (int i = 0; i < rgb.numPixels() ; i++) {
    setAdafruitPixelColor(i);
    rgb.show();
  }
}

void updateLedStrip_fromMiddle(void) {
  for (int i = 0; i < (rgb.numPixels() / 2) ; i++) {
    int d_up = (rgb.numPixels() / 2) + i;
    int d_down = (rgb.numPixels() / 2) - i;
    setAdafruitPixelColor(d_up);
    setAdafruitPixelColor(d_down);
    rgb.show();
  }
  setAdafruitPixelColor(0);
  rgb.show();
}

void setAdafruitPixelColor(int i) {
  rgb.setPixelColor(i, rgb.Color(map(blynkRed, 0, 255, 0, rgbMaxBrightness),
                                 map(blynkGreen, 0, 255, 0, rgbMaxBrightness),
                                 map(blynkBlue, 0, 255, 0, rgbMaxBrightness)));
}

BLYNK_WRITE(V0)
{
  Serial.println("Received signal from ZERGBA V0!");
  // RGB widget may send invalid buffer data. If we try to read those in
  // the ESP8266 crashes. At a minimum, for valid data, the buffer
  // length should be >=5. ("0,0,0" ?)
  if (param.getLength() < 5)
    return;

  setRGBValues(param[0].asInt(), param[1].asInt(), param[2].asInt());
}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  Serial.print("Transition enum is: ");
  Serial.println(pinValue);
  TRANSITION_ENUM = pinValue;
}

BLYNK_WRITE(V2)
{
  setRGBValues(255, 0, 0);
}

BLYNK_WRITE(V3)
{
  setRGBValues(0, 255, 0);
}

BLYNK_WRITE(V4)
{
  setRGBValues(0, 0, 255);
}

BLYNK_WRITE(V5)
{
  blynkRed = param.asInt();
  updateRGB();
}

BLYNK_WRITE(V6)
{
  blynkGreen = param.asInt();
  updateRGB();
}

BLYNK_WRITE(V7)
{
  blynkBlue = param.asInt();
  updateRGB();
}

BLYNK_WRITE(V8)
{
  rgbMaxBrightness = param.asInt();
  updateRGB();
}

BLYNK_WRITE(V9) {
  saveRGBValues();
}

BLYNK_WRITE(V10) {
  loadRGBValues();
}
