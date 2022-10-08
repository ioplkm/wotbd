#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "struct.h"

typedef struct node {
  uint16_t tankId;
  uint32_t battles;
  uint32_t damage;
  uint32_t shots;
  uint32_t hits;
  uint32_t frags;
  uint32_t spots;
  uint32_t wins;
  uint32_t losses;
  uint32_t survival;
  struct node* next;
} node;

node* head;
node* cur;

battle c;

void print(void) {
  printf(" tank |  # | damage | hitrate | winrate | survival | frags | spots\n");
  node* cur = head;
  while (cur) {
    printf("%5d |%3d |%7.1f | %6.2f%% | %6.2f%% | %7.2f%% | %5.2f | %5.2f\n", cur->tankId, cur->battles, 
          (float)cur->damage / cur->battles, (float)cur->hits / cur->shots * 100,
          (float)cur->wins / cur->battles * 100, (float)cur->survival / cur->battles * 100,
          (float)cur->frags / cur->battles, (float)cur->spots / cur->battles);
    cur = cur->next;
  }
}

int main(int argc, char* argv[]) {
  int fd = open("/var/wotbd/battles.db", O_RDONLY);
  lseek(fd, -16, SEEK_END);
  time_t prevDay = time(NULL);
  prevDay -= prevDay % 86400;
  for (;;) {
    read(fd, &c, 16);
    if (lseek(fd, -32, SEEK_CUR) == -1) break;
    if (c.timestamp < prevDay) break;
    cur = head;
    for (;;) {
      if (cur == NULL) {
        node* link = (node*)malloc(sizeof(node));
        link->tankId = c.tankId;
        link->damage = c.damage;
        link->hits = c.hits;
        link->shots = c.shots;
        link->battles = 1;
        link->wins = c.win;
        link->survival = c.survive;
        link->frags = c.frags;
        link->next = head;
        head = link;
        break;
      }
      if (cur->tankId == c.tankId) {
        cur->damage += c.damage;
        cur->hits += c.hits;
        cur->shots += c.shots;
        cur->wins += c.win;
        cur->survival += c.survive;
        cur->frags += c.frags;
        cur->battles++;
        break;
      }
      if (cur->tankId != c.tankId) cur = cur->next;
    }
  }
  print();
}
