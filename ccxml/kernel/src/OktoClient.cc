static const char* id="@(#) $Id: OktoClient.cc,v 1.1.1.1.2.6.2.2 2025/01/02 11:35:11 mram Exp $";
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

#include <Log.hpp>
#include <Inet.hpp>
#include <ECMAScript.hpp>
#include <OutputQueue.hpp>
#include <OktoPlatform.hpp>
#include <OktoClient.h>
#include <OktoEventThread.h>
#include <unistd.h>
#include "Fetcher.hpp"
#include "DocumentParser.hpp"
#include "DocumentModel.hpp"
#include "Util.hpp"
#include "IvrQueApi.h"
#include "Scripter.hpp"

extern IvrQueApi   gIvrQueApi;
unsigned int       gRefNum;
int                lConnectedCount = 0;
INT16              gSessionsJoined[10];
INT16              gCId;
TEXT               gLogtext[MAX_LOG_TEXT_LEN + 1] = "";

static int WtoC(const VXIchar* ws, char **s) {

     if(!ws) {
          return -1;
     }

     int wsLen = ( int ) wcslen(ws) + 1;
     *s = new char[wsLen];

     if(!(*s)) {
          return -1;
     }
     wcstombs(*s, ws, wsLen);

     return 0;
}

static int CtoW(char *s, const VXIchar** ws) {

     if(!s) {
          return -1;
     }

     int sLen = strlen(s) + 1;
     *ws = new VXIchar[sLen];

     if(!(*ws)) {
          return -1;
     }
     mbstowcs((VXIchar*)*ws, s, sLen);

     return 0;
}

void SetMapArgs(VXIMap* Args, CcxmlRequest& lCcxmlRequest)
{
   const VXIchar*   eval;
   char  ConId[5];
   char  Attr[100];
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X EVT:%s SID:%d CID:%d ",
         lCcxmlRequest.refNum, lCcxmlRequest.ccxmlEventRequest.event, 
         lCcxmlRequest.sessionId, lCcxmlRequest.channelId);
   snprintf(gLogtext, MAX_LOG_TEXT_LEN, "%s Setting Map Arguments", lLogText);
   /*T(gTrace,
     printf("%s: ---------------------------------------------------------------\n",
     gProcessName););
     T(gTrace, printf("%s; %s Setting Map Arguments\n", gProcessName, lLogText););
     T(gTrace,
     printf("%s: ---------------------------------------------------------------\n",
     gProcessName););*/
   gLog.GenerateLog (CCXML_IVR10, 2, gLogtext);


   if(strncmp("connection.", lCcxmlRequest.ccxmlEventRequest.event, 11) == 0)
   {
      VXIMapSetProperty( Args, L"connection" , (VXIValue *) VXIStringCreate(L"connection") );

      CtoW(&lCcxmlRequest.ccxmlEventRequest.origAddr[0],&eval);
      VXIMapSetProperty( Args, L"remote" , (VXIValue *) VXIStringCreate(eval) );
      VXIMapSetProperty( Args, L"originator" , (VXIValue *) VXIStringCreate(eval) );

      sprintf(ConId, "%d", lCcxmlRequest.channelId);
      CtoW(&ConId[0],&eval);
      VXIMapSetProperty( Args, L"connectionid" , (VXIValue *) VXIStringCreate(eval) );

      if(strlen(lCcxmlRequest.ccxmlEventRequest.dialogId) > 0)
      {
         CtoW(&lCcxmlRequest.ccxmlEventRequest.dialogId[0],&eval);
         VXIMapSetProperty( Args, L"dialogid" , (VXIValue *) VXIStringCreate(eval) );
      }

      if(strlen(lCcxmlRequest.ccxmlEventRequest.destAddr) > 0)
      {
         CtoW(&lCcxmlRequest.ccxmlEventRequest.destAddr[0],&eval);
         VXIMapSetProperty( Args, L"endpoint" , (VXIValue *) VXIStringCreate(eval) );
      }
   }
   else if(strncmp("conference.", lCcxmlRequest.ccxmlEventRequest.event, 11) == 0)
   {

      const VXIchar*   key;
      char        lkey[10];

      VXIMapSetProperty( Args, L"conference" , (VXIValue *) VXIStringCreate(L"conference") );

      sprintf(ConId, "%d", lCcxmlRequest.channelId);
      CtoW(&ConId[0],&eval);
      VXIMapSetProperty( Args, L"conferenceid" , (VXIValue *) VXIStringCreate(eval) );

      for(int i = 0; i <MAX_KEYS; i++)
      {
         if(strlen(lCcxmlRequest.ccxmlEventRequest.conference.conflist.key[i]) > 0)
         {
            sprintf(lkey, "%s", lCcxmlRequest.ccxmlEventRequest.conference.conflist.key[i]);
            CtoW(&lkey[0],&key);

            /*T(gTrace, printf("%s: ----------------------------------------------------\n", 
              gProcessName););*/
            //T(gTrace, printf("%s: Key      : %s\n", gProcessName,lkey););
            sprintf(lkey, "%s", lCcxmlRequest.ccxmlEventRequest.conference.conflist.value[i]);
            CtoW(&lkey[0],&eval);
            //T(gTrace, printf("%s: Key Value: %s\n", gProcessName,lkey););
            VXIMapSetProperty( Args, key, (VXIValue *) VXIStringCreate(eval) );
         }
      }
      /*T(gTrace, printf("%s: ----------------------------------------------------\n", 
        gProcessName););*/

      //VXIMapSetProperty( Args, L"connections" ,  (VXIValue *) VXIStringCreate(L"connections") );
      /*VXIValue* tval = reinterpret_cast<VXIValue*>(Args);
        VXIPrintValue(tval);*/
   }
   else if(strncmp("dialog.", lCcxmlRequest.ccxmlEventRequest.event, 7) == 0)
   {
      //Set the values of dialog event
      const VXIchar*   key;
      char        lkey[10];

      VXIMapSetProperty( Args, L"dialog" , (VXIValue *) VXIStringCreate(L"dialog") );
      sprintf(ConId, "%d", lCcxmlRequest.channelId);
      CtoW(&ConId[0],&eval);
      VXIMapSetProperty( Args, L"dialogid" , (VXIValue *) VXIStringCreate(eval) );
      //VXIMapSetProperty( Args, L"dialogid" , (VXIValue *) VXIStringCreate(L"1234") );

      /*T(gTrace, printf("%s: ----------------------------------------------------\n", 
        gProcessName););*/
      for(int i = 0; i <MAX_KEYS; i++)
      {
         if(strlen(lCcxmlRequest.ccxmlEventRequest.dialogexit.keyvalue.key[i]) > 0)
         {
            sprintf(lkey, "%s", lCcxmlRequest.ccxmlEventRequest.dialogexit.keyvalue.key[i]);
            CtoW(&lkey[0],&key);
            //T(gTrace, printf("%s: Key      : %s\n", gProcessName,lkey););

            sprintf(lkey, "%s", lCcxmlRequest.ccxmlEventRequest.dialogexit.keyvalue.value[i]);
            CtoW(&lkey[0],&eval);
            //T(gTrace, printf("%s: Key Value: %s\n", gProcessName,lkey););
            VXIMapSetProperty( Args, key, (VXIValue *) VXIStringCreate(eval) );
         }
      }
      /*T(gTrace, printf("%s: ----------------------------------------------------\n", 
        gProcessName););*/
   }
   else if(strcmp(lCcxmlRequest.ccxmlUrlRequest.args, "") != 0)
   {
      sprintf(ConId, "%s", lCcxmlRequest.ccxmlUrlRequest.args);
      CtoW(&lCcxmlRequest.ccxmlUrlRequest.args[0],&eval);
      VXIMapSetProperty( Args, L"parentid" , (VXIValue *) VXIStringCreate(eval) );
   }
   else if(lCcxmlRequest.ccxmlUrlRequest.varCount > 0)
   {
      if(strlen(lCcxmlRequest.ccxmlUrlRequest.var1) > 0)
      {
         CtoW(&lCcxmlRequest.ccxmlUrlRequest.var1[0], &eval);
         VXIMapSetProperty( Args, L"T_VAR1" , (VXIValue *) VXIStringCreate(eval) );
         //T(gTrace, printf("%s: T_VAR1 : %s\n", gProcessName, lCcxmlRequest.ccxmlUrlRequest.var1););
      }
      if(strlen(lCcxmlRequest.ccxmlUrlRequest.var2) > 0)
      {
         CtoW(&lCcxmlRequest.ccxmlUrlRequest.var2[0], &eval);
         VXIMapSetProperty( Args, L"T_VAR2" , (VXIValue *) VXIStringCreate(eval) );
         //T(gTrace, printf("%s: T_VAR2 : %s\n", gProcessName, lCcxmlRequest.ccxmlUrlRequest.var2););
      }
      if(strlen(lCcxmlRequest.ccxmlUrlRequest.var3) > 0)
      {
         CtoW(&lCcxmlRequest.ccxmlUrlRequest.var3[0], &eval);
         VXIMapSetProperty( Args, L"T_VAR3" , (VXIValue *) VXIStringCreate(eval) );
         //T(gTrace, printf("%s: T_VAR3 : %s\n", gProcessName, lCcxmlRequest.ccxmlUrlRequest.var3););
      }
      if(strlen(lCcxmlRequest.ccxmlUrlRequest.var4) > 0)
      {
         CtoW(&lCcxmlRequest.ccxmlUrlRequest.var4[0], &eval);
         VXIMapSetProperty( Args, L"T_VAR4" , (VXIValue *) VXIStringCreate(eval) );
         //T(gTrace, printf("%s: T_VAR4 : %s\n", gProcessName, lCcxmlRequest.ccxmlUrlRequest.var4););
      }
      if(strlen(lCcxmlRequest.ccxmlUrlRequest.var5) > 0)
      {
         CtoW(&lCcxmlRequest.ccxmlUrlRequest.var5[0], &eval);
         VXIMapSetProperty( Args, L"T_VAR5" , (VXIValue *) VXIStringCreate(eval) );
         //T(gTrace, printf("%s: T_VAR5 : %s\n", gProcessName, lCcxmlRequest.ccxmlUrlRequest.var5););
      }
   }

   const VXIValue* evt_attr = reinterpret_cast<VXIValue * > (Args);
   VXIValueToStr(evt_attr, Attr);
   strcat(lLogText, " Attr:");
   strcat(lLogText, Attr);
   snprintf(gLogtext, MAX_LOG_TEXT_LEN, "%s Success to Set Map Arguments", lLogText);
   //T(gTrace, printf("%s; %s Success to Set Map Arguments\n", gProcessName, lLogText););
   gLog.GenerateLog (CCXML_IVR11, 2,gLogtext);
}

void CreateMapArgs(VXIMap** connArgs, CcxmlRequest& lCcxmlRequest)
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN] = "";

   if((connArgs == NULL))
   {
      return;
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X EVT:%s CID:%d SID:%d",
         lCcxmlRequest.refNum, lCcxmlRequest.ccxmlEventRequest.event, 
         lCcxmlRequest.channelId, lCcxmlRequest.sessionId);
   snprintf(gLogtext, MAX_LOG_TEXT_LEN, "%s Creating Map Arguments", lLogText);
   /*T(gTrace, printf("%s : %s Creating Map Arguments\n",
     gProcessName, lLogText););*/
   gLog.GenerateLog (CCXML_IVR12, 2, gLogtext);

   *connArgs = VXIMapCreate();

   SetMapArgs(*connArgs, lCcxmlRequest);
}

OktoClient::OktoClient()
{
   // Initialize the Oktopous system
   outQ = NULL;
   gblLog = new PLog();
   // Init the global logger
   // Initialize Inet subsystem with 2 Async downloaders.
   OktoInet::InetResult res = Inet::Initialize(1);
   // Init ECMA subsystem with appropriate runtime and context size
   ECMAScript::Initialize(0,gblLog,1024*1024,128*1024,10);
   // Init the Interpreter subsystem
   OktoInterpreter::Initialize(gblLog,0,&outQ);

}

//----------------------------------------------------------------------
// METHOD     : GetProperty 
// DESCRIPTION: Get Property Value from Event Map
// PARAMETER  : None
// RETURN     : Value
//----------------------------------------------------------------------
TEXT* OktoClient::GetProperty(VXIchar* attribute, VXIMap* event )
{
   //	TEXT   lAttributeValue[50] = "";
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
int OktoClient::buildClient(char * ccxmlurl, CcxmlRequest& lCcxmlReq)
{
   //sprintf(gProcessName, "IVR_CCXML_%d", lCcxmlReq.sessionId);
   sprintf(gProcessName, "CP-%04d", lCcxmlReq.sessionId);
   // Now, run the Event thread to process commands coming from the Interpreter.
   eventThread = new OktopousEventThread(outQ);
   eventThread->run();
   // create resources needed for interpreter.
   struct OktoInterpreter::Resources *resources;
   resources = new struct OktoInterpreter::Resources;
   ECMAScript* myECMA = new ECMAScript();
   Inet* myInet = new Inet();
   resources->inet = myInet;
   resources->jsi = myECMA;
   resources->log = gblLog;
   resources->platform = this;

   // create a new instance of Interpreter
   myInterpreter = OktoInterpreter::CreateInstance();

   // Run the interpreter
   const VXIchar * myUrl;
   const VXIchar * sid;
   VXIMap*         pid;
   char  lsessionid[10] = "";
   gRefNum = lCcxmlReq.refNum;

   sprintf(lsessionid, "%d", lCcxmlReq.sessionId);
   CtoW(ccxmlurl,&myUrl);	
   CtoW(lsessionid,&sid);	

   OktoInterpreter::Result res;
   if(strcmp(lCcxmlReq.ccxmlUrlRequest.args, "") != 0)
   {
      CreateMapArgs(&pid, lCcxmlReq);
      res = myInterpreter->Run(myUrl, sid, NULL,pid,resources);
   }
   else
   {
      VXIMap*     lCcxmlVarMap;
      CreateMapArgs(&lCcxmlVarMap, lCcxmlReq);

      TEXT   ltemp[100] = "";
      strcpy(ltemp,  GetProperty(L"T_VAR1", lCcxmlVarMap));
      T(gTrace, printf("Retrieved Property : T_VAR1 : %s\n", ltemp););
      strcpy(ltemp,  GetProperty(L"T_VAR2", lCcxmlVarMap));////
      T(gTrace, printf("Retrieved Property : T_VAR2 : %s\n", ltemp););
      //OktoInterpreter::Result	res = myInterpreter->Run(myUrl, sid, NULL,NULL,resources);
      res = myInterpreter->Run(myUrl, sid, sid, lCcxmlVarMap,resources); 
      //  OktoInterpreter::Result    res = myInterpreter->Run(myUrl, sid, NULL,NULL,resources);

      if (res != OktoInterpreter::RESULT_SUCCESS) {

         T(gTrace, printf("%s: RefNum:%08X Interpreter Run Failed with code - %d\n",gProcessName, gRefNum, res););
      }
   }
   delete [] myUrl;
   return res;
}


int OktoClient::PostEvent(CcxmlRequest& lCcxmlRequest)
{
   const VXIchar*   vevent;
   VXIMap*          event;
   char             CId[10];
   TEXT             lLogText[MAX_LOG_TEXT_LEN] = "";

   //sprintf(CId, "%d", lCcxmlRequest.channelId);
   sprintf(CId, "%d", lCcxmlRequest.sessionId);

   event = VXIMapCreate();

   CtoW(&lCcxmlRequest.ccxmlEventRequest.event[0],&vevent);
   VXIMapSetProperty( event, L"name" , (VXIValue *) VXIStringCreate(vevent) );
   CtoW(&CId[0],&vevent);
   VXIMapSetProperty( event, L"connectionid" , (VXIValue *) VXIStringCreate(vevent) );

   if(strcmp(lCcxmlRequest.ccxmlEventRequest.event, "fetch.done") == 0)
   {
      const   VXIchar * fId;
      const   VXIchar * url;
      Inet*   myInet  = new Inet();
      PLog*   myLog   = new PLog();

      CtoW(&lCcxmlRequest.ccxmlEventRequest.fetchDone.url[0],&url);
      CtoW(&lCcxmlRequest.ccxmlEventRequest.fetchDone.fetchId[0],&fId);

      int              res;
      Fetcher          * fetcher = new Fetcher(myLog, myInet);
      DocumentParser   * parser  = new DocumentParser();

      snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%d SID:%d FID:%s Url:%s",
            lCcxmlRequest.refNum, lCcxmlRequest.channelId, lCcxmlRequest.sessionId,
            lCcxmlRequest.ccxmlEventRequest.fetchDone.fetchId, 
            lCcxmlRequest.ccxmlEventRequest.fetchDone.url);

      if ((res = fetcher->FetchDocument(fId, url, NULL, &event )) != OktoInterpreter::RESULT_SUCCESS )
      {
         snprintf(gLogtext, MAX_LOG_TEXT_LEN, "%s Failed to fetch the Document", lLogText);
         TERR(gTrace,printf("%s: %s\n", gProcessName, gLogtext););
         gLog.GenerateLog(CCXML_IVR13, gLogtext );
      }
      else
      {
         CCXMLDocument initialDoc;

         snprintf(gLogtext, MAX_LOG_TEXT_LEN, 
               "%s Fetched the Document successfully", lLogText);
         T(gTrace,printf("%s: %s\n", gProcessName, gLogtext););
         gLog.GenerateLog(CCXML_IVR14, 1, gLogtext );

         VXIContent * content = ( VXIContent * ) VXIMapGetProperty( event, L"content" );
         int parse_result= parser->ParseDocument(url, content, initialDoc );

         if ( parse_result != 0 ) {
            snprintf(gLogtext, MAX_LOG_TEXT_LEN, 
                  "%s Failed to Parse the Document. Map Destroyed", lLogText);
            TERR(gTrace,printf("%s: %s\n", gProcessName, gLogtext););
            gLog.GenerateLog(CCXML_IVR15, gLogtext);
            VXIMapDestroy( &event );
         }
         Create_Fetch_Done(fId, url, content, &event);

         snprintf(gLogtext, MAX_LOG_TEXT_LEN, 
               "%s Success to Create Fetch Done", lLogText);
         T(gTrace,printf("%s: %s\n", gProcessName, gLogtext););
         gLog.GenerateLog(CCXML_IVR16, 1, gLogtext );
      }
   }
   else if(strncmp("connection.", lCcxmlRequest.ccxmlEventRequest.event, 11) == 0)
   {
      if(strcmp("connection.alerting", lCcxmlRequest.ccxmlEventRequest.event) == 0)
      {
         VXIMap   *connArgs;

         CreateMapArgs(&connArgs, lCcxmlRequest);

         CtoW(&CId[0],&vevent);
         Create_Connection_Event(connection_ALERTING, vevent, NULL, NULL, NULL, connArgs, &event);
         //CtoW(&lCcxmlRequest.ccxmlEventRequest.event[0],&vevent);
         //VXIMapSetProperty( event, L"name" , (VXIValue *) VXIStringCreate(vevent) );
         //CtoW(&CId[0],&vevent);
         //VXIMapSetProperty( event, L"connectionid" , (VXIValue *) VXIStringCreate(vevent) );
      }
      if(strcmp("connection.connected", lCcxmlRequest.ccxmlEventRequest.event) == 0)
      {
         gSessionsJoined[lConnectedCount++] = lCcxmlRequest.sessionId; 
         T(gTrace, printf("%s: RefNum:%08X ChannelId Connected:%d\n ", gProcessName, gRefNum,
                  gSessionsJoined[lConnectedCount - 1]););
      }
      if(strcmp("connection.disconnected", lCcxmlRequest.ccxmlEventRequest.event) == 0)
      {
         gCId = atoi(CId);
         T(gTrace, printf("%s: RefNum:%08X Received REL On CID:%d(%s)\n", gProcessName, gRefNum, gCId, CId););
      }
   }
   else if(strncmp("dialog.", lCcxmlRequest.ccxmlEventRequest.event, 7) == 0)
   {
      VXIMap   *connArgs;

      CreateMapArgs(&connArgs, lCcxmlRequest);


      /*CtoW(&lCcxmlRequest.ccxmlEventRequest.dialogId[0],&vevent);
        VXIMapSetProperty( event, L"session.dialog" , (VXIValue *) VXIStringCreate(vevent) );*/
      VXIMapSetProperty( event, DE_VALUES_FIELD, VXIValueClone( ( VXIValue * ) connArgs ) );
   }
   else if(strncmp("conference.", lCcxmlRequest.ccxmlEventRequest.event, 11) == 0)
   {
      VXIMap*  confArgs;

      CreateMapArgs(&confArgs, lCcxmlRequest);
      if(confArgs)
      {
         VXIMapSetProperty( event, L"conference", VXIValueClone( ( VXIValue * ) confArgs ) );
      }

      /*if(strcmp("conference.joined", lCcxmlRequest.ccxmlEventRequest.event) == 0)
        {
        gSessionsJoined[lConnectedCount++] = lCcxmlRequest.sessionId; 
        T(gTrace, printf("%s: ChannelId Joined:%d\n ", gProcessName,  gSessionsJoined[lConnectedCount - 1]););
        }*/
   }

   CtoW(&lCcxmlRequest.ccxmlEventRequest.event[0],&vevent);
   VXIMapSetProperty( event, L"name" , (VXIValue *) VXIStringCreate(vevent) );
   CtoW(&CId[0],&vevent);
   VXIMapSetProperty( event, L"connectionid" , (VXIValue *) VXIStringCreate(vevent) );
   VXIMapSetProperty( event, L"dialogid" , (VXIValue *) VXIStringCreate(vevent) );

   delete [] vevent;

   myInterpreter->PostEvent(event,0);

   //int result = myInterpreter->PostEvent(event,0);
   //T(gTrace, printf("%s: CSI Intergreter PostEvent Result = %d\n", gProcessName, result););
}

void OktoClient::SendExit(const VXIchar *sessionid, const VXIchar *parentid )
{

   TEXT        lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT        lpid[10] = "";
   KernelMsg   lKernelMsg;
   BOOLEAN     lExitParent = false;
   INT16       lExitCId;

   memset(&lKernelMsg, 0, sizeof(KernelMsg));

   lKernelMsg.kernelMsgType              = CCXML_RSP;
   lKernelMsg.ccxmlResponse.ccxmlMsgType = CCXML_EXIT;
   lKernelMsg.ccxmlResponse.exit.reason  = RSN_OK;
   lKernelMsg.refNum                     = gRefNum;
   lKernelMsg.ccxmlResponse.refNum       = lKernelMsg.refNum;
   sprintf(lpid, "%ls", parentid);
   lKernelMsg.ccxmlResponse.exit.pid     = atoi(lpid);
   strcpy(lKernelMsg.ccxmlResponse.cmdName, "Exit");

   sprintf(lKernelMsg.ccxmlResponse.conId, "%ls", sessionid);
   lExitCId = atoi(lKernelMsg.ccxmlResponse.conId);

   T(gTrace, printf("%s: RefNum:%08X Number of Sessions Connected :%d lExitCID:%d\n", 
            gProcessName, gRefNum, lConnectedCount, lExitCId););

   for(int i=0; i<=lConnectedCount; i++)
   {
      if( i == lConnectedCount && lConnectedCount <= 2)
      {
         sprintf(lKernelMsg.ccxmlResponse.conId, "%ls", sessionid);
         lExitParent = true;
      }
      else
      {
         T(gTrace, printf("%s: RefNum:%08X At Exit CID:%d SessionsJoined:%d\n", 
                  gProcessName, gRefNum, gCId, gSessionsJoined[i]););
         if(lExitCId != gSessionsJoined[i])
         {
            sprintf(lKernelMsg.ccxmlResponse.conId, "%d",  gSessionsJoined[i]);
            gSessionsJoined[i] = 0; 
         }
         else
            continue;
      }

      strcpy(lKernelMsg.ccxmlResponse.sessionId, lKernelMsg.ccxmlResponse.conId);
      //strcpy(lKernelMsg.ccxmlResponse.cmdName, "Exit");
      lKernelMsg.ccxmlResponse.channelId = atoi(lKernelMsg.ccxmlResponse.conId);

      INT16 lMType = atoi(lKernelMsg.ccxmlResponse.conId);
      lKernelMsg.ccxmlResponse.channelId = lMType;

      sprintf(lLogText, "RefNum:%08X CID:%s SID:%s KMsgType:%d CMsgType:%d Rsn:%d PID:%d", 
            lKernelMsg.refNum, lKernelMsg.ccxmlResponse.conId, lKernelMsg.ccxmlResponse.sessionId,
            lKernelMsg.kernelMsgType, lKernelMsg.ccxmlResponse.ccxmlMsgType,
            lKernelMsg.ccxmlResponse.exit.reason, lKernelMsg.ccxmlResponse.exit.pid);

      //gIvrQueApi.WriteToCcxmlRspQue(lKernelMsg.ccxmlResponse, lMType);
      gIvrQueApi.WriteToEventChildQue(lKernelMsg.ccxmlResponse, lMType);
      gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_EXIT_SND);

      snprintf(gLogtext, MAX_LOG_TEXT_LEN, 
            "%s Sent Session Exit", lLogText);
      T (gTrace, printf ("%s: %s\n", gProcessName, gLogtext););
      gLog.GenerateLog(CCXML_IVR21, gLogtext);
   }

   if(lExitParent == true)
   {
      snprintf(gLogtext, MAX_LOG_TEXT_LEN, 
            "%s CCXML Session closed", lLogText);
      T (gTrace, printf ("%s: %s\n", gProcessName, gLogtext););
      gLog.GenerateLog(CCXML_IVR20, gLogtext);
      _exit(0);
   }
}
