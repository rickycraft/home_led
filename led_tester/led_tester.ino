#define LED_PIN 5
int i = 0;

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    analogWriteRange(255);
    // analogWriteFreq(20000);
    analogWriteFreq(100);
}

void loop() {
    // power on
    digitalWrite(BUILTIN_LED, LOW);
    while (i < 255) {
        i++;
        analogWrite(LED_PIN, i);
        delay(100);
    }
    delay(1000);
    // power off
    digitalWrite(BUILTIN_LED, HIGH);
    while (i > 0) {
        i--;
        analogWrite(LED_PIN, i);
        delay(100);
    }
    delay(1000);
}
