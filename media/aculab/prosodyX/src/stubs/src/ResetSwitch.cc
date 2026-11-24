#include <iostream.h>

int main(int argc, char * argv[])
{
   char lCmd[500] = "";
   int  lSerialNum = 0;

   if (argc != 2)
      printf("Format: <Exe> <Serial Number>\n");

   lSerialNum = atoi(argv[1]);

   for (int nst = 32; nst <= 39; nst++)
   {
      for(int nts = 0; nts < 32; nts++)
      {
         sprintf(lCmd, "/opt/aculab/TiNG/diag/switch/gen-LINUX_V6/switch -x %d set %d:%d 0x54", 
         lSerialNum, nst, nts);

         int lRet = system(lCmd);

         if(!lRet)
         printf("Success to Reset NW [%d:%d]\n", nst, nts);
      }
   }

   for (int mst = 64; mst <= 71; mst++)
   {
      for(int mts = 0; mts < 32; mts++)
      {
         sprintf(lCmd, "/opt/aculab/TiNG/diag/switch/gen-LINUX_V6/switch -x %d set %d:%d 0x54", 
         lSerialNum, mst, mts);

         int lRet = system(lCmd);

         if(!lRet)
         printf("Success to Reset MEDIA [%d:%d]\n", mst, mts);
      }
   }

   printf("Success to Reset the Network and Media Stream\n");

   return 0;
}

