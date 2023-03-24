/******************************************************************************/
/*  AIU:   AI Unite API                                                       */
/*  Common Plugin X(interchange)                                              */
/******************************************************************************/

#ifndef AIUNITE_PROTOCOL_H
#define AIUNITE_PROTOCOL_H

#include <cstring>

#define AIU_STRINGIFY2(X) #X
#define AIU_STRINGIFY(X) AIU_STRINGIFY2(X)

#define AIUNITE_VERSION 1.0
#define AIUNITE_VERSION_STR "AIU/" AIU_STRINGIFY(AIUNITE_VERSION)

/* RESULT CODE */
#define AIU_SUCCESS 0
#define AIU_FAILURE 1
#define AIU_FAILURE_MASK -1

typedef int AIUResultCode;

typedef int AIUActionCode;

/* REQUEST CODES */
#define AIU_REQUEST_GET 0
#define AIU_REQUEST_GET_BEST 1
#define AIU_REQUEST_GET_NOW 2
#define AIU_REQUEST_PARTITION 8
#define AIU_REQUEST_TUNE 16
#define AIU_REQUEST_BAD -1

typedef int AIURequestCode;

#define AIU_REQUEST_TEST_STRING(VAL, CODE)                                     \
  if (!strcmp(VAL, #CODE))                                                     \
  return AIU_REQUEST_##CODE

AIURequestCode AIUGetRequestCode(const char *code) {
  if (code && *code) {
    AIU_REQUEST_TEST_STRING(code, GET);
    AIU_REQUEST_TEST_STRING(code, GET_NOW);
    AIU_REQUEST_TEST_STRING(code, PARTITION);
    AIU_REQUEST_TEST_STRING(code, TUNE);
  }
  return AIU_REQUEST_BAD;
}

#define AIU_REQUEST_TEST_CODE(CODE)                                            \
  case AIU_REQUEST_##CODE:                                                     \
    return #CODE

const char *AIUGetRequestString(AIURequestCode code) {
  switch (code) {
    AIU_REQUEST_TEST_CODE(GET);
    AIU_REQUEST_TEST_CODE(GET_NOW);
    AIU_REQUEST_TEST_CODE(PARTITION);
    AIU_REQUEST_TEST_CODE(TUNE);
  }
  return "BAD";
}

/* RESPONSE CODES */
#define AIU_RESPONSE_SUCCESS 0
#define AIU_RESPONSE_FAILURE 1

typedef int AIUResponseCode;

#endif /* AIUNITE_PROTOCOL_H */

