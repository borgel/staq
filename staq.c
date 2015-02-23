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

#include "stackexchange.h"

int main(int argc, char* argv[]) {
   SEError err;

   puts("asdaa");

   //TODO parse input flags

   // setup our stack exchange lib
   SEInit();

   // run the query, as specifid by the user
   SEQuestion* questions = NULL;
   err = SEEasyFindQuestions(questions, "c static type");
   if(err != SE_OK) {
      fprintf(stderr, "Query failed. Code %d\n", err);
      exit(-1);
   }

   // TODO write a function to get all answers for a question


   //TODO for each question. print it, then print each answer

   // TODO draw GUI and display results

   SEFreeQuestions(questions);
   free(questions);
   SECleanup();
   return 0;
}
