#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Servo.h>

// ===== WiFi & Blynk Setup =====
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "mez";
char pass[] = "20202020";

// ===== Pin Configuration =====
#define DHTPIN 15        // GPIO15
#define DHTTYPE DHT11
#define LDR_PIN 34       // Analog input only pin
#define TRIG_PIN 4
#define ECHO_PIN 5
#define LED_PIN 16
#define BUZZER_PIN 17
#define SERVO_PIN 18

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;
BlynkTimer timer;

// ===== Kirim Data ke Blynk App =====
void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int ldrValue = analogRead(LDR_PIN);

  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca DHT!");
    return;
  }

  // Tampilkan ke Serial Monitor
  Serial.print("Suhu: "); Serial.print(t); Serial.print("°C | ");
  Serial.print("Kelembaban: "); Serial.print(h); Serial.print("% | ");
  Serial.print("LDR: "); Serial.println(ldrValue);

  // Kirim ke Blynk
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, ldrValue);

  // Logika LED berdasarkan LDR
  if (ldrValue < 2000) {
    digitalWrite(LED_PIN, HIGH); // Gelap = nyalakan lampu
  } else {
    digitalWrite(LED_PIN, LOW);  // Terang = matikan lampu
  }

  // Ultrasonic untuk jarak
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;
  Blynk.virtualWrite(V3, distance); // Jarak

  if (distance > 0 && distance < 50) {
    Serial.println("Orang terdeteksi! Membuka pintu...");
    myServo.write(90);
    Blynk.virtualWrite(V4, "Pintu Terbuka");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(3000);
    myServo.write(0);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    myServo.write(0);
    Blynk.virtualWrite(V4, "Pintu Tertutup");
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  dht.begin();
  myServo.attach(SERVO_PIN);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  timer.setInterval(2000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
