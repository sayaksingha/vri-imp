static const char* id="@(#) $Id: AcuWriteReqBulk.cc,v 1.1.4.3 2016/04/12 21:59:29 shijuc Exp $";
//----------------------------------------------------------------
// NAME: WriteReqBulk.cc
//
// COPYRIGHT
// BITL -- Copyright (C) 2006 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// DESCRIPTION
//
// Originated: Roopa A M         
//----------------------------------------------------------------
# include <stdio.h>
# include <iostream.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/msg.h>
# include "MsgQueue.h"
# include "TsStructs.h"
#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>

//g++ -I/home/prbt/cvs/util/src/msgq/include -I/home/prbt/cvs/tts/src/include -I/home/prbt/cvs/util/src/include -I/home/prbt/cvs/util/src/trace/include -I/home/prbt/cvs/util/src/protocolstack/include InReqBulkMsgStub.cc /home/prbt/cvs/util/src/lib/libutil.a  -Wno-deprecated -o Stub_InReqBulkMsg


int main(int argc, char *argv[])
{
   int       lRetval;
   unsigned int       lMsgCount = 0;

   int    lTotNumOfSeconds;
   int     lNumOfMessage;
   struct timeval tval  ;
   double lPreTime;
   double lPostTime;
   int    i ;
   int    lTotalNoOfMsg;
   int   lAppId;
   TsRequestDetails        lrequest;
   TsResponseDetails       lresponse;

   MsgQueue  gWriteMsgQueue;
   msgQueueError  lmMsgQueueErr;
   int           lMsglen;
   int       circuitId = 1;
   int       k = 0;
   int      lCic=0;
   char filename[50];
   char fileName1[50];
   int lCount;
   int lCount1=1;
   int lMsgType;

   if (argc !=6)
   {
      printf("\nInvalid arguments <exe> <ReQueKey> <lNumOfMessage>  <lTotNumOfSeconds> <MaxCic> <FILE_PATH>\n");
      return 0;
   }
   key_t                 mVSQkey = atoi(argv[1]);
   lNumOfMessage = atoi(argv[2]);
   lTotNumOfSeconds = atoi(argv[3]);
   lCic = atoi(argv[4]);
   strcpy(fileName1, argv[5]);

   if (gWriteMsgQueue.Create(mVSQkey, IPC_CREAT|0666,1)!= Q_SUCCESS)
   {
      printf ("Queue creation failed\n");
      return 0;
   }
   printf("\n----------------------------------------------------");
   while( 0  != lTotNumOfSeconds)
   {
      lCount = 0;
      if ( lCount1 == 800000)
         lCount1 = 1;
      gettimeofday(&tval , NULL);
      lPreTime = (tval.tv_sec +tval.tv_usec/1000000.0);
      for (i=0  ; i<lNumOfMessage ;  i++)
      {
         memset(&lrequest,0,sizeof(TsRequestDetails));
         if(circuitId > lCic)
            circuitId = 1;

         if ( circuitId > 32 && circuitId <= 64)
            lMsgType =2;
         else
            lMsgType = 1;
         lrequest.rspMsgType = lMsgType;

         char fileName[100] = "";

         lrequest.circuitId = circuitId;
         lrequest.cmdType = (EnumCmdTypes)4;
         lrequest.seqNum = lCount1;
         sprintf(fileName,"%s%02d.vox",fileName1, circuitId);
         printf("Request SEC: %d MSG_NO: %d CKTID: %d rspMsgType: %d\n",
               lTotNumOfSeconds,(lCount++) + 1, circuitId, lrequest.rspMsgType);

         switch(lrequest.cmdType)
         {
            case CNTTN:
               memcpy(lrequest.requestdata.playToneMsg.voiceFile, fileName, sizeof(fileName));
               lrequest.requestdata.playToneMsg.vFileCount = 1;
               break;

            case DISTN:
               break;

         }

         lmMsgQueueErr= gWriteMsgQueue.WriteMsg(&lrequest,sizeof(TsRequestDetails),lMsgType ,BLOCKING);
         circuitId++;
         lMsgCount++;
         lCount1++;

      }
      gettimeofday(&tval , NULL);
      lPostTime = ( tval.tv_sec +tval.tv_usec/1000000.0 );
      if ( lPostTime  < (lPreTime + 1.00) )
      {
         UINT32 lSleepTime = (UINT32)(((lPreTime +1) -lPostTime)*1000000);
         // printf("\33[5;1;47;37m");
         printf("Sleeping\n");
         usleep(lSleepTime);
         printf("Going out of sleep state");
         // printf("\33[0m");
         printf("\n");
         printf("\n");
         printf("\n");
      }
      lTotNumOfSeconds-- ;
   }
   return 1;
}

