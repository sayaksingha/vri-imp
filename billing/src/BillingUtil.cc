static const char* id="@(#) $Id: BillingUtil.cc,v 1.1.4.3 2016/04/12 21:59:00 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// BillingUtil.cc
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
// Originated :                                              12-AUG-2009
//----------------------------------------------------------------------
#include "BillingUtil.h"

//----------------------------------------------------------------------
// METHOD      : GetVersion
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None 
//----------------------------------------------------------------------
const char* BillingUtil::GetVersion()
{
   return id;
}

//----------------------------------------------------------------------
// METHOD      : ConvertBillingEventTypeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : int
//----------------------------------------------------------------------
TEXT * BillingUtil::ConvertBillingEventTypeToStr (const int event)
{
	switch(event)
	{
		case BILLING_CALL_RECEIVED:
			return (TEXT*)"CALL_RECEIVED";

		case BILLING_CALL_ANSEWERED:
			return (TEXT*)"CALL_ANSEWERED";

		case BILLING_CALL_DISCONNECTED:
			return (TEXT*)"CALL_DISCONNECTED";

		case BILLING_CONTENT_PLAY:
			return (TEXT*)"CONTENT_PLAY";

		case BILLING_CONTENT_START:
			return (TEXT*)"CONTENT_START";

		case BILLING_CONTENT_STOP:
			return (TEXT*)"CONTENT_STOP";

      case BILLING_OFFLINE_DEDICATION:
         return (TEXT*)"DEDICATION";

      case BILLING_OFFLINE:
         return (TEXT*)"OFFLINE";

      case BILLING_TIMER_EXPIRY:
         return (TEXT*)"TIMER_EXPIRY";

		default:
			return (TEXT*)"UNKNOWN_EVENT";
	}
}

//----------------------------------------------------------------------
// METHOD      : ConvertSubsTypeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : int
//----------------------------------------------------------------------
TEXT * BillingUtil::ConvertSubsTypeToStr (const int subsType)
{
	switch(subsType)
	{
		case BILLING_SUBS_TYPE_PREPAID:
			return (TEXT*)"PREPAID";

		case BILLING_SUBS_TYPE_POSTPAID:
			return (TEXT*)"POSTPAID";

		default:
			return (TEXT*)"UNKNOWN";
	}
}

//----------------------------------------------------------------------
// METHOD      : ConvertBillingStatusToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : int
//----------------------------------------------------------------------
TEXT * BillingUtil::ConvertBillingStatusToStr (const int status)
{
	switch(status)
	{
		case BILLING_SUCCESS:
			return (TEXT*)"SUCCESS";

      case BILLING_DISABLED:
         return (TEXT*)"DISABLED";

		case BILLING_FAILED:
			return (TEXT*)"FAILED";

		case BILLING_UNKNOWN_SUBS:
			return (TEXT*)"UNKNOWN_SUBS";

		case BILLING_INVALID_CHARGE_INFO:
			return (TEXT*)"INVALID_CHARGE_INFO";

		case BILLING_SCP_NOT_AVAILABLE:
			return (TEXT*)"SCP_NOT_AVAILABLE";

		case BILLING_TIMEOUT:
			return (TEXT*)"TIMEOUT";

		case BILLING_SYSTEM_FAILURE:
			return (TEXT*)"SYSTEM_FAILURE";

      case BILLING_LOW_BALANCE:
         return (TEXT*)"LOW_BALANCE";

      case BILLING_NO_PACKAGE:
         return (TEXT*)"NO_PACKAGE";

		default:
			return (TEXT*)"UNKNOWN";
	}
}

