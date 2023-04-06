/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_INTERNAL_SUPPORT_H
#define AIUNITE_INTERNAL_SUPPORT_H

#define AIU_CHECK_OBJECT(X) if (X != nullptr) ; \
  else return AIU_INVALID_OBJECT
#define AIU_GET_OBJECT(X) AIU_CHECK_OBJECT(X ## _);    \
  auto X = X ## _->get()

#define AIU_CHECK_RESULT(X) if (X != nullptr) ; \
  else return AIU_INVALID_RESULT_PTR

#endif /* AIUNITE_INTERNAL_SUPPORT_H */

