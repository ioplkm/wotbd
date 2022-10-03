#include <stdint.h>
#include <stdbool.h>
#include <time.h>

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
