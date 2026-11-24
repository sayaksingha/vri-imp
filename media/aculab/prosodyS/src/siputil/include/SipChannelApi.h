// @(#) $Id: SipChannelApi.h,v 1.1.2.1 2016/07/05 16:57:47 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// SipChannelApi.h
//
// COPYRIGHT
//       Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//       HashTable introduced to store the handle and related info 
//----------------------------------------------------------------------
#ifndef INCL_SIP_CHANNEL_H
#define INCL_SIP_CHANNEL_H

//#define  SIP_MAX_HASH_TABLE_ENTRY 2500
#include "SipStructs.h"

typedef HashTable < TEXT, SIP_MAX_REQ_KEY_LEN + 1, SipChannel > ChannelHashTable;

class SipChannelApi
{
   public:
      SipChannelApi ();

      ~SipChannelApi ();

      BOOLEAN ReloadConfig ();

      BOOLEAN Init ();

      BOOLEAN GetReqMapDetails (TEXT *, SipChannel & );

      BOOLEAN InsertRequestIntoHash (SipChannel &);

      BOOLEAN DeleteRequestFromHash (TEXT *);

      BOOLEAN GetAllRequest (TEXT (*key)[SIP_MAX_REQ_KEY_LEN + 1], 
            SipChannel* lSipChannel, UINT32 &lcount);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN InitHashTable ();

      BOOLEAN ClearReqMapHashTable ();

    //  INT32    mReqShmId;

    //  UINT32   mReqShmKey;

      TEXT     *mpHashTablePtr;

      key_t    mSipChannelSemKey;

      INT32    mReqMapSemId;

      ChannelHashTable mReqMapDir;
};

#endif
