#include <Util.h>

float absolute(float t) { return (t < 0) ? -1 * t : t; }

void prettyPrint(String s, String t) {  // test
  int l = PRETTY_PRINT_LEN - s.length();
  Serial.print("~ ");
  Serial.print(s);
  for (int i = 0; i < l; i++) Serial.print(" ");
  Serial.println(t);
};

void prettyPrint(char *s, char *t) { prettyPrint(String(s), String(t)); };

void prettyPrint(char *s, int t) { prettyPrint(String(s), String(t)); };

String byte_concat(byte *payload, size_t length) {
  return String((char *)payload).substring(0, length);
};