#define READER 25
#define LED 20

int imap(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {

  Serial.begin(9600);

}

void loop() {
  
  int sensorValue = analogRead(READER);

  Serial.print(map(sensorValue, 256, 4096, 0, 100));
  Serial.print("%\n");

  analogWrite(LED, imap(sensorValue, 256, 4096, 0, 256));

  delay(200);

}
