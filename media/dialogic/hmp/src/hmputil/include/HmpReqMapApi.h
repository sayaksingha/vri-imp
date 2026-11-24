// @(#) $Id: HmpReqMapApi.h,v 1.1.2.1 2012/02/21 08:31:33 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// HmpReqMapApi.h
//
// COPYRIGHT
//       Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//       HashTable introduced in Diameter protocol to achieve parallel
// processing. Wherein we will be storing the cgw request.
//----------------------------------------------------------------------
#ifndef INCL_HMP_REQ_MAP_H
#define INCL_HMP_REQ_MAP_H

#include "HmpUtil.h"

typedef HashTable < TEXT, HMP_MAX_REQ_KEY_LEN + 1, HmpReqMap > ParamHashTable;

class HmpReqMapApi
{
   public:
      HmpReqMapApi ();

      ~HmpReqMapApi ();

      BOOLEAN ReloadConfig ();

      BOOLEAN Init ();

      BOOLEAN GetReqMapDetails (TEXT *, HmpReqMap & );

      BOOLEAN InsertRequestIntoHash (HmpReqMap &);

      BOOLEAN UpdateRequestIntoHash (HmpReqMap &);

      BOOLEAN DeleteRequestFromHash (TEXT *);

      BOOLEAN GetAllRequest (TEXT (*key)[HMP_MAX_REQ_KEY_LEN + 1], 
            HmpReqMap* lHmpReqMap, UINT32 &lcount);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN InitHashTable ();

      BOOLEAN ClearReqMapHashTable ();

      INT32    mReqShmId;

      UINT32   mReqShmKey;

      TEXT     *mpHashTablePtr;

      key_t    mReqMapSemKey;

      INT32    mReqMapSemId;

      ParamHashTable mReqMapDir;
};

#endif
