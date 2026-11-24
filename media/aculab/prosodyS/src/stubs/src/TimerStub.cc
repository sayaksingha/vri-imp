static const char* id = "@(#) $Id: TimerStub.cc,v 1.1.4.1.2.1 2017/01/06 20:31:51 mram Exp $";

#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "Timer.h"
//#include "AseUtil.h"

//Log        gLog;
CTrace     gAseTrace ("STUB_TRACE");
TEXT       gProcessName[100];
Timer      gTimerObj;
INT32      gIndex;
tag        gCancelResponseTimerTags[2000];

//-------------------------------------------------------------
// FUNCTION    : CancelResponseTimer
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
bool CancelResponseTimer(tag  responseTimerTag)
{
   timerError     lTimerRes=TIMER_SUCCESS;

   if (responseTimerTag > 0)
   {
      lTimerRes = gTimerObj.CancelTimeout(responseTimerTag);
   }
   else
   {
      TERR(gAseTrace, printf("%s: ResponseTimer Tag not set\n", gProcessName););
      //printf("%s: ResponseTimer Tag not set\n", gProcessName);
      return false;
   }

   if (lTimerRes != TIMER_SUCCESS)
   {
      TERR(gAseTrace, printf("%s: Failed to CancelResponseTimer Tag: %d Error:%d\n",
               gProcessName, responseTimerTag, lTimerRes););
      //printf("%s: Failed to CancelResponseTimer Tag: %d Error:%d\n",
      return false;
   }

   T(gAseTrace, printf("%s: Success to cancel ResponseTimer Tag:%d\n", gProcessName, responseTimerTag););
   //printf("%s: Success to cancel ResponseTimer Tag:%d\n", gProcessName, responseTimerTag);
   return true;
}

//-------------------------------------------------------------
// FUNCTION    : Func1
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
INT32 Func1(UINT32 lmIndex)
{
   T(gAseTrace, printf("%s: Timeout gIndex:%d\n", gProcessName, lmIndex););

   if (gCancelResponseTimerTags[lmIndex] != 0)
   {
      if (CancelResponseTimer(gCancelResponseTimerTags[lmIndex]))
      {
            printf("CancelResponseTimer\n");
         // Do Nothing
         // 
      }
   }

   return lmIndex;
}

//-------------------------------------------------------------
// FUNCTION    : StartResponseTimer
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
bool StartResponseTimer(UINT32 lKIndex)
{
   timerError     lTimerRes = TIMER_SUCCESS;
   INT32          (*lpFuncPtr)(UINT32 ) ;
   tag            lResponseTimerTag;

   lpFuncPtr = Func1;

   lTimerRes = gTimerObj.SetTimeout(lpFuncPtr,
         lKIndex,
         5,
         lResponseTimerTag);

   if (lTimerRes != TIMER_SUCCESS)
   {
      TERR(gAseTrace, printf("%s: Failed to StartResponseTimer Error:%d\n", gProcessName, lTimerRes););
      //printf("%s: Failed to StartResponseTimer Error:%d\n", gProcessName, lTimerRes);
      lResponseTimerTag = 0;
      return false;
   }
   gCancelResponseTimerTags[lKIndex] = lResponseTimerTag;
   T(gAseTrace, printf("%s: Success to StartResponseTimer gIndex:%d Tag:%d %lu\n",
            gProcessName, lKIndex, lResponseTimerTag, lpFuncPtr););
   //printf("%s: Success to StartResponseTimer gIndex:%d Tag:%d\n", gProcessName, lKIndex, lResponseTimerTag);
   return true;
}



//-------------------------------------------------------------
// FUNCTION    : CheckAndCancelTimers
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
bool CheckAndCancelTimers()
{
   for(INT32 lIndex = 0; lIndex <= 2000; lIndex++)
   {
      if (gCancelResponseTimerTags[lIndex] != 0)
      {
         if (CancelResponseTimer(gCancelResponseTimerTags[lIndex]))
         {
            // Do Nothing
            printf("CancelResponseTimer\n");
         }
         gCancelResponseTimerTags[lIndex]=0;
      }
   }
   return true;
}


//-------------------------------------------------------------
// FUNCTION    : MAIN
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
int main(int argc, char* argv[])
{
   //AseUtil::LoadSignals ();

   /*gLog.SetProcessName (basename (argv[0]), argv[1]);

   if (true != gLog.Init ("STUB_Timer"))
   {
      printf ("%s:\33[31m Log object Init Failed. Exiting...\33[0m\n",
            gProcessName);
      return false;
   }*/

   //Initialise Timeout Table
   if (gTimerObj.InitTimeoutTable(2000) != TIMER_SUCCESS)
   {
      TERR(gAseTrace, printf("%s: Timer Initialization failed\n", gProcessName););
      //printf("%s: Timer Initialization failed\n", gProcessName);
      return false;
   }

   //while (AseUtil::KeepRunning ())
         StartResponseTimer(1);
         sleep(2);
         StartResponseTimer(2);
   while (1)
   {
      sleep(1);
      gIndex++;
      
      if(gIndex)
      {
         gIndex=2;
         StartResponseTimer(3);
         gIndex=0;
      }
   }
   return true;
}
