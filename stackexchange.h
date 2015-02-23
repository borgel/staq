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

typedef enum {
   SE_OK = 0,
   SE_BAD_PARAM,
   SE_CURL_ERROR,
   SE_JSON_ERROR,
   SE_ERROR
} SEError;

// Types for holding results

// a single answer
typedef struct {
   int datePublished;
   int dateEdited;

   char* body;

   // other stuff
} SEAnswer;

// a single question
typedef struct {
   // the stack exchange ID of this question
   long questionId;

   // is there an accepted answer?
   int isAnswered;
   int acceptedAnswerId;

   // the number of answers to this question
   int answerCount;

   // date things
   long lastActivity;
   long creationDate;

   char* title;

   char* url;

   // versions of the body. These will be allocated and must be freed
   char* body;
   char* bodyMarkdown;

   // the array of answer objects
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
 * query for all questions pertaining to the users query
 */
SEError SEEasyFindQuestions(SEQuestion* questions, char* query);
SEError SEFindQuestions(SEQuestion* questions, char* query, SEQueryOptions* seqo);

/**
 * Free an array of questions
 * FIXME change this API to consume the questions too? We are the ones
 * who allocate them after all
 */
void SEFreeQuestions(SEQuestion* questions);


// Functions for setup and teardown
// MUST be called before any other SE* calls are made
void SEInit();
void SECleanup();


#endif // __STACK_EXCHANGE_H__
