/******************************************************************************/
/*  AIU:   AI Unite API                                                       */
/*  Common Plugin X(interchange)                                              */
/******************************************************************************/

#ifndef AIUNITE_PROTOCOL_H
#define AIUNITE_PROTOCOL_H

#define AIUNITE_VERSION 1.0
#define STRINGIFY(X) #X
#define AIUNITE_VERSION_STR "AIU/" STRINGIFY(AIUNITE_VERSION)

/* RESULT CODE */
#define AIU_SUCCESS    0
#define AIU_FAILURE    1

typedef int AIUResult;

/* ACTION CODE */
#define AIU_POST            8
#define AIU_QUERY_PERF      16
#define AIU_GET             32

typedef int AIUAction;

#endif /* AIUNITE_PROTOCOL_H */

