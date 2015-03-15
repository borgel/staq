// API workflow?

#include <stdlib.h>

#include <curl/curl.h>
#include <jansson.h>

#include "se_query_builder.h"
#include "stackexchange.h"

// the shared CURL handle
static int g_curl = 0;

void SEInit() {
   if(!g_curl) {
      curl_global_init(CURL_GLOBAL_DEFAULT);
      g_curl = 1;
   }
}

void SECleanup() {
   if(g_curl) {
      curl_global_cleanup();
      g_curl = 0;
   }
}

/**
 * Sorting function for stdlib qsort
 */
// qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
// The comparison function must return an integer less than, equal to, or
// greater than zero if the first argument is considered to be respectively
// less than, equal to, or greater than the second.
static int CompareSEAnswer(const void* a, const void* b) {
   SEAnswer* qa = (SEAnswer*)a;
   SEAnswer* qb = (SEAnswer*)b;

   if(!qa || !qb) {
      return 0;
   }

   if(qa->score < qb->score) {
      return 1;
   }
   else if (qa->score > qb->score) {
      return -1;
   }
   return 0;
}

/**
 * Assemble a single answer from its JSON
 */
static SEError SEPopulateAnswer(SEAnswer* answer, json_t* json) {
   if(!answer || !json) {
      fprintf(stderr, "PopulateAnswer: bad params\n");
      return SE_ERROR;
   }

   if(!json_is_object(json)){
      fprintf(stderr, "error: root is not an object\n");
      json_decref(json);
      return SE_JSON_ERROR;
   }

   json_error_t jerror;
   if(json_unpack_ex(json, &jerror, 0,
            "{s:b s:i s:i s:i s?:i s:i s:s s:s}",
            "is_accepted",          &answer->isAccepted,
            "score",                &answer->score,
            "last_activity_date",   &answer->dateLastActivity,
            "creation_date",        &answer->dateCreation,
            "last_edit_date",       &answer->dateLastEdit,
            "answer_id",            &answer->answerId,
            "body",                 &answer->body,
            "body_markdown",        &answer->bodyMarkdown
            )) {
      fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
      return SE_JSON_ERROR;
   }
   return SE_OK;
}

/**
 * Assemble a single question from its JSON (including any answers)
 */
static SEError SEPopulateQuestion(SEQuestion* question, json_t* json) {
   if(!question || !json) {
      fprintf(stderr, "PopulateQuestion: bad params\n");
      return SE_ERROR;
   }

   if(!json_is_object(json)){
      fprintf(stderr, "error: root is not an object\n");
      json_decref(json);
      return SE_JSON_ERROR;
   }

   // take a deep copy of this JSON node and stash it
   json = json_deep_copy(json);
   question->jsonLocalRoot = json;

   json_error_t jerror;
   if(json_unpack_ex(json, &jerror, 0,
            "{s:i s:b s?:i s:i s:i s:i s:i s?:i s:s s:s s:s s:s}",
            "question_id",          &question->questionId,
            "is_answered",          &question->isAnswered,
            "accepted_answer_id",   &question->acceptedAnswerId,
            "answer_count",         &question->answerCount,
            "score",                &question->score,
            "last_activity_date",   &question->dateLastActivity,
            "creation_date",        &question->dateCreation,
            "last_edit_date",       &question->dateLastEdit,
            "link",                 &question->url,
            "title",                &question->title,
            "body",                 &question->body,
            "body_markdown",        &question->bodyMarkdown
            )) {
      fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
      return SE_JSON_ERROR;
   }

   // if it hasn't been answered, there will be no 'answers' object in the
   // JSON
   if(question->answerCount == 0) {
      question->answers = NULL;
      return SE_OK;
   }

   json_t* janswers = json_object_get(json, "answers");
   if(!janswers) {
      fprintf(stderr, "error: couldn't get question's answer items\n");
      json_decref(json);
      return SE_JSON_ERROR;
   }

   int numAnswers = json_array_size(janswers);
   question->answers = (SEAnswer*)malloc(numAnswers * sizeof(SEAnswer));

   json_t* jcur;
   for(int i = 0; i < numAnswers; i++) {
      jcur = json_array_get(janswers, i);

      if(SEPopulateAnswer(&(question->answers[i]), jcur) != SE_OK) {
         fprintf(stderr, "error: populating answer %d\n", i);
         json_decref(jcur);
         return SE_ERROR;
      }
   }

   // sort the answers based on rating
   qsort(question->answers, numAnswers, sizeof(SEAnswer), CompareSEAnswer);

   return SE_OK;
}

/**
 * Generic 'run a query and return the decoded JSON of the response'
 */
static SEError SEQueryAdvanced(json_t** json, SEStructuredQuery* query, SEQueryOptions* seqo) {
   CURLcode res;

   if(!query) {
      return SE_BAD_PARAM;
   }

   *json = NULL;

   FILE* cfile = tmpfile();
   if(!cfile) {
      perror("opening the temp file");
      return SE_ERROR;
   }

   CURL* curl = curl_easy_init();

   // follow redirects
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
   // stack exchange gives us gzipped content, so ask curl to gunzip for us
   curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
   // set the url
   curl_easy_setopt(curl, CURLOPT_URL, query->url);
   // ask curl to write to our file
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, cfile);

   // Perform the actual request, res will get the return code
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

   // we don't need this curl session anymore
   curl_easy_cleanup(curl);

   if(!*json) {
      fprintf(stderr, "error: on line %d: %s\n", jerr.line, jerr.text);
      return SE_JSON_ERROR;
   }

   return SE_OK;
}


SEError SEEasyFindQuestions(SEQuestion*** questions, int* numFoundQuestions, char* query) {
   return SEFindQuestions(questions, numFoundQuestions, query, NULL);
}

SEError SEFindQuestions(SEQuestion*** questions, int* numFoundQuestions, char* humanQueryString, SEQueryOptions* seqo) {
   SEError res;
   SEStructuredQuery stq;
   json_t* root = NULL;

   if(!humanQueryString) {
      return SE_BAD_PARAM;
   }

   if(numFoundQuestions) {
      *numFoundQuestions = -1;
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

   // TODO break the basic validation out?
   // do some json validation
   if(!json_is_object(root)) {
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

   int numQuestions = json_array_size(jquestions);
   // allocate an extra slot for the terminator
   *questions = (SEQuestion**)malloc(1 + numQuestions * sizeof(SEQuestion*));

   json_t* jcur;
   for(int i = 0; i < numQuestions; i++) {
      jcur = json_array_get(jquestions, i);

      (*questions)[i] = calloc(1, sizeof(SEQuestion));

      // fill in this question object (including all its answers)
      if(SEPopulateQuestion((*questions)[i], jcur) != SE_OK) {
         fprintf(stderr, "error: populating question %d\n", i);
         json_decref(root);

         return SE_ERROR;
      }
   }

   // null the terminating array member
   (*questions)[numQuestions] = NULL;

   // free the json
   json_decref(root);

   if(numFoundQuestions) {
      *numFoundQuestions = numQuestions;
   }

   return SE_OK;
}

void SEFreeQuestionWithAnswers(SEQuestion* question) {
   if(!question) {
      return;
   }

   // freeing this json node frees all strings for this question
   // and each of its answers
   json_decref(question->jsonLocalRoot);
}

void SEFreeQuestions(SEQuestion*** questions) {
   // ugh, this is ugly!
   if(!questions || !*questions || **questions) {
      // not sure if this is an error or not, so don't report it
      return;
   }

   SEQuestion** curq;
   for(curq = *questions; *curq; curq++) {
      // free the answers themselves
      free((*curq)->answers);

      // free the contents of the question object and all
      // strings in its answers
      SEFreeQuestionWithAnswers(*curq);
   }

   // free the questions pointer array itself
   free(*questions);
}

