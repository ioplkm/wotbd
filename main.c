#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <json-c/json.h>

struct stats {
  int damage_dealt;
  int shots;
  int hits;
  int wins;
  int battles;
  int time;
  int survived_battles;
};

const char* tank_id = "3681";
const char* account_id = "126219745";
const char* app_id = "74198832ec124e1cfe22490f35a7085f";

int last_battle_time;

int counter;

struct stats initial;
struct stats current;
struct stats last;

size_t json_parse(void* b, size_t size, size_t nmemb, void *userp);

int main(int argc, char* argv[]) {
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *handle = curl_easy_init();
  char *url = (char*)malloc(sizeof(char) * 999); 
  sprintf(url, "https://api.wotblitz.ru/wotb/account/tankstats/?application_id=%s&account_id=%s&tank_id=%s", app_id, account_id, tank_id);
  curl_easy_setopt(handle, CURLOPT_URL, url);
  free(url);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, json_parse);
  printf("\033c");
  printf("num |  damage | hitrate | winrate | survived |  time\n");
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
    struct json_object *survived_battles;
    struct json_object *wins;
    struct json_object *time;
    json_object_object_get_ex(me, "all", &all);
    json_object_object_get_ex(all, "damage_dealt", &damage_dealt);
    json_object_object_get_ex(all, "shots", &shots);
    json_object_object_get_ex(all, "hits", &hits);
    json_object_object_get_ex(all, "wins", &wins);
    json_object_object_get_ex(all, "battles", &battles);
    json_object_object_get_ex(all, "survived_battles", &survived_battles);
    json_object_object_get_ex(me, "battle_life_time", &time);
    if (!last_battle_time) {
      initial.damage_dealt = json_object_get_int(damage_dealt);
      initial.shots = json_object_get_int(shots);
      initial.hits = json_object_get_int(hits);
      initial.wins = json_object_get_int(wins);
      initial.battles = json_object_get_int(battles);
      initial.survived_battles = json_object_get_int(survived_battles);
      initial.time = json_object_get_int(time);
      last = initial;
    } else {
      counter++;
      struct stats current = {.damage_dealt = json_object_get_int(damage_dealt),
                              .hits = json_object_get_int(hits),
                              .shots = json_object_get_int(shots),
                              .wins = json_object_get_int(wins),
                              .battles = json_object_get_int(battles),
                              .survived_battles = json_object_get_int(survived_battles),
                              .time = json_object_get_int(time)};
      struct stats battle = {.damage_dealt = current.damage_dealt - last.damage_dealt,
                              .hits = current.hits - last.hits,
			      .shots = current.shots - last.shots,
			      .wins = current.wins - last.wins,
			      .battles = current.battles - last.battles,
			      .survived_battles = current.survived_battles - last.survived_battles,
			      .time = current.time - last.time};
      printf("%3i | %7.2f | %6.2f%% | %7s | %8s | %im%02is\n", 
             counter,
             (float)battle.damage_dealt / battle.battles, 
	     (float)battle.hits / battle.shots * 100, 
	     battle.wins ? "win" : "lose", 
	     battle.survived_battles ? "survived" : "died", 
	     battle.time / battle.battles / 60, battle.time / battle.battles % 60);
      struct stats session = {.damage_dealt = current.damage_dealt - initial.damage_dealt,
                              .hits = current.hits - initial.hits,
			      .shots = current.shots - initial.shots,
			      .wins = current.wins - initial.wins,
			      .battles = current.battles - initial.battles,
			      .survived_battles = current.survived_battles - initial.survived_battles,
			      .time = current.time - initial.time};
      printf("\033[2K\r");
      printf("avg | %7.2f | %6.2f%% | %6.2f%% | %7.2f%% | %im%02is", 
             (float)session.damage_dealt / session.battles, 
	     (float)session.hits / session.shots * 100, 
	     (float)session.wins / session.battles * 100, 
	     (float)session.survived_battles / session.battles * 100, 
	     session.time / session.battles / 60, session.time / session.battles % 60);
      fflush(stdout);
      last = current;
    }
    last_battle_time = json_object_get_int(lbt);
  }
  return size * nmemb;
}
