static const char *id="@(#) $Id: AcuCardDetails.cc,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $";
//--------------------------------------------------------------
// NAME
// AcuCardDetails.cc
//
// COPYRIGHT
// BITL -- Copyright (C) 2012 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// Originated : Roopa A M                   DATE:30 JUL 2012
//-------------------------------------------------------------
#include "AcuCardDetails.h"

//-------------------------------------------------------------
// METHOD : AcuCardDetails
// DESCRIPTION :
// PARAMETER :
// RETURN :
// THROWS :
//-------------------------------------------------------------
AcuCardDetails::AcuCardDetails()
{
   mpHashMem = NULL;
}

//-------------------------------------------------------------
// METHOD : ~AcuCardDetails
// DESCRIPTION :
// PARAMETER :
// RETURN :
// THROWS :
//-------------------------------------------------------------
AcuCardDetails::~AcuCardDetails()
{
}

//-------------------------------------------------------------
// METHOD : Init
// DESCRIPTION : Initializes the shared Memory
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuCardDetails::Init(UINT32 maxReferenceEntries)
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
      gLog.GenerateLog(ACU_API18, lLogText);
      TERR(gTrace, printf("%s:%s\n", gProcessName, lLogText););
      return false;
   }

   mMaxSize = maxReferenceEntries;

   mCardDir.SetBucketCount(mMaxSize);
   lHashTabSize =  mCardDir.GetHashTableSize();

   int lShmId = shmget (mShmCardDetailsKey, lHashTabSize, IPC_PERM);

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "ShmKey:%d ShmId:%d Size:%d",
         mShmCardDetailsKey, lShmId, lHashTabSize/1024);

   if (lShmId < 0)
   {
      T (gTrace, printf ("%s: %s CardDetails Shm does not Exist\n",
               gProcessName, lLogText););

      lShmId = shmget (mShmCardDetailsKey, lHashTabSize, IPC_CREAT | IPC_PERM);

      snprintf (lLogText, MAX_LOG_TEXT_LEN, "ShmKey:%d ShmId:%d",
            mShmCardDetailsKey, lShmId);

      if (lShmId < 0)
      {
         gLog.GenerateLog (ACU_API19, lLogText);
         TERR (gTrace, printf ("%s: %s Failed To Create CardDetails Shm\n",
                  gProcessName, lLogText););
         return false;
      }
      lShmCreated = true;
   }
   else
      T (gTrace, printf ("%s: %s CardDetails Shm Exist\n",
               gProcessName, lLogText););

   // Attach Shared Memory
   mpHashMem = (struct CardDetails *) shmat (lShmId, (struct CardDetails *) 0, 0);

   if (NULL == mpHashMem)
   {
      gLog.GenerateLog (ACU_API20, lLogText);
      TERR (gTrace, printf ("%s: %s Failed to attach to CardDetails Shm\n",
               gProcessName, lLogText););

      if (lShmCreated)
         shmctl (lShmId, IPC_RMID, NULL);

      return false;
   }
   T (gTrace, printf ("%s: %s Attached to CardDetails Shm\n",
            gProcessName, lLogText););

   if (SHT_SUCCESS !=
         mCardDir.SetHashMemoryPtr (mpHashMem, lShmCreated))
   {
      gLog.GenerateLog (ACU_API21, lLogText);
      TERR (gTrace,
            printf ("%s: %s Failed to Set CardDetails HashMemoryPtr\n",
               gProcessName, lLogText););

      if (lShmCreated)
         shmctl (lShmId, IPC_RMID, NULL);

      return false;
   }
   T (gTrace, printf ("%s: %s CardDetails Ht created Successfully\n",
            gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AcuCardDetails::ReadConfig ()
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

   //Service Code Details
   if (CFG_OK != lCfgRead.GetConfigNum ("SHM_ACU_CARD_DETAILS_CFG_KEY",
            mShmCardDetailsKey, ACULAB_MIN_SHM_KEY, ACULAB_MAX_SHM_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: SHM_ACU_CARD_DETAILS_CFG_KEY = %d\n",
            gProcessName, mShmCardDetailsKey););

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
BOOLEAN AcuCardDetails::DeInit()
{
   return true;
}

//-------------------------------------------------------------
// METHOD : InsertCardDetails
// DESCRIPTION : Inserts the entry into Shared Memory form given key
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuCardDetails::InsertCardDetails(CardDetails & lAcuCardDetail)
{
   TEXT lSeqNum[20]="";
   INT16  lError;

   sprintf(lSeqNum, "%lu", lAcuCardDetail.cardId);

   lError = mCardDir.Insert(lSeqNum, lAcuCardDetail);
   if(SHT_KEY_ALREADY_EXIST == lError)
   {
      if(SHT_SUCCESS != mCardDir.Replace(lSeqNum, lAcuCardDetail))
         return false;
   }
   T(gTrace, printf("%s: Sucess to Insert CardDetails CardId:%lu SerNum:%s\n", 
            gProcessName, lAcuCardDetail.cardId, lAcuCardDetail.cardSerialNum););
   gLog.GenerateLog(ACU_API22, 1);            
   return true;
}

//-------------------------------------------------------------
// METHOD :GetCardDetails
// DESCRIPTION : Fetches the entry from Shared memory for the given Key
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuCardDetails::GetCardDetails(UINT32 lCardId, CardDetails & lAcuCardDetail)
{
   TEXT  lSeqNum[20]="";

   memset(&lAcuCardDetail, 0, sizeof(lAcuCardDetail));

   sprintf(lSeqNum, "%lu", lCardId);
   if(mCardDir.Get(lSeqNum, lAcuCardDetail) != SHT_SUCCESS)
      return false;

   T(gTrace, printf("%s: Sucess to get CardDetails CardId:%lu SerNum:%s\n", 
            gProcessName, lAcuCardDetail.cardId, lAcuCardDetail.cardSerialNum););
   gLog.GenerateLog(ACU_API23, 2);            

   return true;
}

//-------------------------------------------------------------
// METHOD : DeleteCardDetails
// DESCRIPTION : Deletes the entry from Shared memory for the given key
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuCardDetails::DeleteCardDetails(UINT32 lCardId)
{
   TEXT lSequenceNo[10]="";

   sprintf(lSequenceNo, "%lu", lCardId);

   if(mCardDir.Delete(lSequenceNo) != SHT_SUCCESS)
      return false;

   T(gTrace, printf("%s:Success to Delete Card Details for CardId:%lu\n",
            gProcessName, lCardId););
   gLog.GenerateLog(ACU_API24, 3);            

   return true;
}

//-------------------------------------------------------------
// METHOD : UpdateCardDetails
// DESCRIPTION : Updates the particular field in Shared Memory for given key
// PARAMETER :
// RETURN : SUCCESS/FAILURE
// THROWS :
//-------------------------------------------------------------
BOOLEAN  AcuCardDetails::UpdateCardDetails(UINT32 lCardId, CardDetails & lAcuCardDetail)
{
   INT16      lRetKey = 0;
   TEXT       lLogText[MAX_LOG_TEXT_LEN];
   TEXT       lHshTabKey[10]="";

   sprintf(lHshTabKey,"%lu", lCardId);

   lRetKey = mCardDir.Replace(lHshTabKey, lAcuCardDetail);

   if(lRetKey == 1)
   {
      sprintf(lLogText,"CardId:%lu SerNum:%s", lAcuCardDetail.cardId, lAcuCardDetail.cardSerialNum);
      TERR(gTrace, printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACU_API25, lLogText);
      return true;
   }
   else
   {
      sprintf(lLogText,"Update Failed for the HashKey :%s", lHshTabKey);
      TERR(gTrace, printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACU_API26, lLogText);
      return false;
   }
}

//-------------------------------------------------------------
// METHOD : ClearCardDetails
// DESCRIPTION : Clears the Shared Memory
// PARAMETER :
// RETURN :
// THROWS :
//-------------------------------------------------------------
BOOLEAN AcuCardDetails::ClearCardDetails()
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (mCardDir.SetHashMemoryPtr ((void *) mpHashMem, true) != SHT_SUCCESS)
   {
      sprintf (lLogText, "Failed to set hash table memory pointer ");
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API27, lLogText);
      return false;
   }
   return true;
}

//-------------------------------------------------------------
// METHOD : GetTotalCardInfo 
// DESCRIPTION :Gets all the Card Details
// PARAMETER :
// RETURN :
// THROWS :
//-------------------------------------------------------------
int AcuCardDetails::GetTotalCardInfo(UINT32 lCardId[ACULAB_MAX_NUM_CARDS])
{
   TEXT                lKey[20][10];
   UINT32              lCount = 0;
   CardDetails         lAcuCardDetails[ACULAB_MAX_NUM_CARDS];
   INT16               lRetKey = 0;

   lRetKey = mCardDir.GetAll(lKey, lAcuCardDetails, lCount);

   if(lRetKey == 0 || lCount == 0)
      return 0;

   for(int i = 0; i < (int)lCount; i++)
      lCardId[i] = lAcuCardDetails[i].cardId;

   return lCount;
}
