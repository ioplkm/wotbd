#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>

#include <curl/curl.h>
#include <json-c/json.h>

const char* accountId = "595173901";
const char* appId = "e9312825c85a13c661abccd749a42361";

typedef struct achievements {
  uint32_t markOfMastery;
  uint32_t markOfMasteryI;
  uint32_t markOfMasteryII;
  uint32_t markOfMasteryIII;
} achievements;

typedef struct data {
  uint32_t damage;
  uint32_t shots;
  uint32_t hits;
  uint32_t frags;
  uint32_t spots;
  uint32_t win;
  uint32_t lose;
  uint32_t survive;
} data;

typedef struct battle {
  time_t timestamp;
  uint16_t tankId;
  uint16_t damage;
  uint8_t hits;
  uint8_t shots;
  uint8_t frags : 4;
  uint8_t spots : 4;
  bool win : 1;
  bool lose : 1;
  bool survive : 1;
} battle;
