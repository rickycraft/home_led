#include <Util.h>

float absolute(float t) { return (t < 0) ? -1 * t : t; }

void serialPrint(char *s, int i) {
  Serial.print(s);
  Serial.println(i);
};

void serialPrint(char *s, float i) {
  Serial.print(s);
  Serial.println(i);
};

void prettyPrint(String s, String t) {  // test
  int l = PRETTY_PRINT_LEN - s.length();
  Serial.print("~ ");
  Serial.print(s);
  for (int i = 0; i < l; i++) Serial.print(" ");
  Serial.println(t);
};

void prettyPrint(char *s, char *t) {  //
  prettyPrint(String(s), String(t));
};

void prettyPrint(char *s, int t) {  //
  prettyPrint(String(s), String(t));
};

void prettyPrint(char *s, int t, char *u) {
  String _s = String(s);
  int l = PRETTY_PRINT_LEN - _s.length();
  Serial.print("~ ");
  Serial.print(s);
  for (int i = 0; i < l; i++) Serial.print(" ");
  Serial.print(t);
  Serial.println(u);
};

String byte_concat(byte *payload, size_t length) {
  return String((char *)payload).substring(0, length);
};

int int_byte_concat(byte *payload, size_t lenght) { return byte_concat(payload, lenght).toInt(); }

char *stringToChar(String s) {
  int len = s.length() + 1;
  char *buff = (char *)malloc(len * sizeof(char));
  s.toCharArray(buff, len);
  return buff;
};