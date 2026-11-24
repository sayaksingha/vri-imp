// @(#) $Id: IvrEvrApi.h,v 1.1.4.3 2016/04/12 21:59:16 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// IvrEvrApi.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : Roopa A M                                    14-MAR-2012
//----------------------------------------------------------------------
#ifndef INCL_IVR_EVR_API_DEF_H
#define INCL_IVR_EVR_API_DEF_H

#include "IvrConstDef.h"
#include "IvrStructs.h"
#include "IvrUtil.h"

#define IVR_GEN_EVR_DB        1
#define IVR_GEN_EVR_FILE      4

#define IVR_GEN_CDR_DB        1
#define IVR_GEN_CDR_FILE      4

#define IVR_MAX_EVR_EVENT     255
#define IVR_MAX_CDR_EVENT     255

class IvrEvrApi
{
   public:
      IvrEvrApi ();

      BOOLEAN Init ();

      BOOLEAN ReadEvrMessage (IvrEvrMsg & evrMsg);

      BOOLEAN GenerateEvr (IvrEvrMsg & evrMsg);

      BOOLEAN ReadCdrMessage (IvrCdrMsg & cdrMsg);

      BOOLEAN GenerateCdr (IvrCdrMsg & cdrMsg);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN CreateMsgQueue ();

      key_t mEvrHdlrQueKey;
      key_t mCdrHdlrQueKey;

      MsgQueue mEvrHdlrQue;
      MsgQueue mCdrHdlrQue;
};

#endif
