#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

struct stats {
  int damage_dealt;
  int shots;
  int hits;
  int wins;
  int battles;
};

const char* tank_id = "3681";
const char* account_id = "126219745";
const char* app_id = "74198832ec124e1cfe22490f35a7085f";

int last_battle_time;

struct stats initial;
struct stats current;

size_t json_parse(void* b, size_t size, size_t nmemb, void *userp);

int main(int argc, char* argv[]) {
  char* url; 
  asprintf(&url, "https://api.wotblitz.ru/wotb/account/tankstats/?application_id=%s&account_id=%s&tank_id=%s", app_id, account_id, tank_id);
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *handle = curl_easy_init();
  curl_easy_setopt(handle, CURLOPT_URL, url);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, json_parse);
  for (;;) {
    curl_easy_perform(handle);
    sleep(1);
  }
  curl_global_cleanup();
  return 0;
}

size_t json_parse(void *buffer, size_t size, size_t nmemb, void *userp) {
  struct json_object *data;
  struct json_object *me;
  struct json_object *lbt;
  json_object *obj = json_tokener_parse(buffer);
  json_object_object_get_ex(obj, "data", &data);
  json_object_object_get_ex(data, account_id, &me);
  json_object_object_get_ex(me, "last_battle_time", &lbt);
  if (json_object_get_int(lbt) != last_battle_time) {
    //printf("time is updated\n");
    struct json_object *all;
    struct json_object *damage_dealt;
    struct json_object *shots;
    struct json_object *hits;
    struct json_object *battles;
    struct json_object *wins;
    json_object_object_get_ex(me, "all", &all);
    json_object_object_get_ex(all, "damage_dealt", &damage_dealt);
    json_object_object_get_ex(all, "shots", &shots);
    json_object_object_get_ex(all, "hits", &hits);
    json_object_object_get_ex(all, "wins", &wins);
    json_object_object_get_ex(all, "battles", &battles);
    if (!last_battle_time) {
      //printf("init\n");
      initial.damage_dealt = json_object_get_int(damage_dealt);
      initial.shots = json_object_get_int(shots);
      initial.hits = json_object_get_int(hits);
      initial.wins = json_object_get_int(wins);
      initial.battles = json_object_get_int(battles);
    } else {
      //printf("after battle\n");
      current.damage_dealt = json_object_get_int(damage_dealt) - initial.damage_dealt;
      current.hits = json_object_get_int(hits) - initial.hits;
      current.shots = json_object_get_int(shots) - initial.shots;
      current.wins = json_object_get_int(wins) - initial.wins;
      current.battles = json_object_get_int(battles) - initial.battles; 
      printf("damage: %.2f hitrate: %.2f%% winrate: %.2f%%\n", (float)current.damage_dealt / current.battles, (float)current.hits / current.shots * 100, (float)current.wins / current.battles * 100);
    }
    last_battle_time = json_object_get_int(lbt);
  }
  return size * nmemb;
}
