#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// === Pin Setup ===
#define DHTPIN 2
#define DHTTYPE DHT22
#define LDR_PIN A0
#define TRIG_PIN 3
#define ECHO_PIN 4
#define LED_PIN 5
#define BUZZER_PIN 6
#define SERVO_PIN 7

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  dht.begin();
  myServo.attach(SERVO_PIN);

  pinMode(LDR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  myServo.write(0); // Pintu tertutup

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Smart Home Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  // === DHT Sensor ===
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Serial Monitor
  Serial.print("Suhu: "); Serial.print(t); Serial.print(" C | ");
  Serial.print("Kelembaban: "); Serial.println(h);

  // === LDR Sensor ===
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR: "); Serial.println(ldrValue);

  if (ldrValue < 500) {
    digitalWrite(LED_PIN, HIGH); // Gelap = nyalakan lampu
  } else {
    digitalWrite(LED_PIN, LOW);  // Terang = matikan lampu
  }

  // === Ultrasonic Sensor ===
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;
  Serial.print("Jarak: "); Serial.print(distance); Serial.println(" cm");

  // === LCD Baris 1: Tampilkan suhu & kelembaban
  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(t);
  lcd.print((char)223);
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(h);
  lcd.print("%");

  // === Pintu Otomatis Berdasarkan Jarak
  if (distance > 0 && distance < 50) {  // Jika orang terdeteksi < 50 cm
    Serial.println("Orang terdeteksi! Membuka pintu...");
    lcd.setCursor(0, 1);
    lcd.print("Pintu: TERBUKA   ");
    myServo.write(90);   // Buka pintu
    delay(3000);         // Tunggu 3 detik
    myServo.write(0);    // Tutup kembali
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Pintu: TERTUTUP  ");
  }

  delay(1000);
}