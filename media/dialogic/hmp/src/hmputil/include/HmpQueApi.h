// @(#) $Id: HmpQueApi.h,v 1.1.2.1 2012/02/21 08:31:32 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpQueApi.h
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
// AUTHOR         : Hmp                                  23-Nov-2011
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_HMP_QUE_API_DEF_H
#define INCL_HMP_QUE_API_DEF_H

#include "HmpStructs.h"
#include "HmpUtil.h"
#include "MediaInterface.h"

class HmpQueApi
{
   public:
      HmpQueApi ();

      ~HmpQueApi ();

      BOOLEAN Init ();

      BOOLEAN ReloadConfig ();

      BOOLEAN ReadFromMediaCmdReqQue (MpRequestDetails &, INT16 msgType = 0);

      BOOLEAN WriteToMediaCmdRspQue (MpResponseDetails &);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN CreateQueues ();

      key_t mMediaCmdReqQueKey;
      MsgQueue mMediaCmdReqQue;

      key_t mMediaCmdRspQueKey;
      MsgQueue mMediaCmdRspQue;
};
#endif
