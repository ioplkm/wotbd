#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "struct.h"
#include "crutch.h"

#include "config.h"

const char *tankUrl = "https://api.wotblitz.eu/wotb/encyclopedia/vehicles/?application_id=%s&tank_id=%d";

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

typedef struct sum {
  uint32_t level;
  uint32_t battles;
  uint32_t damage;
  uint32_t shots;
  uint32_t hits;
  uint32_t frags;
  uint32_t spots;
  uint32_t wins;
  uint32_t survival;
} sum; //todo

node *head;
node *cur;

char tankName[99];
char *url;
CURL *tankHandle;
char tier;
battle c;
bool color = true;

uint8_t setColor(float damage, char tier) {
  int ok = (tier - 2) * 500;
  return 16 + 
  36 * (damage <= ok - 500 ? 5 : damage >= ok ? 0 : (int)(ok - damage) / 125 + 1) +
  6 * (damage <= ok - 1000 ? 0 : damage + 500 >= ok ? 5 : (int)(damage - ok + 1000) / 125 + 1) +
  1 * 0;
}

size_t nameParse(void *buffer, size_t size, size_t nmemb, char* pName) {
  json_object *obj = json_tokener_parse(buffer);
  json_object *data, *id, *jsonName;
  json_object_object_get_ex(obj, "data", &data);
  char buf[6];
  sprintf(buf, "%d", cur->tankId);
  json_object_object_get_ex(data, buf, &id);
  json_object_object_get_ex(id, "name", &jsonName);
  strcpy(pName, json_object_get_string(jsonName));

  json_object *t;
  json_object_object_get_ex(id, "tier", &t);
  tier = json_object_get_int(t);
  return size * nmemb;
}

char* getName(uint16_t tankId) {
  sprintf(url, tankUrl, appId, tankId);
  curl_easy_setopt(tankHandle, CURLOPT_URL, url);
  curl_easy_perform(tankHandle);
  return tankName;
}

void print(void) {
  printf("                        tank  |  # | damage | hitrate | winrate | survival | frags | spots\n");
  cur = head;
  while (cur) {
    for (int i = 0; i < offset(getName(cur->tankId)); i++) printf(" "); //do smth with this crutch
    printf("%29s |%3d |\x1b[38:5:%dm%7.1f \x1b[0m| %6.2f%% | %6.2f%% | %7.2f%% | %5.2f | %5.2f\n",
           getName(cur->tankId), 
           cur->battles, 
           setColor((float)cur->damage / cur->battles, tier),
           (float)cur->damage / cur->battles,
           (float)cur->hits / cur->shots * 100, 
           (float)cur->wins / cur->battles * 100, 
           (float)cur->survival / cur->battles * 100,
           (float)cur->frags / cur->battles, 
           (float)cur->spots / cur->battles);
    cur = cur->next;
  }
}

int main(int argc, char* argv[]) {
  char opt;
  int days = 0;
  while ((opt = getopt(argc, argv, "d:C")) != -1) {
    switch (opt) {
      case 'd':
        days = atoi(optarg) - 1;
      case 'C':
        color = false;
    }
  }
  curl_global_init(CURL_GLOBAL_ALL);
  url = (char*)malloc(sizeof(char) * 999);
  tankHandle = curl_easy_init();
  curl_easy_setopt(tankHandle, CURLOPT_WRITEFUNCTION, nameParse);
  curl_easy_setopt(tankHandle, CURLOPT_WRITEDATA, tankName);
  int fd = open("/var/wotbd/battles.db", O_RDONLY);
  lseek(fd, 0, SEEK_END);
  time_t prevDay = time(NULL);
  prevDay -= (prevDay + (timezone - start)*3600) % 86400;
  //if (argc == 2 && !strcmp(argv[1], "m")) prevDay -= 29 * 86400; //month stat
  prevDay -= days * 86400;
  for (;;) {
    if (lseek(fd, -16, SEEK_CUR) == -1) break;
    read(fd, &c, 16);
    if (lseek(fd, -16, SEEK_CUR) == -1) break;
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
        link->spots = c.spots;
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
        cur->spots += c.spots;
        cur->battles++;
        break;
      }
      if (cur->tankId != c.tankId) cur = cur->next;
    }
  }
  print();
}
