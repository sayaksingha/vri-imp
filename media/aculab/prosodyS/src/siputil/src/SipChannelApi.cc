static const char* id="@(#) $Id: SipChannelApi.cc,v 1.1.2.1 2016/07/05 16:58:04 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// SipChannelApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//       This class is used for storing outgoing request and retrieving
// incoming response.
//----------------------------------------------------------------------
#include "SipChannelApi.h"

//----------------------------------------------------------------------
// METHOD NAME : SipChannelApi
// DESCRIPTION : constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
SipChannelApi::SipChannelApi ()
{

}

//----------------------------------------------------------------------
// METHOD NAME : ~SipChannelApi
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
SipChannelApi::~SipChannelApi ()
{
   if (mpHashTablePtr)
      delete[] mpHashTablePtr;

   mpHashTablePtr = NULL;
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializes all the private variables and
//               objects used by the SipChannelApi process
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::Init ()
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";

   if (false == this->ReadConfig ())
   {
      return false;
   }

   if (false == this->InitHashTable ())
   {
      sprintf (lLogText, "Failed to Initailize the ReqHashTable");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, 
               lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   if (false == this->ClearReqMapHashTable ())
      return false;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : InitHashTable ()
// DESCRIPTION : Initializes the Parameter Hash Table 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::InitHashTable ()
{
   UINT32 lHashTableSize = 0;

   mReqMapDir.SetBucketCount (SIP_MAX_HASH_TABLE_ENTRY + 1);

   lHashTableSize = mReqMapDir.GetHashTableSize ();

   mpHashTablePtr = new TEXT [lHashTableSize];
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ClearReqMapHashTable 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::ClearReqMapHashTable ()
{
   //TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";

   if (mReqMapDir.SetHashMemoryPtr ((void *) mpHashTablePtr,
            true) != SHT_SUCCESS)
   {
      TERR (gTrace, printf ("%s: Failed to set hash table memory pointer\n",
               gProcessName););
      //gLog.GenerateLog (API_DIAMETER41);
      return false;
   }

   mReqMapDir.SetSemaphoreId(mSipChannelSemKey);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::ReadConfig ()
{
   CfgRead      		lCfgRead(gProcessName);
   TEXT              lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";

   if (false == lCfgRead.CfgInit(SIP_IPC_CFG) )
   {
      snprintf(lLogText, TSS_MAX_LOG_TEXT_LEN, "Could not open file %s", 
            SIP_IPC_CFG);
      TERR(gTrace, printf("%s: Could not open file %s\n",
               gProcessName, SIP_IPC_CFG););
      gLog.GenerateLog(GSYS10, lLogText);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("SIP_CHANNEL_SEM_KEY",
            mSipChannelSemKey,
            TSS_MIN_IPC, TSS_MAX_IPC))
   {
      snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN, 
            "SIP_CHANNEL_SEM_KEY . File:%s", SIP_IPC_CFG);
      TERR (gTrace, printf ("%s: Config read failed for %s \n",
               gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: SIP_CHANNEL_SEM_KEY = %d\n",
               gProcessName, mSipChannelSemKey););
   }

   lCfgRead.CfgDeInit ();

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::ReloadConfig ()
{
   return this->ReadConfig ();
}

//----------------------------------------------------------------------
// METHOD NAME : InsertReqquestIntoHash
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::InsertRequestIntoHash (SipChannel & lSipChannel)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";

   //if (false == this->ClearReqMapHashTable ())
   // return false;

   snprintf (lLogText, 
         TSS_MAX_LOG_TEXT_LEN, 
         "CID:%06d  Channel:%lu Key:%s Idx:%d",
          lSipChannel.index, lSipChannel.channel, lSipChannel.key, lSipChannel.index);

   if (SHT_SUCCESS !=
         mReqMapDir.Insert (lSipChannel.key,
            lSipChannel))
   {
      //gLog.GenerateLog (DIA_REQMAP01, lLogText);
      TERR (gTrace, printf ("%s: %s ReqKey Table insertion Failed ,key:%s\n",
               gProcessName, lLogText, lSipChannel.key););
      return false;
   }

   //gLog.GenerateLog (DIA_REQMAP02, lLogText);
   T (gTrace, printf ("%s: %s Insert into ReqKey Table success %s\n",
            gProcessName, lLogText, lSipChannel.key););

   return true;
}
//----------------------------------------------------------------------
// METHOD NAME : GetReqMapDetails 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::GetReqMapDetails (TEXT * handle,
      SipChannel & lSipChannel)
{
   TEXT           lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   TEXT           lTempString[SIP_MAX_REQ_KEY_LEN + 1] = "";
   HashTableError lHashTableError;

   strcpy (lTempString, handle);


   lHashTableError = (HashTableError)mReqMapDir.Get (
         lTempString, lSipChannel);

   if (lHashTableError != SHT_SUCCESS)   
   {
      snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN, 
            "Failed to get Key details Key:%s Err:%d", 
            lTempString, lHashTableError);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      //gLog.GenerateLog (DIA_REQMAP04, lLogText);
      return false;
   }    
#if 0   
   snprintf (lLogText, 
         TSS_MAX_LOG_TEXT_LEN, 
         "CID:%06d  Channel:%lu Key:%s Idx:%d",
          lSipChannel.index, lSipChannel.channel, lSipChannel.key, lSipChannel.index);

   T (gTrace,
         printf ("%s: %s Key details found \n",
            gProcessName, lLogText););
   //gLog.GenerateLog (DIA_REQMAP03, lLogText);
#endif
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DeleteRequestFromHash 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::DeleteRequestFromHash (TEXT * handle)
{
   TEXT           lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   TEXT           lTempString[SIP_MAX_REQ_KEY_LEN + 1] = "";
   HashTableError lHashTableError;

   strcpy (lTempString, handle);


   lHashTableError = (HashTableError)mReqMapDir.Delete (lTempString);

   if (lHashTableError != SHT_SUCCESS)   
   {
      snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN, 
            "Failed to Delete Key details Key:%s Err:%d", 
            lTempString, lHashTableError);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      //gLog.GenerateLog (DIA_REQMAP05, lLogText);
      return false;
   }     

   snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN, 
         "Deleted from Hash Table Key: %s", handle);

   T (gTrace,
         printf ("%s: %s\n",
            gProcessName, lLogText););
   //gLog.GenerateLog (DIA_REQMAP06, lLogText);

   return true;
}
//----------------------------------------------------------------------
// METHOD NAME : GetAllRequest 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipChannelApi::GetAllRequest (
      TEXT (*key)[SIP_MAX_REQ_KEY_LEN + 1], 
      SipChannel* lSipChannel, UINT32 &lcount )
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   HashTableError lHashTableError;

   lHashTableError = 
      (HashTableError)mReqMapDir.GetAll(key, &lSipChannel[0], lcount);

   if (lHashTableError != SHT_SUCCESS)   
   {
      snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN, 
            "Get All Failed Err:%d", lHashTableError);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      //gLog.GenerateLog (DIA_REQMAP07, lLogText);
      return false;
   }

   /*  T (gTrace,
       printf ("%s: %s Get All Key Details Success\n",
       gProcessName, lLogText););
    */
   return true;
}
