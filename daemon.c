#include <sys/stat.h>

#include "struct.h"

const char* timeUrl = "https://api.wotblitz.eu/wotb/account/info/?application_id=%s&account_id=%s&fields=last_battle_time";
const char* dataUrl = "https://api.wotblitz.eu/wotb/account/info/?application_id=%s&account_id=%s&fields=statistics.all.battles%%2Cstatistics.all.damage_dealt%%2Cstatistics.all.wins%%2Cstatistics.all.survived_battles%%2Cstatistics.all.hits%%2Cstatistics.all.shots%%2Cstatistics.all.losses%%2Cstatistics.all.frags%%2Cstatistics.all.spotted";
const char* tanksUrl = "https://api.wotblitz.eu/wotb/tanks/stats/?application_id=%s&account_id=%s&fields=last_battle_time%%2Ctank_id";
const char* achievementUrl = "https://api.wotblitz.eu/wotb/account/achievements/?application_id=%s&account_id=%s&fields=achievements";

time_t lastBattleTime, newTime;

data lastData;
data currentData;
battle record;

size_t timeParse(void* buffer, size_t size, size_t nmemb, time_t *pTime);
size_t dataParse(void* buffer, size_t size, size_t nmemb, data *pData);
size_t tanksParse(void* buffer, size_t size, size_t nmemb, uint16_t *tankId);
size_t achievementParse(void* buffer, size_t size, size_t nmemb, achievements *pAchievements);

int main(int argc, char* argv[]) {
  //printf("%lu\n", sizeof(battle));
  curl_global_init(CURL_GLOBAL_ALL);
  char *url = (char*)malloc(sizeof(char) * 999); 

  CURL *timeHandle = curl_easy_init();
  sprintf(url, timeUrl, appId, accountId);
  curl_easy_setopt(timeHandle, CURLOPT_URL, url);
  curl_easy_setopt(timeHandle, CURLOPT_WRITEFUNCTION, timeParse);
  curl_easy_setopt(timeHandle, CURLOPT_WRITEDATA, &lastBattleTime);
  curl_easy_perform(timeHandle);
  curl_easy_setopt(timeHandle, CURLOPT_WRITEDATA, &newTime);

  CURL *dataHandle = curl_easy_init();
  sprintf(url, dataUrl, appId, accountId);
  curl_easy_setopt(dataHandle, CURLOPT_URL, url);
  curl_easy_setopt(dataHandle, CURLOPT_WRITEFUNCTION, dataParse);
  curl_easy_setopt(dataHandle, CURLOPT_WRITEDATA, &lastData);
  curl_easy_perform(dataHandle);
  curl_easy_setopt(dataHandle, CURLOPT_WRITEDATA, &currentData);

  CURL *tanksHandle = curl_easy_init();
  sprintf(url, tanksUrl, appId, accountId);
  curl_easy_setopt(tanksHandle, CURLOPT_URL, url);
  curl_easy_setopt(tanksHandle, CURLOPT_WRITEFUNCTION, tanksParse);
  curl_easy_setopt(tanksHandle, CURLOPT_WRITEDATA, &record.tankId);

  /*CURL *achievementHandle = curl_easy_init();
  sprintf(url, achievementUrl, appId, accountId);
  curl_easy_setopt(achievementHandle, CURLOPT_URL, url);
  curl_easy_setopt(achievementHandle, CURLOPT_WRITEFUNCTION, achievementParse);
  curl_easy_setopt(achievementHandle, CURLOPT_WRITEDATA, &initialStats.medals);
  curl_easy_perform(achievementHandle);
  curl_easy_setopt(achievementHandle, CURLOPT_WRITEDATA, &currentStats.medals);*/

  free(url);
  mkdir("/var/wotbd", 0444);
  int fd = open("/var/wotbd/battles.db", O_APPEND | O_CREAT | O_WRONLY, 0440);
  for (;;) {
    curl_easy_perform(timeHandle);
    if (newTime != lastBattleTime) {
      lastBattleTime = newTime;
      record.timestamp = lastBattleTime;
      curl_easy_perform(tanksHandle);
      curl_easy_perform(dataHandle);
      record.damage = currentData.damage - lastData.damage;
      record.shots = currentData.shots - lastData.shots;
      record.hits = currentData.hits - lastData.hits;
      record.spots = currentData.spots - lastData.spots;
      record.frags = currentData.frags - lastData.frags;
      record.win = currentData.win - lastData.win;
      record.lose = currentData.lose - lastData.lose;
      record.survive = currentData.survive - lastData.survive;
      lastData = currentData;
      //curl_easy_perform(achievementHandle);
      write(fd, &record, 16);
    }
    sleep(1);
  }
  curl_global_cleanup();
  return 0;
}

size_t timeParse(void *buffer, size_t size, size_t nmemb, time_t *pTime) {
  json_object *obj = json_tokener_parse(buffer);
  json_object *data, *me, *last_battle_time;
  json_object_object_get_ex(obj, "data", &data);
  json_object_object_get_ex(data, accountId, &me);
  json_object_object_get_ex(me, "last_battle_time", &last_battle_time);
  *pTime = json_object_get_int(last_battle_time);
  return size * nmemb;
}

size_t tanksParse(void *buffer, size_t size, size_t nmemb, uint16_t *pTankId) {
  json_object *obj = json_tokener_parse(buffer);
  json_object *data, *me;
  json_object_object_get_ex(obj, "data", &data);
  json_object_object_get_ex(data, accountId, &me);
  json_object *array, *lbt, *tankId;
  for (int i = 0; i < (int)json_object_array_length(me); i++) {
    array = json_object_array_get_idx(me, i);
    json_object_object_get_ex(array, "last_battle_time", &lbt);
    json_object_object_get_ex(array, "tank_id", &tankId);
    if (json_object_get_int(lbt) == lastBattleTime) {
      *pTankId = json_object_get_int(tankId);
      break;
    }
  }
  return size * nmemb;
}

size_t dataParse(void *buffer, size_t size, size_t nmemb, data *pData) {
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
  pData->damage = json_object_get_int(damage_dealt);
  pData->hits = json_object_get_int(hits);
  pData->shots = json_object_get_int(shots);
  pData->win = json_object_get_int(wins);
  pData->lose = json_object_get_int(losses);
  pData->survive = json_object_get_int(survived_battles);
  pData->frags = json_object_get_int(frags);
  pData->spots = json_object_get_int(spotted);
  return size * nmemb;
}

size_t achievementParse(void *buffer, size_t size, size_t nmemb, achievements *pAchievements) {
  json_object *obj = json_tokener_parse(buffer);
  json_object *data, *me, *achievements;
  json_object_object_get_ex(obj, "data", &data);
  json_object_object_get_ex(data, accountId, &me);
  json_object_object_get_ex(me, "achievements", &achievements);
  json_object *markOfMastery, *markOfMasteryI, *markOfMasteryII, *markOfMasteryIII;
  json_object_object_get_ex(achievements, "markOfMastery", &markOfMastery);
  json_object_object_get_ex(achievements, "markOfMasteryI", &markOfMasteryI);
  json_object_object_get_ex(achievements, "markOfMasteryII", &markOfMasteryII);
  json_object_object_get_ex(achievements, "markOfMasteryIII", &markOfMasteryIII);
  pAchievements->markOfMastery = json_object_get_int(markOfMastery);
  pAchievements->markOfMasteryI = json_object_get_int(markOfMasteryI);
  pAchievements->markOfMasteryII = json_object_get_int(markOfMasteryII);
  pAchievements->markOfMasteryIII = json_object_get_int(markOfMasteryIII);
  return size * nmemb;
}
