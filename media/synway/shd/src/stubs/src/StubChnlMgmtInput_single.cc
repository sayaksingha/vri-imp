static const char* id="@(#) $Id: StubChnlMgmtInput_single.cc,v 1.1.4.3 2016/04/12 21:59:40 shijuc Exp $" ;
//-----------------------------------------------------------------------------
// NAME           : StubChnlMgmtInput_single.cc
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

int main(int argc,char** argv)
{
	if (argc != 4)
	{
		printf("USAGE: <exe> <req_queue_key> <CKT_ID> <Block-->0/Unblock-->1>\n");
		exit(1);
	}
	MsgQueue             mQueue;
	key_t                mQueKey;

	mQueKey= atoi(argv[1]);
	if((mQueue.Create(mQueKey, IPC_PERM | IPC_CREAT)) != Q_SUCCESS)
	{
		return false;
	}

	CktCmdReq            lrequest;
	msgQueueError        lMsgQueErr;
	INT16                lMgsType = 1;
   INT16                lcmdType = atoi(argv[3]);

	memset(&lrequest,0,sizeof(CktCmdReq));
	switch(lcmdType)
	{
		case 0:
			lrequest.channelId = atoi(argv[2]);
         lrequest.cktCmd = CKT_CMND_BLOCK;
			break;
		case 1:
			lrequest.channelId = atoi(argv[2]);
         lrequest.cktCmd = CKT_CMND_UNBLOCK;
			break;
			break;
		default:
			cout<<"ERROR COMMAND------------------"<<endl;
	}

	lMsgQueErr= mQueue.WriteMsg(&lrequest,sizeof(CktCmdReq), lMgsType,BLOCKING);
	if (Q_SUCCESS != lMsgQueErr)
		printf("Write Error\n");

	return 0;
}
