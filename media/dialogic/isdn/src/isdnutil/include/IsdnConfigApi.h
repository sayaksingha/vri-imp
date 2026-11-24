// @(#) $Id: IsdnConfigApi.h,v 1.1.4.3 2016/04/12 21:59:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// IsdnConfigApi.h
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//
// Originated :   Chethan V M                     25/07/2013
//----------------------------------------------------------------------
#ifndef INCL_ISDN_CFG_DEF_H
#define INCL_ISDN_CFG_DEF_H

#include "IsdnUtil.h"


class IsdnConfigApi
{
   public:
      IsdnConfigApi();

      ~IsdnConfigApi();

      BOOLEAN Init();

      BOOLEAN ReloadConfig();

      key_t mShmChnlMgmtKey;

      BOOLEAN mHandleIsdnCall;

      BOOLEAN mSendRspFlag;

      BOOLEAN mHandleChnlMgmt;

      INT32   mOutgoingCallWaitTime;

   protected:
      
      BOOLEAN ReadConfig ();

};

#endif

