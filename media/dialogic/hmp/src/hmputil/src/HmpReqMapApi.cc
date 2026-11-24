static const char* id="@(#) $Id: HmpReqMapApi.cc,v 1.1.2.4 2016/04/12 21:59:33 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// HmpReqMapApi.cc
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
#include "HmpReqMapApi.h"

//----------------------------------------------------------------------
// METHOD NAME : HmpReqMapApi
// DESCRIPTION : constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpReqMapApi::HmpReqMapApi ()
{

}

//----------------------------------------------------------------------
// METHOD NAME : ~HmpReqMapApi
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpReqMapApi::~HmpReqMapApi ()
{
   if (mpHashTablePtr)
      delete[] mpHashTablePtr;

   mpHashTablePtr = NULL;
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializes all the private variables and
//               objects used by the HmpReqMapApi process
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpReqMapApi::Init ()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

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
BOOLEAN HmpReqMapApi::InitHashTable ()
{
   UINT32 lHashTableSize = 0;

   mReqMapDir.SetBucketCount (HMP_MAX_HASH_TABLE_ENTRY + 1);

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
BOOLEAN HmpReqMapApi::ClearReqMapHashTable ()
{
   //TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (mReqMapDir.SetHashMemoryPtr ((void *) mpHashTablePtr,
            true) != SHT_SUCCESS)
   {
      TERR (gTrace, printf ("%s: Failed to set hash table memory pointer\n",
               gProcessName););
      //gLog.GenerateLog (API_HMP41);
      return false;
   }

   mReqMapDir.SetSemaphoreId(mReqMapSemKey);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpReqMapApi::ReadConfig ()
{
   CfgRead      		lCfgRead(gProcessName);
   TEXT              lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (false == lCfgRead.CfgInit(HMP_IPC_CFG) )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s", 
            HMP_IPC_CFG);
      TERR(gTrace, printf("%s: Could not open file %s\n",
               gProcessName, HMP_IPC_CFG););
      gLog.GenerateLog(GSYS10, lLogText);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("HMP_REQ_MAP_SEM_KEY",
            mReqMapSemKey,
            HMP_MIN_SHM_KEY, HMP_MAX_SHM_KEY))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, 
            "HMP_REQ_MAP_SEM_KEY . File:%s", HMP_IPC_CFG);
      TERR (gTrace, printf ("%s: Config read failed for %s \n",
               gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: HMP_REQ_MAP_SEM_KEY = %d\n",
               gProcessName, mReqMapSemKey););
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
BOOLEAN HmpReqMapApi::ReloadConfig ()
{
   return this->ReadConfig ();
}

//----------------------------------------------------------------------
// METHOD NAME : InsertReqquestIntoHash
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpReqMapApi::InsertRequestIntoHash (HmpReqMap & lHmpReqMap)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   //if (false == this->ClearReqMapHashTable ())
   // return false;

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "Handle:%s", lHmpReqMap.key );

   if (SHT_SUCCESS !=
         mReqMapDir.Insert (lHmpReqMap.key,
            lHmpReqMap))
   {
      gLog.GenerateLog (HMP_REQMAP01, lLogText);
      TERR (gTrace, printf ("%s: %s ReqKey Table insertion Failed ,key:%s\n",
               gProcessName, lLogText, lHmpReqMap.key););
      return false;
   }

   gLog.GenerateLog (HMP_REQMAP02, lLogText);
   T (gTrace, printf ("%s: %s Insert into ReqKey Table success %s\n",
            gProcessName, lLogText, lHmpReqMap.key););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : UpdateReqquestIntoHash
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpReqMapApi::UpdateRequestIntoHash (HmpReqMap & lHmpReqMap)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   //if (false == this->ClearReqMapHashTable ())
   // return false;

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "Handle:%s", lHmpReqMap.key );

   if (SHT_SUCCESS !=
         mReqMapDir.Replace (lHmpReqMap.key,
            lHmpReqMap))
   {
      gLog.GenerateLog (HMP_REQMAP01, lLogText);
      TERR (gTrace, printf ("%s: %s ReqKey Table Update Failed ,key:%s\n",
               gProcessName, lLogText, lHmpReqMap.key););
      return false;
   }

   gLog.GenerateLog (HMP_REQMAP02, lLogText);
   T (gTrace, printf ("%s: %s Update into ReqKey Table success %s\n",
            gProcessName, lLogText, lHmpReqMap.key););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetReqMapDetails 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpReqMapApi::GetReqMapDetails (TEXT * handle,
      HmpReqMap & lHmpReqMap)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT           lTempString[HMP_MAX_REQ_KEY_LEN + 1] = "";
   HashTableError lHashTableError;

   memset(&lHmpReqMap, 0, sizeof(HmpReqMap));
   strcpy (lTempString, handle);


   lHashTableError = (HashTableError)mReqMapDir.Get (
         lTempString, lHmpReqMap);

   if (lHashTableError != SHT_SUCCESS)   
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, 
            "Failed to get Key details Key:%s Err:%d", 
            lTempString, lHashTableError);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (HMP_REQMAP04, lLogText);
      return false;
   }     

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "Handle:%s", lHmpReqMap.key );
   
   T (gTrace,
         printf ("%s: %s Key details found \n",
            gProcessName, lLogText););
   gLog.GenerateLog (HMP_REQMAP03, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DeleteRequestFromHash 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpReqMapApi::DeleteRequestFromHash (TEXT * handle)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT           lTempString[HMP_MAX_REQ_KEY_LEN + 1] = "";
   HashTableError lHashTableError;

   strcpy (lTempString, handle);


   lHashTableError = (HashTableError)mReqMapDir.Delete (lTempString);

   if (lHashTableError != SHT_SUCCESS)   
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, 
            "Failed to Delete Key details Key:%s Err:%d", 
            lTempString, lHashTableError);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (HMP_REQMAP05, lLogText);
      return false;
   }     

   snprintf (lLogText, MAX_LOG_TEXT_LEN, 
         "Deleted from Hash Table Key: %s", handle);

   T (gTrace,
         printf ("%s: %s\n",
            gProcessName, lLogText););
   gLog.GenerateLog (HMP_REQMAP06, lLogText);

   return true;
}
//----------------------------------------------------------------------
// METHOD NAME : GetAllRequest 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpReqMapApi::GetAllRequest (
      TEXT (*key)[HMP_MAX_REQ_KEY_LEN + 1], 
      HmpReqMap* lHmpReqMap, UINT32 &lcount )
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   HashTableError lHashTableError;

   lHashTableError = 
      (HashTableError)mReqMapDir.GetAll(key, &lHmpReqMap[0], lcount);

   if (lHashTableError != SHT_SUCCESS)   
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, 
            "Get All Failed Err:%d", lHashTableError);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (HMP_REQMAP07, lLogText);
      return false;
   }

   /*  T (gTrace,
       printf ("%s: %s Get All Key Details Success\n",
       gProcessName, lLogText););
    */
   return true;
}
