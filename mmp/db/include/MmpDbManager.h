// @(#) $Id: MmpDbManager.h,v 1.1.4.3 2016/04/12 21:59:41 shijuc Exp $
//-----------------------------------------------------------------------------
// NAME           : MmpDbManager.h
//
// COPYRIGHT      : TSS -- Copyright (C) 2011 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Divya K                               Dated : 24-JAN-2012
// Modified       : Roopa A M                                     14-MAR-2012
//-----------------------------------------------------------------------------

#ifndef MMP_DBMGR_H
#define MMP_DBMGR_H

#include <vector>

#include "SQLAPI.h"
#include "MmpStructs.h"
#include "MmpLimits.h"
#include "IvrUtil.h"
#include "MmpUtil.h"
#include "BillingStructs.h" 
#include <stdlib.h>

#define MAX_DB_USER 		TSS_MAX_DB_USER_LEN
#define MAX_DB_PASS 		TSS_MAX_DB_PASS_LEN
#define MAX_SQL_STR_LEN		TSS_MAX_SQL_STR_LEN

#define MMP_PROV_VAL_REG_GET_DFT_OUT_PARAMS \
   provDetails.evrReason = lsCmd.Param ("o_result").asShort ();\
   provDetails.subsInfo.status = (EnumMmpSubsStatus) lsCmd.Param ("o_subs_status").asShort ();\
   provDetails.subsInfo.subsType = (EnumBillingSubsType) lsCmd.Param ("i_subsType").asShort ();\
   provDetails.pkgId = lsCmd.Param ("io_pkgid").asLong ();\
   provDetails.subsInfo.pkgInfo.pkgName.length = (UINT8)lsCmd.Param ("o_pkg_length").asShort ();\
      memcpy(&provDetails.subsInfo.pkgInfo.pkgName.data,\
      lsCmd.Param ("o_pkg_name").asBLob (), provDetails.subsInfo.pkgInfo.pkgName.length);\
   T(gTrace, printf ("=========================\n");\
         MMP_PRINT_SPACE;\
         printf ("%s: Result-------------:%d[\33[32m%s\33[0m]\n",\
            gProcessName, provDetails.evrReason,\
            IvrUtil::ConvertEvrReasonToStr(provDetails.evrReason));\
         MMP_PRINT_SPACE;\
         printf ("%s: SubsStatus---------:%d[%s]\n",\
            gProcessName, provDetails.subsInfo.status,\
            MmpUtil::ConvertSubsStatusToStr(provDetails.subsInfo.status));\
         MMP_PRINT_SPACE;\
         printf ("%s: SubsciptionType----:%d[%s]\n", gProcessName,\
            provDetails.subsInfo.subsType, MmpUtil::ConvertSubsTypeToStr(provDetails.subsInfo.subsType));\
         MMP_PRINT_SPACE;\
        printf ("%s: PkgName------------:%s[%d]", gProcessName,\
            provDetails.subsInfo.pkgInfo.pkgName.data,\
            provDetails.subsInfo.pkgInfo.pkgName.length);\
        printf ("\n");\
        MMP_PRINT_SPACE;\
        printf ("%s: MmpPkgId-----------:%d\n",\
            gProcessName, provDetails.pkgId););

#define MMP_GET_DATE_PARAMS \
   provDetails.subsInfo.validityDate = (struct tm)lsCmd.Param ("o_validitydate").asDateTime ();\
   provDetails.subsInfo.expiryDate = (struct tm)lsCmd.Param ("o_expirydate").asDateTime ();\
   provDetails.subsInfo.dailyStartTime = (struct tm)lsCmd.Param ("o_daily_start_time").asDateTime ();\
   provDetails.subsInfo.dailyEndTime = (struct tm)lsCmd.Param ("o_daily_end_time").asDateTime ();\
   T(gTrace, printf ("%s: ValidityDate-------:%02d/%02d/%04d %02d:%02d:%02d\n",\
            gProcessName, provDetails.subsInfo.validityDate.tm_mday,\
            provDetails.subsInfo.validityDate.tm_mon + 1,\
            provDetails.subsInfo.validityDate.tm_year + 1900,\
            provDetails.subsInfo.validityDate.tm_hour,\
            provDetails.subsInfo.validityDate.tm_min,\
            provDetails.subsInfo.validityDate.tm_sec);\
         MMP_PRINT_SPACE;\
         printf ("%s: ExpiryDate---------:%02d/%02d/%04d %02d:%02d:%02d\n",\
            gProcessName, provDetails.subsInfo.expiryDate.tm_mday,\
            provDetails.subsInfo.expiryDate.tm_mon + 1,\
            provDetails.subsInfo.expiryDate.tm_year + 1900,\
            provDetails.subsInfo.expiryDate.tm_hour,\
            provDetails.subsInfo.expiryDate.tm_min,\
            provDetails.subsInfo.expiryDate.tm_sec);\
         MMP_PRINT_SPACE;\
         printf ("%s: DailyStrtTime------:%02d/%02d/%04d %02d:%02d:%02d\n",\
            gProcessName, provDetails.subsInfo.dailyStartTime.tm_mday,\
            provDetails.subsInfo.dailyStartTime.tm_mon + 1,\
            provDetails.subsInfo.dailyStartTime.tm_year + 1900,\
            provDetails.subsInfo.dailyStartTime.tm_hour,\
            provDetails.subsInfo.dailyStartTime.tm_min,\
            provDetails.subsInfo.dailyStartTime.tm_sec);\
         MMP_PRINT_SPACE;\
         printf ("%s: DailyEndTime-------:%02d/%02d/%04d %02d:%02d:%02d\n",\
            gProcessName, provDetails.subsInfo.dailyEndTime.tm_mday,\
            provDetails.subsInfo.dailyEndTime.tm_mon + 1,\
            provDetails.subsInfo.dailyEndTime.tm_year + 1900,\
            provDetails.subsInfo.dailyEndTime.tm_hour,\
            provDetails.subsInfo.dailyEndTime.tm_min,\
            provDetails.subsInfo.dailyEndTime.tm_sec););

#define MMP_GET_VALIDITY_MGR_PARAMS \
   lValidityReq.provDetails.subsInfo.subsType = (EnumBillingSubsType) lsCmd.Param ("o_subs_type").asShort ();\
      lValidityReq.provDetails.subsInfo.pkgInfo.pkgId = lsCmd.Param ("o_pkgid").asLong ();\
      lValidityReq.provDetails.subsInfo.subsId        = lsCmd.Param ("o_subsid").asLong ();\
      lValidityReq.provDetails.subsInfo.validityDate  = lsCmd.Param ("o_validity_date").asDateTime ();\
      lValidityReq.provDetails.subsInfo.expiryDate    = lsCmd.Param ("o_expiry_date").asDateTime ();\
      lValidityReq.backlog          = lsCmd.Param ("o_backlog").asShort ();\
      lValidityReq.totalRent        = lsCmd.Param ("o_rent").asShort ();\
      lValidityReq.dailyRent        = lsCmd.Param ("o_daily_renew_rent").asShort ();\
      lValidityReq.pkgRenewAmt      = lsCmd.Param ("o_renew_amt").asShort ();\
      lValidityReq.pkgRenewInterval = lsCmd.Param ("o_renew_interval").asShort ();\
      lValidityReq.gracePeriod      = lsCmd.Param ("o_grace_period").asShort ();\
      lValidityReq.sendSmsFlg       = lsCmd.Param ("o_sms_flg").asShort ();\
      lValidityReq.expired          = lsCmd.Param ("o_expired").asShort ();\
      lValidityReq.provDetails.subsInfo.languageId = lsCmd.Param ("o_languageid").asDateTime ();\
      lValidityReq.provDetails.subsInfo.pkgInfo.pkgName.length = (UINT8)lsCmd.Param ("o_pkg_length").asShort ();\
      memcpy(&lValidityReq.provDetails.subsInfo.pkgInfo.pkgName.data,\
      lsCmd.Param ("o_pkg_name").asBLob (), lValidityReq.provDetails.subsInfo.pkgInfo.pkgName.length);\
      T (gTrace, printf("%s: =================================\n", gProcessName);\
            MMP_PRINT_SPACE;\
            printf ("%s: SubsType-----------:%d\n",\
               gProcessName, lValidityReq.provDetails.subsInfo.subsType);\
            MMP_PRINT_SPACE;\
            printf ("%s: SubsId-------------:%d\n",\
               gProcessName, lValidityReq.provDetails.subsInfo.subsId);\
            MMP_PRINT_SPACE;\
            printf ("%s: PkgId--------------:%d\n",\
               gProcessName, lValidityReq.provDetails.subsInfo.pkgInfo.pkgId);\
            MMP_PRINT_SPACE;\
            printf ("%s: PkgName------------:%s[%d]", gProcessName,\
            lValidityReq.provDetails.subsInfo.pkgInfo.pkgName.data,\
            lValidityReq.provDetails.subsInfo.pkgInfo.pkgName.length);\
            printf ("\n");\
            MMP_PRINT_SPACE;\
            printf ("%s: RenewPkgAmt--------:%d\n",\
               gProcessName, lValidityReq.pkgRenewAmt);\
            MMP_PRINT_SPACE;\
            printf ("%s: RenewInterval------:%d\n",\
               gProcessName, lValidityReq.pkgRenewInterval);\
            MMP_PRINT_SPACE;\
            printf ("%s: RenewDailyRent-----:%d\n",\
                  gProcessName, lValidityReq.dailyRent);\
            MMP_PRINT_SPACE;\
            printf ("%s: Backlog------------:%d\n",\
                  gProcessName, lValidityReq.backlog);\
            MMP_PRINT_SPACE;\
            printf ("%s: TotalRent----------:%d\n",\
                  gProcessName, lValidityReq.totalRent);\
            MMP_PRINT_SPACE;\
            printf ("%s: GracePeriod--------:%d\n",\
                  gProcessName, lValidityReq.gracePeriod);\
            MMP_PRINT_SPACE;\
            printf ("%s: SmsFlag------------:%d\n",\
                  gProcessName, lValidityReq.sendSmsFlg);\
            MMP_PRINT_SPACE;\
            printf ("%s: Expired------------:%d\n",\
                  gProcessName, lValidityReq.expired);\
            MMP_PRINT_SPACE;\
            printf ("%s: Validity dd/mm/yyyy:%02d/%02d/%04d\n",\
                  gProcessName, lValidityReq.provDetails.subsInfo.validityDate.tm_mday,\
                  1 + lValidityReq.provDetails.subsInfo.validityDate.tm_mon,\
                  lValidityReq.provDetails.subsInfo.validityDate.tm_year + 1900);\
            MMP_PRINT_SPACE;\
            printf ("%s: Expiry  dd/mm/yyyy:%02d/%02d/%04d\n",\
                  gProcessName, lValidityReq.provDetails.subsInfo.expiryDate.tm_mday,\
                  1 + lValidityReq.provDetails.subsInfo.expiryDate.tm_mon,\
                  lValidityReq.provDetails.subsInfo.expiryDate.tm_year + 1900););

using namespace std;

class  MmpDbManager
{
   public:

      MmpDbManager();

      ~MmpDbManager();

      //----------------------------------------------------------------
      DbErrorCodes   Connect();

      DbErrorCodes  Init(int lDbClient);
      DbErrorCodes  Init(TEXT* lDbName = "", TEXT* lUser = "", 
            TEXT* lPasswd = "", int lDbClient = 1);

      DbErrorCodes  Disconnect();

      DbErrorCodes   CheckDBStatus();

      void           CommitNow();

      //----------------------------------------------------------------
      DbErrorCodes    ErrorHandler(SAException&  err,TEXT*  funcName,TEXT* debugInfo = "");

      //----------------------------------------------------------------
      DbErrorCodes    ReadCfgNum (const TEXT * key, INT32 & Value, INT32 minVal, INT32 maxVal);

      DbErrorCodes    ReadCfgStr (const TEXT * key, TEXT * Value, INT32 minVal, INT32 maxVal);

      DbErrorCodes    FetchDedications(DedicationReq& lDedicationReq, INT32&, TEXT* lSysStartTime, TEXT* lSysEndTime);

      DbErrorCodes    ScheduleDedications(DedicationReq& lDedicationReq, TEXT *appl);

      //----------------------------------------------------------------
      DbErrorCodes GetAllInterfaceDetails (GgiInterfaceDetails &
            rInterfaceDetails);

      DbErrorCodes InsertInterfaceResponse (GgiInterfaceRsp lInterfaceRspMsg);

      //----------------------------------------------------------------
      DbErrorCodes GetProvKeyDetails (BINARY & provKey,
            MmpProvKey & rProvKeyDetails);

      DbErrorCodes MmpGetSubsImsi (TEXT * msisdn, TEXT * imsi);

      DbErrorCodes GetMmpSubsDetails (TEXT * msisdn, MmpSubsInfo & subsInfo);

      DbErrorCodes DeletePreviousProfile (TEXT * msisdn);

      //----------------------------------------------------------------
      DbErrorCodes    PollScheduleResponse(DedicationRsp& lDedicationRsp);

      DbErrorCodes    UpdateDedicationRsp(DedicationRsp& lDedicationRsp);

      DbErrorCodes    UpdateBillingRsp(UINT32& lId);

      DbErrorCodes    FetchNotifications(NotiReq& lNotiReq);

      DbErrorCodes    UpdateNotiStatus(EnumNotiStatus , UINT32&);

      //-------------------- Validation Specific -----------------------
      DbErrorCodes   MmpValidateRegPkg (MmpSubsProvDetails & provDetails);

      DbErrorCodes   MmpValidateDereg (MmpSubsProvDetails & provDetails);

      DbErrorCodes   MmpValidateRenew (MmpSubsProvDetails & provDetails);

      DbErrorCodes   MmpValidateDedicate (MmpSubsProvDetails & provDetails, UINT32 & lContentId);

      DbErrorCodes   MmpValidateGift (MmpSubsProvDetails & provDetails);

      DbErrorCodes   MmpUpdateValidityDate(UINT32, INT16, INT16);

      DbErrorCodes   MmpRevertValidityDate (UINT32 lSubsId, INT16 lNumOfDays, INT16 lKeyword);

      DbErrorCodes   MmpUpdateDailyBillFlg(UINT32 lSubsId, INT16 & lDailyBillFlg);

      //-------------------- Provisioning Specific ---------------------
      DbErrorCodes   MmpReg (MmpSubsProvDetails & provDetails);

      DbErrorCodes   MmpDereg (MmpSubsProvDetails & provDetails);

      DbErrorCodes   MmpRenew (MmpSubsProvDetails & provDetails);

      DbErrorCodes   MmpDedicate (MmpSubsProvDetails & provDetails);

      DbErrorCodes   MmpGift (MmpSubsProvDetails & provDetails);

      DbErrorCodes   GetSubsPackage(TEXT * subsNo, EnumMmpPkgType , MmpSubsInfo &, UINT32);

      DbErrorCodes   GetSubsPackage(TEXT * subsNo, EnumMmpPkgType , SubsPackageInfo &, UINT32);

      DbErrorCodes   ChargeFromPackage(UINT32 subscriptionId, EnumMmpPkgType packageType, 
            UINT32 usage, SubsPackageInfo &packageInfo);

      DbErrorCodes   MmpGetDailyDetails(TEXT*, MmpValidityReq&);

      DbErrorCodes   GetContentInfo (TEXT *lData, UINT32 & contentId);

      DbErrorCodes   MmpSubsStatusUpdate(TEXT * lMsisdn);

      DbErrorCodes   UpdateDedicationStatus(EnumDedicationStatus lStatus, UINT32& lId);

   public:
      SAConnection    mConnection;
      SACommand       mCommand;
      SAClient_t      mDbClient;

      TEXT            mDbName[IVR_MAX_DB_NAME_LEN];                  // Database Name
      TEXT            mDbUser[MAX_DB_USER];                  // User Name
      TEXT            mDbPass[MAX_DB_PASS];                  // Password

      TEXT            mLogText[MAX_LOG_TEXT_LEN];            // Log Text
      TEXT            mSqlStr[MMP_MAX_SQL_STR_LEN];              // Sql String
};

#endif

