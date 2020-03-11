#ifndef MY_UTIL
#define MY_UTIL

#include <Arduino.h>
#define PRETTY_PRINT_LEN 10

void serialPrint(char *s, int i);
void serialPrint(char *s, float i);
float absolute(float t);
String byte_concat(byte *payload, size_t length);
int int_byte_concat(byte *payload, size_t lenght);
// char *char_byte_concat(byte *payload, size_t lenght);
char *stringToChar(String s);
void prettyPrint(String s, String t);
void prettyPrint(char *s, char *t);
void prettyPrint(char *s, int t);

#endif  // MY_UTIL