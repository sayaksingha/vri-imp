// @(#) $Id: BillingUtil.h,v 1.1.4.3 2016/04/12 21:59:00 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// BillingUtil.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2006
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated :                                              22-OCT-2010
//----------------------------------------------------------------------
#ifndef INCL_BILLING_UTIL_DEF_H
#define INCL_BILLING_UTIL_DEF_H

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) ((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )


#include "IvrConstDef.h"
//#include "IvrStructs.h"
#include "BillingApi.h"

class BillingUtil //:public IvrUtil
{
   public:

      static TEXT* ConvertBillingEventTypeToStr(const int);

      static TEXT* ConvertSubsTypeToStr(const int);

      static TEXT* ConvertBillingStatusToStr(const int);

      const char* GetVersion();
};

#endif
