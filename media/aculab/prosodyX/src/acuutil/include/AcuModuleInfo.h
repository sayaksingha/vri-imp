//@(#) $Id: AcuModuleInfo.h,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $
//---------------------------------------------------------------
// NAME
// fullpath/AcuModuleInfo.h
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// CLASS TYPE
// abstract or concrete
//
// DESCRIPTION
//
// Originated :
//----------------------------------------------------------------
#ifndef ACU_MODULE_INFO_H
#define ACU_MODULE_INFO_H

#define MAX_SEQUENCE_STRING 10

#include "AcuCardDetails.h"
#include "AcuMediaConstDef.h"
#include "AcuMediaUtil.h"

typedef HashTable< TEXT, MAX_SEQUENCE_STRING, ModuleInfo > ModuleDetails;

class AcuModuleInfo 
{
   public:

      AcuModuleInfo();

      ~AcuModuleInfo();

      BOOLEAN Init(UINT32 maxReferenceEntries);
      
      BOOLEAN ReadConfig();

      BOOLEAN DeInit();

      BOOLEAN InsertModuleDetails(ModuleInfo &);

      BOOLEAN GetModuleDetails(tSMModuleId lModuleId , ModuleInfo &);

      BOOLEAN DeleteModuleDetails(tSMModuleId lModuleId );

      BOOLEAN UpdateModuleDetails(tSMModuleId lModuleId, ModuleInfo &);

      BOOLEAN ClearModuleDetails();

      int GetTotalModuleInfo(tSMModuleId lModuleId[ACULAB_MAX_NUM_MODULES]);

   protected:
      key_t          mShmModuleDetailsKey;
      ModuleInfo*    mpHashMem;
      ModuleDetails  mModuleDir;

      int       mMaxSize;
};
#endif
