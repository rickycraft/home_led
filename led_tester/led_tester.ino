#define LED_PIN 15
int i = 0;

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // power on
    digitalWrite(BUILTIN_LED, LOW);
    while (i < 255) {
        i++;
        analogWrite(LED_PIN, i);
        delay(100);
    }
    delay(2000);
    digitalWrite(LED_PIN, HIGH);
    delay(2000);
    // power off
    digitalWrite(BUILTIN_LED, HIGH);
    while (i > 0) {
        i--;
        analogWrite(LED_PIN, i);
        delay(100);
    }
    delay(2000);
    digitalWrite(LED_PIN, LOW);
    delay(2000);
}
