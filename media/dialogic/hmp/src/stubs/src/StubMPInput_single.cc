static const char* id="@(#) $Id: StubMPInput_single.cc,v 1.1.2.1 2012/02/21 08:31:40 shijuc Exp $" ;
//-----------------------------------------------------------------------------
// NAME           : StubMPInput_single.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Chethan V M           Date:
//----------------------------------------------------------------------------
#include "HmpUtil.h"

int gBoardId ;
int gFileFormat;

int main(int argc,char** argv)
{
	if (argc < 8)
	{
		printf("USAGE: <exe> <req_queue_key> <CKT_ID> "
				"<PLAY:1||STOP:2||CNTSP:3||DISSP:4||GETDIG:5"
				"||GETDIGSTP:6||RECSTR:7||RECSTP:8||SETVOL:9||SETSPD:10||STREAM:15||STREAMSTOP:16> "
				"<SEQNO> <FILE_NAME> <FILE_FORMAT wav->0 vox->1> <BOARDID> <Url>\n");
		exit(1);
	}
	MsgQueue             mQueue;
	key_t                mQueKey;

	mQueKey= atoi(argv[1]);
   gFileFormat = atoi(argv[6]);
	gBoardId = atoi(argv[7]);
	if((mQueue.Create(mQueKey, IPC_PERM | IPC_CREAT)) != Q_SUCCESS)
	{
		return false;
	}

	MpRequestDetails     lrequest;
	msgQueueError        lMsgQueErr;
	INT16                lMgsType = gBoardId + 1;

	memset(&lrequest,0,sizeof(MpRequestDetails));
	lrequest.cmdType = (EnumMediaCmdTypes)atoi(argv[3]);
	switch(lrequest.cmdType)
	{
		case MC_CNTTN:
			lrequest.mpReqData.playMsg.circuitId = atoi(argv[2]);
			lrequest.mpReqData.playMsg.seqNum = atoi(argv[4]);
			lrequest.mpReqData.playMsg.dtmfEnable = 1;
			lrequest.mpReqData.playMsg.repeatPlay = 1;
         lrequest.mpReqData.playMsg.fileFormat = (EnumMediaFileFormat)gFileFormat;
			strcpy(lrequest.mpReqData.playMsg.dtmfStopPlayCharSet, "a#");
			strcpy(lrequest.mpReqData.playMsg.voiceFile[0], argv[5]);
			strcpy(lrequest.mpReqData.playMsg.voiceFile[1], argv[5]);
			strcpy(lrequest.mpReqData.playMsg.voiceFile[2], argv[5]);
			lrequest.mpReqData.playMsg.vFileCount = 1;
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
			lrequest.mpReqData.recordMsg.recTime = 10000;
			sprintf(lrequest.mpReqData.recordMsg.voiceFile, 
					"/opt/ivr/bin/record_MYFILE_%d.wav", atoi(argv[2]));
         strcpy(lrequest.mpReqData.recordMsg.dtmfStopRecCharSet, "0*");

         lrequest.mpReqData.recordMsg.fileFormat = (EnumMediaFileFormat)gFileFormat;
			break;
		case MC_RECSTP:
			lrequest.mpReqData.stopMsg.circuitId = atoi(argv[2]);
			lrequest.mpReqData.stopMsg.seqNum = atoi(argv[4]);
			break;
      case MC_STREAM:
         lrequest.mpReqData.stream.circuitId = atoi(argv[2]);
         lrequest.mpReqData.stream.seqNum = atoi(argv[4]);
         strcpy(lrequest.mpReqData.stream.rtspUrl, argv[8]);
      case MC_STREAM_STOP:
         lrequest.mpReqData.stopMsg.circuitId = atoi(argv[2]);
         lrequest.mpReqData.stopMsg.seqNum = atoi(argv[4]);
         break;
		default:
			cout<<"ERROR COMMAND------------------"<<endl;
	}

	lMsgQueErr= mQueue.WriteMsg(&lrequest,sizeof(MpRequestDetails), lMgsType,BLOCKING);
	if (Q_SUCCESS != lMsgQueErr)
		printf("Write Error\n");

	return 0;
}
