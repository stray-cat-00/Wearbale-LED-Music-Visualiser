#include <FastLED.h>  // Download the FastLED NeoPixel library for LED control
#include <Wire.h>  // Include the Wire library for I2C communication
#include <Adafruit_MPU6050.h>  // Download the Adafruit MPU6050 library for the accelerometer and gyroscope
#include <Adafruit_Sensor.h>  // Download the Adafruit Sensor library

#define SOUND_SENSOR_PIN A0  // Analog pin for Grove Sound Sensor
#define LED_PIN1 6  // Pin for the first LED strip
#define LED_PIN2 5  // Pin for the second LED strip
#define NUM_LEDS 60  // Total number of LEDs per strip
#define BRIGHTNESS 35  // LED brightness level
#define LED_TYPE WS2812B  // Type of LED strip, WS2812B NeoPixel
#define COLOR_ORDER GRB  // Color order Green-Red-Blue

// Creating LED array for each strip
CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

// Accelerometer
Adafruit_MPU6050 mpu;

// Global variables to store accumulated angle for X and Y axes
float accumulatedAngleX = 0.0;
float accumulatedAngleY = 0.0;

void setup() {
  // Initializing the LED strips
  FastLED.addLeds<LED_TYPE, LED_PIN1, COLOR_ORDER>(leds1, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // Configuring the sound sensor pin as input
  pinMode(SOUND_SENSOR_PIN, INPUT);
  Serial.begin(115200); // Begin serial communication at 115200 baud!!!

  // Initializing MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);  // Stop everything if sensor is not found
    }
  }

  // Configuring MPU6050 settings
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  visualize();  // Function to handle sound visualization on LEDs
  checkTwirl();  // Function to check for twirl motion and update LEDs
}

void visualize() {
  int soundLevel = analogRead(SOUND_SENSOR_PIN);  // Reading the current sound level
  int numLedsLit = map(soundLevel, 0, 1023, 0, NUM_LEDS);  // Mapping sound level to number of LEDs

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < numLedsLit) {
      leds1[i] = CRGB::White;  // Lighting LEDs up to the mapped number
      leds2[i] = CRGB::White;
    } else {
      leds1[i] = CRGB::Black;  // Turning off the rest
      leds2[i] = CRGB::Black;
    }
  }
  FastLED.show();  // Updating the LED strip
}

void checkTwirl() {
  sensors_event_t a, g, temp;  // Structures to hold sensor data
  mpu.getEvent(&a, &g, &temp);  // Reading sensors

  float deltaTime = 0.01;  // Sample rate (time in seconds between readings)

  // Accumulating the change in angle based on the gyroscope data
  accumulatedAngleX += g.gyro.x * deltaTime;
  accumulatedAngleY += g.gyro.y * deltaTime;

  // Checking if either accumulated angle on X or Y axis exceeds 180 degrees
  if (abs(accumulatedAngleX) >= 180 || abs(accumulatedAngleY) >= 180) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds1[i] = CRGB(random(255), random(255), random(255));  // Setting a random color
      leds2[i] = CRGB(random(255), random(255), random(255));
    }
    FastLED.show();  // Updating LEDs with new colors
    delay(20);  // Short delay to prevent too frequent updates

    // Reset the accumulated angles after triggering
    accumulatedAngleX = 0;
    accumulatedAngleY = 0;
  }
}
