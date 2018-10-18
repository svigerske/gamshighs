/* HiGHS link code */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gamshighs.h"
#include "gmomcc.h"
#include "gevmcc.h"
#include "optcc.h"

struct gamshighs_s
{
   gmoHandle_t gmo;
   gevHandle_t gev;
   optHandle_t opt;
   int         debug;
};

void his_Create(
   gamshighs_t** highs,
   char*         msgBuf,
   int           msgBufLen
)
{
   assert(highs != NULL);

   *highs = (gamshighs_t*) malloc(sizeof(gamshighs_t));
}

void his_Free(
   gamshighs_t** highs
)
{
   free(*highs);
   *highs = NULL;
}

int his_CallSolver(
   gamshighs_t* highs,
   void*        gmo
)
{
   int rc = 1;
   char buffer[1024];
   double reslim;

   if( !gmoGetReady(buffer, sizeof(buffer)) )
   {
      fputs(buffer, stderr);
      return 1;
   }

   if( !gevGetReady(buffer, sizeof(buffer)) )
   {
      fputs(buffer, stderr);
      return 1;
   }

   memset(highs, 0, sizeof(gamshighs_t));
   highs->gmo = gmo;
   highs->gev = gmoEnvironment(gmo);

   gevLogStat(highs->gev, "This is the GAMS link to HiGHS.");

   gmoModelStatSet(highs->gmo, gmoModelStat_NoSolutionReturned);
   gmoSolveStatSet(highs->gmo, gmoSolveStat_SystemErr);

   /*
   if( dooptions(highs) )
      goto TERMINATE;
      */

   /* get the problem into a normal form */
   gmoObjStyleSet(highs->gmo, gmoObjType_Fun);
   gmoObjReformSet(highs->gmo, 1);
   gmoIndexBaseSet(highs->gmo, 0);
   gmoSetNRowPerm(highs->gmo); /* hide =N= rows */

   reslim = gevGetDblOpt(highs->gev, gevResLim);
   gevTimeSetStart(highs->gev);

   /* TODO solve the problem here */

   gmoSetHeadnTail(highs->gmo, gmoHresused, gevTimeDiffStart(highs->gev));

   /* TODO pass solution, status, etc back to GMO here */

   rc = 0;
TERMINATE:

   if( highs->opt != NULL )
      optFree(&highs->opt);

   return rc;
}


void his_Initialize(void)
{
   gmoInitMutexes();
   gevInitMutexes();
   optInitMutexes();
}

void his_Finalize(void)
{
   gmoFiniMutexes();
   gevFiniMutexes();
   optFiniMutexes();
}
