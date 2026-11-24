// @(#) $Id: IvrChannelApi.h,v 1.1.1.1.2.5.2.1 2016/07/05 02:54:50 mram Exp $
//----------------------------------------------------------------------
// NAME
// IvrChannelApi.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2006
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : KOTHARI KOMAL                                22-OCT-2010
//----------------------------------------------------------------------
#include "Types.h"
#include "IvrUtil.h"
#include "IvrQueApi.h"
#include "IvrDbManager.h"

#ifndef INCL_IVR_CHANNEL_API_H
#define INCL_IVR_CHANNEL_API_H

/*enum EnumIVRChannelStatus
{
	IVR_CHANNEL_UNINITIALIZED   = 0,
	IVR_CHANNEL_FREE            = 1,
	IVR_CHANNEL_IN_CALL         = 2,
	IVR_CHANNEL_REL_IN_PROGRESS = 3,
	IVR_CHANNEL_PLAYING         = 4,
	IVR_CHANNEL_RECORDING       = 5,
	IVR_CHANNEL_UNDEFINED       = 6
};

typedef struct _IvrChannelStat
{
	UINT32                channelNum; // (unsigned int/VXIunsigned)
	UINT32                refNum;
   EnumIVRChannelStatus  status;
} ShmIvrChannelDetails;
*/
# define DOT_LINE  "-------------------------------------------------------------------------"
# define STAR_LINE "*************************************************************************"

class IvrChannelApi
{
	public:
		IvrChannelApi()
		{
			mInit = false;
		}

		~IvrChannelApi()
		{
			mInit = false;
		}

		BOOLEAN Init(BOOLEAN reset=true, UINT32 minChannelId = IVR_MIN_CHANNEL_ID , UINT32 maxChannelId = IVR_MAX_CHANNEL_ID);

		UINT32 SetChannelStatus(UINT32 channelId, EnumIVRChannelStatus status, UINT32 refNum=0);

		UINT32 SetChannelDetails(UINT32 channelId, EnumIVRChannelDirection dir=IVR_CHANNEL_DIR_UNKNOWN, 
            TEXT *clgNum=NULL, TEXT *cldNum=NULL, TEXT *file=NULL, UINT32 refNum=0);

		EnumIVRChannelStatus GetChannelStatus(UINT32 channelId);
		
		EnumIVRChannelStatus GetChannelDetails(UINT32 channelId, ShmIvrChannelDetails &detail);
		
		UINT32  GetReferneceNum(UINT32 channelId);

		static UINT32  AllocateNewReferenceNumber();

		BOOLEAN SendChannelStatus(UINT32 channelId, EnumIVRChannelStatus status, UINT32 refNum=0);

		BOOLEAN ReadChannelStatus(ShmIvrChannelDetails &channelDetail);

		UINT32 GetMinChannelId() { return mMinChannelId; }

		UINT32 GetMaxChannelId() { return mMaxChannelId; }

      //UINT32 GetOutChannelId(TEXT *shortCode=NULL)
      //UINT32 GetOutChannelId(UINT32 appId=-1);
      UINT32 GetOutChannelId(TEXT *appName);

      UINT32 GetOutChannelIdLowestAvailable(TEXT *appName);

      BOOLEAN ReloadConfig();
	private:
      BOOLEAN  mChnlStatusReqd;
		int      mChannelDetailsShmId;
		int      mChannelShmKey;
		
      INT16    mIvrTotalCkts;
      //TEXT     mIvrInCkts[20 + 1];
      //TEXT     mIvrOutCkts[20 + 1];
      
      //int     mIvrMinInCkts;
      //int     mIvrMaxInCkts;
      
      //int     mIvrMinOutCkts;
      //int     mIvrMaxOutCkts;

		INT16    mMinChannelId;
		UINT16    mMaxChannelId;

		static UINT32  mRefNum;

		BOOLEAN mInit;

      IvrChannelConfig      mChannelConfig;
		ShmIvrChannelDetails *mpChannelDetails;

      IvrDbManager          mDbObj;

		IvrQueApi  mQueueApi;
		BOOLEAN CreateShm();

		BOOLEAN ReadConfig();

};

#endif
