// @(#) $Id: IsdnQueApi.h,v 1.1.4.3 2016/04/12 21:59:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME IsdnQueApi.h
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
// AUTHOR : Chethan V M                        Date : 14/08/2013
//----------------------------------------------------------------------
#ifndef INCL_ISDN_QUE_API_DEF_H
#define INCL_ISDN_QUE_API_DEF_H

#include "IsdnStructs.h"
#include "IsdnUtil.h"
#include "MediaInterface.h"

class IsdnQueApi
{
   public:
      IsdnQueApi ();

      ~IsdnQueApi ();

      BOOLEAN Init ();

      BOOLEAN ReloadConfig ();

      BOOLEAN ReadFromMediaCmdReqQue (MpRequestDetails &, INT16 msgType = 0);

      BOOLEAN ReadFromIsupReqQue (GenIsupMsg &, INT16 msgType = 0);

      BOOLEAN ReadFromChnlMgmtReqQue (CktCmdReq &, INT16 msgType = 0);

      BOOLEAN WriteToMediaCmdRspQue (MpResponseDetails &);

      BOOLEAN WriteToIsupRspQue(GenIsupMsg &);

      BOOLEAN WriteToChnlMgmtRspQue (CktCmdRsp &);

      long SetChannelId(EnumMediaCmdTypes, unsigned int &, MpData &);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN CreateQueues ();

      key_t mMediaCmdReqQueKey;
      MsgQueue mMediaCmdReqQue;

      key_t mMediaCmdRspQueKey;
      MsgQueue mMediaCmdRspQue;

      key_t mIsupReqQueKey;
      MsgQueue mIsupReqQue;

      key_t mIsupRspQueKey;
      MsgQueue mIsupRspQue;

      key_t mChnlMgmtReqQueKey;
      MsgQueue mChnlMgmtReqQue;

      key_t mChnlMgmtRspQueKey;
      MsgQueue mChnlMgmtRspQue;
};
#endif
