/**
 * Encapsulates the query-building logic for stack exchange sites
 */

#ifndef __SE_QUERY_BUILDER_H__
#define __SE_QUERY_BUILDER_H__

#include "stackexchange.h"

typedef enum {
   SESearch
} SEQueryType;

SEError SEBuildSearchQuery(SEStructuredQuery* stq, char* humanString);

/**
 * Cleanup function to free allocated MEMBERS.
 */
void SEFreeQuery(SEStructuredQuery* stq);

// TODO generalize a version of the above

#endif //__SE_QUERY_BUILDER_H__
