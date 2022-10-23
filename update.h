#include <stdbool.h>

#include "struct.h"

const char* battleUrl = "https://api.wotblitz.eu/wotb/account/info/?application_id=%s&account_id=%s&fields=statistics.all.battles";
uint32_t lastBattles, battles;
CURL *battleHandle;

size_t battleParse(void *buffer, size_t size, size_t nmemb, uint32_t *pBattles) {
  json_object *obj = json_tokener_parse(buffer);
  json_object *data, *me, *statistics, *all, *battles;
  json_object_object_get_ex(obj, "data", &data);
  json_object_object_get_ex(data, accountId, &me);
  json_object_object_get_ex(me, "statistics", &statistics);
  json_object_object_get_ex(statistics, "all", &all);
  json_object_object_get_ex(all, "battles", &battles);
  *pBattles = json_object_get_int(battles);
  return size * nmemb;
}

void checkInit(void) {
  battleHandle = curl_easy_init();
  char *url = (char*)malloc(sizeof(char) * 999);
  sprintf(url, battleUrl, appId, accountId);
  curl_easy_setopt(battleHandle, CURLOPT_URL, url);
  free(url);
  curl_easy_setopt(battleHandle, CURLOPT_WRITEFUNCTION, battleParse);
  curl_easy_setopt(battleHandle, CURLOPT_WRITEDATA, &lastBattles);
  curl_easy_perform(battleHandle);
  curl_easy_setopt(battleHandle, CURLOPT_WRITEDATA, &battles);
}

bool checkUpdate(void) {
  curl_easy_perform(battleHandle);
  if (battles == lastBattles) return 0;
  printf("new battle\n");
  lastBattles = battles;
  return 1;
}
