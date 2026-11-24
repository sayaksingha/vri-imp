// @(#) $Id: IvrWrapper.h,v 1.1 2011/03/08 08:24:32 root Exp $
//---------------------------------------------------------------
// NAME      IvrWrapper.h
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
//
// DESCRIPTION
//
// Originated : Anupama R                       Date: 21-JAN-2011
//---------------------------------------------------------------
#ifndef INCL_IVR_WRAPPER_H
#define INCL_IVR_WRAPPER_H

#include "IvrUtil.h"
#include "IsupStructs.h"

enum EnumSignalingProtocol
{
	IVR_SIGNALING_UNKNOWN = 0,
	IVR_SIGNALING_ISUP    = 1,
	IVR_SIGNALING_SIP     = 2
};	

class IvrWrapper
{
   public:
      //Constructor
      IvrWrapper();
      
      //Destructor
      ~IvrWrapper();

      //Initialises IvrWrapper Module 
      BOOLEAN Init() ;

		BOOLEAN ReadIsupRequest();

		BOOLEAN ReadAndProcessRequest();

		BOOLEAN ProcessISUPMsg();
		
		BOOLEAN ProcessSIPMsg();

		void ConvertToKernelMsg( GenIsupMsg& ,  KernelMsg& );
	

   private:
      int       mReadQKey;
      MsgQueue  mReadQObj;
		
      int       mWriteQKey;
      MsgQueue  mWriteQObj;
     
		EnumSignalingProtocol  mSignalMsgType;

      BOOLEAN  CreateQueue();
      
      BOOLEAN  ReadConfig() ;
    
		BOOLEAN  WriteMsgToKernelQueue( KernelMsg );

		BOOLEAN ReadRequest();
		
		GenIsupMsg  mGenIsupMsg;
};

#endif
