static const char* id="@(#) $Id: StubMPOutput_Load.cc,v 1.1.4.3 2016/04/12 21:59:40 shijuc Exp $" ;
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
int gmincic = 0;
int gmaxcic = 0;

int main(int argc,char** argv)
{
	if (argc != 4)
	{
		printf("USAGE: <exe> <req_queue_key> <min cic> <max cic>");
		exit(1);
	}
	MsgQueue             mQueue;
	key_t                mQueKey;
   gmincic = atoi(argv[2]);
   gmaxcic = atoi(argv[3]);

	mQueKey= atoi(argv[1]);
	if((mQueue.Create(mQueKey, IPC_PERM | IPC_CREAT)) != Q_SUCCESS)
	{
		return false;
	}

	MpResponseDetails		lresponse;
	msgQueueError        lMsgQueErr;
	LONG                lMgsType = 0;
	INT16						lMsgLen = sizeof(MpResponseDetails);

   lMgsType = gmincic;

while(true)
{
   if(lMgsType > gmaxcic)
      lMgsType = gmincic;
	memset(&lresponse ,0,sizeof(MpResponseDetails));

   
	lMsgQueErr= mQueue.ReadMsg(&lresponse , lMsgLen, lMgsType, NON_BLOCKING);
	if (Q_SUCCESS != lMsgQueErr)
	{
      lMsgLen = sizeof(MpResponseDetails);
      if(Q_NO_MSG_WITH_TYPE != lMsgQueErr)
         printf("Read Error :%ld  %d\n", lMgsType, lMsgQueErr);
      lMgsType++;
      usleep(20);
      continue;
	}

	printf("Msg Read Typ:%ld ", lMgsType);
   lMgsType++;
	switch(lresponse.cmdType)
	{
		case MC_CNTTN:
			printf("PLAY %s CID:%d SEQ:%d FC:%d\n",
					(MP_RSP_SUCCESS == lresponse.rspMsg) ? "SUCCESS" : "FAILURE",
					lresponse.mpRspData.playMsg.circuitId,
					lresponse.mpRspData.playMsg.seqNum,
					lresponse.mpRspData.playMsg.vFileCount);
			break;
		case MC_DISTN:
			printf("PLAY STOP %s CID:%d SEQ:%d\n",
					(MP_RSP_SUCCESS == lresponse.rspMsg) ? "SUCCESS" : "FAILURE",
					lresponse.mpRspData.stopMsg.circuitId,
					lresponse.mpRspData.stopMsg.seqNum);
			break;
		case MC_CNTSP:
			printf("CNTSP %s CID:%d CID:%d SEQ:%d\n",
					(MP_RSP_SUCCESS == lresponse.rspMsg) ? "SUCCESS" : "FAILURE",
					lresponse.mpRspData.switchChnl.circuitId_1,
					lresponse.mpRspData.switchChnl.circuitId_2,
					lresponse.mpRspData.switchChnl.seqNum);
			break;
		case MC_DISSP:
			printf("DISSP %s CID:%d CID:%d SEQ:%d\n",
					(MP_RSP_SUCCESS == lresponse.rspMsg) ? "SUCCESS" : "FAILURE",
					lresponse.mpRspData.switchChnl.circuitId_1,
					lresponse.mpRspData.switchChnl.circuitId_2,
					lresponse.mpRspData.switchChnl.seqNum);
			break;
		case MC_GETDIG:
			printf("GETDIG  %s CID:%d SEQ:%d NOD:%d IDT:%d TC:%s ITC:%d CHARS:%s\n",
					(MP_RSP_SUCCESS == lresponse.rspMsg) ? "SUCCESS" : "FAILURE",
					lresponse.mpRspData.getDigit.circuitId,
					lresponse.mpRspData.getDigit.seqNum,
					lresponse.mpRspData.getDigit.numOfDig,
					lresponse.mpRspData.getDigit.interDigitTimeOut,
					lresponse.mpRspData.getDigit.teminateChars,
					lresponse.mpRspData.getDigit.includeTerminateChar,
					lresponse.mpRspData.getDigit.rspDigits);
			break;
		case MC_GETDIGSTP:
			printf("GETDIGSTP  %s CID:%d SEQ:%d NOD:%d IDT:%d TC:%s ITC:%d CHARS:%s\n",
					(MP_RSP_SUCCESS == lresponse.rspMsg) ? "SUCCESS" : "FAILURE",
					lresponse.mpRspData.getDigit.circuitId,
					lresponse.mpRspData.getDigit.seqNum,
					lresponse.mpRspData.getDigit.numOfDig,
					lresponse.mpRspData.getDigit.interDigitTimeOut,
					lresponse.mpRspData.getDigit.teminateChars,
					lresponse.mpRspData.getDigit.includeTerminateChar,
					lresponse.mpRspData.getDigit.rspDigits);
			break;
		case MC_RECSTR:
			printf("RECSTR %s CID:%d SEQ:%d RECTM:%d FN:%s\n",
					(MP_RSP_SUCCESS == lresponse.rspMsg) ? "SUCCESS" : "FAILURE",
					lresponse.mpRspData.recordMsg.circuitId,
					lresponse.mpRspData.recordMsg.seqNum,
					lresponse.mpRspData.recordMsg.recTime,
					lresponse.mpRspData.recordMsg.voiceFile);
			break;
		case MC_RECSTP:
			printf("RECSTP %s CID:%d SEQ:%d RECTM:%d FN:%s\n",
					(MP_RSP_SUCCESS == lresponse.rspMsg) ? "SUCCESS" : "FAILURE",
					lresponse.mpRspData.recordMsg.circuitId,
					lresponse.mpRspData.recordMsg.seqNum,
					lresponse.mpRspData.recordMsg.recTime,
					lresponse.mpRspData.recordMsg.voiceFile);
			break;
		default:
			cout<<"ERROR COMMAND------------------"<<endl;
	}

}

	return 0;
}






































