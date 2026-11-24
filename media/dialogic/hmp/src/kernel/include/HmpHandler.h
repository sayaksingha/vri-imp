// @(#) $Id: HmpHandler.h,v 1.1.2.1 2012/02/21 08:31:36 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpHandler.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Ramprasad M                              12-JAN-2012
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef HmpHandler_H
#define HmpHandler_H

#include "HmpQueApi.h"
#include "HmpConfigApi.h"
#include "HmpDbMgr.h"
#include "HmpDevice.h"

extern HmpDbMgr gDbObj;

class HmpHandler
{
   public:

      HmpHandler();

      ~HmpHandler();

      BOOLEAN  Init();

      BOOLEAN  DeInit();
      
      BOOLEAN  ReloadConfig();

      BOOLEAN ReadRequest();

      BOOLEAN ProcessRequest();

   public:

      HmpQueApi         mHmpQueApi;

      INT16    mMinCktId;    
      
      INT16    mMaxCktId;    

      HmpDevice     mHmpDevice;

      MpResponseDetails mMpResponse;

   private:
      
      BOOLEAN     StartSrlThread();

      BOOLEAN  InitRtspSession();

      BOOLEAN  DeInitRtspSession();

   private:
      
      HmpConfigApi      mHmpConfigApi;

      MpRequestDetails  mMpRequest;


};

#endif
