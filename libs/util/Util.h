#ifndef MY_UTIL
#define MY_UTIL

#include <Arduino.h>
#define PRETTY_PRINT_LEN 10

float absolute(float t);
String byte_concat(byte *payload, size_t length);
void prettyPrint(String s, String t);
void prettyPrint(char *s, char *t);
void prettyPrint(char *s, int t);

#endif  // MY_UTIL