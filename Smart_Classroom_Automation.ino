#include <Adafruit_NeoPixel.h>

// ---------------- PIN DEFINITIONS ----------------
#define LDR_PIN        39
#define OCCUPANCY_PIN  27
#define LIGHT_PIN      2
#define BUZZER_PIN     15
#define NEOPIXEL_PIN   14

#define NUM_PIXELS     8
#define DARK_THRESHOLD 2000   // Dark if ADC value > this

// ✅ CORRECT COLOR ORDER FOR YOUR NEOPIXEL
Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);

// ---------------- VARIABLES ----------------
bool manualMode = false;
bool buzzerState = false;

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);

  pinMode(OCCUPANCY_PIN, INPUT_PULLUP);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  pixels.begin();
  pixels.clear();
  pixels.show();

  Serial.println("Smart Classroom Automation Started");
}

// ---------------- HELPER FUNCTIONS ----------------
void setNeoPixel(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void buzzerOn() {
  if (!buzzerState) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerState = true;
  }
}

void buzzerOff() {
  if (buzzerState) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = false;
  }
}

// ---------------- LOOP ----------------
void loop() {

  int ldrValue = analogRead(LDR_PIN);
  bool occupied = (digitalRead(OCCUPANCY_PIN) == LOW);
  bool isDark = (ldrValue > DARK_THRESHOLD);

  Serial.print("LDR: ");
  Serial.print(ldrValue);
  Serial.print(" | Occupied: ");
  Serial.print(occupied);
  Serial.print(" | Dark: ");
  Serial.println(isDark);

  // -------- MANUAL MODE --------
  if (manualMode) {
    digitalWrite(LIGHT_PIN, HIGH);
    buzzerOff();
    setNeoPixel(255, 255, 0);   // 🟡 YELLOW
  }

  // -------- AUTOMATIC MODE --------
  else {

    // Occupied + Dark → Light ON
    if (occupied && isDark) {
      digitalWrite(LIGHT_PIN, HIGH);
      buzzerOff();
      setNeoPixel(0, 255, 0);   // 🟢 GREEN
    }

    // Empty + Dark → Energy waste
    else if (!occupied && isDark) {
      digitalWrite(LIGHT_PIN, LOW);
      buzzerOn();
      setNeoPixel(255, 0, 0);   // 🔴 RED
    }

    // Bright → Everything OFF
    else {
      digitalWrite(LIGHT_PIN, LOW);
      buzzerOff();
      pixels.clear();
      pixels.show();
    }
  }

  delay(500);
}