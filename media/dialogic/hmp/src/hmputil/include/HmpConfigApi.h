// @(#) $Id: HmpConfigApi.h,v 1.1.2.1 2012/02/21 08:31:32 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// HmpConfigApi.h
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
// Originated :                                              17-JAN-2012
//----------------------------------------------------------------------
#ifndef INCL_HMP_CFG_DEF_H
#define INCL_HMP_CFG_DEF_H

#include "HmpUtil.h"
#include "HmpDbMgr.h"


class HmpConfigApi
{
   public:
      HmpConfigApi();

      ~HmpConfigApi();

      BOOLEAN Init();

      BOOLEAN ReloadConfig();

      INT16 mHmpDb;
      INT16 mIpmDeviceCount;
      TEXT  mRtspUrls[HMP_MAX_IPM_RESOURSE][HMP_MAX_IPM_RESOURSE];

   protected:
      
      HmpDbMgr    mDbObj;

      BOOLEAN ReadConfig ();
      BOOLEAN ReadConfigFromDb();

};

#endif

