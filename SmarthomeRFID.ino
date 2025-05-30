#include <DHT.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
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
#define SS_PIN 10
#define RST_PIN 9

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// UID yang diizinkan
byte authorizedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};

void setup() {
  Serial.begin(9600);
  dht.begin();
  myServo.attach(SERVO_PIN);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LDR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  myServo.write(0); // Pintu awal tertutup

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Smart Home Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  // === DHT11 Sensor ===
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Tampilkan ke Serial Monitor
  Serial.print("Suhu: "); Serial.print(t); Serial.print(" C | ");
  Serial.print("Kelembaban: "); Serial.println(h);

  // === LDR Sensor ===
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR: "); Serial.println(ldrValue);

  if (ldrValue < 500) {
    digitalWrite(LED_PIN, HIGH); // Nyalakan lampu jika gelap
  } else {
    digitalWrite(LED_PIN, LOW);
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

  // === Tampilkan ke LCD (Baris 1) ===
  lcd.setCursor(0, 0);
  lcd.print("S:"); 
  lcd.print(t); 
  lcd.print((char)223); 
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(h);
  lcd.print("%");

  // === RFID ===
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("UID terdeteksi: ");
    bool authorized = true;
    for (byte i = 0; i < 4; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
      if (rfid.uid.uidByte[i] != authorizedUID[i]) {
        authorized = false;
      }
    }
    Serial.println();

    if (authorized) {
      Serial.println("Akses DITERIMA");
      digitalWrite(BUZZER_PIN, LOW);
      myServo.write(90); // Buka pintu
      lcd.setCursor(0, 1);
      lcd.print("Akses: DITERIMA ");
      delay(3000);
      myServo.write(0); // Tutup kembali
    } else {
      Serial.println("Akses DITOLAK");
      digitalWrite(BUZZER_PIN, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Akses: DITOLAK  ");
      delay(1000);
      digitalWrite(BUZZER_PIN, LOW);
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  } else {
    // Tidak ada kartu RFID → tampilkan status pintu
    lcd.setCursor(0, 1);
    lcd.print("Pintu: TERTUTUP ");
  }

  delay(1000);
}
