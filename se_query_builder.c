#include <string.h>
#include <stdlib.h>

#include "se_query_builder.h"

SEError SEBuildSearchQuery(SEStructuredQuery* stq, char* humanString) {
   if(!stq || !humanString) {
      return SE_BAD_PARAM;
   }

   //TODO uhhh fill this in for real

   // get questions for query
   stq->url = strdup("https://api.stackexchange.com/2.2/search/advanced?order=desc&sort=relevance&q=c%20string%20concatenation&site=stackoverflow&filter=!)5cCAwGtW9nSs24.zZQuaru1fQ7m");

   return SE_OK;
}

void SEFreeQuery(SEStructuredQuery* stq) {
   if(stq) {
      free(stq->url);
   }
}
