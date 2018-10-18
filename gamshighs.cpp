/* HiGHS link code */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gamshighs.h"

/* GAMS API */
#include "gmomcc.h"
#include "gevmcc.h"
#include "optcc.h"

/* HiGHS API */
#include "HModel.h"

struct gamshighs_s
{
   gmoHandle_t gmo;
   gevHandle_t gev;
   optHandle_t opt;
   int         debug;

   HModel*     model;
};

static int setupProblem(
   gamshighs_t* highs
)
{
   int objSense;

   int numCol;
   double* colCost = NULL;
   double* colLower = NULL;
   double* colUpper = NULL;

   int numRow;
   double* rowLower = NULL;
   double* rowUpper = NULL;

   int numNz;
   int* Astart = NULL;
   int* Aindex = NULL;
   double* Avalue = NULL;

   int i;
   int rc = 1;

   assert(highs != NULL);

   numCol = gmoN(highs->gmo);
   numRow = gmoM(highs->gmo);
   numNz = gmoNZ(highs->gmo);

   colCost = new double[numCol];
   colLower = new double[numCol];
   colUpper = new double[numCol];

   rowLower = new double[numRow];
   rowUpper = new double[numRow];

   Astart = new int[numCol+1];  /* FIXME numCol or numRow?, +1 */
   Aindex = new int[numNz];
   Avalue = new double[numNz];

   if( gmoSense(highs->gmo) == gmoObj_Min )
      objSense = 0; /* FIXME */
   else
   {
      assert(gmoSense(highs->gmo) == gmoObj_Max);
      objSense = 1;  /* FIXME */
   }

   gmoGetObjVector(highs->gmo, colCost, NULL);
   gmoGetVarLower(highs->gmo, colLower);
   gmoGetVarUpper(highs->gmo, colUpper);

   for( i = 0; i < numRow; ++i )
   {
      switch( gmoGetEquTypeOne(highs->gmo, i) )
      {
         case gmoequ_E :
            rowLower[i] = rowUpper[i] = gmoGetRhsOne(highs->gmo, i);
            break;

         case gmoequ_G :
            rowLower[i] = gmoGetRhsOne(highs->gmo, i);
            rowUpper[i] = gmoPinf(highs->gmo);
            break;

         case gmoequ_L :
            rowLower[i] = gmoMinf(highs->gmo);
            rowUpper[i] = gmoGetRhsOne(highs->gmo, i);
            break;

         case gmoequ_N:
         case gmoequ_X:
         case gmoequ_C:
         case gmoequ_B:
            /* these should not occur */
            goto TERMINATE;
      }
   }

   /* FIXME gmoGetMatrixCol or gmoGetMatrixRow ? */
   gmoGetMatrixCol(highs->gmo, Astart, Aindex, Avalue, NULL);

   highs->model = new HModel();
   highs->model->load_fromArrays(numCol, objSense,
      colCost, colLower, colUpper,
      numRow, rowLower, rowUpper,
      numNz, Astart, Aindex, Avalue);

   highs->model->util_reportModel();

   rc = 0;
TERMINATE:

   delete[] Avalue;
   delete[] Aindex;
   delete[] Astart;
   delete[] rowUpper;
   delete[] rowLower;
   delete[] colUpper;
   delete[] colLower;
   delete[] colCost;

   return rc;
}

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
   highs->gmo = (gmoHandle_t)gmo;
   highs->gev = (gevHandle_t)gmoEnvironment(highs->gmo);

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
   gmoMinfSet(highs->gmo, -1E20);  /* FIXME: set to Highs value for -infinity */
   gmoPinfSet(highs->gmo,  1E20);  /* FIXME: set to Highs value for -infinity */

   if( !setupProblem(highs) )
	   goto TERMINATE;

   /* set timelimit */
   highs->model->dblOption[DBLOPT_TIME_LIMIT] = gevGetDblOpt(highs->gev, gevResLim);

   gevTimeSetStart(highs->gev);

   /* TODO solve the problem here */

   gmoSetHeadnTail(highs->gmo, gmoHresused, gevTimeDiffStart(highs->gev));

   /* TODO pass solution, status, etc back to GMO here */

   rc = 0;
TERMINATE:

   if( highs->opt != NULL )
      optFree(&highs->opt);

   delete highs->model;
   highs->model = NULL;

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
