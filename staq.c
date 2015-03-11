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

// TODO usage msg

int main(int argc, char* argv[]) {
   int res;

   puts("Start");

   // run getopt until it finishes, then take whatever's left as
   // if its the user's query string and cat it together
   int c;
   while((c = getopt(argc, argv, "h")) != -1) {
      switch(c) {
         case 'h':
         default:
            printf("help msg\n");
            exit(0);
      }
   }

   // increment argc and argv to get query string
   argc -= optind;
   argv += optind;

   if(argc <= 0) {
      printf("help msg\n");
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
   SEQuestion* questions = NULL;
   res = SEEasyFindQuestions(&questions, queryString);
   if(res < 0) {
      //error, compare to SEError enum
      fprintf(stderr, "Query failed. Code %d\n", res);
      //exit(-1);
   }

   free(queryString);

   printf("M null? %p\n", questions);
   printf("M q0 id = %d\n", questions[0].questionId);
   printf("M q0a0 id = %d\n", questions[0].answers[0].answerId);

   // TODO write a function to get all answers for a question
   // Even better! the advanced search API will give you answers for free...

   //TODO for each question. print it, then print each answer

   // TODO draw GUI and display results

   SEFreeQuestions(&questions);
   //SECleanup();
   return 0;
}
