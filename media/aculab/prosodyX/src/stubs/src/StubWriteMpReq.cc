static const char* id="@(#) $Id: StubWriteMpReq.cc,v 1.1.4.3 2016/04/12 21:59:33 shijuc Exp $";
//-----------------------------------------------------------------------------
// NAME           : StubWriteMpReq.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Roopa A M                                 Date: AUG-2012
//----------------------------------------------------------------------------
#include "MediaInterface.h"

int gCardId ;
int gFileFormat;

int main(int argc,char** argv)
{
	if (argc != 8)
	{
		printf("USAGE: <exe> <REQ_Q_KEY> <CKT_ID> "
				"\33[33m<PLAY:1 || STOP:2 || CNTSP:3 || DISSP:4 || GETDIG:5 "
				"|| GETDIGSTP:6 || RECSTR:7 || RECSTP:8 || SETVOL:9 || SETSPD:10 || PAUSE:11 || RESUME:12 "
            " FORWARD:13 || REWIND:14 || STREAM:15 || STREAMSTOP:16>\33[0m "
            " <SEQNO> <FILE_CNT> <FILE_FORMAT wav->0 vox->1> <CARDID>\n");
		exit(1);
	}

	MpRequestDetails     lrequest;
	msgQueueError        lMsgQueErr;
	MsgQueue             mQueue;
	key_t                mQueKey;
	INT16                lMsgType = 1;
	//INT16                lMsgType = gCardId + 1;

	mQueKey= atoi(argv[1]);
   gFileFormat = atoi(argv[6]);
	gCardId = atoi(argv[7]);

	if((mQueue.Create(mQueKey, IPC_PERM | IPC_CREAT)) != Q_SUCCESS)
		return false;

	memset(&lrequest,0,sizeof(MpRequestDetails));

	lrequest.cmdType = (EnumMediaCmdTypes)atoi(argv[3]);

	switch(lrequest.cmdType)
	{
		case MC_CNTTN:
			lrequest.mpReqData.playMsg.circuitId = atoi(argv[2]);
			lrequest.mpReqData.playMsg.seqNum = atoi(argv[4]);
			lrequest.mpReqData.playMsg.dtmfEnable = 1;
			lrequest.mpReqData.playMsg.repeatPlay = 0;
         lrequest.mpReqData.playMsg.fileFormat = (EnumMediaFileFormat)gFileFormat;
			strcpy(lrequest.mpReqData.playMsg.dtmfStopPlayCharSet, "a#");

         lrequest.mpReqData.playMsg.vFileCount = atoi(argv[5]);
         for(int i=0; i<atoi(argv[5]); i++)
         {
            TEXT lFileName[MAX_LOG_TEXT_LEN + 1] = "";
            printf("Enter the %d File to Play:", i+1);
            cin>>lFileName;
            sprintf(lrequest.mpReqData.playMsg.voiceFile[i], "%s", lFileName);
            printf("File[%d]:%s\n", i, lrequest.mpReqData.playMsg.voiceFile[i]);
         }
         break;

      case MC_DISTN:
         lrequest.mpReqData.stopMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.stopMsg.seqNum = atoi(argv[4]);
         break;

      case MC_CNTSP:
         lrequest.mpReqData.switchChnl.seqNum = atoi(argv[4]);
         lrequest.mpReqData.switchChnl.circuitId_1 = 1;
         lrequest.mpReqData.switchChnl.circuitId_2 = 29;
         break;

      case MC_DISSP:
         lrequest.mpReqData.switchChnl.seqNum = atoi(argv[4]);
         lrequest.mpReqData.switchChnl.circuitId_1 = 1;
         lrequest.mpReqData.switchChnl.circuitId_2 = 29;
         break;

      case MC_GETDIG:
         lrequest.mpReqData.getDigit.seqNum = atoi(argv[4]);
         lrequest.mpReqData.getDigit.circuitId = atoi(argv[2]);
         lrequest.mpReqData.getDigit.numOfDig = 5;
         lrequest.mpReqData.getDigit.interDigitTimeOut = 10000;
         strcpy(lrequest.mpReqData.getDigit.teminateChars, "a*1");
         lrequest.mpReqData.getDigit.includeTerminateChar = true;
         lrequest.mpReqData.getDigit.maxTime = 40;
         break;

      case MC_GETDIGSTP:
         lrequest.mpReqData.stopMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.stopMsg.seqNum = atoi(argv[4]);
         break;

      case MC_RECSTR:
         lrequest.mpReqData.recordMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.recordMsg.seqNum = atoi(argv[4]);
         lrequest.mpReqData.recordMsg.recTime = 500000;
         sprintf(lrequest.mpReqData.recordMsg.voiceFile, 
               "/opt/ivr/bin/record_MYFILE_%d.wav", atoi(argv[2]));
         strcpy(lrequest.mpReqData.recordMsg.dtmfStopRecCharSet, "0*");

         lrequest.mpReqData.recordMsg.fileFormat = (EnumMediaFileFormat)gFileFormat;
         break;

      case MC_RECSTP:
         lrequest.mpReqData.stopMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.stopMsg.seqNum = atoi(argv[4]);
         break;

      case MC_PAUSE:
         lrequest.mpReqData.pauseMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.pauseMsg.seqNum = atoi(argv[4]);
         break;

      case MC_RESUME:
         lrequest.mpReqData.resumeMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.resumeMsg.seqNum = atoi(argv[4]);
         break;

      case MC_FORWARD:
         lrequest.mpReqData.forwardMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.forwardMsg.seqNum = atoi(argv[4]);
         break;

      case MC_REWIND:
         lrequest.mpReqData.rewindMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.rewindMsg.seqNum = atoi(argv[4]);
         break;

      default:
         cout<<"WRONG COMMAND------------------"<<endl;
   }

   INT16 lMsgLen = sizeof(MpRequestDetails);

   lMsgQueErr= mQueue.WriteMsg(&lrequest, lMsgLen, lMsgType, BLOCKING);

   if (Q_SUCCESS != lMsgQueErr)
      printf("Write Error\n");
   else
      printf("Msg Successfully wrote to Q\n");

   return 0;
}
