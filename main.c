#include <stdlib.h>
#include <stdio.h>

#include "gmomcc.h"
#include "gevmcc.h"

#include "gamshighs.h"

int main(
   int    argc,
   char** argv
)
{
   char buffer[GMS_SSSIZE];
   gmoHandle_t gmo;
   gevHandle_t gev;
   gamshighs_t* highs = NULL;
   int rc = EXIT_FAILURE;

   if( argc < 2 )
   {
      printf("usage: %s <cntrlfile> [<optfile>]\n", argv[0]);
      return 1;
   }

   /* create GMO and GEV handles */
   if( !gmoCreate(&gmo, buffer, sizeof(buffer)) || !gevCreate(&gev, buffer, sizeof(buffer)) )
   {
      fprintf(stderr, "%s\n", buffer);
      goto TERMINATE;
   }

   /* load GAMS control file */
   if( gevInitEnvironmentLegacy(gev, argv[1]) )
   {
      fprintf(stderr, "Could not load control file %s\n", argv[1]);
      goto TERMINATE;
   }

   /* let gmo know about gev */
   if( gmoRegisterEnvironment(gmo, gev, buffer) )
   {
      fprintf(stderr, "Error registering GAMS Environment: %s\n", buffer);
      goto TERMINATE;
   }

   /* load instance data */
   if( gmoLoadDataLegacy(gmo, buffer) )
   {
      fprintf(stderr, "Could not load model data.\n");
      goto TERMINATE;
   }

   his_Initialize();

   his_Create(&highs, buffer, sizeof(buffer));
   if( highs == NULL )
   {
      fprintf(stderr, buffer);
      goto TERMINATE;
   }

   /* overwrite option file setting in GMO */
   if( argc >= 3 )
   {
      gmoOptFileSet(gmo, 1);
      gmoNameOptFileSet(gmo, argv[2]);
   }

   if( his_CallSolver(highs, gmo) != 0 )
      goto TERMINATE;

   rc = EXIT_SUCCESS;

TERMINATE:
   if( highs != NULL )
      his_Free(&highs);

   his_Finalize();

   if( gmo != NULL )
   {
      gmoUnloadSolutionLegacy(gmo);
      gmoFree(&gmo);
   }
   if( gev != NULL )
      gevFree(&gev);

   gmoLibraryUnload();
   gevLibraryUnload();

   return rc;
}
