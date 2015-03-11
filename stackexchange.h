/**
 * Provides hooks for working with the stack exchange APIs
 */

#ifndef __STACK_EXCHANGE_H__
#define __STACK_EXCHANGE_H__

// Types for passing options and config around
enum SESite {
   StackOverflow,
   ServerFault
};

typedef struct {
   enum SESite site;
} SEQueryOptions;

//FIXME this is crappy. find a better way
typedef enum {
   SE_OK          = 0,
   SE_ERROR       = -1,
   SE_BAD_PARAM   = -2,
   SE_CURL_ERROR  = -3,
   SE_JSON_ERROR  = -4
} SEError;

// Types for holding results

// a single answer
typedef struct {
   // TODO add owner struct/info?

   int isAccepted;

   int score;

   int dateLastActivity;
   int dateCreation;
   int dateLastEdit;

   int answerId;

   char* body;
   char* bodyMarkdown;

   // other stuff
} SEAnswer;

// a single question
typedef struct {
   // TODO add owner struct/info?

   // the stack exchange ID of this question
   int questionId;

   // is there an accepted answer?
   int isAnswered;
   int acceptedAnswerId;

   // the number of answers to this question
   int answerCount;

   int score;

   // date things
   int dateLastActivity;
   int dateCreation;
   int dateLastEdit;

   char* title;

   char* url;

   // versions of the body. These will be allocated and must be freed
   char* body;
   char* bodyMarkdown;

   // the array of answer objects for this question
   SEAnswer* answers;
} SEQuestion;

// FIXME do we need this?
// Root result type. NOT SPECIFICALLY questions and answers!
typedef struct {
   // TODO uhh, other metadata?
   int a;
} SEQueryResults;

// FIXME TODO take query params in here too I guess
typedef struct {
   // TODO needs a 'freed' flag?
   char* url;
} SEStructuredQuery;


/**
 * query for all questions with answers pertaining to the users query.
 * Takes:
 *    pointer to array of filled questions to receive
 *    pointer to error type (can be null)
 *    query string
 * Returns:
 *    >= 0: number of questions found
 *    < 0: Error (see error type)
 */
int SEEasyFindQuestions(SEQuestion** questions, char* query);
int SEFindQuestions(SEQuestion** questions, char* query, SEQueryOptions* seqo);

/**
 * Free an array of questions
 * FIXME change this API to consume the questions too? We are the ones
 * who allocate them after all
 */
void SEFreeQuestions(SEQuestion** questions);


// Functions for setup and teardown
// MUST be called before any other SE* calls are made
void SEInit();
void SECleanup();


#endif // __STACK_EXCHANGE_H__
