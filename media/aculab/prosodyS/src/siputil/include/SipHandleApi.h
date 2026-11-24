// @(#) $Id: SipHandleApi.h,v 1.1.4.1.2.2 2016/07/05 03:01:52 mram Exp $
//----------------------------------------------------------------------
// NAME
// SipHandleApi.h
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
#ifndef INCL_SIP_HANDLE_H
#define INCL_SIP_HANDLE_H

//#define  SIP_MAX_HASH_TABLE_ENTRY 2500
#include "SipStructs.h"

typedef HashTable < TEXT, SIP_MAX_REQ_KEY_LEN + 1, SipCallHandlers > ParamHashTable;

class SipHandleApi
{
   public:
      SipHandleApi ();

      ~SipHandleApi ();

      BOOLEAN ReloadConfig ();

      BOOLEAN Init ();

      BOOLEAN GetReqMapDetails (TEXT *, SipCallHandlers & );

      BOOLEAN InsertRequestIntoHash (SipCallHandlers &);

      BOOLEAN DeleteRequestFromHash (TEXT *);

      BOOLEAN GetAllRequest (TEXT (*key)[SIP_MAX_REQ_KEY_LEN + 1], 
            SipCallHandlers* lSipCallHandlers, UINT32 &lcount);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN InitHashTable ();

      BOOLEAN ClearReqMapHashTable ();

    //  INT32    mReqShmId;

    //  UINT32   mReqShmKey;

      TEXT     *mpHashTablePtr;

      key_t    mSipHandleSemKey;

      INT32    mReqMapSemId;

      ParamHashTable mReqMapDir;
};

#endif
