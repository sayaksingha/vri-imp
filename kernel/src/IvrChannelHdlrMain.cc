//----------------------------------------------------------------------
// NAME
// IvrChannelHdlrMain.cc 
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright (C) 2005
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated: Divya K                        Date: 17-JAN-2012
//----------------------------------------------------------------------
#include "IvrChannelHdlr.h"

Log              gLog;
CTrace           gTrace (TRACE_IVR_CHANNEL_MGR_ENV);
TEXT             gProcessName[MAX_PROCESS_NAME + 1] = "";
extern TEXT      gLogText[MAX_LOG_TEXT_LEN];
//HealthCheckApi   gHealthCheckObj;
unsigned int     gReferenceNum;

UINT32           gMinChannelId;
UINT32           gMaxChannelId;
INT16            gReqType;
IvrQueApi        gIvrQueApi;
EnumChannelCmd   gChannelCmd;
INT16            gCallType;

//-------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
int main(int argc, char **argv)
{
   ProcessLock      lProcessLock;
   IvrChannelHdlr   lChannelHdlrObj;

   printf ("Trace: export %s=1\n", TRACE_IVR_CHANNEL_MGR_ENV);
   printf ("For Single Ckt operation both MinChannelId & MaxChannelId must be same\n");
   if (argc != 5)
   {
      printf ("Usage %s <Cmd> <MinChannelId> <MaxChannelId> <GUI/CMD-0/1>\n", gProcessName);
      return 1;
   }
   else
   {
      sprintf(gProcessName, "CHMGMT_%s_%s", argv[2], argv[3]);
   }

   gChannelCmd   = (EnumChannelCmd)atoi(argv[1]);
   gMinChannelId = atoi(argv[2]);
   gMaxChannelId = atoi(argv[3]);
   gCallType     = atoi(argv[4]);

   if(gMaxChannelId <= 0 || gMaxChannelId !=  gMinChannelId)
   {
      printf("Invalid Channel Range\n");
      return 1;
   }
   UINT32 lTotalCkts = (gMaxChannelId - gMinChannelId) + 1;

   if(lTotalCkts > MAX_NUM_OF_CKT_CMDS)
   {
      printf("Only %d circuits are allowed at-a-time\n", MAX_NUM_OF_CKT_CMDS);
      return 1;
   }
   IvrUtil::LoadSignals ();

   gLog.SetProcessName (gProcessName);
   printf ("%s: Starting...\n", gProcessName);

   // Initialize log object
   if (true != gLog.Init ())
   {
      printf ("%s: \33[31mCould not initialize log object. Exiting...\33[0m\n",
            gProcessName);
      return 1;
   }

   lProcessLock.SetProdProcName ((TEXT*)IVR_NAME, gProcessName);
   if (LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog (GSYS16);
      printf ("%s: \33[31mProcess already running. Exiting...\33[0m\n",
            gProcessName);
      return 1;
   }
   // Startup log
   gLog.GenerateLog (GSYS01);

   // Set Trace Key
   gTrace.SetTraceKey (TRACE_IVR_CHANNEL_MGR_ENV);

   if (false == lChannelHdlrObj.Init ())
      return 1;

   gLog.GenerateLog (GSYS03);
   printf ("%s: \33[32mInitialization Okay...\33[0m\n", gProcessName);

   lChannelHdlrObj.ProcessRequest(lTotalCkts);
   
   /*switch(gCallType)
   {
       case 0;
            if (true == lChannelHdlrObj.FetchReq (gMinChannelId, gMaxChannelId, lTotalCkts))
            {
               lChannelHdlrObj.ProcessRequest(lTotalCkts);
            }            
            break;
       case 1;
            lChannelHdlrObj.ProcessRequest(lTotalCkts);
            break;
   }*/

   printf ("%s: \33[31mShutting down...\33[0m\n", gProcessName);
   gLog.GenerateLog (GSYS02);

   return 0;
}

