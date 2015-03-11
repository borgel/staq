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
   int res;

   puts("Start");

   //TODO parse input flags

   // setup our stack exchange lib
   SEInit();

   // run the query, as specifid by the user
   SEQuestion* questions = NULL;
   res = SEEasyFindQuestions(&questions, "c static type");
   if(res < 0) {
      //error, compare to SEError enum
      fprintf(stderr, "Query failed. Code %d\n", res);
      //exit(-1);
   }

   printf("M null? %p\n", questions);
   printf("M q0 id = %d\n", questions[0].questionId);
   printf("M q0a0 id = %d\n", questions[0].answers[0].answerId);

   // TODO write a function to get all answers for a question
   // Even better! the advanced search API will give you answers for free...

   //TODO for each question. print it, then print each answer

   // TODO draw GUI and display results

   //SEFreeQuestions(questions);
   //free(questions);
   //SECleanup();
   return 0;
}
