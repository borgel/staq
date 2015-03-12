#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "se_query_builder.h"

// FIXME this needs to be... improved. Lots.

#define API_VERSION              "2.2"
#define API_BASE                 "https://api.stackexchange.com/" API_VERSION "/"
#define API_SEARCH               "search/advanced?"
#define DEFAULT_ORDER            "order=desc"
#define DEFAULT_SORT             "sort=relevance"
#define API_PARAM_SEARCH_QUERY   "q="
#define API_PARAM_SITE           "site="
#define SITE_STACKOVERFLOW       "stackoverflow"
#define API_PARAM_FILTER         "filter="
#define DEFAULT_FILTER           "!)5cCAwGtW9nSs24.zZQuaru1fQ7m"

#define DEFAULT_SEARCH_STR       API_BASE API_SEARCH DEFAULT_ORDER "&" DEFAULT_SORT "&" API_PARAM_SEARCH_QUERY "%s" "&" API_PARAM_SITE SITE_STACKOVERFLOW "&" API_PARAM_FILTER DEFAULT_FILTER

/**
 * There's some ineffeciencies in here in the number of allocations and the
 * local curl init. Considering the time wasted here is so small compared
 * to the web request (and the rest of the lifecycle of this tool) I think
 * its a worth tradeoff for more encapsulation.
 */
// TODO make this actually handle non-default parameters (in stq)
SEError SEBuildSearchQuery(SEStructuredQuery* stq, char* humanString) {
   if(!stq || !humanString) {
      return SE_BAD_PARAM;
   }

   //stq->url = strdup("https://api.stackexchange.com/2.2/search/advanced?order=desc&sort=relevance&q=c%20string%20concatenation&site=stackoverflow&filter=!)5cCAwGtW9nSs24.zZQuaru1fQ7m");

   CURL* curl = curl_easy_init();

   char* safeQueryStr = curl_easy_escape(curl, humanString, 0);

   // this includes the formatting separator for the sprintf, but the extra
   // char or two wont matter
   int len = strlen(DEFAULT_SEARCH_STR) + strlen(safeQueryStr);

   stq->url = calloc(len, 1);
   sprintf(stq->url, DEFAULT_SEARCH_STR, safeQueryStr);

   // now that the string has been dupped, free the special CURL allocated str
   curl_free(safeQueryStr);

   curl_easy_cleanup(curl);

   return SE_OK;
}

void SEFreeQuery(SEStructuredQuery* stq) {
   if(stq) {
      free(stq->url);
      stq->url = NULL;
   }
}
