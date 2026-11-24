static const char* id="@(#) $Id: StubReadModReqMsg.cc,v 1.1.4.3 2016/04/12 21:59:33 shijuc Exp $" ;
//-----------------------------------------------------------------------------
// NAME           : StubMPOutput.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Chethan V M           Date:
//----------------------------------------------------------------------------
#include "MediaInterface.h"

int gBoardId ;

int main(int argc,char** argv)
{
	if (argc != 2)
	{
		printf("USAGE: <exe> <req_queue_key> ");
		exit(1);
	}
	MsgQueue             mQueue;
	key_t                mQueKey;

	mQueKey= atoi(argv[1]);
	if((mQueue.Create(mQueKey, IPC_PERM | IPC_CREAT)) != Q_SUCCESS)
	{
		return false;
	}

   MpRequestDetails     lreq;
	msgQueueError        lMsgQueErr;
	LONG                lMgsType = 0;
	INT16						lMsgLen = sizeof(MpRequestDetails);


while(true)
{
	lMgsType = 0;
	memset(&lreq ,0,sizeof(MpRequestDetails));

	lMsgQueErr= mQueue.ReadMsgWithType(&lreq , lMsgLen, lMgsType,BLOCKING);
	if (Q_SUCCESS != lMsgQueErr)
	{
		printf("Write Error\n");
		return false;
	}

	printf("Msg Read Typ:%ld ", lMgsType);
	switch(lreq.cmdType)
	{
		case MC_CNTTN:
			printf("PLAY  CID:%d SEQ:%d FC:%d\n",
					//(MP_RSP_SUCCESS == lreq.rspMsg) ? "SUCCESS" : "FAILURE",
					lreq.mpReqData.playMsg.circuitId,
					lreq.mpReqData.playMsg.seqNum,
					lreq.mpReqData.playMsg.vFileCount);
			break;
		case MC_DISTN:
			printf("PLAY STOP CID:%d SEQ:%d\n",
					//(MP_RSP_SUCCESS == lreq.rspMsg) ? "SUCCESS" : "FAILURE",
					lreq.mpReqData.stopMsg.circuitId,
					lreq.mpReqData.stopMsg.seqNum);
			break;
         #if 0
		case MC_CNTSP:
			printf("CNTSP %s CID:%d CID:%d SEQ:%d\n",
					(MP_RSP_SUCCESS == lreq.rspMsg) ? "SUCCESS" : "FAILURE",
					lreq.mpreqData.switchChnl.circuitId_1,
					lreq.mpreqData.switchChnl.circuitId_2,
					lreq.mpreqData.switchChnl.seqNum);
			break;
		case MC_DISSP:
			printf("DISSP %s CID:%d CID:%d SEQ:%d\n",
					(MP_RSP_SUCCESS == lreq.rspMsg) ? "SUCCESS" : "FAILURE",
					lreq.mpreqData.switchChnl.circuitId_1,
					lreq.mpreqData.switchChnl.circuitId_2,
					lreq.mpreqData.switchChnl.seqNum);
			break;
		case MC_GETDIG:
			printf("GETDIG  %s CID:%d SEQ:%d NOD:%d IDT:%ld TC:%s ITC:%d CHARS:%s\n",
					(MP_RSP_SUCCESS == lreq.rspMsg) ? "SUCCESS" : "FAILURE",
					lreq.mpreqData.getDigit.circuitId,
					lreq.mpreqData.getDigit.seqNum,
					lreq.mpreqData.getDigit.numOfDig,
					lreq.mpreqData.getDigit.interDigitTimeOut,
					lreq.mpreqData.getDigit.teminateChars,
					lreq.mpreqData.getDigit.includeTerminateChar,
					lreq.mpreqData.getDigit.rspDigits);
			break;
		case MC_GETDIGSTP:
			printf("GETDIGSTP  %s CID:%d SEQ:%d NOD:%d IDT:%ld TC:%s ITC:%d CHARS:%s\n",
					(MP_RSP_SUCCESS == lreq.rspMsg) ? "SUCCESS" : "FAILURE",
					lreq.mpreqData.getDigit.circuitId,
					lreq.mpreqData.getDigit.seqNum,
					lreq.mpreqData.getDigit.numOfDig,
					lreq.mpreqData.getDigit.interDigitTimeOut,
					lreq.mpreqData.getDigit.teminateChars,
					lreq.mpreqData.getDigit.includeTerminateChar,
					lreq.mpreqData.getDigit.rspDigits);
			break;
		case MC_RECSTR:
			printf("RECSTR %s CID:%d SEQ:%d RECTM:%ld FN:%s\n",
					(MP_RSP_SUCCESS == lreq.rspMsg) ? "SUCCESS" : "FAILURE",
					lreq.mpreqData.recordMsg.circuitId,
					lreq.mpreqData.recordMsg.seqNum,
					lreq.mpreqData.recordMsg.recTime,
					lreq.mpreqData.recordMsg.voiceFile);
			break;
		case MC_RECSTP:
			printf("RECSTP %s CID:%d SEQ:%d RECTM:%ld FN:%s\n",
					(MP_RSP_SUCCESS == lreq.rspMsg) ? "SUCCESS" : "FAILURE",
					lreq.mpreqData.recordMsg.circuitId,
					lreq.mpreqData.recordMsg.seqNum,
					lreq.mpreqData.recordMsg.recTime,
					lreq.mpreqData.recordMsg.voiceFile);
			break;
         #endif
		default:
			cout<<"ERROR COMMAND------------------"<<endl;
	}
}

	return 0;
}






































