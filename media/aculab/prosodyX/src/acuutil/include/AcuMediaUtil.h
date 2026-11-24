// @(#) $Id: AcuMediaUtil.h,v 1.1.4.3 2016/04/12 21:59:26 shijuc Exp $"
//----------------------------------------------------------------------
// NAME
//       AcuMediaUtil.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Roopa A M                            23-JUL-2012
//----------------------------------------------------------------------
#ifndef INCL_ACUMEDIA_UTIL_DEF_H
#define INCL_ACUMEDIA_UTIL_DEF_H

#include "acu_type.h"
#include "res_lib.h"
#include "cl_lib.h"
#include "sw_lib.h"
#include "iptel_lib.h"
#include "smclib.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smrtp.h"
#include "smbesp.h"       
#include "smhlib.h"        
#include "smfaxapi.h"
#include "smwavlib.h"      
#include "MediaUtil.h"
#include "AcuMediaLimits.h"
#include "AcuMediaConstDef.h"
#include "AcuMediaLogCodes.h"
#include "AcuMediaStructs.h"
#include "MediaInterface.h"

class AcuMediaUtil : public MediaUtil
{
   public:
     static TEXT* ConvertPlayStatusToStr(EnumAcuPlaystatus playStatus);

     static TEXT* ConvertFaxRcToStr(int lRc);

     static TEXT* ConvertPlayCtrlsToStr(EnumAcuPlayCtrl playCtrl);

     static TEXT* ConvertTingTypeToStr(EnumAcuTingTrace tingTrace);

     static TEXT* ConvertMediaTypeToStr(EnumAcuMediaTrace mediaTrace);

     static TEXT* ConvertDataTypeToStr(kSMDataFormat dataFormat);

   protected:
};
#endif
