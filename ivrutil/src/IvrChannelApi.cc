static const char* id="@(#) $Id: IvrChannelApi.cc,v 1.1.1.1.2.6.2.3 2017/06/03 02:43:17 mram Exp $";
//----------------------------------------------------------------------
// NAME
// IvrChannelApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2008
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION:
// This library used for maintaining Channel status

// Originated: KOTHARI KOMAL                          Date: 07-FEB-2011
//----------------------------------------------------------------------

#include "IvrChannelApi.h"

extern TEXT gProcessName[];
extern CTrace gTrace;
extern Log    gLog;
extern IvrQueApi gIvrQueApi;
extern unsigned int       gReferenceNum;

UINT32 IvrChannelApi::mRefNum = 0;
int    gDbClient;
//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION : Initialize the Channel Status API
// PARAMETERS  : MinChannelId, MaxChannelId
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrChannelApi::Init(BOOLEAN reset, UINT32 minChannelId, UINT32 maxChannelId)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN+1];
	mInit = false;

	mRefNum = 0;

	//mMinChannelId = minChannelId;
	//mMaxChannelId = maxChannelId;

   //T(gTrace, printf("%s: MinChannel: %ld, MaxChannel: %ld\n", gProcessName, minChannelId, maxChannelId););

   if( false == this->ReadConfig())
   {
                TERR(gTrace, printf("%s: ReadConfig Failed.\n", gProcessName););
   }
   if (DB_NO_ERR !=  mDbObj.Init(gDbClient))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Ivr DB Object Init failed. Exiting...");
      gLog.GenerateLog (GSYS04, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   
   if(DB_NO_ERR != mDbObj.GetTotalIvrChannels(mIvrTotalCkts))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Could not read Total channel Count from DB");

      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS11, lLogText);

      return false;
   }
   else
   {
      mMaxChannelId = mIvrTotalCkts;
      T (gTrace, printf ("%s: TOTAL_IVR_CHANNELS_CONFIGURED = %d\n", gProcessName,
               mIvrTotalCkts););
      mMinChannelId = 0;

      if(false == this->CreateShm())
      {
	      TERR(gTrace, printf("%s: CreateShm Failed.\n", gProcessName););
      }
      else
	      mInit = true;
   }

   if(DB_NO_ERR != mDbObj.ReadChannelConfig(mChannelConfig,IVR_CHANNEL_DIR_UNKNOWN))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Could not load Channel Configuration from DB");

      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS11, lLogText);

      return false;
   }

   if(mInit && reset)
   {
      T(gTrace, printf("%s: ----------------- IVR CHANNEL CONFIGURATION ------------------ \n", gProcessName);); 
      for (INT16 i=mMinChannelId; i<=mMaxChannelId; i++)
      {
         T(gTrace, printf("%s: CID: %05d, DIR: %02d, STATUS: %02d, AppId:%d AppName:%s\n", gProcessName, 
                  i, mChannelConfig.details[i].dir, mChannelConfig.details[i].status,
                  mChannelConfig.details[i].appId,
                  mChannelConfig.details[i].appName););
         this->SetChannelStatus(i, IVR_CHANNEL_UNINITIALIZED);
         if(IVR_CHANNEL_DIR_IN == mChannelConfig.details[i].dir)
            //this->SetChannelDetails(i, mChannelConfig.details[i].dir, NULL, mChannelConfig.details[i].appId );
            this->SetChannelDetails(i, mChannelConfig.details[i].dir, NULL);
         if(IVR_CHANNEL_DIR_OUT == mChannelConfig.details[i].dir)
            //this->SetChannelDetails(i, mChannelConfig.details[i].dir, mChannelConfig.details[i].appId);
            this->SetChannelDetails(i, mChannelConfig.details[i].dir);
      }

      T(gTrace, printf("%s: -------------------------------------------------------------- \n", gProcessName);); 
      T(gTrace, printf("%s: \33[32mIvrChannel API - Initialization Ok ...\33[0m\n", gProcessName););
   }
   return mInit;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrChannelApi::ReadConfig()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // ***************************** Read ipc Params ********************
   if (false == lCfgRead.CfgInit (IVR_IPC_CFG))
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n", gProcessName,
               IVR_IPC_CFG););
      gLog.GenerateLog (GSYS09,(TEXT*) IVR_IPC_CFG);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("SHM_IVR_CHANNEL_STATUS_KEY", mChannelShmKey,
            IVR_MIN_SHM_KEY, IVR_MAX_SHM_KEY))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read Failed for %s File:%s", "SHM_IVR_CHANNEL_STATUS_KEY",
            IVR_IPC_CFG);

      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS11, lLogText);
      lCfgRead.CfgDeInit ();

      return false;
   }
   else
   {
      T (gTrace, printf ("%s: SHM_IVR_CHANNEL_STATUS_KEY = %d\n", gProcessName,
               mChannelShmKey););
   }

   lCfgRead.CfgDeInit ();

   if (lCfgRead.CfgInit (IVR_KER_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, IVR_KER_CFG););
      gLog.GenerateLog (GSYS09,(TEXT*) IVR_KER_CFG);
      return false;
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("IVR_DB_CLIENT", gDbClient,
            0, 10))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "IVR_DB_CLIENT", IVR_KER_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: IVR_DB_CLIENT = %d\n",
               gProcessName, gDbClient););
   }
   lCfgRead.CfgDeInit ();

   if (false == lCfgRead.CfgInit (IVR_SYS_CFG))
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n", gProcessName,
               IVR_SYS_CFG););
      gLog.GenerateLog (GSYS09,(TEXT*) IVR_SYS_CFG);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("IVR_CHANNEL_STATUS_REQD", mChnlStatusReqd,
            0, 1))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read Failed for %s File:%s", "IVR_CHANNEL_STATUS_REQD",
            IVR_SYS_CFG);

      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS11, lLogText);
      mChnlStatusReqd = 1;
   }
   else
   {
      T (gTrace, printf ("%s: IVR_CHANNEL_STATUS_REQD = %d\n", gProcessName,
                mChnlStatusReqd););
   }

   lCfgRead.CfgDeInit ();


   /*
      TEXT    lIvrMinCkts[20 + 1];
      TEXT    lIvrMaxCkts[20 + 1];
   // ***************************** Read kernel Params ********************
   if (false == lCfgRead.CfgInit (IVR_KER_CFG))
   {
   TERR (gTrace, printf ("%s: Could not open file %s\n", gProcessName,
   IVR_KER_CFG););
   gLog.GenerateLog (GSYS09, IVR_KER_CFG);
   return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("IVR_TOTAL_CKTS", mIvrTotalCkts,
   IVR_MIN_CHANNEL_ID, IVR_MAX_CHANNEL_ID))
   {
   snprintf (lLogText, MAX_LOG_TEXT_LEN,
   "Config read Failed for %s File:%s", "IVR_TOTAL_CKTS",
   IVR_KER_CFG);

   TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS11, lLogText);
   lCfgRead.CfgDeInit ();

   return false;
   }
   else
   {
   T (gTrace, printf ("%s: IVR_TOTAL_CKTS = %d\n", gProcessName,
   mIvrTotalCkts););
   }

   if (CFG_OK != lCfgRead.GetConfigStr ("IVR_IN_CKTS", mIvrInCkts,
   IVR_MIN_CHANNEL_ID, IVR_MAX_CHANNEL_ID))
   {
   snprintf (lLogText, MAX_LOG_TEXT_LEN,
   "Config read Failed for %s File:%s", "IVR_IN_CKTS",
   IVR_KER_CFG);

   TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS11, lLogText);
   lCfgRead.CfgDeInit ();

   return false;
   }
   else
   {
   T (gTrace, printf ("%s: IVR_IN_CKTS = %s\n", gProcessName,
   mIvrInCkts););

   StrTokenizer lStrTok (mIvrInCkts);
   lStrTok.GetNextToken(lIvrMinCkts, ',');
   lStrTok.GetNextToken(lIvrMaxCkts, ',');

   mIvrMinInCkts = atoi(lIvrMinCkts);
   mIvrMaxInCkts = atoi(lIvrMaxCkts);

   if(0 == mIvrMinInCkts || 0 == mIvrMaxInCkts 
   || mIvrMinInCkts > mIvrTotalCkts || mIvrMaxInCkts > mIvrTotalCkts
   || mIvrMinInCkts > mIvrMaxInCkts)
   {
   sprintf (lLogText, "Wrong Configuration for IVR_IN_CKTS");
   TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   return false;
   }
   else
   {
   T(gTrace, printf("%s: IVR Incoming Circiuts Range %d-%d\n",
   gProcessName, mIvrMinInCkts, mIvrMaxInCkts););
   }
   }

   if (CFG_OK != lCfgRead.GetConfigStr ("IVR_OUT_CKTS", mIvrOutCkts,
            IVR_MIN_CHANNEL_ID, IVR_MAX_CHANNEL_ID))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read Failed for %s File:%s", "IVR_OUT_CKTS",
            IVR_KER_CFG);

      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS11, lLogText);
      lCfgRead.CfgDeInit ();
   }
   else
   {
      T (gTrace, printf ("%s: IVR_OUT_CKTS = %s\n", gProcessName,
               mIvrOutCkts););

      StrTokenizer lStrTok (mIvrOutCkts);
      lStrTok.GetNextToken(lIvrMinCkts, ',');
      lStrTok.GetNextToken(lIvrMaxCkts, ',');

      mIvrMinOutCkts = atoi(lIvrMinCkts);
      mIvrMaxOutCkts = atoi(lIvrMaxCkts);

      if(0 == mIvrMinOutCkts || 0 == mIvrMaxOutCkts
            || mIvrMinOutCkts > mIvrTotalCkts  || mIvrMaxOutCkts > mIvrTotalCkts
            || mIvrMinOutCkts > mIvrMaxOutCkts 
            || mIvrMinOutCkts <= mIvrMinInCkts || mIvrMinOutCkts <= mIvrMaxInCkts
            || mIvrMaxOutCkts <= mIvrMinInCkts || mIvrMaxOutCkts <= mIvrMaxInCkts)
      {
         sprintf (lLogText, "Config falied for IVR_OUT_CKTS");
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         return false;
      }
      else
      {
         T(gTrace, printf("%s: IVR Outgoing Circiuts Range %d-%d\n",
                  gProcessName, mIvrMinOutCkts, mIvrMaxOutCkts););
      }
   }
   */
      return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrChannelApi::ReloadConfig()
{
   TEXT    lLogText[MAX_LOG_TEXT_LEN];
   if(DB_NO_ERR != mDbObj.ReadChannelConfig(mChannelConfig,IVR_CHANNEL_DIR_UNKNOWN))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Could not load Channel Configuration from DB");

      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS11, lLogText);

      return false;
   }
      T(gTrace, printf("%s: ----------------- IVR CHANNEL CONFIGURATION ------------------ \n", gProcessName);); 
      for (INT16 i=mMinChannelId; i<=mMaxChannelId; i++)
      {
         T(gTrace, printf("%s: CID: %05d, DIR: %02d, STATUS: %02d, AppId:%d AppName:%s\n", gProcessName, 
                  i, mChannelConfig.details[i].dir, 
                  GetChannelStatus(i),  //mChannelConfig.details[i].status,
                  mChannelConfig.details[i].appId,
                  mChannelConfig.details[i].appName););
      }

      T(gTrace, printf("%s: -------------------------------------------------------------- \n", gProcessName);); 
      T(gTrace, printf("%s: \33[32mIvrChannel API - Reload Config Ok ...\33[0m\n", gProcessName););
      return true;
}
//----------------------------------------------------------------------
// METHOD NAME : CreateShm
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrChannelApi::CreateShm()
{
   TEXT    lLogText[MAX_LOG_TEXT_LEN];
   BOOLEAN lFlagExistingShm = false;
   int     lShmDetailSize;

   // Creating Shared Memory for Channel Status Information
   lShmDetailSize = ((mMaxChannelId - mMinChannelId + 1) * sizeof (ShmIvrChannelDetails));

   mChannelDetailsShmId = shmget ((key_t) mChannelShmKey, lShmDetailSize, IPC_EXCL | IPC_CREAT | IPC_PERM);

   if (mChannelDetailsShmId < 0)
   {
      T (gTrace, printf ("%s: Shared Memory Already Exists\n", gProcessName););

      //Get Shared memory Id
      mChannelDetailsShmId = shmget ((key_t) mChannelShmKey, lShmDetailSize, IPC_PERM);

      if (mChannelDetailsShmId < 0)
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Failed To Create Shared Memory With Key:%d", mChannelShmKey);

         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         return false;
      }

      lFlagExistingShm = true;
   }

   T (gTrace, printf ("%s: ShmId:%d Key:%d Shared Memory Created Successfully\n",
            gProcessName, mChannelDetailsShmId, mChannelShmKey););

   mpChannelDetails =
      (ShmIvrChannelDetails *) shmat (mChannelDetailsShmId, (ShmIvrChannelDetails *) 0, 0);

   if (mpChannelDetails == NULL)
   {
      TERR (gTrace, printf ("%s: ShmId:%d Key:%d Failed To Attach Shared Memory\n",
               gProcessName, mChannelDetailsShmId, mChannelShmKey););
      return false;
   }

   T (gTrace, printf ("%s: Shared Memory Attach Success\n", gProcessName););

   if (lFlagExistingShm == false)
   {
      // Reseting the Shared Memory.
      T (gTrace, printf ("%s: Reseting the Shared Memory.\n", gProcessName););
      memset (mpChannelDetails, 0, lShmDetailSize);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SetChannelStatus
// DESCRIPTION :
// PARAMETERS  : ChannelId, Status
// RETURN      : ReferenceNumber
//----------------------------------------------------------------------
UINT32 IvrChannelApi::SetChannelStatus(UINT32 channelId, EnumIVRChannelStatus status, UINT32 refNum)
{
   if (channelId >mMaxChannelId)
      return false;
   mpChannelDetails[channelId - mMinChannelId].channelNum = channelId;
   mpChannelDetails[channelId - mMinChannelId].status = status;
   if(IVR_CHANNEL_IN_CALL == status)
   {
      mpChannelDetails[channelId - mMinChannelId].refNum = refNum; 
   }
   else if (IVR_CHANNEL_FREE == status)
   {
      snprintf(mpChannelDetails[channelId - mMinChannelId].clgNum, MAX_MSISDN_LEN, "%s", "");
      snprintf(mpChannelDetails[channelId - mMinChannelId].cldNum, MAX_MSISDN_LEN, "%s", "");
      snprintf(mpChannelDetails[channelId - mMinChannelId].file, MAX_MSISDN_LEN, "%s", "");
      mpChannelDetails[channelId - mMinChannelId].refNum = 0; 
   }

   this->SendChannelStatus(channelId, status, refNum);
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SetChannelDetails
// DESCRIPTION :
// PARAMETERS  : ChannelId, Direction, ClgNum, CldNum, File, RefNum
// RETURN      : ChannelId
//----------------------------------------------------------------------
UINT32 IvrChannelApi::SetChannelDetails(UINT32 channelId, EnumIVRChannelDirection dir, 
      TEXT *clgNum, TEXT *cldNum, TEXT *file, UINT32 refNum)
{
   if (channelId >mMaxChannelId)
      return false;
   mpChannelDetails[channelId - mMinChannelId].dir = dir;
   snprintf(mpChannelDetails[channelId - mMinChannelId].clgNum, MAX_MSISDN_LEN, "%s", clgNum);
   snprintf(mpChannelDetails[channelId - mMinChannelId].cldNum, MAX_MSISDN_LEN, "%s", cldNum);
   snprintf(mpChannelDetails[channelId - mMinChannelId].file, MAX_MSISDN_LEN, "%s", file);

   if(refNum != 0)
      mpChannelDetails[channelId - mMinChannelId].refNum = refNum;

   return channelId;
}

//----------------------------------------------------------------------
// METHOD NAME : GetChannelDetails
// DESCRIPTION :
// PARAMETERS  : ChannelId  ShmIvrChannelDetails 
// RETURN      : EnumIVRChannelStatus
//----------------------------------------------------------------------
EnumIVRChannelStatus IvrChannelApi::GetChannelDetails(UINT32 channelId, ShmIvrChannelDetails &details)
{
   memset(&details, 0, sizeof(ShmIvrChannelDetails));
   if(channelId > mMaxChannelId)
   {
      return IVR_CHANNEL_UNDEFINED;
   }
   memcpy(&details, &mpChannelDetails[channelId - mMinChannelId], sizeof(ShmIvrChannelDetails));
   return mpChannelDetails[channelId - mMinChannelId].status;
}

//----------------------------------------------------------------------
// METHOD NAME : GetChannelStatus
// DESCRIPTION :
// PARAMETERS  : ChannelId
// RETURN      : EnumIVRChannelStatus
//----------------------------------------------------------------------
EnumIVRChannelStatus IvrChannelApi::GetChannelStatus(UINT32 channelId)
{
   if(channelId > mMaxChannelId)
      return IVR_CHANNEL_UNDEFINED;
   return mpChannelDetails[channelId - mMinChannelId].status;
}

//----------------------------------------------------------------------
// METHOD NAME : GetReferneceNum
// DESCRIPTION :
// PARAMETERS  : ChannelId
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
UINT32 IvrChannelApi::GetReferneceNum(UINT32 channelId)
{
   if(channelId <= mMaxChannelId)
   {
      return mpChannelDetails[channelId - mMinChannelId].refNum;
   }

   return 0;
}

//----------------------------------------------------------------------
// METHOD NAME : AllocateNewReferenceNumber
// DESCRIPTION :
// PARAMETERS  : ChannelId
// RETURN      : ReferenceNumber
//----------------------------------------------------------------------
UINT32 IvrChannelApi::AllocateNewReferenceNumber()
{
   mRefNum++;

   if(mRefNum <= 0)
      mRefNum = 1;
   
   T(gTrace, printf("%s: Allocated RefNum:%08X\n",
            gProcessName, mRefNum););

   return mRefNum;
}

//----------------------------------------------------------------------
// METHOD NAME : SendChannelStatus
// DESCRIPTION :
// PARAMETERS  : ChannelId, status, reference number
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrChannelApi::SendChannelStatus(UINT32 channelId, EnumIVRChannelStatus status, UINT32 refNum)
{
   if(mChnlStatusReqd)
      return true;

   ShmIvrChannelDetails channelDetail;

   channelDetail.channelNum = channelId;
   channelDetail.status     = status;
   channelDetail.refNum     = refNum;

   return gIvrQueApi.WriteToChannelStatusQueue(channelDetail);
}

//----------------------------------------------------------------------
// METHOD NAME : ReadChannelStatus
// DESCRIPTION :
// PARAMETERS  : ShmIvrChannelDetails
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrChannelApi::ReadChannelStatus(ShmIvrChannelDetails &channelDetail)
{
   return gIvrQueApi.ReadFromChannelStatusQueue(channelDetail);
}

//----------------------------------------------------------------------
// METHOD NAME : GetOutChannelId
// DESCRIPTION :
// PARAMETERS  : ChannelId
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
//UINT32 IvrChannelApi::GetOutChannelId(UINT32 appId)
//UINT32 IvrChannelApi::GetOutChannelId(TEXT *shortCode)
UINT32 IvrChannelApi::GetOutChannelId(TEXT *appName)
{
   static UINT32 lsLastChannelId=0;
   /*for(int i = mIvrMinOutCkts; i <= mIvrMaxOutCkts; i++)
   {
      if(IVR_CHANNEL_FREE  == mpChannelDetails[i].status 
         || IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status)
      {
         return i;
      }
   }*/
   
   //if((shortCode == NULL) || (strcmp(shortCode, "-1")==0))
   //if(shortCode == NULL)
   if((strcmp(appName, "Dummy")==0))
   {
      for(int i = lsLastChannelId+1; i <= mIvrTotalCkts; i++)
      {
         if((IVR_CHANNEL_DIR_OUT == mChannelConfig.details[i].dir || IVR_CHANNEL_DIR_IN_OUT == mChannelConfig.details[i].dir) && 
             (strcmp(mChannelConfig.details[i].appName, "Dummy") == 0) &&
               (IVR_CHANNEL_FREE  == mpChannelDetails[i].status || IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status))
         {
            T(gTrace, printf("%s: RefNum:%08X CID:%05d AppName:%s Allocated Outgoing Channel\n",
                  gProcessName, gReferenceNum, i, mChannelConfig.details[i].appName););
            lsLastChannelId = i;
            if(IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status)
               mpChannelDetails[i].status = IVR_CHANNEL_UNINITIALIZED_ALLOCATED;
            else
               mpChannelDetails[i].status = IVR_CHANNEL_FREE_ALLOCATED;
            return i;
         }
      }
      for(UINT32 i = 0; i <= lsLastChannelId; i++)
      {
         if((IVR_CHANNEL_DIR_OUT == mChannelConfig.details[i].dir || IVR_CHANNEL_DIR_IN_OUT == mChannelConfig.details[i].dir) && 
             (strcmp(mChannelConfig.details[i].appName, "Dummy") == 0) &&
               (IVR_CHANNEL_FREE  == mpChannelDetails[i].status || IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status))
         {
            T(gTrace, printf("%s: RefNum:%08X CID:%05d AppName:%s Allocated Outgoing Channel\n",
                  gProcessName, gReferenceNum, i, mChannelConfig.details[i].appName););
            lsLastChannelId = i;
            if(IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status)
               mpChannelDetails[i].status = IVR_CHANNEL_UNINITIALIZED_ALLOCATED;
            else
               mpChannelDetails[i].status = IVR_CHANNEL_FREE_ALLOCATED;
            return i;
         }
      }

   }
   else
   {
      for(int i = lsLastChannelId+1; i <= mIvrTotalCkts; i++)
      {
         if((IVR_CHANNEL_DIR_OUT == mChannelConfig.details[i].dir || IVR_CHANNEL_DIR_IN_OUT == mChannelConfig.details[i].dir) && 
             (strcmp(appName, mChannelConfig.details[i].appName) == 0) &&
             (IVR_CHANNEL_FREE  == mpChannelDetails[i].status || IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status))
         {
            T(gTrace, printf("%s: RefNum:%08X CID:%05d AppId:%d Allocated Outgoing Channel\n",
                  gProcessName, gReferenceNum, i, mChannelConfig.details[i].appId););
            lsLastChannelId = i;
            if(IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status)
               mpChannelDetails[i].status = IVR_CHANNEL_UNINITIALIZED_ALLOCATED;
            else
               mpChannelDetails[i].status = IVR_CHANNEL_FREE_ALLOCATED;
            return i;
         }
      }
      for(UINT32 i = 0; i <= lsLastChannelId; i++)
      {
         if((IVR_CHANNEL_DIR_OUT == mChannelConfig.details[i].dir || IVR_CHANNEL_DIR_IN_OUT == mChannelConfig.details[i].dir) && 
             (strcmp(appName, mChannelConfig.details[i].appName) == 0) &&
             (IVR_CHANNEL_FREE  == mpChannelDetails[i].status || IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status))
         {
            T(gTrace, printf("%s: RefNum:%08X CID:%05d AppId:%d Allocated Outgoing Channel\n",
                  gProcessName, gReferenceNum, i, mChannelConfig.details[i].appId););
            lsLastChannelId = i;
            if(IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status)
               mpChannelDetails[i].status = IVR_CHANNEL_UNINITIALIZED_ALLOCATED;
            else
               mpChannelDetails[i].status = IVR_CHANNEL_FREE_ALLOCATED;
            return i;
         }
      }
      return GetOutChannelId((TEXT*)"Dummy");
   }

   T(gTrace, printf("%s: RefNum:%08X AppName:%s Outgoing Channel Allocation Failed\n",
      gProcessName, gReferenceNum, appName););
   return -1;
}

//----------------------------------------------------------------------
// METHOD NAME : GetOutChannelIdLowestAvailable
// DESCRIPTION :
// PARAMETERS  : ChannelId
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
UINT32 IvrChannelApi::GetOutChannelIdLowestAvailable(TEXT *appName)
{
   /*for(int i = mIvrMinOutCkts; i <= mIvrMaxOutCkts; i++)
   {
      if(IVR_CHANNEL_FREE  == mpChannelDetails[i].status 
         || IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status)
      {
         return i;
      }
   }*/
   
   //if((shortCode == NULL) || (strcmp(shortCode, "-1")==0))
   //if(shortCode == NULL)
   if((strcmp(appName, "Dummy")==0))
   {
      for(int i = 0; i <= mIvrTotalCkts; i++)
      {
         if((IVR_CHANNEL_DIR_OUT == mChannelConfig.details[i].dir || IVR_CHANNEL_DIR_IN_OUT == mChannelConfig.details[i].dir) && 
             (strcmp(mChannelConfig.details[i].appName, "Dummy") == 0) &&
               (IVR_CHANNEL_FREE  == mpChannelDetails[i].status || IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status))
         {
            T(gTrace, printf("%s: RefNum:%08X CID:%05d AppName:%s Allocated Outgoing Channel\n",
                  gProcessName, gReferenceNum, i, mChannelConfig.details[i].appName););
            return i;
         }
      }
   }
   else
   {
      for(int i = 0; i <= mIvrTotalCkts; i++)
      {
         if((IVR_CHANNEL_DIR_OUT == mChannelConfig.details[i].dir || IVR_CHANNEL_DIR_IN_OUT == mChannelConfig.details[i].dir) && 
             (strcmp(appName, mChannelConfig.details[i].appName) == 0) &&
             (IVR_CHANNEL_FREE  == mpChannelDetails[i].status || IVR_CHANNEL_UNINITIALIZED == mpChannelDetails[i].status))
         {
            T(gTrace, printf("%s: RefNum:%08X CID:%05d AppId:%d Allocated Outgoing Channel\n",
                  gProcessName, gReferenceNum, i, mChannelConfig.details[i].appId););
            return i;
         }
      }
      return GetOutChannelIdLowestAvailable((TEXT*)"Dummy");
   }

   T(gTrace, printf("%s: RefNum:%08X AppName:%s Outgoing Channel Allocation Failed\n",
      gProcessName, gReferenceNum, appName););
   return -1;
}
