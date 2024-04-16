#include <FastLED.h> // Download the FastLED NeoPixel library by David Madison or FastLED 

#define SOUND_SENSOR_PIN A0   // Analog pin for Grove Sound Sensor
#define LED_PIN1 6  // Pin for first LED strip
#define LED_PIN2 5  // Pin for second LED strip
#define NUM_LEDS 60 // Number of individual LEDs on the strip
#define BRIGHTNESS 35 // Level of brightness (Do not put on full brightness unless the powersupply can handle it!)
#define LED_TYPE WS2812B  // Specific Type for NeoPixel
#define COLOR_ORDER GRB

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN1, COLOR_ORDER>(leds1, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(SOUND_SENSOR_PIN, INPUT);
  Serial.begin(115200); // Starting serial communication
}

void loop() {
  visualize();
}

void visualize() {
  static int lastSoundLevel = 0;
  int soundLevel = analogRead(SOUND_SENSOR_PIN); // Reading sound level from sound sensor
  soundLevel = (lastSoundLevel + soundLevel) / 2; // Simple averaging for debounce
  lastSoundLevel = soundLevel;

  int numLedsLit = map(soundLevel, 0, 1023, 0, NUM_LEDS); // Mapping sound level to number of LEDs to light up
  
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < numLedsLit) {
      leds1[i] = CRGB::White; // Setting LED color to white for the lit LEDs on the first strip
      leds2[i] = CRGB::White; // Setting LED color to white for the lit LEDs on the second strip
    } else {
      leds1[i] = CRGB::Black; // Turning off LEDs that should not be lit up on the first strip
      leds2[i] = CRGB::Black; // Turning off LEDs that should not be lit up on the second strip
    }
  }
  
  FastLED.show();
  delay(5); // Adding a small delay to debounce 
  
  // Printing sound level and number of LEDs lit up to the serial monitor
  Serial.print("Sound Level: ");
  Serial.print(soundLevel);
  Serial.print("   Number of LEDs Lit: ");
  Serial.println(numLedsLit);
}
