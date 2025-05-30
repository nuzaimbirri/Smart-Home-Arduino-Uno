#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ========================
// KONFIGURASI PIN
// ========================
#define DHTPIN 11
#define DHTTYPE DHT22
#define LDR_PIN A0
#define RELAY 3

#define TRIG_PIN 12
#define ECHO_PIN 13

#define RED_LED 7
#define GREEN_LED 6

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ========================
// SETUP
// ========================
void setup() {
  Serial.begin(9600);
  
  pinMode(LDR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  pinMode(RELAY, OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  dht.begin();
  lcd.begin(16,2);
  lcd.backlight();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistem SmartHome");
  delay(2000);
  lcd.clear();
}

// ========================
// LOOP UTAMA
// ========================
void loop() {
  int ldrValue = analogRead(LDR_PIN);
  float suhu = dht.readTemperature();

  // =============== 1. KONDISI RUANGAN GELAP ===============
  if (ldrValue < 500) {
    // digitalWrite(RED_LED, HIGH);
    // digitalWrite(GREEN_LED, HIGH); // Lampu nyala
    digitalWrite(RELAY, HIGH);
  
  } else {
    // digitalWrite(RED_LED, LOW);
    // digitalWrite(GREEN_LED, LOW);  // Lampu mati
  }

  // =============== 2. KONDISI ORANG MENDEKATI PINTU ===============
  long durasi, jarak;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  durasi = pulseIn(ECHO_PIN, HIGH);
  jarak = durasi * 0.034 / 2; // dalam cm

  if (jarak > 0 && jarak < 20) {  // orang terdeteksi di depan pintu
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pintu Dibuka");
    delay(2000);
    lcd.clear();
  } else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
// NO = kabel merah water pump
// COM = VCC arduino uno / daya eksternal
// kabel hitam water pump -> GND Arduino uno

  // =============== 3. KONDISI SUHU PANAS ===============
  if (suhu > 30.0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Suhu Panas");
    lcd.setCursor(0, 1);
    lcd.print("Nyalakan AC");

    // LED merah kedap-kedip
    digitalWrite(RED_LED, HIGH);
    delay(300);
    digitalWrite(RED_LED, LOW);
    delay(300);
  }

  delay(500); // jeda baca sensor
}
