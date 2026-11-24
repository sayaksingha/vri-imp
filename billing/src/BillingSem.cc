//----------------------------------------------------------------------
// NAME
// BillingSem.cc 

// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//
// Originated :                                         Date:05-MAR-2012
//----------------------------------------------------------------------
#include "BillingSem.h"
#include "BillingUtil.h"

//----------------------------------------------------------------------
// METHOD NAME : BillingSem
// DESCRIPTION : constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
BillingSem::BillingSem ()
{
   mSemKey = 0;
   mSemId  = 0;
   mSemNo  = 0;
}

//----------------------------------------------------------------------
// METHOD NAME : ~BillingSem
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
BillingSem::~BillingSem ()
{
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializes all the private variables and
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingSem::Init (INT32 semkey, INT32 semnos)
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "SemKey:%d SemNo:%d", semkey, semnos);
   
   if(0 == semkey || 0 == semnos)
   {
      TERR(gTrace, printf ("%s: %s BillingSem Initilization failed.\n", 
          gProcessName, lLogText););
      return false;
   }
   
   mSemKey = semkey; 
   mSemNo  = semnos;

   if (false == this -> CreateSem())
   {
      TERR(gTrace, printf ("%s: %s BillingSem Initilization failed.",
      gProcessName, lLogText););
      gLog.GenerateLog (GSYS06, lLogText);
   }

   T(gTrace, printf ("%s: BillingSem Initilization success..\n", gProcessName););
   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreatSem
// DESCRIPTION : Create the Semaphore correspoding to the SemKey.
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingSem::CreateSem ()
{
   // Create the Semapore
   mSemId = semget(mSemKey, mSemNo, IPC_PERM|IPC_CREAT);
   if (-1 == mSemId)
   {
      T(gTrace, printf ("%s: Billing Semphore Creation Failed.SemKey:%d\n", 
                  gProcessName, mSemKey););
      return false;
   }
   T(gTrace, printf ("%s: Billing Semphore Creation success.SemKey:%d\n", 
                  gProcessName, mSemKey););
   return true;
}

//----------------------------------------------------------------------
// METHOD      : Lock
// DESCRIPTION : do a zero operation and one possitive operation.
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingSem::Lock ()
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN +1] = "";
   if (0 == mSemId)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Lock: SemId=0 ERROR");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }
   
   sembuf      lsemLocker[2];
   int         lReturnVal;
   int         lnsops = 2;

   lsemLocker[0].sem_num = 0 ;
   lsemLocker[0].sem_op  = 0 ;
   //lsemLocker[0].sem_flg = SEM_UNDO ;

   lsemLocker[1].sem_num = 0 ;
   lsemLocker[1].sem_op  = 1 ;
   //lsemLocker[1].sem_flg = SEM_UNDO | IPC_NOWAIT ;

   lReturnVal = semop(mSemId, &lsemLocker[0], lnsops);
   if (-1 == lReturnVal)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Lock: SemLock failed");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);

      return false;
   }
   
   T(gTrace, printf("%s: Lock: Success..\n", gProcessName););
   return true;
}

//----------------------------------------------------------------------
// METHOD      : UnLock
// DESCRIPTION : do one negetive operation.
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN BillingSem::UnLock ()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN +1] = "";
   if (0 == mSemId)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "UnLock: SemId=0 ERROR");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }
   
   sembuf   lsemLocker[2];
   int      lReturnVal;
   int      lnsops = 1;

   lsemLocker[0].sem_num = 0 ;
   lsemLocker[0].sem_op  = -1;
   //lsemLocker[0].sem_flg = SEM_UNDO | IPC_NOWAIT ;


   lReturnVal = semop(mSemId, &lsemLocker[0], lnsops);
   if (-1 == lReturnVal)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "UnLock: SemUnLock failed");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }
   
   T(gTrace, printf("%s: Sem Unlock Success..\n", gProcessName););
   return true;
}
