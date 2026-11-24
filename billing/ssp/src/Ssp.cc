static const char *id = "@(#) $Id: Ssp.cc,v 1.1.4.3 2016/04/12 21:59:01 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
// Ssp.cc
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//
// Originated : KOTHARI KOMAL                           Date:01-MAR-2012
//----------------------------------------------------------------------
#include "SspProc.h"

//----------------------------------------------------------------------
// METHOD NAME : SspProc
// DESCRIPTION : constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
SspProc::SspProc ()
{
}

//----------------------------------------------------------------------
// METHOD NAME : ~SspProc
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
SspProc::~SspProc ()
{
}

//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION : Initilize the object.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::Init ()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Read the Configuration and Create queues...
   // By default Read in Blocking.. and Write in Non-Blocking
   if (false == mQueObj.Init())
   {
      sprintf (lLogText, "Initilising the Queue failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Queue Initilization success..\n", gProcessName););

   // Read the Configuration...
   if (false == mCfgObj.Init())
   {
      sprintf (lLogText, "Initilising the Configuration failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Configuration Initilization success..\n", gProcessName););

   // Create Semaphore which will be used as Lock for O/G Dialog Allocation...
   // SemKey and Semno
   if (false == mSemObj.Init(mCfgObj.mFreeDlgSemKey, 1))
   {
      sprintf (lLogText, "Initilising the Semaphore failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Semaphore Initilization success..\n", gProcessName););

   // Create Dilogue Details Shared Memory...
   // ShmKey startDlg  EndDlg
   if (false == mDlgDet.Init(mCfgObj.mFreeDlgSemKey, IN_RLY_MIN_DLG, IN_RLY_MAX_DLG))
   {
      sprintf (lLogText, "Initilising the Dilogue Details failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Dilogue Details Initilization success..\n", gProcessName););

   // Create Outgoing Dilogue pool Shared Memory...
   // ShmKey startDlg  EndDlg
   // Note: these #def has to be defined by the product
   // MIN_IVR_DLGS and MAX_IVR_DLGS
   if (false == mFreePool.Init(mCfgObj.mFreeDlgSemKey, MIN_IVR_DLGS, MAX_IVR_DLGS))
   {
      sprintf (lLogText, "Initilising the Free DlgId pool failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s:  Free DlgId pool Initilization success..\n", gProcessName););

   // store the Shm Pointer
   mpSspDlgDet   = mDlgDet.GetShmPointer();
   mpInRspDlgDet = mDlgDet.GetShmPointer();
#endif
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION : Reload the object.
// PARAMETERS  : pthread_t
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::ReloadConfig()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   
   // Reload the Configuration...
   if (false == mCfgObj.ReadloadConfig())
   {
      sprintf (lLogText, "Reloading the Configuration failed");
      TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }
   T(gTrace, printf ("%s: Reloading the Configuration success..\n", gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadReqFromMscQue
// DESCRIPTION : Initilize the object.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::ReadReqFromChargingQue (BillingRequest &rBillingRequest)
{
   memset(rBillingRequest, 0, sizeof(BillingRequest));

   if(false == mQueObj.ReadFromMscQue(rBillingRequest))
   {
      TERR(gTrace, printf ("%s_%ld: Message Read From Msc Que failed..\n", 
                       gProcessName););
      return false;
   }
   T(gTrace, printf ("%s_%ld: Message Read From Msc Que success..\n", 
                       gProcessName););

   return true;
}

void SspProc::PopulateSspDetails(const BillingRequest rBillingRequest, SspDlgDetails &rSspDlgDet)
{
   switch(rBillingRequest.event)
   {
      case BILLING_CALL_RECEIVED:
      case BILLING_CONTENT_PLAY:
      case BILLING_OFFLINE_DEDICATION:
      case BILLING_OFFLINE:
         rSspDlgDet.startFlag = true;
         rSspDlgDet.initialEvent = rBillingRequest.event;
         rSspDlgDet.appId= rBillingRequest.appId;
         rSspDlgDet.subsType = rBillingRequest.subsType;
         rSspDlgDet.chargeMode = rBillingRequest.mode;
         rSspDlgDet.chargeId = rBillingRequest.chargeId;
         rSspDlgDet.channelId = rBillingRequest.channelId;
         rSspDlgDet.referenceNum = rBillingRequest.referenceNum;
         rSspDlgDet.expiryTime = rBillingRequest.expiryTime;
         strncpy(rSspDlgDet.msisdn, rBillingRequest.msisdn, MAX_MSISDN_LEN);
         strncpy(rSspDlgDet.imsi, rBillingRequest.imsi, MAX_MSISDN_LEN);
         strncpy(rSspDlgDet.purpose, rBillingRequest.purpose, MAX_PURPOSE_LEN);
         break;
      default:
         rSspDlgDet.startFlag = false;
   }

   rSspDlgDet.currentEvent = rBillingRequest.event;
   
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessRequest
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::ProcessChargingRequest(BillingRequest rBillingRequest)
{
   SspDlgDetails lSspDlgDet;
   TEXT          lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   //Populate SSP State details from Billing Request in lSspDlgDet
   this->PopulateSspDetails(rBillingRequest, lSspDlgDet);
   
   mSspReqIndex = lSspDlgDet.channelId;

   // allocate New Dlg if new req.. and the Slot is free..
   if(true == lSspDlgDet.startFlag)
   {
      //Last Request Dlg Details...
      if(true == mpSspDlgDet[mSspReqIndex].startFlag)
      {
         TERR(gTrace, printf ("%s: Resetting Last Req Dlg Details..\n",
                  gProcessName););
         memset(&mpSspDlgDet[mSspReqIndex], '\0', sizeof(mpSspDlgDet[mSspReqIndex]));
      }

      // Copy the Dlg Details to Shm....
      memcpy(&mpSspDlgDet[mSspReqIndex], &lSspDlgDet, sizeof(lSspDlgDet));

      // allocate New Dlg and store in Shm also.
      // Save the Msc DlgId for Mapping...
      // Save Msc DlgId to IN DlgId in free pool.
      mSemObj.Lock();
      mpSspDlgDet[mSspReqIndex].outDlg =  mFreePool.GetFreeDlgId(mSspReqIndex);
      mSemObj.UnLock();
      if(-1 == mpSspDlgDet[mSspReqIndex].outDlg)
      {
         TERR(gTrace, printf ("%s: Dilogue Allocation Error..\n", gProcessName););
         return false;
      }
   }

   // Set the Event and state.
   mSspThrdState = mpSspDlgDet[mSspReqIndex].currstate;
   if(false == this->GetEvent(rBillingRequest, mSspThrdEvt))
   {
      T(gTrace,printf("%s: Unknown Billing Request Received.. Cann't Set Event\n", 
               gProcessName););
      return false;
   }

   // Print the Billing Request Traces..
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNo:%ld ChannelId:%d INDlg:%d MSISDN:%s AppId:%d ChargeId:%d Mode:%d Purpose:%s",
         mpSspDlgDet[mSspReqIndex].referenceNum,
         mpSspDlgDet[mSspReqIndex].channelId,
         mpSspDlgDet[mSspReqIndex].dlgId,
         mpSspDlgDet[mSspReqIndex].msisdn,
         mpSspDlgDet[mSspReqIndex].appId,
         mpSspDlgDet[mSspReqIndex].chargeId,
         mpSspDlgDet[mSspReqIndex].chargeMode,
         mpSspDlgDet[mSspReqIndex].purpose,
         );   
   T(gTrace, printf ("%s: -----------Dlg Detail for Charging Request---------\n",
            gProcessName););
   T(gTrace, printf ("%s: %s\n", gProcessName, lLogText););
}

//----------------------------------------------------------------------
// METHOD NAME : ReadRspFromInQue
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::ReadRspFromInQue(UINT32 myid)
{
   if(false == mQueObj.ReadFromInQue(mInTcapMsg))
   {
      TERR(gTrace, printf ("%s_%ld: Message Read From In-Rsp Que failed..\n", 
               gProcessName, myid););
      return false;
   }

   T(gTrace, printf ("%s_%ld: Message Read From In-Rsp Que success..\n", 
            gProcessName, myid););
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessInRsp
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::ProcessInRsp(UINT32 myid)
{
   // Print the Rcvd tcap Msg traces..
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Tcap Recvd DlgId:%d TcapDlg:%s TcapComp:%s",
         mInTcapMsg.dialogueId,
         SS7Util::ConvertTcapDlgTypeToStr(mInTcapMsg.tcapDlg),
         SS7Util::ConvertTcapCompTypeToStr(mInTcapMsg.tcapComponent[0].tcapComp)
         );
   T(gTrace, printf ("%s_%ld: %s Rcvd Tcap Message..\n", gProcessName, myid, lLogText););

   // Decode the TCAP Message.. and Get Cap Message
   if(false == this -> DecodeTcapMsg(mInTcapMsg, mInCapMsg, myid))
   {
      TERR(gTrace, printf ("%s_%ld: Tcap Message Decode for In-Rsp failed..\n", 
               gProcessName, myid););
      return false;
   }
   T(gTrace, printf ("%s_%ld: Tcap Message Decode for IN-Rsp success..\n", 
            gProcessName, myid););

   // Dlg Mapping Here.. from mFreePool.
   // Get the Incoming DlgId from the Out going DlgId
   mInRspToMscReqDlgId = 0;
   mFreePool.GetData(&mInRspToMscReqDlgId, mInCapMsg.dialogueId);

   // Set the Event and state.
   mInThrdState = mpInRspDlgDet[mInRspToMscReqDlgId].currstate;
   if(false == this -> GetEvent(mInCapMsg, mInThrdEvt, myid))
   {
      T(gTrace,printf("%s_%ld: Unknown Cap Msg Received.. Cann't Set Event\n", 
               gProcessName, myid););
      return false;
   }

   // Print the Cap Msg traces..
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "MscDlg:%d INDlg:%d ScfAddr:%s MscAddr:%s CalledNo:%s CallingNo:%s FwdCall:%s",
         mpInRspDlgDet[mInRspToMscReqDlgId].inDlg,
         mpInRspDlgDet[mInRspToMscReqDlgId].outDlg,
         mpInRspDlgDet[mInRspToMscReqDlgId].otherDet.scfAddr,
         mpInRspDlgDet[mInRspToMscReqDlgId].otherDet.mscAddr,
         mpInRspDlgDet[mInRspToMscReqDlgId].otherDet.calledNo,
         mpInRspDlgDet[mInRspToMscReqDlgId].otherDet.callingNo,
         (mpInRspDlgDet[mInRspToMscReqDlgId].fwdedCallFlag == IN_RLY_CF_UNKNOWN)?"NO":"YES"
         );
   T(gTrace, printf ("%s_%ld: ----------Dlg Detail for IN Request/Response -----------\n",
            gProcessName, myid););
   T(gTrace, printf ("%s_%ld: %s\n", gProcessName, myid, lLogText););

   return true;
}

//---------------------------------------------------------------------
//*****************************************************************
//*Please Note All the Below functions are Shared by both Threads.*
//*****************************************************************
//---------------------------------------------------------------------

//----------------------------------------------------------------------
// METHOD NAME : GetEvent
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::GetEvent(CapMsg& capmsg, EnumSspEvents& event, UINT32 myid)
{
   switch (capmsg.capDlgType)
   {
      case CAP_BEGIN:
         {
            T(gTrace,printf("%s_%ld: Cap Begin Received..\n", gProcessName, myid););
            if(CAP_INTITAL_DP_MSG == capmsg.invoke.opCode)
            {
               switch(capmsg.invoke.initialDpMsg.eventTypeBCSM)
               {
                  case TERM_ATTEMPT_AUTHORIZED:
                     T(gTrace,printf("%s_%ld: DP-12 Msg Received..\n", gProcessName, myid););
                     event = IN_RLY_EVT_DP12_RCVD;
                     break;

                  case COLLECTED_INFO:
                     T(gTrace,printf("%s_%ld: DP-2 Msg Received..\n", gProcessName, myid););
                     event = IN_RLY_EVT_DP2_RCVD;
                     break;

                  default:
                     TERR(gTrace,printf("%s_%ld: Unknown Cap Msg Received Discarding..\n", 
                              gProcessName, myid););
                     event = IN_RLY_EVT_NULL;
                     break;
               }
            }
            break;
         }

      case CAP_CONTINUE:
         {
            T(gTrace,printf("%s_%ld: Cap continue Received..\n", gProcessName, myid););

            switch(capmsg.capMsgType)
            {
               case CAP_RET_RESULT:
                  switch(capmsg.retResult.opCode)
                  {
                     case CAP_ACTIVITY_TEST_MSG:
                        T(gTrace,
                              printf("%s_%ld: Activity Test Rsp Recvd.\n",gProcessName,myid);
                         );
                        event = IN_RLY_EVT_ACTV_TEST_RSP;
                        break;

                     default:
                        TERR(gTrace,
                              printf("%s_%ld: Unknown Cap Msg Received Discarding..\n", 
                                 gProcessName, myid);
                            );
                        event = IN_RLY_EVT_NULL;
                        break;
                  }
                  break;

               case CAP_INVOKE:
                  {
                     if(false == GetCapInvokeEvent(capmsg, event, myid))
                        return false;
                     else
                        break;
                  }

               default:
                  TERR(gTrace,printf("%s_%ld: Unknown Cap Comp Received Discarding..\n", 
                           gProcessName, myid););
                  event = IN_RLY_EVT_NULL;
                  break;
            } // end of possible Cap Msgtype in continue
            break;
         }  // end of continue Message Block

      case CAP_END:
         {
            T(gTrace,printf("%s_%ld: Cap End Received..\n", gProcessName, myid););
            switch(capmsg.capMsgType)
            {
               case CAP_INVOKE:
                  switch(capmsg.invoke.opCode)
                  {
                     case CAP_CANCEL_MSG:
                        T(gTrace,
                              printf("%s_%ld: Cancel Msg Recvd.\n",gProcessName,myid);
                         );
                        event = IN_RLY_EVT_IN_RELEASE;
                        break;

                     case CAP_RELEASE_CALL_MSG:
                        T(gTrace,
                              printf("%s_%ld: Release Call Recvd.\n",gProcessName,myid);
                         );
                        event = IN_RLY_EVT_IN_RELEASE;
                        break;

                     default:
                        TERR(gTrace,printf("%s_%ld: Unknown Cap Msg Received Discarding..\n", 
                                 gProcessName, myid););
                        event = IN_RLY_EVT_NULL;
                        break;
                  }
                  break;

               default:
                  TERR(gTrace,printf("%s_%ld: Unknown Cap Comp Received Discarding..\n", 
                           gProcessName, myid););
                  event = IN_RLY_EVT_NULL;
                  break;
            }
            break;
         }

      case CAP_PRE_ARRANGED_END:
         {
            T(gTrace,printf("%s_%ld: Cap Pre-Arrenged End Received..\n",gProcessName,myid););
            T(gTrace,printf("%s_%ld: Unknown Cap Msg Received Discarding..\n",
                     gProcessName, myid););
            event = IN_RLY_EVT_NULL;
            break;
         }

      case CAP_ABORT:
         {
            T(gTrace,printf("%s_%ld: Cap Abort Received..\n", gProcessName, myid););
            // Note: these #def has to be defined by the product
            // MIN_IVR_DLGS and MAX_IVR_DLGS

            //Abort from IN...
            if(capmsg.dialogueId > MIN_IVR_DLGS && 
                  capmsg.dialogueId < MAX_IVR_DLGS  
              )
               event = IN_RLY_EVT_ABORT_FRM_IN;
            else
               event = IN_RLY_EVT_ABORT_FRM_MSC;

            break;
         }

      case CAP_CANCEL:
         {
            T(gTrace,printf("%s_%ld: Cap Cancel Received..\n", gProcessName, myid););
            T(gTrace,printf("%s_%ld: Unknown Cap Msg Received Discarding..\n",
                     gProcessName, myid););
            event = IN_RLY_EVT_NULL;  //????
            break;
         }

      default:
         {
            TERR(gTrace,printf("%s_%ld: Unknown Cap Msg Received..\n", gProcessName, myid););
            event = IN_RLY_EVT_NULL;
            return false;
         }
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetCapInvokeEvent
// DESCRIPTION :  
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::GetCapInvokeEvent(CapMsg& capmsg, EnumSspEvents& event, UINT32 myid)
{
   switch(capmsg.invoke.opCode)
   {
      case CAP_CONNECT_MSG:
         T(gTrace,
               printf("%s_%ld: Connect Req Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_CONN;
         break;

      case CAP_CONTINUE_MSG:
         T(gTrace,
               printf("%s_%ld: Continue Req Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_CONTINUE_RCVD;
         break;

      case CAP_REQUEST_REPORT_BSCM_MSG:
         T(gTrace,
               printf("%s_%ld: Continue Req Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_BCSM_REQ;
         break;

      case CAP_EVENT_REPORT_BCSM_MSG:
         T(gTrace,
               printf("%s_%ld: Event Report Recvd.\n",gProcessName,myid);
          );

         switch(capmsg.invoke.eventReportBCSMMsg.eventTypeBCSM)
         {
            case O_CALLED_PARTY_BUSY:
               T(gTrace,
                     printf("%s_%ld: O-Busy Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_O_BUSY;
               break;

            case O_NO_ANSWER:
               T(gTrace,
                     printf("%s_%ld: O-NoAnswer Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_O_NO_ANS;
               break;

            case O_ANSWER:
               T(gTrace,
                     printf("%s_%ld: O-Answer Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_O_ANS;
               break;

            case O_ABANDON:
            case O_DISCONNECT:
               T(gTrace,
                     printf("%s_%ld: O-Disconnect Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_O_DISCONN;
               break;

            case T_BUSY:
               T(gTrace,
                     printf("%s_%ld: T-Busy Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_T_BUSY;
               break;

            case T_NO_ANSWER:
               T(gTrace,
                     printf("%s_%ld: T-NoAnswer Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_T_NO_ANS;
               break;

            case T_ANSWER:
               T(gTrace,
                     printf("%s_%ld: T-Answer Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_T_ANS;
               break;

            case T_ABANDON:
            case T_DISCONNECT:
               T(gTrace,
                     printf("%s_%ld: T-Disconnect Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_T_DISCONN;
               break;

            default:
               T(gTrace,
                     printf("%s_%ld: Unknown Event Recvd.\n",gProcessName,myid);
                );
               event = IN_RLY_EVT_NULL;
               break;
         }
         break;

      case CAP_APPLY_CHARGING_MSG:
         T(gTrace,
               printf("%s_%ld: Apply charging Req Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_APPLY_CHRG_REQ;
         break;

      case CAP_APPLY_CHARGING_REPORT_MSG:
         T(gTrace,
               printf("%s_%ld: Apply charging Rsp Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_APPLY_CHRG_RSP;
         break;

      case CAP_CALL_INFORMATION_REPORT_MSG:
         T(gTrace,
               printf("%s_%ld: Call information Rsp Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_CALL_INFO_RSP;
         break;

      case CAP_CALL_INFORAMTION_REQUEST_MSG:
         T(gTrace,
               printf("%s_%ld: Call information Req Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_CALL_INFO_REQ;
         break;

      case CAP_ACTIVITY_TEST_MSG:
         T(gTrace,
               printf("%s_%ld: Apply charging Rsp Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_ACTV_TEST_REQ;
         break;

      case CAP_RELEASE_CALL_MSG: // Not needed here.. still okay..
         T(gTrace,
               printf("%s_%ld: Release Recvd.\n",gProcessName,myid);
          );
         event = IN_RLY_EVT_IN_RELEASE;
         break;

      default:
         TERR(gTrace,printf("%s_%ld: Unknown Cap Msg Received Discarding..\n", 
                  gProcessName, myid););
         event = IN_RLY_EVT_NULL;
         return false;
   }
   return true;
}
//----------------------------------------------------------------------
// METHOD NAME : DecodeTcapMsg
// DESCRIPTION : Decode the TCAP Message.. and Get Cap Message 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::DecodeTcapMsg(TcapMsg& tcapmsg, CapMsg& capmsg, UINT32 myid)
{
   // Copy the Dilogue portion of the Message..
   capmsg.dialogueId       = tcapmsg.dialogueId;
   capmsg.handlerSsn       = tcapmsg.ssn;
   capmsg.invokeId         = tcapmsg.tcapComponent[0].invokeId;
   //capmsg.lastComponent    = tcapmsg.tcapComponent.lastComponent;
   //capmsg.capAcn           = tcapmsg.       TDArray                 applicationContext;
   //capmsg.               = tcapmsg.numOfComponents;
   //capmsg.address          = tcapmsg.address;

   switch (tcapmsg.tcapDlg)
   {
      case TCAP_BEGIN:
         {
            capmsg.capDlgType = CAP_BEGIN;
            T(gTrace,printf("%s_%ld: Tcap Begin Received..\n", gProcessName, myid););
            switch(tcapmsg.tcapComponent[0].tcapComp)
            {
               case TCAP_INVOKE_COMP:
                  {
                     if(false == this -> DecodeTcapInvoke(tcapmsg, capmsg, myid))
                        return false;
                     else
                        break;
                     break;  // End of Invoke
                  }

               default:
                  {
                     TERR(gTrace,printf("%s_%ld: Tcap Unkown Component inside Begin\n", 
                              gProcessName, myid););
                     return false;
                  }
            }

            break;
         } // End of BEGIN

      case TCAP_CONTINUE:
         {
            capmsg.capDlgType = CAP_CONTINUE;
            T(gTrace,printf("%s_%ld: Tcap continue Received..\n", gProcessName, myid););
            switch(tcapmsg.tcapComponent[0].tcapComp)
            {
               case TCAP_INVOKE_COMP:
                  {
                     if(false == this -> DecodeTcapInvoke(tcapmsg, capmsg, myid))
                        return false;
                     else
                        break;
                  }

               case TCAP_RET_RESULT_COMP:
                  {
                     T(gTrace,printf("%s_%ld: Tcap Ret Result Comp Rcvd..\n", 
                              gProcessName, myid););
                     capmsg.capMsgType = CAP_RET_RESULT;

                     if(CAP_ACTIVITY_TEST_MSG == 
                           tcapmsg.tcapComponent[0].tcapRetResultComp.operation.operationCode
                       )
                     {
                        Cap_ActivityTest lActivityTestObj;
                        lActivityTestObj.DecodeActivityTest(
                              tcapmsg.tcapComponent[0].tcapRetResultComp.parameterData,
                              capmsg.retResult.activityTestMsg
                              );
                        break;
                     }

                     else
                     {
                        T(gTrace,printf("%s_%ld: Unknown Tcap Comp Rcvd.. in Continue\n", 
                                 gProcessName, myid););
                        return false;
                     }
                  } // End of Ret Result comp

               case TCAP_RET_ERROR_COMP:
                  {
                     T(gTrace,printf("%s_%ld: Tcap Ret Error Comp Rcvd..\n", 
                              gProcessName, myid););
                     capmsg.capMsgType = CAP_RET_ERROR;
                     break;
                  } // End of Ret Err comp

               default:
                  {
                     TERR(gTrace,printf("%s_%ld: Tcap Unkown Component inside Continue\n", 
                              gProcessName, myid););
                     return false;
                  }

                  break;
            } // end of Tcap Comps switch

            break;
         }  //  End of Continue

      case TCAP_END:
         {
            capmsg.capDlgType = CAP_END;
            T(gTrace,printf("%s_%ld: Tcap End Received..\n", gProcessName, myid););
            switch(tcapmsg.tcapComponent[0].tcapComp)
            {
               case TCAP_INVOKE_COMP:
                  {
                     if(false == this -> DecodeTcapInvoke(tcapmsg, capmsg, myid))
                        return false;
                     else
                        break;
                  }

               case TCAP_RET_ERROR_COMP:   //?????
                  {
                     T(gTrace,printf("%s_%ld: Tcap Ret Error Comp Rcvd..\n", 
                              gProcessName, myid););
                     capmsg.capMsgType = CAP_RET_ERROR;
                     break;
                  } // End of Ret Err comp

               default:
                  {
                     TERR(gTrace,printf("%s_%ld: Tcap Unkown Component inside Tcap-End\n", 
                              gProcessName, myid););
                     return false;
                  }

                  break;
            } // end of Tcap Comps switch
            break;
         } // End of TCAP END.

      case TCAP_PRE_ARRANGED_END:
         {
            capmsg.capDlgType = CAP_PRE_ARRANGED_END;
            T(gTrace,printf("%s_%ld: Tcap Pre-Arrenged End Received..\n",gProcessName,myid););
            return false;  //????
         }

      case TCAP_RSP_TIMEOUT:
         {
            capmsg.capDlgType = CAP_ABORT;
            T(gTrace,printf("%s_%ld: Tcap Rsp timeout Received..\n", gProcessName, myid););
            return false;  // ????
         }

      case TCAP_ABORT:
         {
            capmsg.capDlgType = CAP_ABORT;
            T(gTrace,printf("%s_%ld: Tcap Abort Received..\n", gProcessName, myid););
            if(false == this -> DecodeTcapAbort(tcapmsg, capmsg, myid))
               return false;
            else
               break;
         }

      default:
         {
            capmsg.capDlgType = CAP_CANCEL;
            T(gTrace,printf("%s_%ld: Unknown Tcap Msg Received..\n", gProcessName, myid););
            return false;
         }
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DecodeTcapInvoke
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::DecodeTcapInvoke(TcapMsg& tcapmsg, CapMsg& capmsg, UINT32 myid)
{
   T(gTrace,printf("%s_%ld: Tcap Invoke Comp Rcvd.\n",
            gProcessName, myid););
   capmsg.capMsgType = CAP_INVOKE;
   switch(tcapmsg.tcapComponent[0].tcapInvokeComp.operation.operationCode)
   {
      case CAP_INTITAL_DP_MSG:
         {
            Cap_InitialDP lInitialDPObj;
            lInitialDPObj.DecodeInitialDP(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.initialDpMsg
                  );
            break;
         }

      case CAP_CONNECT_MSG:
         {
            Cap_Connect lConnectObj;
            lConnectObj.DecodeConnect(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.connectMsg
                  );
            break;
         }

      case CAP_RELEASE_CALL_MSG:
         {
            Cap_ReleaseCall lReleaseCallObj;
            lReleaseCallObj.DecodeReleaseCall(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.releaseCallMsg
                  );
            break;
         }

      case CAP_REQUEST_REPORT_BSCM_MSG:
         {
            Cap_RequestReportBCSM lRequestReportBCSMObj;
            lRequestReportBCSMObj.DecodeRequestReportBCSM(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.requestReportBCSMMsg
                  );
            break;
         }

      case CAP_EVENT_REPORT_BCSM_MSG:
         {
            Cap_EventReportBCSM lEventReportBCSMObj;
            lEventReportBCSMObj.DecodeEventReportBCSM(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.eventReportBCSMMsg
                  );
            break;
         }

      case CAP_CONTINUE_MSG:
         {
            Cap_Continue lContinueObj;
            lContinueObj.DecodeContinue(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.continueMsg
                  );
            break;
         }

      case CAP_APPLY_CHARGING_MSG:
         {
            Cap_ApplyCharging lApplyChargingObj;
            lApplyChargingObj.DecodeApplyCharging(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.applyChargingMsg
                  );
            break;
         }

      case CAP_APPLY_CHARGING_REPORT_MSG:
         {
            Cap_ApplyChargingReport lApplyChargingReportObj;
            lApplyChargingReportObj.DecodeApplyChargingReport(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.applyChargingReportMsg
                  );
            break;
         }

      case CAP_CALL_INFORMATION_REPORT_MSG:
         {
            Cap_CallInfoReport lCallInfoReportObj;
            lCallInfoReportObj.DecodeCallInfoReport(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.callInfoReportMsg
                  );
            break;
         }

      case CAP_CALL_INFORAMTION_REQUEST_MSG:
         {
            Cap_CallInfoRequest lCallInfoRequestObj;
            lCallInfoRequestObj.DecodeCallInfoRequest(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.callInfoRequestMsg
                  );
            break;
         }

      case CAP_ACTIVITY_TEST_MSG:
         {
            Cap_ActivityTest lActivityTestObj;
            lActivityTestObj.DecodeActivityTest(
                  tcapmsg.tcapComponent[0].tcapInvokeComp.parameterData,
                  capmsg.invoke.activityTestMsg
                  );
            break;
         }


      default:
         {
            TERR(gTrace,printf("%s_%ld: Tcap Unkown tcap Msg..\n", 
                     gProcessName, myid););
            return false;
         }
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DecodeTcapAbort
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::DecodeTcapAbort(TcapMsg& tcapmsg, CapMsg& capmsg, UINT32 myid)
{
   // only one tcap component assumed
   switch(tcapmsg.tcapComponent[0].tcapAbortComp.abortType)
   {
      case TCAP_NULL_ABORT:
         T(gTrace,printf("%s_%ld: Tcap Null Abort Comp Rcvd..\n", gProcessName, myid););
         capmsg.capMsgType = CAP_API_ERROR;
         break;

      case TCAP_USER_ABORT:
         T(gTrace,printf("%s_%ld: Tcap U-Abort Comp Rcvd..\n", gProcessName, myid););
         capmsg.capMsgType = CAP_UABORT;
         break;

      case TCAP_PROTOCOL_ABORT:
         T(gTrace,printf("%s_%ld: Tcap P-Abort Comp Rcvd..\n", gProcessName, myid););
         capmsg.capMsgType = CAP_PABORT;
         break;

      default:
         T(gTrace,printf("%s_%ld: Tcap Unknown Abort Comp Rcvd..\n", 
                  gProcessName, myid););
         capmsg.capMsgType = CAP_MSG_ABSENT;
         return false;
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : PutGsmScfAddr
// DESCRIPTION : Used by both threads.
// PARAMETERS  : TCAPAddress* addrptr
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::PutGsmScfAddr(TCAPAddress* addrptr, TEXT* replaceaddr, UINT32 myid)
{
   //Global title includes translation type, numbering plan and encoding scheme
   addrptr -> addressIndicator = 0x12;
   addrptr -> subsystemNumber  = GSMSCF_SSN;
   addrptr -> pointCode        = mCfgObj.mDstNodePt;
   addrptr -> natureOfAddress  = 4;
   addrptr -> translationType  = 0;
   addrptr -> numberingPlan    = SCCP_ISDN_TEL_NP;
   addrptr -> numberOfDigits   = strlen (replaceaddr);

   if (addrptr ->numberOfDigits % 2)
      addrptr -> encodingScheme = SCCP_BCD_ODD_ENC;
   else
      addrptr -> encodingScheme = SCCP_BCD_EVEN_ENC;

   for (UINT8 lLen = 0; lLen < addrptr->numberOfDigits; lLen++)
      addrptr->digits[lLen] = replaceaddr[lLen] - '0';

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : PutMscAddr
// DESCRIPTION : Used by both threads.
// PARAMETERS  : TCAPAddress* addrptr
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::PutMscAddr(TCAPAddress* addrptr, TEXT* replaceaddr, UINT32 myid)
{
   //Global title includes translation type, numbering plan and encoding scheme
   addrptr -> addressIndicator = 0x12;
   addrptr -> subsystemNumber  = MSC_SSN;
   addrptr -> pointCode        = mCfgObj.mDstNodePt;
   addrptr -> natureOfAddress  = 4;
   addrptr -> translationType  = 0;
   addrptr -> numberingPlan    = SCCP_ISDN_TEL_NP;
   addrptr -> numberOfDigits   = strlen (replaceaddr);

   if (addrptr->numberOfDigits % 2)
      addrptr -> encodingScheme = SCCP_BCD_ODD_ENC;
   else
      addrptr -> encodingScheme = SCCP_BCD_EVEN_ENC;

   for (UINT8 lLen = 0; lLen < addrptr->numberOfDigits; lLen++)
      addrptr->digits[lLen] = replaceaddr[lLen] - '0';

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DoNothing
// DESCRIPTION : Used by both threads.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::DoNothing(INT32 mscdlgid, UINT32 myid)
{
   InRlyDlgDetails*  lptr = mDlgDet.GetShmPointer();  //???????
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d NULL event Rcvd\n", 
            gProcessName, myid, mscdlgid, lptr[mscdlgid].outDlg););
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendAbortToIn
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendAbortToIn(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Abort which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      //Map the Cap abort... to Tcap Abort.
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   // Change the Addr...
   if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
   {
      if(IN_RLY_ST_DP2_SENT_TO_IN == mpSspDlgDet[mscdlgid].currstate ||
            IN_RLY_ST_EVENT_O_BCSM   == mpSspDlgDet[mscdlgid].currstate
        )
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.callingNo,mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }
      else
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
   }

   // Put GSMSCF Addr As Destination
   this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

   // Put Our Node Addr As Origination Addr
   memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Abort To IN Success..\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendAbortToMsc
// DESCRIPTION : used by In Rsp thread only.....
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendAbortToMsc(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Abort which is received from IN. 
   memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));

   // Change the Outgoing DlgId from IN to Incoming DlgId to MSC  
   lTcapMsg.dialogueId = mscdlgid;

   // Change the Addr...
   if(mpInRspDlgDet[mscdlgid].otherDet.mscAddr[0] == '\0')
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Msc Addr Corrupted..\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }

   // Put GSMSCF Addr As Destination
   this-> PutMscAddr(&lTcapMsg.address, mpInRspDlgDet[mscdlgid].otherDet.mscAddr, myid);

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Abort To IN Success..\n", 
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendDp2
// DESCRIPTION : Used by Main thread. 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendDp2(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same DP2 which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ...
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_BEGIN;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_INTITAL_DP_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.callingNo,mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      // Application Context  CAP-2 version. 
      memcpy(&lTcapMsg.applicationContext, CapAcn[4], sizeof(lTcapMsg.applicationContext));

      //Fill Cap Msg...??????
      Cap_InitialDP lDp2Obj;
      lDp2Obj.EncodeInitialDP(lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.initialDpMsg
            );
   }
   
   // It is Incoming MSC Dlg mapped to outgoing IN Dlg.
   lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending DP-2 To IN Success..\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   //Change the State...
   mMscThrdState = mpSspDlgDet[mscdlgid].currstate = IN_RLY_ST_DP2_SENT_TO_IN;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendDp12
// DESCRIPTION : Used by Main thread. 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendDp12(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same DP12 which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ???...
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_BEGIN;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_INTITAL_DP_MSG;
      ////lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      // Application Context  CAP-2 version. 
      memcpy(&lTcapMsg.applicationContext, CapAcn[4], sizeof(lTcapMsg.applicationContext));

      //Fill Cap Msg...??????
      Cap_InitialDP lDp12Obj;
      lDp12Obj.EncodeInitialDP(lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.initialDpMsg
            );
   }

   // It is Incoming MSC Dlg mapped to outgoing IN Dlg.
   lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending DP-12 To IN Success..\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   //Change the State...
   mMscThrdState = mpSspDlgDet[mscdlgid].currstate = IN_RLY_ST_DP12_SENT_TO_IN;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendApplChrgReq
// DESCRIPTION : used by InRsp thread. 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendApplChrgReq(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Abort which is received from IN. 
   memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));

   // Change the Outgoing DlgId from IN to Incoming DlgId to MSC  
   lTcapMsg.dialogueId = mscdlgid;

   // Change the Addr...
   if(mpInRspDlgDet[mscdlgid].otherDet.mscAddr[0] == '\0')
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Msc Addr Corrupted..\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }

   // Put GSMSCF Addr As Destination
   this-> PutMscAddr(&lTcapMsg.address, mpInRspDlgDet[mscdlgid].otherDet.mscAddr, myid);

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Apply Charging Req to MSC Success..\n", 
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendApplChrgRsp
// DESCRIPTION : Used by Main thread. 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendApplChrgRsp(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Apply Charging Report which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_APPLY_CHARGING_REPORT_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_ApplyChargingReport lApplyChrgReportObj;
      lApplyChrgReportObj.EncodeApplyChargingReport(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.applyChargingReportMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Apply Charging Report To IN Success..\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendCallInfoReq
// DESCRIPTION : used by InRsp thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendCallInfoReq(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Abort which is received from IN. 
   memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));

   // Change the Outgoing DlgId from IN to Incoming DlgId to MSC  
   lTcapMsg.dialogueId = mscdlgid;

   // Change the Addr...
   if(mpInRspDlgDet[mscdlgid].otherDet.mscAddr[0] == '\0')
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Msc Addr Corrupted..\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }

   // Put GSMSCF Addr As Destination
   this-> PutMscAddr(&lTcapMsg.address, mpInRspDlgDet[mscdlgid].otherDet.mscAddr, myid);

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Call Info Request To Msc Success..\n", 
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendCallInfoRsp
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendCallInfoRsp(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Call information Report which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ...
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_CALL_INFORMATION_REPORT_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_CallInfoReport lCallInfoReportObj;
      lCallInfoReportObj.EncodeCallInfoReport(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.callInfoReportMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Call information Report To IN Success..\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendEvtBCSMReq
// DESCRIPTION : Used by InRsp thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendEvtBCSMReq(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Abort which is received from IN. 
   memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));

   // Change the Outgoing DlgId from IN to Incoming DlgId to MSC  
   lTcapMsg.dialogueId = mscdlgid;

   // Change the Addr...
   if(mpInRspDlgDet[mscdlgid].otherDet.mscAddr[0] == '\0')
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Msc Addr Corrupted..\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }

   // Put GSMSCF Addr As Destination
   this-> PutMscAddr(&lTcapMsg.address, mpInRspDlgDet[mscdlgid].otherDet.mscAddr, myid);

   //Add the Additional Armed Events...?????

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d BCSM Event Request To Msc Success..\n", 
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendTBusy
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendTBusy(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same T-Busy which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? check whether IN armed it or not..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_EVENT_REPORT_BCSM_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_EventReportBCSM lEventReportBCSMObj;
      lEventReportBCSMObj.EncodeEventReportBCSM(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.eventReportBCSMMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Call Event T-Busy To IN Success\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendTNoAns
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendTNoAns(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same T-NoAns which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? check whether IN armed it or not..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_EVENT_REPORT_BCSM_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_EventReportBCSM lEventReportBCSMObj;
      lEventReportBCSMObj.EncodeEventReportBCSM(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.eventReportBCSMMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Call Event T-NoAns To IN Success\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendTAns
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendTAns(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same T-Ans which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? check whether IN armed it or not..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_EVENT_REPORT_BCSM_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_EventReportBCSM lEventReportBCSMObj;
      lEventReportBCSMObj.EncodeEventReportBCSM(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.eventReportBCSMMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Call Event T-Ans To IN Success\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendTDiscon
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendTDiscon(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same T-Disconnect which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? check whether IN armed it or not..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_EVENT_REPORT_BCSM_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_EventReportBCSM lEventReportBCSMObj;
      lEventReportBCSMObj.EncodeEventReportBCSM(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.eventReportBCSMMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Event T-Disconnect To IN Success\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendOBusy
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendOBusy(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same O-Busy which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? check whether IN armed it or not..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_EVENT_REPORT_BCSM_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.callingNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_EventReportBCSM lEventReportBCSMObj;
      lEventReportBCSMObj.EncodeEventReportBCSM(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.eventReportBCSMMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Call Event O-Busy To IN Success\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendONoAns
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendONoAns(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same O-NoAns which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? check whether IN armed it or not..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_EVENT_REPORT_BCSM_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.callingNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_EventReportBCSM lEventReportBCSMObj;
      lEventReportBCSMObj.EncodeEventReportBCSM(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.eventReportBCSMMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Call Event O-NoAns To IN Success\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendOAns
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendOAns(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same O-Ans which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? check whether IN armed it or not..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_EVENT_REPORT_BCSM_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.callingNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_EventReportBCSM lEventReportBCSMObj;
      lEventReportBCSMObj.EncodeEventReportBCSM(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.eventReportBCSMMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Call Event O-Ans To IN Success\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendODiscon
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendODiscon(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same O-Disconnect which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? check whether IN armed it or not..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_INVOKE_COMP;
      lTcapMsg.tcapComponent[0].tcapInvokeComp.operation.operationCode = CAP_EVENT_REPORT_BCSM_MSG;
      //lTcapMsg.tcapComponent[0].tcapInvokeComp.linkedId                = 0;

      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.callingNo, mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));

      //Fill Cap Msg...??????
      Cap_EventReportBCSM lEventReportBCSMObj;
      lEventReportBCSMObj.EncodeEventReportBCSM(
            lTcapMsg.tcapComponent[0].tcapInvokeComp.parameterData,
            mDlgDetObj.capMsg.invoke.eventReportBCSMMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Sending Event O-Disconnect To IN Success\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendActivityReq
// DESCRIPTION : used by InRsp thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendActivityReq(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Abort which is received from IN. 
   memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));

   // Change the Outgoing DlgId from IN to Incoming DlgId to MSC  
   lTcapMsg.dialogueId = mscdlgid;

   // Change the Addr...
   if(mpInRspDlgDet[mscdlgid].otherDet.mscAddr[0] == '\0')
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Msc Addr Corrupted..\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }

   // Put GSMSCF Addr As Destination
   this-> PutMscAddr(&lTcapMsg.address, mpInRspDlgDet[mscdlgid].otherDet.mscAddr, myid);

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Activity Request to Msc Success..\n", 
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendActivityRsp
// DESCRIPTION : Used by Main thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendActivityRsp(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Activity Response which is received from MSC.
   if(mDlgDetObj.tcapFlag)
   {
      memcpy(&lTcapMsg, &mDlgDetObj.tcapMsg, sizeof(TcapMsg));
   }
   else
   {
      // Fill Tcap Dilogue Details ??? return result..
      lTcapMsg.ssn                                                     = CAP_SSN;
      lTcapMsg.tcapDlg                                                 = TCAP_CONTINUE;
      lTcapMsg.numOfComponents                                         = 1;
      lTcapMsg.tcapComponent[0].lastComponent                          = TCAP_LAST_COMPONENT;
      lTcapMsg.tcapComponent[0].tcapComp                               = TCAP_RET_RESULT_COMP;
      lTcapMsg.tcapComponent[0].tcapRetResultComp.operation.operationCode = CAP_ACTIVITY_TEST_MSG;
      if(mpSspDlgDet[mscdlgid].otherDet.scfAddr[0] == '\0')
      {
         if(IN_RLY_ST_DP2_SENT_TO_IN == mpSspDlgDet[mscdlgid].currstate ||
               IN_RLY_ST_EVENT_O_BCSM   == mpSspDlgDet[mscdlgid].currstate
           )
         {
            mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.callingNo, 
                  mpSspDlgDet[mscdlgid].otherDet.scfAddr
                  );
         }
         else
            mCfgObj.FindScfAddr(mpSspDlgDet[mscdlgid].otherDet.calledNo,mpSspDlgDet[mscdlgid].otherDet.scfAddr);
      }

      // Put GSMSCF Addr As Destination
      this-> PutGsmScfAddr(&lTcapMsg.address, mpSspDlgDet[mscdlgid].otherDet.scfAddr, myid);

      // Put Our Node Addr As Origination Addr
      memcpy(&lTcapMsg.origAddr, &mMscCapMsg.address, sizeof(lTcapMsg.origAddr));
      //Fill Cap Msg...??????
      Cap_ActivityTest lActivityTestObj;
      lActivityTestObj.EncodeActivityTest(
            lTcapMsg.tcapComponent[0].tcapRetResultComp.parameterData,
            mDlgDetObj.capMsg.invoke.activityTestMsg
            );
   }

   // Change the Incoming DlgId from MSC to Outgoing DlgId to IN  
   if(0 != mpSspDlgDet[mscdlgid].outDlg)
      lTcapMsg.dialogueId = mpSspDlgDet[mscdlgid].outDlg;
   else
   {
      T(gTrace,printf("%s_%ld: Dlg Details got Corrupted..for Msc Dlg:%u\n", 
               gProcessName, myid, mDlgDetObj.capMsg.dialogueId););
      return false;
   }

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Apply Charging Report To IN Success..\n", 
            gProcessName, myid, mscdlgid, lTcapMsg.dialogueId););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendContinueToMsc
// DESCRIPTION : used by InRsp thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendContinueToMsc(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Abort which is received from IN. 
   memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));

   // Change the Outgoing DlgId from IN to Incoming DlgId to MSC  
   lTcapMsg.dialogueId = mscdlgid;

   // Change the Addr...
   if(mpInRspDlgDet[mscdlgid].otherDet.mscAddr[0] == '\0')
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Msc Addr Corrupted..\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }

   // Put GSMSCF Addr As Destination
   this-> PutMscAddr(&lTcapMsg.address, mpInRspDlgDet[mscdlgid].otherDet.mscAddr, myid);

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Continue to Msc Success..\n", 
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   //Change the State...
   if(IN_RLY_ST_DP12_SENT_TO_IN == mpSspDlgDet[mscdlgid].currstate)
      mInThrdState= mpSspDlgDet[mscdlgid].currstate = IN_RLY_ST_EVENT_T_BCSM;
   else
      mInThrdState = mpSspDlgDet[mscdlgid].currstate = IN_RLY_ST_EVENT_O_BCSM;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendConnectToMsc
// DESCRIPTION : used by InRsp thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendConnectToMsc(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same Abort which is received from IN. 
   memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));

   // Change the Outgoing DlgId from IN to Incoming DlgId to MSC  
   lTcapMsg.dialogueId = mscdlgid;

   // Change the Addr...
   if(mpInRspDlgDet[mscdlgid].otherDet.mscAddr[0] == '\0')
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Msc Addr Corrupted..\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }

   // Put GSMSCF Addr As Destination
   this-> PutMscAddr(&lTcapMsg.address, mpInRspDlgDet[mscdlgid].otherDet.mscAddr, myid);

   // Modification Required ?????

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Connect to Msc Success..\n", 
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   //Change the State...
   if(IN_RLY_ST_DP12_SENT_TO_IN == mpSspDlgDet[mscdlgid].currstate)
      mInThrdState= mpSspDlgDet[mscdlgid].currstate = IN_RLY_ST_EVENT_T_BCSM;
   else
      mInThrdState = mpSspDlgDet[mscdlgid].currstate = IN_RLY_ST_EVENT_O_BCSM;
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendReleaseToMsc
// DESCRIPTION : used by InRsp thread.
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::SendReleaseToMsc(INT32 mscdlgid, UINT32 myid)
{
   TcapMsg lTcapMsg;
   memset(&lTcapMsg, '\0', sizeof(lTcapMsg));

   //Send the same END which is received from IN. 
   memcpy(&lTcapMsg, &mInTcapMsg, sizeof(TcapMsg));

   // Change the Outgoing DlgId from IN to Incoming DlgId to MSC  
   lTcapMsg.dialogueId = mscdlgid;

   // Change the Addr...
   if(mpInRspDlgDet[mscdlgid].otherDet.mscAddr[0] == '\0')
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Msc Addr Corrupted..\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }

   // Put GSMSCF Addr As Destination
   this-> PutMscAddr(&lTcapMsg.address, mpInRspDlgDet[mscdlgid].otherDet.mscAddr, myid);

   //Write to Tcap Handle.
   if(false == mQueObj.WriteToTcapHdlrQue(lTcapMsg, lTcapMsg.ssn))
   {
      T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler Failed...\n",
               gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););
      return false;
   }
   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Write to Tcap Handler success...\n",
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Release to Msc Success..\n", 
            gProcessName, myid, mscdlgid, mpInRspDlgDet[mscdlgid].outDlg););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ClearDlgInfo
// DESCRIPTION : Both threads use this function
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SspProc::ClearDlgInfo(INT32 mscdlgid, UINT32 myid)
{
   InRlyDlgDetails*  lptr = mDlgDet.GetShmPointer();  //???????


   T(gTrace,printf("%s_%ld: MscDlg:%ld InDlg:%d Clearing Dilogue Details\n", 
            gProcessName, myid, mscdlgid, lptr[mscdlgid].outDlg););
   // Msc Dlg Details Shm
   memset(&lptr[mscdlgid], '\0', sizeof(InRlyDlgDetails));
   // Free DlgId pool.
   mFreePool.ResetSlot(lptr[mscdlgid].outDlg);
   return true;
}
