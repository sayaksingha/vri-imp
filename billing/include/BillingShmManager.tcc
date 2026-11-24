//----------------------------------------------------------------------
// NAME
// BillingShmManager.tcc
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
// Originated : KOTHARI KOMAL                           Date:05-MAR-2012
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// METHOD NAME : BillingShmManager
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
BillingShmManager<Data>::BillingShmManager ()
{
}

//----------------------------------------------------------------------
// METHOD NAME : ~BillingShmManager
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
BillingShmManager<Data>::~BillingShmManager ()
{
   // Detach from the Shared Memory...
   shmdt(mpShmPtr);
}

//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
BOOLEAN BillingShmManager<Data>::Init(INT32 key, INT32 startOff, INT32 endOff)
{
   if(0 == key || 0 == startOff || 0 == endOff)
   {
      TERR(gTrace, printf("%s: Couldn't Initilize the Shared Memory With NULL Values (Key:%d, Start:%d, End:%d\n", 
                     gProcessName, key, startOff, endOff););
      return false;
   }
   
   mMax     = endOff;
   mMin     = startOff;
   mShmKey  = key;
   
   if(false == CreateShm())
      return false;
   
   else
    {
       T(gTrace, printf("%s: Shared Memory Object Initilization Success Key:%d\n",
        gProcessName, mShmKey););
       return true;
    }
}

//----------------------------------------------------------------------
// METHOD NAME : CreateShm
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
BOOLEAN BillingShmManager<Data>::CreateShm()
{
   INT32       lSize = (mMax - mMin) * sizeof(Data);
   BOOLEAN     lFlagExistingShm = false;
   INT32       lShmLookUpId = 0;
   TEXT        lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Create Shared Memory
   lShmLookUpId = shmget((key_t)mShmKey, lSize, IPC_CREAT |IPC_EXCL | 0666);
   if (lShmLookUpId < 0)
   {
      T(gTrace, printf("%s: Shared Memory already Exists Key:%d\n", 
                     gProcessName, mShmKey););
      lFlagExistingShm = true;
      lShmLookUpId = shmget((key_t)mShmKey, lSize, IPC_CREAT |0666);
      if(lShmLookUpId < 0)
      {
         switch (errno)
         {
            case EACCES:
               {
                  sprintf(lLogText, "EACCES");
                  break;
               }
            case EINVAL:
               {
                  sprintf(lLogText, "EINVAL");
                  break;
               }
            case ENOMEM:
               {
                  sprintf(lLogText, "ENOMEM");
                  break;
               }
         }
         TERR(gTrace,printf("%s: %s Failed to create Shared Memory with Key:%d\n",
                  gProcessName, lLogText, mShmKey););
         gLog.GenerateLog(SYD11, lLogText);
         return false;
      }
   }

   // Attach Shared Memory
   mpShmPtr = (Data*) shmat(lShmLookUpId, (Data*) 0, 0);

   if (NULL == mpShmPtr)
   {
      gLog.GenerateLog(SYD11);
      TERR(gTrace, printf("%s: Failed to attach Shared Memory Key:%d\n", 
                     gProcessName, mShmKey););
      return false;
   }
   
   //Shared Merory Created Newly
   if(false == lFlagExistingShm)
   {
      memset(mpShmPtr, '\0', (mMax - mMin));
   }
   
   //Shared Merory already Exist.. don't Reset
   else
   {
       T(gTrace, printf("%s: Shared Memory Already Contain Data:%d\n", 
                     gProcessName, mShmKey););
   }
   
   T(gTrace, printf("%s: Shared Memory created successfully Key:%d\n", 
                     gProcessName, mShmKey););
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : InsertData
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
BOOLEAN BillingShmManager<Data>::InsertData(Data* data, INT32 indx)
{
   if(NULL == mpShmPtr || indx > mMax || indx < mMin)
   {
      TERR(gTrace, printf("%s: InsertData Err Key:%d Indx:%d\n",gProcessName,mShmKey,indx););
      TERR(gTrace, printf("%s: Start Indx:%d End Indx:%d\n", gProcessName, mMin, mMax););
      return false;
   }
   
   INT32    lShmIndx = indx - mMin;
   memcpy(&mpShmPtr[lShmIndx], data, sizeof(Data));
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetData
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
BOOLEAN BillingShmManager<Data>::GetData(Data* data, INT32 indx)
{
   if(NULL == mpShmPtr || indx > mMax || indx < mMin)
   {
      TERR(gTrace, printf("%s: GetData Err Key:%d Indx:%d\n",gProcessName, mShmKey,indx););
      TERR(gTrace, printf("%s: Start Indx:%d End Indx:%d\n", gProcessName, mMin, mMax););
      return false;
   }
   
   INT32    lShmIndx = indx - mMin;
   memcpy(data, &mpShmPtr[lShmIndx] , sizeof(Data));
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ResetSlot
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
BOOLEAN BillingShmManager<Data>::ResetSlot(INT32 indx)
{
   if(NULL == mpShmPtr || indx > mMax || indx < mMin)
   {
      TERR(gTrace, printf("%s: ResetSlot Err Key:%d Indx:%d\n",gProcessName, mShmKey,indx););
      TERR(gTrace, printf("%s: Start Indx:%d End Indx:%d\n", gProcessName, mMin, mMax););
      return false;
   }
   
   INT32    lShmIndx = indx - mMin;
   memset(&mpShmPtr[lShmIndx], '\0', sizeof(Data));
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetShmPointer
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
inline Data* BillingShmManager<Data>::GetShmPointer()
{
   return mpShmPtr;
}

//----------------------------------------------------------------------
// METHOD NAME : GetFreeDlgId
// DESCRIPTION : This method can be used only with intiger types.
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
template <typename Data>
INT32 BillingShmManager<Data>::GetFreeSlot(INT32 insertdlg)
{
   for(int i = 0; i < 3; i++)
   {
      if(mFreeDlgId == mMax)
      mFreeDlgId = 0;

      for(int j = mFreeDlgId; j <= mMax; j++)
      {
         if (0 == mpShmPtr[j])
         {
            // Save for next use.
            mFreeDlgId = j;
            // Mark the Dlg as Busy.
            mpShmPtr[j] = insertdlg;
            T(gTrace, printf("%s: Slot Alotted:%ld\n", gProcessName, mFreeDlgId););
            return mFreeDlgId;
         }
      }
   }
   TERR(gTrace, printf("%s: Start Offset:%ld End Offset:%ld Cann't find Free Slot..\n", 
            gProcessName, mMin, mMax););
   return -1;
}
