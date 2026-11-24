// @(#) $Id: SipQueApi.h,v 1.1.4.1.2.1 2017/10/27 17:06:31 mram Exp $
//----------------------------------------------------------------------
// NAME
//       SipQueApi.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2015
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Sip                                  23-Feb-2015
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_SIP_QUE_API_DEF_H
#define INCL_SIP_QUE_API_DEF_H

#include "SipStructs.h"
#include "SipUtil.h"

class SipQueApi
{
   public:
      SipQueApi ();

      ~SipQueApi ();

      BOOLEAN Init ();

      BOOLEAN ReloadConfig ();

      BOOLEAN ReadFromSipReqQue (GenIsupMsg &, INT16 msgType = 0, modeOption mode=BLOCKING);

      BOOLEAN WriteToSipRspQue (GenIsupMsg &, TEXT*);

      BOOLEAN ReadFromMediaReqQue(MpRequestDetails&, INT16 msgType = 0, modeOption mode=BLOCKING );

      BOOLEAN WriteToMediaRspQue(MpResponseDetails&, INT16, TEXT*);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN CreateQueues ();

      key_t mSipReqQueKey;
      MsgQueue mSipReqQue;

      key_t mSipRspQueKey;
      MsgQueue mSipRspQue;

      key_t mMediaReqQueKey;
      MsgQueue mMediaReqQue;

      key_t mMediaRspQueKey;
      MsgQueue mMediaRspQue;
};
#endif
