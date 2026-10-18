#define TRIG_PIN 12
#define ECHO_PIN 13 

void setup() {
  Serial.begin(57600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  float distance = duration * 0.17; // mm cinsinden mesafe

  Serial.print("Mesafe (mm): ");
  Serial.println(distance);
  delay(100);
}
