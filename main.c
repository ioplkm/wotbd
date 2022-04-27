#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>

#include <curl/curl.h>
#include <json-c/json.h>

struct stats {
  uint16_t damageDealt;
  uint8_t shots;
  uint8_t hits;
  uint8_t wins;
  uint8_t losses;
  uint8_t battles;
  uint8_t survivedBattles;
  uint8_t frags;
  uint8_t spots;
};

const char* accountId = "126219745";
const char* appId = "74198832ec124e1cfe22490f35a7085f";

time_t lastBattleTime;
bool isTimeChanged;

struct stats initial;
struct stats last;

size_t initParse(void* b, size_t size, size_t nmemb);
size_t timeParse(void* b, size_t size, size_t nmemb);
size_t dataParse(void* b, size_t size, size_t nmemb);

int main(int argc, char* argv[]) {
  curl_global_init(CURL_GLOBAL_ALL);
  char *url = (char*)malloc(sizeof(char) * 999); 

  CURL *handle = curl_easy_init();
  sprintf(url, "https://api.wotblitz.ru/wotb/account/info/?application_id=%s&account_id=%s&fields=last_battle_time%%2Cstatistics.all.battles%%2Cstatistics.all.damage_dealt%%2Cstatistics.all.wins%%2Cstatistics.all.survived_battles%%2Cstatistics.all.hits%%2Cstatistics.all.shots%%2Cstatistics.all.losses%%2Cstatistics.all.frags%%2Cstatistics.all.spotted", appId, accountId);
  curl_easy_setopt(handle, CURLOPT_URL, url);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, initParse);
  curl_easy_perform(handle);

  CURL *timeHandle = curl_easy_init();
  sprintf(url, "https://api.wotblitz.ru/wotb/account/info/?application_id=%s&account_id=%s&fields=last_battle_time", appId, accountId);
  curl_easy_setopt(timeHandle, CURLOPT_URL, url);
  curl_easy_setopt(timeHandle, CURLOPT_WRITEFUNCTION, timeParse);

  sprintf(url, "https://api.wotblitz.ru/wotb/account/info/?application_id=%s&account_id=%s&fields=statistics.all.battles%%2Cstatistics.all.damage_dealt%%2Cstatistics.all.wins%%2Cstatistics.all.survived_battles%%2Cstatistics.all.hits%%2Cstatistics.all.shots%%2Cstatistics.all.losses%%2Cstatistics.all.frags%%2Cstatistics.all.spotted", appId, accountId);
  curl_easy_setopt(handle, CURLOPT_URL, url);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, dataParse);

  free(url);
  printf("\033c");
  printf(" num | damage | hitrate | winrate | survival | frags | spots\n");
  for (;;) {
    curl_easy_perform(timeHandle);
    if (isTimeChanged) {
      curl_easy_perform(handle);
    }
    sleep(1);
  }
  curl_global_cleanup();
  return 0;
}

size_t initParse(void *buffer, size_t size, size_t nmemb) {
  json_object *obj = json_tokener_parse(buffer);
  json_object *data, *me, *last_battle_time, *all, *statistics;
  json_object_object_get_ex(obj, "data", &data);
  json_object_object_get_ex(data, accountId, &me);
  json_object_object_get_ex(me, "last_battle_time", &last_battle_time);
  json_object_object_get_ex(me, "statistics", &statistics);
  json_object_object_get_ex(statistics, "all", &all);
  json_object *damage_dealt, *shots, *hits, *battles, *survived_battles, *wins, *losses, *frags, *spotted;
  json_object_object_get_ex(all, "damage_dealt", &damage_dealt);
  json_object_object_get_ex(all, "shots", &shots);
  json_object_object_get_ex(all, "hits", &hits);
  json_object_object_get_ex(all, "wins", &wins);
  json_object_object_get_ex(all, "battles", &battles);
  json_object_object_get_ex(all, "survived_battles", &survived_battles);
  json_object_object_get_ex(all, "losses", &losses);
  json_object_object_get_ex(all, "frags", &frags);
  json_object_object_get_ex(all, "spotted", &spotted);
  initial.damageDealt = json_object_get_int(damage_dealt);
  initial.shots = json_object_get_int(shots);
  initial.hits = json_object_get_int(hits);
  initial.wins = json_object_get_int(wins);
  initial.battles = json_object_get_int(battles);
  initial.survivedBattles = json_object_get_int(survived_battles);
  initial.frags = json_object_get_int(frags);
  initial.spots = json_object_get_int(spotted);
  last = initial;
  last.losses = json_object_get_int(losses);
  lastBattleTime = json_object_get_int(last_battle_time);
  return size * nmemb;
}

size_t timeParse(void *buffer, size_t size, size_t nmemb) {
  json_object *obj = json_tokener_parse(buffer);
  json_object *data, *me, *last_battle_time;
  json_object_object_get_ex(obj, "data", &data);
  json_object_object_get_ex(data, accountId, &me);
  json_object_object_get_ex(me, "last_battle_time", &last_battle_time);
  isTimeChanged = json_object_get_int(last_battle_time) != lastBattleTime;
  if (isTimeChanged) lastBattleTime = json_object_get_int(last_battle_time);
  return size * nmemb;
}

size_t dataParse(void *buffer, size_t size, size_t nmemb) {
  json_object *obj = json_tokener_parse(buffer);
  json_object *data, *me, *statistics, *all;
  json_object_object_get_ex(obj, "data", &data);
  json_object_object_get_ex(data, accountId, &me);
  json_object_object_get_ex(me, "statistics", &statistics);
  json_object_object_get_ex(statistics, "all", &all);
  json_object *damage_dealt, *shots, *hits, *battles, *survived_battles, *wins, *losses, *frags, *spotted;
  json_object_object_get_ex(all, "damage_dealt", &damage_dealt);
  json_object_object_get_ex(all, "shots", &shots);
  json_object_object_get_ex(all, "hits", &hits);
  json_object_object_get_ex(all, "wins", &wins);
  json_object_object_get_ex(all, "battles", &battles);
  json_object_object_get_ex(all, "survived_battles", &survived_battles);
  json_object_object_get_ex(all, "losses", &losses);
  json_object_object_get_ex(all, "frags", &frags);
  json_object_object_get_ex(all, "spotted", &spotted);
      struct stats current = {.damageDealt = json_object_get_int(damage_dealt),
                              .hits = json_object_get_int(hits),
                              .shots = json_object_get_int(shots),
                              .wins = json_object_get_int(wins),
                              .losses = json_object_get_int(losses),
                              .battles = json_object_get_int(battles),
                              .survivedBattles = json_object_get_int(survived_battles),
                              .frags = json_object_get_int(frags),
                              .spots = json_object_get_int(spotted)};
      struct stats battle = {.damageDealt = current.damageDealt - last.damageDealt,
                              .hits = current.hits - last.hits,
			      .shots = current.shots - last.shots,
			      .wins = current.wins - last.wins,
                              .losses = current.losses - last.losses,
			      .battles = current.battles - last.battles,
			      .survivedBattles = current.survivedBattles - last.survivedBattles,
                              .frags = current.frags - last.frags,
                              .spots = current.spots - last.spots};
      printf("\033[2K\r");
      printf("%4i | %6i | %6.2f%% | %7s | %8s | %5d | %5d\n", 
             current.battles - initial.battles,
             battle.damageDealt, 
	     (float)battle.hits / battle.shots * 100, 
	     battle.wins ? "win" : battle.losses ? "lose" : "draw", 
	     battle.survivedBattles ? "survived" : "died",
             battle.frags,
             battle.spots);
      struct stats session = {.damageDealt = current.damageDealt - initial.damageDealt,
                              .hits = current.hits - initial.hits,
			      .shots = current.shots - initial.shots,
			      .wins = current.wins - initial.wins,
			      .battles = current.battles - initial.battles,
			      .survivedBattles = current.survivedBattles - initial.survivedBattles,
                              .frags = current.frags - initial.frags,
                              .spots = current.spots - initial.spots};
      printf(" avg |%7.1f | %6.2f%% | %6.2f%% | %7.2f%% | %5.2f | %5.2f", 
             (float)session.damageDealt / session.battles, 
	     (float)session.hits / session.shots * 100, 
	     (float)session.wins / session.battles * 100, 
	     (float)session.survivedBattles / session.battles * 100,
             (float)session.frags / session.battles,
             (float)session.spots / session.battles);
      fflush(stdout);
      last = current;
  return size * nmemb;
}
