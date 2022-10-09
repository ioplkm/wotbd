int offset(char *str) {
  int c = 0;
  while (*str != 0) {
    if (*str & 128) {
      c++;
      str++;
    }
    str++;
  }
  return c;
}
