#include <string.h>
#include <stdlib.h>

#include "se_query_builder.h"

SEError SEBuildSearchQuery(SEStructuredQuery* stq, char* humanString) {
   if(!stq || !humanString) {
      return SE_BAD_PARAM;
   }

   //TODO uhhh fill this in for real
   //stq->url = strdup("www.example.com");

   // get answers for given question
   //stq->url = strdup("https://api.stackexchange.com/2.2/questions/9555167/answers?order=desc&sort=votes&site=stackoverflow&filter=!9YdnSMldD");

   // get questions for query
   stq->url = strdup("api.stackexchange.com/2.2/search/advanced?order=desc&sort=relevance&q=c%20string%20concatenation&site=stackoverflow&filter=!4*SyY(M(4WWPiOhna");


   return SE_OK;
}

void SEFreeQuery(SEStructuredQuery* stq) {
   if(stq) {
      free(stq->url);
   }
}
