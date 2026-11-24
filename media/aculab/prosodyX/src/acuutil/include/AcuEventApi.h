// @(#) $Id: AcuEventApi.h,v 1.1.4.3 2016/04/12 21:59:26 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// AcuEventApi.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2012
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : Roopa A M                                    31-JUL-2012
//----------------------------------------------------------------------
#ifndef INCL_ACU_EVENT_API_DEF_H
#define INCL_ACU_EVENT_API_DEF_H

#include "MediaInterface.h"
#include "AcuMediaUtil.h"

class AcuEventApi
{
   public:
      AcuEventApi ();

      ~AcuEventApi ();

      //----------------------------------------------------
      BOOLEAN Init ();

      BOOLEAN ReloadConfig ();
     
      //----------------------------------------------------
      BOOLEAN CreateProsodyEvents ();

      //----------------------------------------------------
      tSMEventId  mPlayEvent;
      tSMEventId  mRecEvent;
      tSMEventId  mDtmfEvent;

   private:
      //----------------------------------------------------
      BOOLEAN ReadConfig ();
};

#endif
