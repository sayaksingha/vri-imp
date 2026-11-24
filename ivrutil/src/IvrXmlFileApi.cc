static const char* id="@(#) $Id: IvrXmlFileApi.cc,v 1.1.1.1.2.3 2016/04/12 21:59:17 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// IvrXmlFileApi.cc
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
//----------------------------------------------------------------------
#include "IvrXmlFileApi.h"

//----------------------------------------------------------------------
// METHOD NAME : IvrXmlFileApi
// DESCRIPTION : constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IvrXmlFileApi::IvrXmlFileApi ()
{

}

//----------------------------------------------------------------------
// METHOD NAME : ~IvrXmlFileApi
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IvrXmlFileApi::~IvrXmlFileApi ()
{
	if (mpHashTablePtr)
		delete[] mpHashTablePtr;

	mpHashTablePtr = NULL;
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializes all the private variables and
//               objects used by the IvrXmlFileApi process
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrXmlFileApi::Init ()
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	if (false == this->InitHashTable ())
	{
		sprintf (lLogText, "Failed to Initailize the ProvHashTable");
		TERR (gTrace, printf ("%s: %s. Exiting...\n", gProcessName, lLogText););
		//gLog.GenerateLog (GSYS04, lLogText);
		return false;
	}

	return true;
}

//----------------------------------------------------------------------
// METHOD NAME : InitHashTable ()
// DESCRIPTION : Initializes the Parameter Hash Table 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrXmlFileApi::InitHashTable ()
{
	UINT32 lHashTableSize = 0;

	mXmlFileDir.SetBucketCount (IVR_MAX_XML_FILE_ENTRY + 1);

	lHashTableSize = mXmlFileDir.GetHashTableSize ();

	mpHashTablePtr = new TEXT[lHashTableSize];

	if (false == this->ReadConfig ())
	{
		//gLog.GenerateLog (API_IVR40);
		TERR (gTrace, printf ("%s: Failed to Read from Config And insert into hash table\n",
					gProcessName););
		return false;
	}

	return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ClearXmlFileHashTable 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrXmlFileApi::ClearXmlFileHashTable ()
{
	if (mXmlFileDir.SetHashMemoryPtr ((void *) mpHashTablePtr,
				true) != SHT_SUCCESS)
	{
		TERR (gTrace, printf ("%s: Failed to set hash table memory pointer\n",
					gProcessName););
		//gLog.GenerateLog (API_IVR41);
		return false;
	}

	return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrXmlFileApi::ReadConfig ()
{
	CfgRead      		lCfgRead(gProcessName);
	INT32 		 		mNumOfAccounts;
	TEXT					lSearchKey[20] = "";
	TEXT 					lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	StrTokenizer 		lStrTok;
	TEXT					lAccountXmlFile[100] = "";
	IvrDnisXmlDetails	lIvrDnisXmlDetails;

	memset(&lIvrDnisXmlDetails, 0, sizeof(IvrDnisXmlDetails));

	if (lCfgRead.CfgInit(IVR_DNIS_CFG) == false)
	{
		TERR(gTrace,
				printf("%s: Failed to open the file %s\n",
					gProcessName, IVR_DNIS_CFG););
		return false;
	}

	if (lCfgRead.GetConfigNum("NO_OF_ACCOUNTS", mNumOfAccounts,
				IVR_MIN_XML_FILE_ENTRY, IVR_MAX_XML_FILE_ENTRY) != CFG_OK)
	{
	   sprintf(lLogText,"Config read failed for NO_OF_ACCOUNTS");
		TERR(gTrace , printf("%s: %s\n" , gProcessName, lLogText););
		lCfgRead.CfgDeInit();
		return false;
	}
	else
	{
		T(gTrace, printf("%s: NO_OF_ACCOUNTS = %d\n",
					gProcessName, mNumOfAccounts););
	}

	if (false == this->ClearXmlFileHashTable ())
		return false;

	for(int lcount = 1 ; lcount <=  mNumOfAccounts ; lcount++)
	{
		sprintf(lSearchKey, "%s%d", "Account_", lcount);
		if (lCfgRead.GetConfigStr(lSearchKey, lAccountXmlFile,
					10,  50) !=  CFG_OK)
		{
			sprintf(lLogText,"Config read failed for %s ", lSearchKey);
			TERR(gTrace , printf("%s: %s\n" , gProcessName, lLogText););
			lCfgRead.CfgDeInit();
			return false;
		}
		else
		{
			T(gTrace, printf("%s: %s = %s\n",
						gProcessName, lSearchKey, lAccountXmlFile););
		}

		TEXT	lTempText[100] = "";
		char* lTokenizer;

		strcpy(lTempText, lAccountXmlFile);
		lTokenizer = strtok(lTempText, ":");
		strcpy(lIvrDnisXmlDetails.dnis, lTokenizer);

		{
			lTokenizer = strtok(NULL, ":");
			strcpy(lIvrDnisXmlDetails.xmlFile, lTokenizer);
		}
		
		snprintf (lLogText, MAX_LOG_TEXT_LEN, "Dnis:%s XmlFile:%s",
				lIvrDnisXmlDetails.dnis, lIvrDnisXmlDetails.xmlFile);

		if (SHT_SUCCESS !=
				mXmlFileDir.Insert (lIvrDnisXmlDetails.dnis,
					lIvrDnisXmlDetails))
		{
			//gLog.GenerateLog (API_CSSN42, lLogText);
			TERR (gTrace, printf ("%s: \33[31m %s XmlFileKey Table insertion Failed\33[0m\n",
						gProcessName, lLogText););
			return false;
		}

		//gLog.GenerateLog (API_CSSN43, 2, lLogText);
		//T (gTrace, printf ("%s: %s Insert into XmlFileKey Table success\n",
		//			gProcessName, lLogText););
	}
	return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrXmlFileApi::ReloadConfig ()
{
	return this->ReadConfig ();
}

//----------------------------------------------------------------------
// METHOD NAME : GetXmlFileDetails 
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IvrXmlFileApi::GetXmlFileDetails (TEXT * destNum,
		IvrDnisXmlDetails & lIvrDnisXmlDetails)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	TEXT lTempString[IVR_MAX_XML_FILE_LEN + 1] = "";

	strcpy (lTempString, destNum);

	switch (mXmlFileDir.Get (lTempString, lIvrDnisXmlDetails))
		//switch (0)
	{
		case SHT_SUCCESS:
			break;

		case SHT_NO_DATA_FOUND:
		default:
			snprintf (lLogText, MAX_LOG_TEXT_LEN, "Key:%s", lTempString);
			//gLog.GenerateLog (API_IVR44, lLogText);
			T (gTrace, printf ("%s: %s Failed to get Key details\n",
						gProcessName, lLogText););
			return false;
	}

	snprintf (lLogText, MAX_LOG_TEXT_LEN, "Dnis:%s XmlFile:%s\n",
			lIvrDnisXmlDetails.dnis, lIvrDnisXmlDetails.xmlFile);
//	strcpy (xmlFile, lIvrDnisXmlDetails.xmlFile);
	T (gTrace,
			printf ("%s: Key:%s Key details found\n",
				gProcessName, destNum););

	return true;
}
