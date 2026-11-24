#include <time.h>
#include <sys/time.h>
#include <sys/poll.h>

#include "acu_type.h"
#include "res_lib.h"
#include "cl_lib.h"
#include "sw_lib.h"
#include "iptel_lib.h"

#include "bfopen.h"
#include "smfaxapi.h"
#include "prosrtpapi.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smrtp.h"
#include "smbesp.h"        // Basic Speech Processing functionallity
#include "smhlib.h"        // High-level Speech functionallity
#include "smwavlib.h"      // Wav file replay/record funtions
#include "MediaInterface.h"
#include "AcuMediaConstDef.h"
#include "AcuMediaLimits.h"
#include "AcuMediaUtil.h"
//#include "Fax.h"

int main(int argc, char * argv[])
{
   TEXT  lFileName[200] = "";
   TEXT  lNewFile[200] = "";
   INT16 lResult = 0;
   int    * lFileErr;
   ACTIFF_FILE * lFile;
   ACTIFF_INSERT_PAGES_PARMS insert_parms; 

   if(argc != 2)
   {
      printf("Expected Format: <exe> <Tiff FileName>\n");
      return false;
   }

   sprintf(lFileName, "/opt/ivr/bin/stubs/%s", argv[1]);

   lFile = actiff_create();
   if (lFile == NULL)
   {
      printf("Failed to Create ACTIFF file\n");
      return 0;
   }

   printf("Converting Tiff File:%s to ACTIFF file\n", lFileName);
   
   memset(&insert_parms, 0, sizeof(insert_parms));
   
   insert_parms.actiff = lFile;
   insert_parms.filename = &lFileName[0];
   insert_parms.source_first_page = 0;
   insert_parms.source_num_pages = kACTIFFPagesAll;
   insert_parms.before_target_page = kACTIFFPageAppend;

   lResult = actiff_insert_tiff_pages(&insert_parms);

   if (lResult != 0)
   {
      printf("Failed to Insert Tiff Pages Err:%d\n", lResult);
      return 0;
   }
   
   sprintf(lNewFile, "/opt/ivr/bin/stubs/New_%s", argv[1]);
   lResult = actiff_save(lFile, lNewFile, lFileErr); 
   if (lResult != 0)
   {
      printf("Failed to Save ACTIFF file Err:%d %s\n", lResult, *lNewFile);
      return 0;
   }

   return 1;
}

