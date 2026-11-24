static const char *id="@(#) $Id: AcuModuleInfo.cc,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $";
//--------------------------------------------------------------
// NAME
// fullpath/AcuModuleInfo.cc
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// Originated : Roopa A M                   DATE:30 JUL 2012
//-------------------------------------------------------------
#include "AcuModuleInfo.h"

//-------------------------------------------------------------
// METHOD : AcuModuleInfo
// DESCRIPTION :
// PARAMETER :
// RETURN :
// THROWS :
//-------------------------------------------------------------
AcuModuleInfo::AcuModuleInfo()
{
   mpHashMem = NULL;
}

//-------------------------------------------------------------
// METHOD : ~AcuModuleInfo
// DESCRIPTION :
// PARAMETER :
// RETURN :
// THROWS :
//-------------------------------------------------------------
AcuModuleInfo::~AcuModuleInfo()
{
}

//-------------------------------------------------------------
// METHOD : Init
// DESCRIPTION : Initializes the shared Memory
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuModuleInfo::Init(UINT32 maxReferenceEntries)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BOOLEAN lShmCreated = false;
   size_t lHashTabSize;

   if (true != this->ReadConfig ())
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "ReadConfig Failed. Exiting...");

      gLog.GenerateLog (GSYS04, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););

      return false;
   }
   T (gTrace, printf ("%s:\n", gProcessName););

   if(maxReferenceEntries <= 0)
   {
      sprintf(lLogText, "Wrong MaxReference Entry");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API06, lLogText);
      return false;
   }

   mMaxSize = maxReferenceEntries;

   mModuleDir.SetBucketCount(mMaxSize);
   lHashTabSize =  mModuleDir.GetHashTableSize();

   int lShmId = shmget (mShmModuleDetailsKey, lHashTabSize, IPC_PERM);

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "ShmKey:%d ShmId:%d Size:%d",
         mShmModuleDetailsKey, lShmId, lHashTabSize/1024);

   if (lShmId < 0)
   {
      T (gTrace, printf ("%s: %s ModuleDetails Shm does not Exist\n",
               gProcessName, lLogText););

      lShmId = shmget (mShmModuleDetailsKey, lHashTabSize, IPC_CREAT | IPC_PERM);

      snprintf (lLogText, MAX_LOG_TEXT_LEN, "ShmKey:%d ShmId:%d",
            mShmModuleDetailsKey, lShmId);

      if (lShmId < 0)
      {
         gLog.GenerateLog (ACU_API07, lLogText);
         TERR (gTrace, printf ("%s: %s Failed To Create ModuleDetails Shm\n",
                  gProcessName, lLogText););
         return false;
      }
      lShmCreated = true;
   }
   else
      T (gTrace, printf ("%s: %s ModuleDetails Shm Exist\n",
               gProcessName, lLogText););

   // Attach Shared Memory
   mpHashMem = (struct ModuleInfo *) shmat (lShmId,
         (struct ModuleInfo *) 0, 0);

   if (NULL == mpHashMem)
   {
      gLog.GenerateLog (ACU_API08, lLogText);
      TERR (gTrace, printf ("%s: %s Failed to attach to ModuleDetails Shm\n",
               gProcessName, lLogText););

      if (lShmCreated)
         shmctl (lShmId, IPC_RMID, NULL);

      return false;
   }
   T (gTrace, printf ("%s: %s Attached to ModuleDetails Shm\n",
            gProcessName, lLogText););

   if (SHT_SUCCESS !=
         mModuleDir.SetHashMemoryPtr (mpHashMem, lShmCreated))
   {
      gLog.GenerateLog (ACU_API09, lLogText);
      TERR (gTrace,
            printf ("%s: %s Failed to Set ModuleDetails HashMemoryPtr\n",
               gProcessName, lLogText););

      if (lShmCreated)
         shmctl (lShmId, IPC_RMID, NULL);

      return false;
   }
   T (gTrace, printf ("%s: %s ModuleDetails Ht created Successfully\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AcuModuleInfo::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   //============================ ipc.cfg ==============================
   if (true != lCfgRead.CfgInit (ACULAB_IPC_CFG))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s",
            ACULAB_IPC_CFG);

      gLog.GenerateLog (GSYS09, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););

      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("SHM_ACU_MODULE_DETAILS_CFG_KEY",
            mShmModuleDetailsKey, ACULAB_MIN_SHM_KEY, ACULAB_MAX_SHM_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: SHM_ACU_MODULE_DETAILS_CFG_KEY = %d\n",
            gProcessName, mShmModuleDetailsKey););

   lCfgRead.CfgDeInit ();

   return true;
}

//-------------------------------------------------------------
// METHOD : DeInit
// DESCRIPTION :
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuModuleInfo::DeInit()
{
   return true;
}

//-------------------------------------------------------------
// METHOD : InsertModuleDetails
// DESCRIPTION : Inserts the entry into Shared Memory form given key
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuModuleInfo::InsertModuleDetails(ModuleInfo & lAcuModuleInfo)
{
   INT16  lError;
   TEXT   lSeqNum[10] = "";
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   sprintf(lSeqNum, "%ld", lAcuModuleInfo.moduleId);

   lError = mModuleDir.Insert(lSeqNum, lAcuModuleInfo);
   if(SHT_KEY_ALREADY_EXIST == lError)
   {
      if(SHT_SUCCESS != mModuleDir.Replace(lSeqNum, lAcuModuleInfo))
         return false;
   }

   if(lError != SHT_SUCCESS)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Failed to Insert ModuleInfo ModuleIdx: %d", 
            lAcuModuleInfo.moduleIdx);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API10, lLogText);               
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "Success to Insert ModuleDetails ModuleId:%ld ModuleIdx:%d",
         lAcuModuleInfo.moduleId, lAcuModuleInfo.moduleIdx);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_API11, lLogText);

   return true;
}

//-------------------------------------------------------------
// METHOD :GetModuleDetails
// DESCRIPTION : Fetches the entry from Shared memory for the given Key
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuModuleInfo::GetModuleDetails(tSMModuleId lModuleId, ModuleInfo & lAcuModuleDetail)
{
   TEXT  lSequenceNum[10]="";
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   sprintf(lSequenceNum, "%ld", lModuleId);

   if(mModuleDir.Get(lSequenceNum, lAcuModuleDetail) != SHT_SUCCESS)
      return false;
   
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "Success to get ModuleDetails ModuleId:%ld ModuleIdx:%d",
         lAcuModuleDetail.moduleId, lAcuModuleDetail.moduleIdx);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_API12, lLogText);

   return true;
}

//-------------------------------------------------------------
// METHOD : DeleteModuleDetails
// DESCRIPTION : Deletes the entry from Shared memory for the given key
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuModuleInfo::DeleteModuleDetails(tSMModuleId lModuleId)
{
   TEXT lSequenceNo[10]="";
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   sprintf(lSequenceNo, "%ld", lModuleId);

   if(mModuleDir.Delete(lSequenceNo) != SHT_SUCCESS)
      return false;

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "Success to Delete Module Details for ModuleId:%ld", lModuleId);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_API13, lLogText);

   return true;
}

//-------------------------------------------------------------
// METHOD : UpdateModuleDetails
// DESCRIPTION : Updates the particular field in Shared Memory for given key
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN  AcuModuleInfo::UpdateModuleDetails(tSMModuleId lModuleId, ModuleInfo & lAcuModuleDetail)
{
   INT16      lRetKey = 0;
   TEXT       lLogText[MAX_LOG_TEXT_LEN];
   TEXT       lHshTabKey[10]="";

   sprintf(lHshTabKey, "%ld", lModuleId);

   lRetKey = mModuleDir.Replace(lHshTabKey, lAcuModuleDetail);

   if(lRetKey == 1)
   {
      sprintf(lLogText,"ModuleId:%ld ModuleIdx:%d", 
            lAcuModuleDetail.moduleId, lAcuModuleDetail.moduleIdx);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API14, lLogText);
      return true;
   }
   else
   {
      sprintf(lLogText,"Update Failed for the HashKey :%s", lHshTabKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API15, lLogText);
      return false;
   }
}

//-------------------------------------------------------------
// METHOD : ClearModuleDetails
// DESCRIPTION : Clears the Shared Memory
// PARAMETER :
// RETURN :
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuModuleInfo::ClearModuleDetails()
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (mModuleDir.SetHashMemoryPtr ((void *) mpHashMem,true) != SHT_SUCCESS)
   {
      sprintf (lLogText, "Failed to set hash table memory pointer ");
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API16, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// METHOD : GetTotalModuleInfo 
// DESCRIPTION :Gets all the Module Details
// PARAMETER :
// RETURN :
// THROWS :
//-------------------------------------------------------------
int AcuModuleInfo::GetTotalModuleInfo(tSMModuleId lModuleId[ACULAB_MAX_NUM_MODULES])
{
   TEXT                lKey[20][10];
   UINT32              lCount = 0;
   ModuleInfo          lAcuModuleDetails[ACULAB_MAX_NUM_MODULES];
   INT16               lRetKey = 0;

   lRetKey = mModuleDir.GetAll(lKey, lAcuModuleDetails, lCount);

   if(lRetKey == 0 || lCount == 0)
      return 0;

   for(int i = 0; i < (int)lCount; i++)
      lModuleId[i] = lAcuModuleDetails[i].moduleId;

   return lCount;
}
