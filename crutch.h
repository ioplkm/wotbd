int offset(char *str) {
  int c = 0;
  while (*str++) {
    if (*str & 128) {
      c++;
      str++;
    }
  }
  return c;
}
