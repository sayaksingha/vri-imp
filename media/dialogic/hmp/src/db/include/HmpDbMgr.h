// @(#) $Id: HmpDbMgr.h,v 1.1.2.1 2012/02/21 08:31:31 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpDbMgr.h
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
#ifndef INCL_HMP_DB_MGR_H
#define INCL_HMP_DB_MGR_H
#include "SQLAPI.h"
#include "DbConstDef.h"

#include "HmpConstDef.h"
#include "HmpStructs.h"
#include "HmpUtil.h"

#define HMP_DB_NAME          "bplora"
#define HMP_MAX_DB_NAME_LEN  20
#define HMP_DB_USER          "hmp"
#define HMP_MAX_DB_USER_LEN  20
#define HMP_DB_PASS          "hmp"
#define HMP_MAX_DB_PASS_LEN  20
#define HMP_MAX_SQL_STR_LEN  750

class HmpDbMgr
{
   public:
      HmpDbMgr();

      ~HmpDbMgr();

      DbErrorCodes   Init(TEXT * DbUserName = "", TEXT * DbUserPass = "",
               TEXT * DbName = "");

      DbErrorCodes   DeInit ();

      DbErrorCodes InsertHmpProvEvr (const HmpEvrMsg & );

      DbErrorCodes ReadCfgNum(const TEXT*, INT32&, INT32, INT32);

      DbErrorCodes ReadCfgStr(const TEXT*, TEXT*, INT32, INT32);

   protected:
      DbErrorCodes   CheckDbStatus ();

      DbErrorCodes   ErrorHandler (SAException & err, TEXT * funName = "",
               TEXT * debugInfo = "");

      SAConnection mConnection;
      SACommand mCommand;

      TEXT mDbName[HMP_MAX_DB_NAME_LEN + 1];
      TEXT mDbUser[HMP_MAX_DB_USER_LEN + 1];
      TEXT mDbPass[HMP_MAX_DB_PASS_LEN + 1];

};
#endif
