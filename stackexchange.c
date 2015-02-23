// API workflow?

#include <stdlib.h>

#include <curl/curl.h>
#include <jansson.h>

#include "se_query_builder.h"
#include "stackexchange.h"

// temp file naming scheme
static char TEMP_FILE_FORMAT[] = "/tmp/staq-working-XXXX";

// the shared CURL handle
// FIXME rename to g_
static CURL *curl = NULL;

void SEInit() {
   if(!curl) {
      curl = curl_easy_init();
   }

   // configure some CURL options
   // follow redirects
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

   // stack exchange gives us gzipped content, so ask curl to gunzip for us
   curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
}

void SECleanup() {
   if(curl) {
      curl_easy_cleanup(curl);
   }
}

/**
 * Assemble a single SEQuestion object from it's JSON counterpart
 */
static SEError SEPopulateQuestion(SEQuestion* question, json_t* json) {
   if(!question || !json) {
      return SE_ERROR;
   }

   if(!json_is_object(json))
   {
      fprintf(stderr, "error: root is not an object\n");
      json_decref(json);
      return SE_JSON_ERROR;
   }

   //TODO get all members

   return SE_OK;
}

/**
 * Generic 'run a query and return the decoded JSON of the response'
 */
static SEError SEQueryAdvanced(json_t** json, SEStructuredQuery* query, SEQueryOptions* seqo) {
   CURLcode res;

   if(!query || !curl) {
      return SE_BAD_PARAM;
   }

   *json = NULL;

   // instead of writing the curl response into memory, spool it to a temp file
   int cfd = mkstemp(TEMP_FILE_FORMAT);
   if(cfd == -1) {
      perror("opening the temp file");
      return SE_ERROR;
   }

   FILE* cfile = fdopen(cfd, "w+");
   if(!cfile) {
      perror("opening the temp file");
      return SE_ERROR;
   }

   // set the url
   curl_easy_setopt(curl, CURLOPT_URL, query->url);

   // ask curl to write to our file
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, cfile);

   /* Perform the request, res will get the return code */ 
   res = curl_easy_perform(curl);

   /* Check for errors */ 
   if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
      return SE_CURL_ERROR;
   }

   // reset output stream to start
   fseek(cfile, 0, SEEK_SET);

   // check for errors and return status
   json_error_t jerr;
   *json = json_loadf(cfile, 0, &jerr);

   // no matter what, we don't need this curl session anymore
   curl_easy_cleanup(curl);

   // TODO unlink temp file

   if(!*json) {
      fprintf(stderr, "error: on line %d: %s\n", jerr.line, jerr.text);
      return SE_JSON_ERROR;
   }

   return SE_OK;
}



SEError SEEasyFindQuestions(SEQuestion* questions, char* query) {
   return SEFindQuestions(questions, query, NULL);
}

SEError SEFindQuestions(SEQuestion* questions, char* humanQueryString, SEQueryOptions* seqo) {
   SEError res;
   SEStructuredQuery stq;
   json_t* root = NULL;

   if(!humanQueryString) {
      return SE_BAD_PARAM;
   }

   // build the query
   res = SEBuildSearchQuery(&stq, humanQueryString);
   if(res != SE_OK) {
      printf("problem building query. code %d\n", res);
      return SE_ERROR;
   }

   //run an advanced query and get the json
   res = SEQueryAdvanced(&root, &stq, seqo);

   SEFreeQuery(&stq);

   if(res != SE_OK) {
      printf("problem running query. code %d\n", res);
      return SE_ERROR;
   }

   puts("converting json to our structs");

   // TODO break this validation out?

   // do some json validation
   if(!json_is_object(root))
   {
      fprintf(stderr, "error: root is not an object\n");
      json_decref(root);
      return SE_JSON_ERROR;
   }

   // get the array from the root object
   json_t* jquestions = json_object_get(root, "items");
   if(!jquestions) {
      fprintf(stderr, "error: couldn't get question items\n");
      json_decref(root);
      return SE_JSON_ERROR;
   }

   puts("before loop");

   //TODO convert the json into SEQuestion struct arrays and such
   int numQuestions = json_array_size(jquestions);
   questions = (SEQuestion*)malloc(numQuestions * sizeof(SEQuestion));

   json_t* jcur;
   for(int i = 0; i < json_array_size(jquestions); i++) {
      jcur = json_array_get(jquestions, i);

      // fill in this question object
      SEPopulateQuestion(&questions[i], jcur);

      //jcur = json_object_get(jcur, "body");
      //printf("\n\n>>>>%s\n", json_string_value(jcur));
   }

   // free the json
   json_decref(root);

   return SE_ERROR;
}

// FIXME TODO function to get all answers for a question


// TODO fill this in
void SEFreeQuestions(SEQuestion* questions) {
   // free all allocated strings inside each one
}

