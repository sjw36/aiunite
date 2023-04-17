/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <aiunite/common/protocol.h>

extern "C" int returnResult(const char *resultCode, const char *file_name,
                            int line_num) {
#ifndef NDEBUG
  std::cout << "error(" << file_name << "[" << line_num << "]): " << resultCode
            << std::endl;
#endif
  return -1;
}

#define AIU_REQUEST_TEST_STRING(VAL, CODE)                                     \
  if (!strcmp(VAL, #CODE))                                                     \
  return AIU_REQUEST_##CODE

AIURequestCode AIUGetRequestCode(const char *code) {
  if (code && *code) {
    AIU_REQUEST_TEST_STRING(code, LOOKUP);
    AIU_REQUEST_TEST_STRING(code, GET);
    AIU_REQUEST_TEST_STRING(code, GET_BEST);
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
    AIU_REQUEST_TEST_CODE(LOOKUP);
    AIU_REQUEST_TEST_CODE(GET);
    AIU_REQUEST_TEST_CODE(GET_BEST);
    AIU_REQUEST_TEST_CODE(PARTITION);
    AIU_REQUEST_TEST_CODE(TUNE);
  }
  return "BAD";
}
