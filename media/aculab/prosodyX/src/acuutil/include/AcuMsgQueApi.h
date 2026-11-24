// @(#) $Id: AcuMsgQueApi.h,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// AcuMsgQueApi.h
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
// Originated : Roopa A M                                    30-JUL-2012
//----------------------------------------------------------------------
#ifndef INCL_ACU_MSG_QUE_API_DEF_H
#define INCL_ACU_MSG_QUE_API_DEF_H

#include "MediaInterface.h"
#include "AcuMediaUtil.h"

class AcuMsgQueApi
{
   public:
      AcuMsgQueApi ();

      ~AcuMsgQueApi ();

      BOOLEAN Init ();

      BOOLEAN ReloadConfig ();
     
      //----------------------------------------------------------------
      BOOLEAN ReadFromReqQue (MpRequestDetails &, INT16);

      BOOLEAN WriteToRspQue (MpResponseDetails &, INT16);

      BOOLEAN ReadFromModuleReqQue (MpRequestDetails &, INT16);

      BOOLEAN WriteToModuleReqQue (MpRequestDetails &, INT16);

   private:
      BOOLEAN ReadConfig ();
      
      BOOLEAN CreateQueues ();

      key_t mMpReqQueKey;
      MsgQueue mMpReqQue;
      
      key_t mMpModReqQueKey;
      MsgQueue mMpModReqQue;
      
      key_t mMpRspQueKey;
      MsgQueue mMpRspQue;
};

#endif
