/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_TYPES_H
#define AIUNITE_TYPES_H

#define AIU_DECLARE_TYPE(X)                                                    \
  struct _AIU##X;                                                              \
  typedef _AIU##X *AIU##X

/******************************************************************************/
/*  AIUNITE TYPES                                                             */
/******************************************************************************/

AIU_DECLARE_TYPE(Device);
AIU_DECLARE_TYPE(DeviceRequest);

AIU_DECLARE_TYPE(Model);
AIU_DECLARE_TYPE(Type);
AIU_DECLARE_TYPE(Value);
AIU_DECLARE_TYPE(Attr);
AIU_DECLARE_TYPE(Operation);

AIU_DECLARE_TYPE(Request);

AIU_DECLARE_TYPE(Solution);

AIU_DECLARE_TYPE(Binary);

#endif /* AIUNITE_TYPES_H */
