static const char* id="@(#) $Id: IvrDbOperation.cc,v 1.1.1.1.2.5.2.4 2025/01/02 11:41:20 mram Exp $" ;
//---------------------------------------------------------------------------------
// NAME        : IvrDbOperation.cc
//
// COPYRIGHT   : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//               All rights reserved. No part of this computer program
//               may be used or reproduced in any form by any
//               means without prior written permission of
//               Tayana Software Solutions Pvt Ltd.
//
// Originated  : Divya K                                     Dated: 09-FEB-2011
//---------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "Log.h"
#include "LogApi.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ProcessLock.h"
#include "CfgRead.h"
#include "IvrLimits.h"
#include "IvrDbManager.h"

Log                 gLog;
CTrace              gTrace("TRACE_HTTP");
TEXT                gProcessName[MAX_PROCESS_NAME] = " ";
TEXT                gLogText[MAX_LOG_TEXT_LEN + 1] = "";
TEXT                gCallReference[20] = "";

/** Convert a two-char hex string into the char it represents. **/
char x2c(char *what) {
	register char digit;

	digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
	digit *= 16;
	digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
	return(digit);
}

/** Reduce any %xx escape sequences to the characters they represent. **/
void unescape_url(char *url) {
	register int i,j;

	for(i=0,j=0; url[j]; ++i,++j) {
		if((url[i] = url[j]) == '%') {
			url[i] = x2c(&url[j+1]) ;
			j+= 2 ;
		}
	}
	url[i] = '\0' ;
}

char **getcgivars() 
{
	register int i ;
	char *request_method ;
	int content_length;
	char *cgiinput ;
	char **cgivars ;
	char **pairlist ;
	int paircount ;
	char *nvpair ;
	char *eqpos ;

	// Depending on the request method, read all CGI input into cgiinput. 
	request_method= getenv("REQUEST_METHOD") ;

	if (!strcmp(request_method, "GET") || !strcmp(request_method, "HEAD") ) {
		// Some servers apparently don't provide QUERY_STRING if it's empty, //
		//   so avoid strdup()'ing a NULL pointer here.                      //
		char *qs ;
		qs= getenv("QUERY_STRING") ;
		cgiinput= strdup(qs  ? qs  : "") ;
	}
	else if (!strcmp(request_method, "POST")) {
		// strcasecmp() is not supported in Windows-- use strcmpi() instead 
		if ( strcasecmp(getenv("CONTENT_TYPE"), "application/x-www-form-urlencoded")) {
			TERR(gTrace, printf("Content-Type: text/plain\n\n"););
			TERR(gTrace, printf("getcgivars(): Unsupported Content-Type.\n");) ;
			exit(1) ;
		}
		if ( !(content_length = atoi(getenv("CONTENT_LENGTH"))) ) {
			TERR(gTrace, printf("Content-Type: text/plain\n\n");) ;
			TERR(gTrace, printf("getcgivars(): No Content-Length was sent with the POST request.\n");) ;
			exit(1) ;
		}
		if ( !(cgiinput= (char *) malloc(content_length+1)) ) {
			TERR(gTrace, printf("Content-Type: text/plain\n\n"););
			TERR(gTrace, printf("getcgivars(): Couldn't malloc for cgiinput.\n");) ;
			exit(1) ;
		}
		if (!fread(cgiinput, content_length, 1, stdin)) {
			TERR(gTrace, printf("Content-Type: text/plain\n\n");) ;
			TERR(gTrace, printf("getcgivars(): Couldn't read CGI input from STDIN.\n");) ;
			exit(1) ;
		}
		cgiinput[content_length]='\0' ;
	}
	else {
		printf("Content-Type: text/plain\n\n") ;
		printf("getcgivars(): Unsupported REQUEST_METHOD.\n") ;
		exit(1) ;
	}

	// Change all plusses back to spaces. //
	for (i=0; cgiinput[i]; i++) if (cgiinput[i] == '+') cgiinput[i] = ' ' ;

	// First, split on "&" and ";" to extract the name-value pairs into //
	//   pairlist.                                                      //
	pairlist= (char **) malloc(256*sizeof(char **)) ;
	paircount= 0 ;
	nvpair= strtok(cgiinput, "&;") ;
	while (nvpair) {
		pairlist[paircount++]= strdup(nvpair) ;
		if (!(paircount%256))
			pairlist= (char **) realloc(pairlist,(paircount+256)*sizeof(char **)) ;
		nvpair= strtok(NULL, "&;") ;
	}
	pairlist[paircount]= 0 ;    // terminate the list with NULL //

	// Then, from the list of pairs, extract the names and values. //
	cgivars= (char **) malloc((paircount*2+1)*sizeof(char **));
	for (i= 0; i<paircount; i++) 
	{
		if ((eqpos=strchr(pairlist[i], '=')) )
		{
			*eqpos= '\0' ;
			unescape_url(cgivars[i*2+1]= strdup(eqpos+1)) ;
		} else {
			unescape_url(cgivars[i*2+1]= strdup("")) ;
		}
		unescape_url(cgivars[i*2]= strdup(pairlist[i])) ;
	}
	cgivars[paircount*2]= 0 ;   // terminate the list with NULL /

	// Free anything that needs to be freed. //
	free(cgiinput) ;
	for (i=0; pairlist[i]; i++) free(pairlist[i]) ;
	free(pairlist) ;

	// Return the list of name-value strings. //
	return cgivars ;

}

void PrintError(int lErrorCode, TEXT* ErrMsg, TEXT* lAppName)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN+1] = "";
   //IvrUtil::ReplaceAll(ErrMsg, "'", "\\'");
   //printf(" ");
   printf("<?xml version=\"1.0\"?>\n");
   printf("<vxml version=\"2.0\" application=\"%s\" xmlns=\"http://www.w3.org/2001/vxml\">\n", lAppName);
   printf("<form>\n");
   printf("<block>\n");
   printf("<var name=\"Result\" expr=\"%d\"/>\n", lErrorCode);
   if(strlen(ErrMsg) > 0 || ErrMsg != nullptr)
   {
      printf("<var name=\"ErrMsg\" expr=\"'%s'\"/>\n", ErrMsg);
      printf("<return namelist=\"Result ErrMsg\"/>\n");
   }
   else
		printf("<return namelist=\"Result\"/>\n");
	printf("</block>");
	printf("</form>\n");
	printf("</vxml>\n");
   
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%s %s", gCallReference, ErrMsg);
   gLog.GenerateLog(CGI_DBP01, lLogText);
	exit(0);
}

void PrintError(int lErrorCode, TEXT* ErrMsg, TEXT* lAppName, TEXT lVars[][MAX_VARIABLE_LEN], int& varCnt, int& lRowsFetched)
{
   //IvrUtil::ReplaceAll(ErrMsg, "'", "\\'");
   TEXT*   lToken;
   TEXT    lDtype[10] = "";
   TEXT    fieldName[20] = "";
   TEXT    lReturn_vars[MAX_NO_OF_VARIABLES][MAX_VARIABLE_LEN];
   TEXT    lLogText[MAX_LOG_TEXT_LEN+1] = "";

   for(int i=0;  i<varCnt ; i++)
   {
      lToken = strtok(lVars[i],":");
      strcpy(fieldName, lToken);
      strncpy(lDtype, lToken+strlen(lToken)+1, 1);
      strcat(lReturn_vars[varCnt], " ");
      strcat(lReturn_vars[varCnt], fieldName);
      if(lRowsFetched == 1)
         sprintf(lReturn_vars[i], "<var name=\"%s\" expr=\"''\"/>", fieldName);
      else
         sprintf(lReturn_vars[i], "<var name=\"%s\" expr=\"new Array()\"/>", fieldName);
      //printf("%s\n", lReturn_vars[i]);
   }
   memcpy(lVars, lReturn_vars, sizeof(lReturn_vars));

   //printf(" ");
   printf("<?xml version=\"1.0\"?>\n");
   printf("<vxml version=\"2.0\" application=\"%s\" xmlns=\"http://www.w3.org/2001/vxml\">\n", lAppName);
   printf("<form>\n");
   printf("<block>\n");
   printf("<var name=\"Result\" expr=\"%d\"/>\n", lErrorCode);
   printf("<var name=\"RCount\" expr=\"0\"/>\n");
   for(int j=0; j<varCnt; j++)
      printf("%s\n", lReturn_vars[j]);

   printf("<var name=\"ErrMsg\" expr=\"'%s'\"/>\n", ErrMsg);
   printf("<return namelist=\"Result ErrMsg RCount%s\"/>\n",lReturn_vars[varCnt]);
   printf("</block>");
   printf("</form>\n");
   printf("</vxml>\n");

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%s %s", gCallReference, ErrMsg);
   gLog.GenerateLog(CGI_DBP01, lLogText);
   exit(0);
}
//-------------------------------------------------------------
// METHOD         : main
// DESCRIPTION    :
// PARAMETER      :
// RETURN         : BOOLEAN
//-------------------------------------------------------------
int main(int argc,char* argv[])
{
   char            **cgivars ;
	TEXT            lAppName[IVR_MAX_DB_NAME_LEN + 1]     = "";
	TEXT            lDbName[IVR_MAX_DB_NAME_LEN + 1]      = "";
	TEXT            lDbHost[IVR_MAX_DB_NAME_LEN + 1]      = "";
	TEXT            lDbHostName[IVR_MAX_DB_NAME_LEN + 1]  = "";
   TEXT            lDbUser[MAX_DB_USER_NAME + 1]     = "";
	TEXT            lDbPasswd[MAX_DB_PASSWD_LEN + 1]  = "";
	TEXT            lQType[MAX_QTYPE_LEN + 1]         = "";
	TEXT            lDbQuery[MAX_QUERY_LEN + 1]       = "";
	TEXT            lReturnVars[MAX_VARIABLE_LEN + 1] = "";
	TEXT            lRVariables[MAX_NO_OF_VARIABLES][MAX_VARIABLE_LEN];
   int             lRows=0;
   int             lDbClient=0;
	IvrDbManager    lIvrDbManager;
	DbErrorCodes    lDbErr = DB_NO_ERR;

	memset(lRVariables, 0, sizeof(lRVariables));

	strcpy( gProcessName, basename("IEO_CGI"));
   memset (gCallReference, 0, sizeof(gCallReference));
	/** Print the CGI response header, required for all HTML output. **/
	/** Note the extra \n, to send the blank line.                  **/
   printf("Content-type: text/xml\r\n\r\n") ;

	if(gLog.Init()== false)
	{
		//printf("%s\n",HTTP_LOGINIT_FAILED);
		snprintf(gLogText, MAX_LOG_TEXT_LEN ,
				"Failed to Initialize log object. Exiting..\n");
		gLog.GenerateLog(GSYS04, gLogText);
		PrintError(99,(TEXT*) "Failed to Initialize log object. Exiting..", lAppName);
		return 0;
	}
	gLog.SetProcessName(gProcessName);

	cgivars = getcgivars();
#ifdef DB_TEXT_DELAY
   //TBR - Added for Testing
   char* slp;
   int  slptime = 0;
   slp = getenv("DELAY_TIME") ;
   if(slp != NULL)
      slptime = atoi(slp);
#endif

	for(int i=0; cgivars[i]; i+=2)
	{
		if(strcasecmp(cgivars[i], "AppName") == 0)
			strcpy(lAppName, cgivars[i+1]);
		if(strcasecmp(cgivars[i], "DbName") == 0)
			strcpy(lDbName, cgivars[i+1]);
		if(strcasecmp(cgivars[i], "DbHost") == 0)
			strcpy(lDbHost, cgivars[i+1]);
		if(strcasecmp(cgivars[i], "DbUser") == 0)
			strcpy(lDbUser, cgivars[i+1]);
		if(strcasecmp(cgivars[i], "DbPassword") == 0)
			strcpy(lDbPasswd, cgivars[i+1]);
		if(strcasecmp(cgivars[i], "QType") == 0)
			strcpy(lQType, cgivars[i+1]);
		if(strcasecmp(cgivars[i], "Query") == 0)
			strcpy(lDbQuery, cgivars[i+1]);
		if(strcasecmp(cgivars[i], "ReturnVars") == 0)
			strcpy(lReturnVars, cgivars[i+1]);
		if(strcasecmp(cgivars[i], "Rows") == 0)
         lRows = atoi(cgivars[i+1]);
		if(strcasecmp(cgivars[i], "DbClient") == 0)
         lDbClient = atoi(cgivars[i+1]);
		if(strcasecmp(cgivars[i], "CallRef") == 0)
			strcpy(gCallReference, cgivars[i+1]);
	}

	/*if(lAppName == '\0'||strlen(lAppName) == 0)
		PrintError(HTTP_APPLICATION_NAME_MISSING, "Application Name Missing", lAppName);*/
	if(lDbName == nullptr || strlen(lDbName) == 0)
		PrintError(HTTP_DB_NAME_MISSING, (TEXT*)"Db Name Missing", lAppName);
	if(lDbUser == nullptr || strlen(lDbUser) == 0)
		PrintError(HTTP_DB_USER_NAME_MISSING, (TEXT*)"Db User Name Missing", lAppName);
	if(lDbPasswd == nullptr || strlen(lDbPasswd) == 0)
		PrintError(HTTP_DB_PASSWORD_MISSING, (TEXT*)"Db Password Missing", lAppName);
	if(lQType == nullptr || strlen(lQType) == 0)
		PrintError(HTTP_DB_QUERY_TYPE_MISSING, (TEXT*)"Query Type Missing", lAppName);
	if(lDbQuery == nullptr || strlen(lDbQuery) == 0)
		PrintError(HTTP_DB_QUERY_MISSING,(TEXT*) "Query Missing", lAppName);

	EnumQueryType lqtype = IvrUtil::GetQueryType(lQType);
	
	if(lqtype == SELECT && (lReturnVars == nullptr || strlen(lReturnVars) == 0))
		PrintError(HTTP_RETURN_VARIABLES_MISSING,(TEXT*) "Return Variables are Missing", lAppName);

	int   lVarCnt = 0;
	TEXT* lretvar = strtok(lReturnVars, ",");
	while(lretvar != NULL)
	{
		strcpy(lRVariables[lVarCnt], lretvar);
		lVarCnt++;
		lretvar = strtok(NULL, ",");
	}

   if((lDbClient == 1) && (strlen(lDbHost) > 0))
      snprintf(lDbHostName, IVR_MAX_DB_NAME_LEN, "%s@%s", lDbHost, lDbName);
   else
      strcpy(lDbHostName, lDbName);

	lDbErr = lIvrDbManager.Init(lDbHostName, lDbUser, lDbPasswd, lDbClient);
	if(lDbErr != DB_NO_ERR)
	{
		//printf("%s\n",HTTP_DBINIT_FAILED);
		//snprintf(gLogText, MAX_LOG_TEXT_LEN ,
		//		"Failed to Initialize DbManager. Exiting..\n");
		snprintf(gLogText, MAX_LOG_TEXT_LEN ,
				"Failed to Initialize DbManager. Db Connect Failed:(%s,%s,%d)\n", lDbName, lDbUser, lDbClient);
		gLog.GenerateLog(GSYS04, gLogText);
		//PrintError(HTTP_DB_CONNECT_FAILED, gLogText, lAppName);
		PrintError(HTTP_DB_CONNECT_FAILED, gLogText, lAppName, lRVariables, lVarCnt, lRows);
		return 0;
	}

	DbErrorCodes   lDbError;
	TEXT           lTemp[500] = "";
	TEXT           lErrMsg[IVR_MAX_CGI_DB_ERR_MSG_LEN + 1] = "";
	TEXT           lTempErrMsg[IVR_MAX_CGI_DB_ERR_MSG_LEN + 50] = "";  //used to store the ErrMsg after replacing ' in ErrMsg
	//TEXT           lArray[100][1000];
	int            lRowsFetched = lRows;

#ifdef DB_TEXT_DELAY
   //TBR - Added for testing
   sleep(slptime);
#endif
	lDbError = lIvrDbManager.ExecuteQuery1(lqtype, lDbQuery, lRVariables, lVarCnt, lErrMsg, lRowsFetched);
	if(lDbError != DB_NO_ERR)
	{
      IvrUtil::ReplaceAll(lErrMsg,(TEXT*) "\\",(TEXT*) "\\\\", lTempErrMsg);
      IvrUtil::ReplaceAll(lErrMsg,(TEXT*) "'",(TEXT*) "\\'", lTempErrMsg);
      IvrUtil::ReplaceAll(lErrMsg, (TEXT*)"\"",(TEXT*) "\\'", lTempErrMsg);
		PrintError(lDbError, lTempErrMsg, lAppName, lRVariables, lVarCnt, lRowsFetched);
	}

	//printf(" ");
	printf("<?xml version=\"1.0\"?>\n");
	if(lAppName == nullptr ||strlen(lAppName) == 0)
		printf("<vxml version=\"2.0\" xmlns=\"http://www.w3.org/2001/vxml\">\n");
	else
		printf("<vxml version=\"2.0\" application=\"%s\" xmlns=\"http://www.w3.org/2001/vxml\">\n", lAppName);
	printf("<form>\n");
	printf("<block>\n");

	printf("<var name=\"Result\" expr=\"%d\"/>\n", lDbError);
	if(lDbError == DB_NO_ERR)
		printf("<var name=\"RCount\" expr=\"%d\"/>\n", lRowsFetched);

	if(lqtype == SELECT)
	{
		for(int j=0; j<lVarCnt; j++)
			printf("%s\n", lRVariables[j]);

	   sprintf(lTemp, "Result RCount%s", lRVariables[lVarCnt]);
	}
	else
	{
		if(lDbError == DB_NO_ERR)
			sprintf(lTemp, "Result RCount");
		else
			sprintf(lTemp, "Result");
	}

   printf("<var name=\"ErrMsg\" expr=\"''\"/>\n");
	printf("<return namelist=\"%s ErrMsg\"/>\n", lTemp);
	printf("</block>");
	printf("</form>\n");
	printf("</vxml>\n");

   snprintf(gLogText, MAX_LOG_TEXT_LEN ,"RefNum:%s Result: %d, Rows:%d",gCallReference, lDbError, lRowsFetched);
   gLog.GenerateLog(CGI_DBP02, gLogText);
	/** Free anything that needs to be freed **/
	for (int i=0; cgivars[i]; i++) free(cgivars[i]) ;
	free(cgivars) ;


	return 0;
}

