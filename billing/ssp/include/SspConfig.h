//----------------------------------------------------------------------
// NAME
// SspCfg.h
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
// Originated : KOTHARI KOMAL                           Date:22-FEB-2012
//----------------------------------------------------------------------
#ifndef IN_SSP_CFG_H
#define IN_SSP_CFG_H
#include "SspConstDef.h"
#include "SspUtil.h"

class SspCfg
{
   public:
   TEXT           mServNodeAdrr[IN_RLY_MAX_MSISDN_LEN +1];

   UINT16         mServNodePt;
   
   UINT16         mDstNodePt;
   
   INT16          mNoOfScfAddr;

   GsmscfInfo     mScfInfo[IN_RLY_MAX_NO_SCF_ADDR +1];

   INT32          mDlgDetKey;

   INT32          mFreeDlgPoolKey;

   INT32          mFreeDlgSemKey;
   
   TEXT           mProductHomePath[COAM_PATH_SIZE + 1];

// ------------- Public Methods ----------
   SspCfg();

   ~SspCfg();

   BOOLEAN Init ();
   
   BOOLEAN ReadloadConfig ();
   
   BOOLEAN FindScfAddr (TEXT*, TEXT*);

private:

   BOOLEAN ReadConfig ();

   void MyCpy(TEXT*, TEXT*);

   void SetFilePaths();
};
#endif
