/******************************************************************************/
/*  AIU:   AI Unite API                                                       */
/*  Common Plugin X(interchange)                                              */
/******************************************************************************/

#ifndef AIUNITE_PROTOCOL_H
#define AIUNITE_PROTOCOL_H

#include <cstring>

#ifndef NDEBUG
#include <iostream>
#endif

#define AIU_STRINGIFY2(X) #X
#define AIU_STRINGIFY(X) AIU_STRINGIFY2(X)

#define AIUNITE_VERSION 1.0
#define AIUNITE_VERSION_STR "AIU/" AIU_STRINGIFY(AIUNITE_VERSION)

/* RESULT CODE */
#define AIU_SUCCESS 0
#define AIU_FAILURE 1
#define AIU_INVALID_OBJECT 2
#define AIU_INVALID_RESULT_PTR 3
#define AIU_FAILURE_MASK -1

extern "C" int returnResult(const char *resultCode, const char *file_name,
                            int line_num);

#define AIU_CHECK_SUCCESS(X)                                                   \
  if (X == AIU_SUCCESS)                                                        \
    ;                                                                          \
  else                                                                         \
    return returnResult(#X, __FILE__, __LINE__)

typedef int AIUResultCode;

typedef int AIUActionCode;

/* REQUEST CODES */

enum AIURequestCode {
  AIU_REQUEST_GET,
  AIU_REQUEST_GET_BEST,
  AIU_REQUEST_GET_NOW,
  AIU_REQUEST_PARTITION,
  AIU_REQUEST_TUNE,
  AIU_REQUEST_BAD
};

AIURequestCode AIUGetRequestCode(const char *code);

const char *AIUGetRequestString(AIURequestCode code);

/* RESPONSE CODES */
#define AIU_RESPONSE_SUCCESS 0
#define AIU_RESPONSE_FAILURE 1

typedef int AIUResponseCode;

#endif /* AIUNITE_PROTOCOL_H */
