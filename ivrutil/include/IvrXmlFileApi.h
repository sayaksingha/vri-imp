// @(#) $Id: IvrXmlFileApi.h,v 1.1 2011/03/08 08:24:32 root Exp $
//----------------------------------------------------------------------
// NAME
// IvrXmlFileApi.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2006
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
//----------------------------------------------------------------------
#ifndef INCL_IVR_XML_DIR_H
#define INCL_IVR_XMLDIR_H

#include "IvrConstDef.h"
#include "IvrStructs.h"
#include "IvrUtil.h"

//TEXT key[IVR_MAX_XML_FILE_LEN + 1];

struct IvrDnisXmlDetails
{
	TEXT dnis[MAX_MSISDN_LEN + 1];
	TEXT xmlFile[IVR_MAX_XML_FILE_LEN + 1];
};

typedef HashTable < TEXT, IVR_MAX_XML_FILE_LEN + 1, IvrDnisXmlDetails > ParamHashTable;

class IvrXmlFileApi
{
   public:
      IvrXmlFileApi ();

      ~IvrXmlFileApi ();

      BOOLEAN Init ();

      BOOLEAN ReloadConfig ();

      BOOLEAN InitHashTable ();

      BOOLEAN GetXmlFileDetails (TEXT *, IvrDnisXmlDetails & );

   protected:
      BOOLEAN ClearXmlFileHashTable ();

      BOOLEAN ReadConfig ();

      INT32 mProvShmId;
      UINT32 mProvShmKey;

      ParamHashTable mXmlFileDir;

      TEXT *mpHashTablePtr;
      
      INT32 mNumOfAccounts;
		TEXT 	mAccounts[IVR_MAX_XML_FILE_ENTRY][100];
		TEXT 	mXmlFiles[IVR_MAX_XML_FILE_ENTRY][100];
};

#endif
