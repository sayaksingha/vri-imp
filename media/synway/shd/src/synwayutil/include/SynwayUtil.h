// @(#) $Id: SynwayUtil.h,v 1.1.4.3 2016/04/12 21:59:40 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       SynwayUtil.h
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
// AUTHOR         : Hmp                                  23-Nov-2011
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_SYNWAY_UTIL_DEF_H
#define INCL_SYNWAY_UTIL_DEF_H

#include "SynwayStructs.h"

class SynwayUtil : public MediaUtil
{
   public:

   SynwayUtil();
   ~SynwayUtil();

   static TEXT* ConvertStatusToStr(EnumChannelStatus);

   static TEXT* ConvertSnwSigLinkStToStr(DWORD);

   static TEXT* ConvertChnlStToStr(int);

   static TEXT* ConvertSnwDialStToStr(const DWORD);

   static TEXT* ConvertSigStatusToStr(EnumSigChannelStatus);

   static TEXT* ConvertIsupMsgToStr(const GenIsupMsgType);

   static TEXT* ConvertCktCmdReqToStr(const EnumCktCmd);

   static TEXT* ConvertCktCmdStToStr(const EnumCktCmdStatus);

   static TEXT* ConvertProcSigChnlStToStr(const EnumSigChannelStatus);

   static TEXT* ConvertPlayStopResToStr(const DWORD dwParam);

   static TEXT* ConvertRecordStopResToStr(const DWORD dwParam);

   static TEXT* ConvertGetDtmfStopResToStr(const DWORD dwParam);

   static TEXT* ConvertCallPendingResToStr(const int pedRes);

   static TEXT* ConvertProcCallDirToStr(const EnumSnwCallDirection);

   protected:
};
#endif
