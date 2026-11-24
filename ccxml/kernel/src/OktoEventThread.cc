static const char* id="@(#) $Id: OktoEventThread.cc,v 1.1.1.1.2.6.2.1 2025/01/02 11:36:08 mram Exp $";
/****************License************************************************
 *
 * Copyright 2003-2004.  Phonologies (India) Pvt Ltd.
 *
 * Use of this software is subject to notices and obligations set forth
 * in the Phonologies License 1.0  which is included with this software.
 *
 * Phonologies, InterpreXer, Oktopous Media Server are registered trademarks or
 * trademarks of Phonologies (India) Pvt Ltd . in India and other countries.
 *
 * Originated: Divya K                        Date: DEC-2010
 ***********************************************************************/

#include "OktoEventThread.h"
#include "OktoCommand.hpp"
#include <sstream>
#include "MsgQueue.h"
#include "CfgRead.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ConfigInfo.h"
#include "IvrConstDef.h"
#include "IvrStructs.h"
#include "IvrUtil.h"
#include "IvrQueApi.h"

key_t     gCxCmdResQueKey;
MsgQueue  gCxCmdResQue;
UINT32    mChannelId;
UINT32    gSecondaryChannelId;
extern	 IvrQueApi   gIvrQueApi;
extern    UINT32      gRefNum;
extern    INT16       gCId;

typedef std::basic_string<VXIchar> vxistring;

static std::basic_ostream<VXIchar>& operator<<(std::basic_ostream<VXIchar>& os,
      const VXIValue * val)
{
   if (val == NULL)
      return os << L"NULL";

   switch (VXIValueGetType(val)) {
      case VALUE_INTEGER:
         return os << VXIIntegerValue(reinterpret_cast<const VXIInteger*>(val));
      case VALUE_FLOAT:
         return os << VXIFloatValue(reinterpret_cast<const VXIFloat*>(val));
      case VALUE_STRING:
         return os << VXIStringCStr(reinterpret_cast<const VXIString *>(val));
      case VALUE_VECTOR:
         {
            const VXIVector * v = reinterpret_cast<const VXIVector *>(val);
            const VXIunsigned len = VXIVectorLength(v);

            os << L"{ ";
            for (VXIunsigned i = 0; i < len; ++i) {
               if (i != 0) os << L", ";
               os << VXIVectorGetElement(v, i);
            }
            return os << L" }";
         }
      case VALUE_MAP:
         {
            const VXIMap * m = reinterpret_cast<const VXIMap *>(val);
            const VXIchar * key;
            const VXIValue * value;

            os << L"{ ";
            if (VXIMapNumProperties(m) != 0) {
               VXIMapIterator * i = VXIMapGetFirstProperty(m, &key, &value);
               os << L"(" << key << L", " << value << L")";
               while (VXIMapGetNextProperty(i, &key, &value)
                     == VXIvalue_RESULT_SUCCESS)
                  os << L", (" << key << L", " << value << L")";

               VXIMapIteratorDestroy(&i);
            }
            return os << L" }";
         }
      default:
         break;
   }
   return os << L"(unprintable result)";
}

//----------------------------------------------------------------------
// METHOD     : GetProperty 
// DESCRIPTION: Get Property Value from Event Map
// PARAMETER  : None
// RETURN     : Value
//----------------------------------------------------------------------
TEXT* GetProperty(VXIchar* attribute, VXIMap* event )
{
   //TEXT   lAttributeValue[50] = "";
   static TEXT   lAttributeValue[50] = "";
   memset(lAttributeValue, 0, sizeof(lAttributeValue));
   VXIString* evtAttr  = (VXIString *) VXIMapGetProperty(event, attribute);
   if (evtAttr != NULL)
   {
      vxistring  evtValue = VXIStringCStr(evtAttr);
      sprintf(lAttributeValue, "%ls", evtValue.c_str());
   }
   else
      strcpy(lAttributeValue, "NULL");

   return lAttributeValue;
}

//----------------------------------------------------------------------
// METHOD     : GetCommandAttributes 
// DESCRIPTION: Get the command attributes
// PARAMETER  : None
// RETURN     : NONE
//----------------------------------------------------------------------
void GetCommandAttributes(VXIMap* event, CcxmlResponse&  lCcxmlResponse, INT16&  lPegEvent)
{
   char   attr[200];
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if(event == NULL)
      return;

   memset(&lCcxmlResponse, 0, sizeof(CcxmlResponse));

   const VXIValue* evt = reinterpret_cast<VXIValue * > (event);
   VXIValueToStr(evt, attr);

   VXIString* evtAttr  = (VXIString *) VXIMapGetProperty(event, L"name");
   vxistring  evtValue = VXIStringCStr(evtAttr);
   sprintf(lCcxmlResponse.cmdName, "%ls", evtValue.c_str());

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X Cmd:%s Attr:%s", 
         lCcxmlResponse.refNum, lCcxmlResponse.cmdName, attr);
   T(gTrace, printf ("%s: Map Attr:%s Received a Command\n", 
            gProcessName, attr););
   gLog.GenerateLog(CCXML_IVR17, lLogText);

   strcpy(lCcxmlResponse.sessionId, GetProperty(L"sessionid", event));

   lCcxmlResponse.ccxmlMsgType = IvrUtil::GetCommandType(lCcxmlResponse.cmdName);
   switch(lCcxmlResponse.ccxmlMsgType)
   {
      case CCXML_FETCH:
         {
            TEXT   largs[100] = "";

            lPegEvent = PEG_CCXML_FETCH_CMD_SND;
            strcpy(lCcxmlResponse.fetch.fetchId, GetProperty(L"fetchid", event));
            strcpy(lCcxmlResponse.fetch.next, GetProperty(L"uri", event));

            VXIValue * tempMapValue= (VXIValue * ) VXIMapGetProperty(event,L"args");
            VXIMap *  fetch_map;
            VXIMap *  queryarg_map;
            if(tempMapValue!=NULL)
            {
               fetch_map = reinterpret_cast <VXIMap *> (tempMapValue);

               VXIValue * tempV = (VXIValue * ) VXIMapGetProperty(fetch_map, L"okto.fetch.queryargs");
               queryarg_map = reinterpret_cast <VXIMap *> (tempV);

               const VXIchar* key;
               const VXIValue* keyValue;
               int   lkeyCount = 0;
               VXIMapIterator *it = VXIMapGetFirstProperty(queryarg_map,&key,&keyValue);

               do{
                  const VXIString* tmpvalue;
                  tmpvalue = reinterpret_cast<const VXIString*>(keyValue);
                  sprintf(lCcxmlResponse.fetch.namelist.key[lkeyCount], "%ls", key);
                  sprintf(lCcxmlResponse.fetch.namelist.value[lkeyCount], "%ls", VXIStringCStr(tmpvalue));
                  T(gTrace, printf("%s: Key:%s  KValue:%s\n", gProcessName, 
                           lCcxmlResponse.fetch.namelist.key[lkeyCount],
                           lCcxmlResponse.fetch.namelist.value[lkeyCount]););
                  lkeyCount++;

               }while( VXIMapGetNextProperty(it,&key,&keyValue) == VXIvalue_RESULT_SUCCESS );

               strcpy(largs, GetProperty(L"okto.fetch.method", fetch_map));
               lCcxmlResponse.fetch.method = IvrUtil::GetMethodType(largs);

               const VXIValue * tmpVal= VXIMapGetProperty(fetch_map, L"okto.fetch.timeout");
               lCcxmlResponse.fetch.timeout = VXIIntegerValue(reinterpret_cast<const VXIInteger*> (tmpVal));

               tmpVal= VXIMapGetProperty(fetch_map, L"okto.fetch.urlbase");
               const VXIString* tmpbase;
               tmpbase = reinterpret_cast<const VXIString*>(tmpVal);
               vxistring  base = VXIStringCStr(tmpbase);
               sprintf(lCcxmlResponse.fetch.urlBase, "%ls", base.c_str());
            }
            break;
         }
      case CCXML_ACCEPT:
         {
            TEXT   lArg[5] = "";
            //strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            strcpy(lArg, GetProperty(L"connectionid", event));
            //strcpy(lCcxmlResponse.sessionId, GetProperty(L"sessionid", event));
            strncpy(lCcxmlResponse.conId, lArg, strlen(lArg));
            sprintf(lCcxmlResponse.conId, "%s", lArg);
            lPegEvent = PEG_CCXML_ACCEPT_CMD_SND;
            break;
         }
      case	CCXML_CANCEL:
         {
            lPegEvent = PEG_CCXML_CANCEL_CMD_SND;
            break;
         }
      case	CCXML_CREATECALL:
         {
            //char ltemp[25] = "";
            strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            strcpy(lCcxmlResponse.createcall.destAddr, GetProperty(L"dest", event));
            strcpy(lCcxmlResponse.createcall.callerId, GetProperty(L"callerId", event));
            strcpy(lCcxmlResponse.createcall.aai, GetProperty(L"aai", event));
            /*strcpy(ltemp, GetProperty(L"joinid", event));
              lCcxmlResponse.createcall.joinId  = atoi(ltemp);
              strcpy(ltemp, GetProperty(L"joindirection", event));
              lCcxmlResponse.createcall.Direction = IvrUtil::GetJoinDirectionValue(ltemp);*/
            lPegEvent = PEG_CCXML_CREATE_CALL_CMD_SND;
            break;
         }
      case	CCXML_CREATECONFERENCE:
         {
            char ltemp[25] = "";
            strcpy(ltemp,  GetProperty(L"conferenceid", event));
            INT16 tmp = atoi( ltemp);
            lCcxmlResponse.conference.conferenceId = tmp;
            strcpy(lCcxmlResponse.conference.confName, GetProperty(L"confname", event));
            strcpy(ltemp, GetProperty(L"reservedtalkers", event));
            lCcxmlResponse.conference.rtalkers = atoi( ltemp);
            strcpy(ltemp, GetProperty(L"reservedlisteners", event));
            lCcxmlResponse.conference.rlisteners = atoi( ltemp);
            lPegEvent = PEG_CCXML_CREATE_CONFERENCE_CMD_SND;
            break;
         }
      case	CCXML_CREATECCXML:
         {
            pid_t pid;
            pid = getppid();
            char  ltemp[10];

            VXIValue * tempMapValue = (VXIValue * ) VXIMapGetProperty(event,L"args");
            VXIMap   * args_map;
            args_map = reinterpret_cast <VXIMap *> (tempMapValue);

            //strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            strcpy(lCcxmlResponse.createccxml.next, GetProperty(L"next", event));
            strcpy(lCcxmlResponse.createccxml.parentid, GetProperty(L"parentid", args_map));
            strcpy(ltemp, GetProperty(L"method", args_map));
            lCcxmlResponse.createccxml.method = IvrUtil::GetMethodType(ltemp);

            const VXIValue * tmpVal= VXIMapGetProperty(args_map, L"timeout");
            lCcxmlResponse.createccxml.timeout = VXIIntegerValue(reinterpret_cast<const VXIInteger*> (tmpVal));
            lPegEvent = PEG_CCXML_CREATE_CCXML_CMD_SND;
            break;
         }
      case	CCXML_DESTROYCONFERENCE:
         {
            char ltemp[25] = "";
            strcpy(ltemp,  GetProperty(L"conferenceid",event));
            lCcxmlResponse.conference.conferenceId = atoi(ltemp);
            lPegEvent = PEG_CCXML_DESTROY_CONF_CMD_SND;
            break;
         }
      case	CCXML_DIALOGPREPARE:
         {
            char ltemp[50] = "";
            strcpy(lCcxmlResponse.dlgStart.dlgSrc, GetProperty(L"src", event));
            strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            strcpy(lCcxmlResponse.dlgStart.type, GetProperty(L"type", event));
            //strcpy(lCcxmlResponse.dlgStart.dialogId, GetProperty(L"dialogid", event));
            strcpy(ltemp, GetProperty(L"dialogid", event));
            lCcxmlResponse.dlgStart.dialogId = atoi(ltemp);
            /*strcpy(ltemp, GetProperty(L"conferenceid", event));
              lCcxmlResponse.dlgStart.conferenceId = atoi( ltemp);*/

            lPegEvent = PEG_CCXML_DLG_PREPARE_CMD_SND;
            break;
         }
      case CCXML_DIALOGSTART:
         {
            //char ltemp[80] = "";
            if(NULL != GetProperty(L"connectionid", event) )
               strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            strcpy(lCcxmlResponse.dlgStart.duplex, GetProperty(L"duplex", event));
            strcpy(lCcxmlResponse.dlgStart.dlgSrc, GetProperty(L"src", event));
            strcpy(lCcxmlResponse.dlgStart.type, GetProperty(L"type", event));

            VXIMap *  namelist_map;
            VXIValue * tempV = (VXIValue * ) VXIMapGetProperty(event, L"namelist");
            if(tempV != NULL)
            {
               namelist_map = reinterpret_cast <VXIMap *> (tempV);

               const VXIchar* key;
               const VXIValue* keyValue;
               int   lkeyCount = 0;
               VXIMapIterator *it = VXIMapGetFirstProperty(namelist_map,&key,&keyValue);

               do{
                  const VXIString* tmpvalue;
                  tmpvalue = reinterpret_cast<const VXIString*>(keyValue);
                  sprintf(lCcxmlResponse.dlgStart.namelist.key[lkeyCount], "%ls", key);
                  sprintf(lCcxmlResponse.dlgStart.namelist.value[lkeyCount], "%ls", VXIStringCStr(tmpvalue));
                  lkeyCount++;

               }while( VXIMapGetNextProperty(it,&key,&keyValue) == VXIvalue_RESULT_SUCCESS );
            }

            /*strcpy(ltemp, GetProperty(L"dialogid", event));
              lCcxmlResponse.dlgStart.dialogId = atoi(ltemp);
              strcpy(lCcxmlResponse.dlgStart.dialogId, GetProperty(L"prepareddialogid", event));
              strcpy(ltemp, GetProperty(L"conferenceid", event));
              lCcxmlResponse.dlgStart.conferenceId = atoi( ltemp);
              strcpy(ltemp, GetProperty(L"mediadirection", event));
              lCcxmlResponse.createcall.Direction = IvrUtil::GetJoinDirectionValue(ltemp);*/

            lPegEvent = PEG_CCXML_DLG_START_CMD_SND;

            break;
         }
      case	CCXML_DIALOGTERMINATE:
         {
            strcpy(lCcxmlResponse.dlgTerminate.dialogId, GetProperty(L"dialogid", event));
            strcpy(lCcxmlResponse.dlgTerminate.immediate, GetProperty(L"immediate", event));
            lPegEvent = PEG_CCXML_DLG_TERMINATE_CMD_SND;
            break;
         }
      case	CCXML_DISCONNECT:
         {
            strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            strcpy(lCcxmlResponse.disconnect.reason, GetProperty(L"reason", event));
            lPegEvent = PEG_CCXML_DISCONNECT_CMD_SND;
            break;
         }
      case	CCXML_JOIN:
         {
            char ltemp[25] = "";
            strcpy(ltemp,  GetProperty(L"ID1", event));
            lCcxmlResponse.join.id1 = atoi( ltemp);
            strcpy(ltemp,  GetProperty(L"ID2", event));
            lCcxmlResponse.join.id2 = atoi( ltemp);
            strcpy(lCcxmlResponse.join.duplex, GetProperty(L"duplex", event));
            lPegEvent = PEG_CCXML_JOIN_CMD_SND;
            break;
         }
      case	CCXML_MOVE:
         {
            strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            strcpy(lCcxmlResponse.redirect.reason, GetProperty(L"reason", event));
            lPegEvent = PEG_CCXML_MOVE_CMD_SND;
            break;
         }
      case	CCXML_REDIRECT:
         {
            strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            strcpy(lCcxmlResponse.redirect.destAddr, GetProperty(L"dest", event));
            strcpy(lCcxmlResponse.redirect.reason, GetProperty(L"reason", event));
            lPegEvent = PEG_CCXML_REDIRECT_CMD_SND;

            break;
         }
      case	CCXML_REJECT:
         {
            strcpy(lCcxmlResponse.conId, GetProperty(L"connectionid", event));
            //strcpy(lCcxmlResponse.reject.reason, GetProperty(L"reason", event));
            lPegEvent = PEG_CCXML_REJECT_CMD_SND;
            break;
         }
      case	CCXML_SEND:
         {
            lPegEvent = PEG_CCXML_SEND_CMD_SND;
            break;
         }
      case	CCXML_UNJOIN:
         {
            char ltemp[25] = "";
            strcpy(ltemp,  GetProperty(L"ID1", event));
            lCcxmlResponse.unjoin.id1 = atoi( ltemp);
            strcpy(ltemp,  GetProperty(L"ID2", event));
            lCcxmlResponse.unjoin.id2 = atoi( ltemp);
            lPegEvent = PEG_CCXML_UNJOIN_CMD_SND;
            break;
         }
      default:break;

   }
}

//----------------------------------------------------------------------
// METHOD     : thread() 
// DESCRIPTION: Creates Okto Event Thread
// PARAMETER  : None
// RETURN     : NONE
//----------------------------------------------------------------------
void OktopousEventThread::thread()
{
   VXIMap*          event;
   CcxmlResponse    lccxmlresponse;
   int              count = 0;
   INT16     lCId;

   memset(&lCId, 0, sizeof(lCId));

   strcpy(gProcessName, "IVR_CP");
   while ( true )
   {
      event = myEventQueue->GetCommand();
      memset(&lccxmlresponse, 0, sizeof(CcxmlResponse));
      VXIString* evtAttr  = (VXIString *) VXIMapGetProperty(event, L"name");
      vxistring  evtValue = VXIStringCStr(evtAttr);
      sprintf(lccxmlresponse.cmdName, "%ls", evtValue.c_str());

      if(strcmp(lccxmlresponse.cmdName,"command.exit") == 0)
         break;

      assert(event != 0);

      if (VXIMapNumProperties(event) > 0) 
      {
         std::basic_ostringstream<VXIchar> osstream;
         std::basic_ostream<VXIchar> outstream(osstream.rdbuf());
         outstream << event;
         //printf(" Received Event : %S",outstream.c_str());
         //fprintf(stdout," Received Event : %S",out.str().c_str());
      }

      VXIString* evtsidAttr  = (VXIString *) VXIMapGetProperty(event, L"sessionid");
      vxistring  evtsidValue = VXIStringCStr(evtsidAttr);

      INT16   lPegEvent;
      GetCommandAttributes( event, lccxmlresponse, lPegEvent);
      //printf("******************************** 1 : lccxmlresponse.conId:%d\n", lccxmlresponse.conId);

      if((count == 0 || lCId == 0)&& strlen(lccxmlresponse.conId) > 0 )
      {
         //printf("******************************** 2 : lccxmlresponse.conId:%d count:%d lCId:%d\n", lccxmlresponse.conId, count, lCId);
         mChannelId = atoi(lccxmlresponse.conId);
         count = 1;
         lCId = mChannelId;
      }
      //else if(lCId == 0)
      if(lCId == 0)
      {
         //printf("******************************** 3 : lccxmlresponse.conId:%d count:%d lCId:%d\n", lccxmlresponse.conId, count, lCId);
         lCId = atoi(lccxmlresponse.sessionId);
      }

      sprintf(lccxmlresponse.sessionId, "%ls", evtsidValue.c_str());
      //printf("******************************** 4 : lccxmlresponse.conId:%d count:%d lCId:%d\n", lccxmlresponse.conId, count, lCId);
      lccxmlresponse.channelId = lCId;
      lccxmlresponse.refNum    = gRefNum;

      switch(lccxmlresponse.ccxmlMsgType)
      {
         case CCXML_CREATECALL:
            gIvrQueApi.WriteToCcxmlRspQue(lccxmlresponse, lCId);
            break;
         case CCXML_JOIN:
            if(lCId == lccxmlresponse.join.id1)
               gSecondaryChannelId = lccxmlresponse.join.id2; 
            else
            {
               gSecondaryChannelId     = lccxmlresponse.join.id1;
               lccxmlresponse.join.id1 = lccxmlresponse.join.id2;
               lccxmlresponse.join.id2 = gSecondaryChannelId;
            }

            T(gTrace, printf("%s: RefNum:%08X SecCID:%ld\n", gProcessName,gRefNum, gSecondaryChannelId););
            gIvrQueApi.WriteToEventChildQue(lccxmlresponse, gSecondaryChannelId);
            break;

         case CCXML_UNJOIN:
            T(gTrace, printf("%s: RefNum:%08X Received REL On CID:%d\n", gProcessName, gRefNum, gCId););
            if(lCId == lccxmlresponse.unjoin.id1)
               gSecondaryChannelId = lccxmlresponse.unjoin.id2; 
            else
            {
               gSecondaryChannelId = lccxmlresponse.unjoin.id1;
               lccxmlresponse.unjoin.id1 = lccxmlresponse.unjoin.id2;
               lccxmlresponse.unjoin.id2 = gSecondaryChannelId;
            }

            if(gCId == gSecondaryChannelId)
            {
               T(gTrace, printf("%s: RefNum:%08X Sending CCXML_UNJOIN for CID:%d\n", gProcessName, gRefNum, lCId););
               gIvrQueApi.WriteToEventChildQue(lccxmlresponse, lCId);
            }
            else if(gCId == lCId)
            {
               T(gTrace, printf("%s: RefNum:%08X Sending CCXML_UNJOIN for CID:%ld\n", gProcessName, gRefNum, gSecondaryChannelId););
               gIvrQueApi.WriteToEventChildQue(lccxmlresponse, gSecondaryChannelId);
            }
            break;

         default:   
            gIvrQueApi.WriteToEventChildQue(lccxmlresponse, lCId);
            break;
      }

      gIvrQueApi.WriteToIvrPegQueue(lPegEvent);
      gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CMD_SND);
   }

   VXIMapDestroy(&event);
} 

