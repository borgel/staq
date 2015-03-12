/**
 * TODO this comment block
 *
 * Basic Workflow:
 * 1) parse user params
 * 2) make a query to Stack Exchange to get questions and answers
 * 3) Build the GUI and display the results of the query
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "stackexchange.h"

#define USAGE  "Usage: staq [-h] <a query to search for>"

int main(int argc, char* argv[]) {
   SEError err;

   puts("Start");

   // run getopt until it finishes, then take whatever's left as
   // if its the user's query string and cat it together
   int c;
   while((c = getopt(argc, argv, "h")) != -1) {
      switch(c) {
         case 'h':
         default:
            printf(USAGE "\n");
            exit(0);
      }
   }

   // increment argc and argv to get query string
   argc -= optind;
   argv += optind;

   if(argc <= 0) {
      printf(USAGE "\n");
      exit(0);
   }

   // glom together everything left in argv to be the user's query string
   char* queryString = malloc(strlen(argv[1]));
   for(int i = 0; i < argc; i++) {
      queryString = realloc(queryString, strlen(queryString) + strlen(argv[i]) + 2);

      queryString = strcat(queryString, argv[i]);
      queryString = strcat(queryString, " ");
   }
   printf("total line [%s]\n", queryString);

   // setup our stack exchange lib
   SEInit();

   // run the query, as specifid by the user
   SEQuestion** questions = NULL;
   err = SEEasyFindQuestions(&questions, queryString);
   if(err != SE_OK) {
      //error, compare to SEError enum
      fprintf(stderr, "Query failed. Code %d\n", err);
      //exit(-1);
   }

   free(queryString);

   // FIXME rm bunch of random test code
   printf("M addr? %p\n", questions);
   printf("M* addr? %p\n", *questions);
   printf("M q0 id = %d\n", questions[0]->questionId);
   printf("M q0a0 id = %d\n", questions[0]->answers[0].answerId);
   puts("");
   printf("question 0's answers\n");
   SEAnswer* cura;
   for(int i = 0; i < questions[0]->answerCount; i++) {
      cura = &questions[0]->answers[i];
      printf("\tid %d\n", cura->answerId);
   }
   puts("");

   SEQuestion** curq;
   SEQuestion* q;
   for(curq = questions; *curq; curq++) {
      q = *curq;
      //printf("q %p\n", q);
      printf("Question %d\n", q->questionId);
      //printf("\t%s\n", q->body);
   }


   // TODO draw GUI and display results

   SEFreeQuestions(&questions);
   SECleanup();
   return 0;
}
