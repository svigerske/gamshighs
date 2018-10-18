#ifndef GAMSHIGHS_H_
#define GAMSHIGHS_H_

typedef struct gamshighs_s gamshighs_t;
typedef gamshighs_t his_Rec_t;  /* for GAMS buildsystem */

#ifdef __cplusplus
extern "C" {
#endif

void his_Free(
   gamshighs_t** highs
);

void his_Create(
   gamshighs_t** highs,
   char*         msgBuf,
   int           msgBufLen
);

int  his_CallSolver(
   gamshighs_t* highs,
   void*        gmo
);

void his_Initialize(void);
void his_Finalize(void);

#ifdef __cplusplus
}
#endif

#endif /* GAMSHIGHS_H_ */
