/********************************************************************

 This benchmark test program is measuring a cpu performance
 of floating point operation by a Poisson equation solver.

 If you have any question, please ask me via email.
 written by Ryutaro HIMENO, November 26, 2001.
 Version 3.0
 ----------------------------------------------
 Ryutaro Himeno, Dr. of Eng.
 Head of Computer Information Division,
 RIKEN (The Institute of Pysical and Chemical Research)
 Email : himeno@postman.riken.go.jp
 ---------------------------------------------------------------
 This program is to measure a computer performance in MFLOPS
 by using a kernel which appears in a linear solver of pressure
 Poisson eq. which appears in an incompressible Navier-Stokes solver.
 A point-Jacobi method is employed in this solver as this method can 
 be easyly vectrized and be parallelized.
 ------------------
 Finite-difference method, curvilinear coodinate system
 Vectorizable and parallelizable on each grid point
 No. of grid points : imax x jmax x kmax including boundaries
 ------------------
 A,B,C:coefficient matrix, wrk1: source term of Poisson equation
 wrk2 : working area, OMEGA : relaxation parameter
 BND:control variable for boundaries and objects ( = 0 or 1)
 P: pressure
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define MR(mt,n,r,c,d)  mt->m[(n) * mt->mrows * mt->mcols * mt->mdeps + (r) * mt->mcols* mt->mdeps + (c) * mt->mdeps + (d)]

struct Mat {
  float* m;
  int mnums;
  int mrows;
  int mcols;
  int mdeps;
};

/* prototypes */
typedef struct Mat Matrix;

int newMat(Matrix* Mat, int mnums, int mrows, int mcols, int mdeps);
void clearMat(Matrix* Mat);
void mat_set(Matrix* Mat,int l,float z);
void mat_set_init(Matrix* Mat);
float jacobi(int n);
void* tempthread();
void* partialJacob(void* x);
void partJcob(void *x);


float   omega=0.8;
Matrix  a,b,c,p,bnd,wrk1,wrk2;
  float  gosa,s0,ss;
  int noOfThreads = 0;
int step = 0;
int    imax,jmax,kmax;
  // pthread_t threads[noOfThreads];
  float gosaFinal = 0.0;
int
main(int argc, char *argv[])
{
  int    nn;
  int    mimax,mjmax,mkmax,msize[3];
  

  scanf("%d", &msize[0]);
  scanf("%d", &msize[1]);
  scanf("%d", &msize[2]);
  scanf("%d", &nn);
 // msize[0] = 64;
 // msize[1] = 64;
 // msize[2] = 128;
 // nn = 10;
  mimax= msize[0];
  mjmax= msize[1];
  mkmax= msize[2];
  imax= mimax-1;
  jmax= mjmax-1;
  kmax= mkmax-1;

     noOfThreads = get_nprocs();
    // nprocs() might return wrong amount inside of a container.
    // Use MAX_CPUS instead, if available.
    if (getenv("MAX_CPUS")) {
        noOfThreads = atoi(getenv("MAX_CPUS"));
    }
  /*
   *    Initializing matrixes
   */
  newMat(&p,1,mimax,mjmax,mkmax);
  newMat(&bnd,1,mimax,mjmax,mkmax);
  newMat(&wrk1,1,mimax,mjmax,mkmax);
  newMat(&wrk2,1,mimax,mjmax,mkmax);
  newMat(&a,4,mimax,mjmax,mkmax);
  newMat(&b,3,mimax,mjmax,mkmax);
  newMat(&c,3,mimax,mjmax,mkmax);

  mat_set_init(&p);
  mat_set(&bnd,0,1.0);
  mat_set(&wrk1,0,0.0);
  mat_set(&wrk2,0,0.0);
  mat_set(&a,0,1.0);
  mat_set(&a,1,1.0);
  mat_set(&a,2,1.0);
  mat_set(&a,3,1.0/6.0);
  mat_set(&b,0,0.0);
  mat_set(&b,1,0.0);
  mat_set(&b,2,0.0);
  mat_set(&c,0,1.0);
  mat_set(&c,1,1.0);
  mat_set(&c,2,1.0);

  /*
   *    Start measuring
   */
  jacobi(nn);

  printf("%.6f\n",gosaFinal);

  /*
   *   Matrix free
   */ 
  clearMat(&p);
  clearMat(&bnd);
  clearMat(&wrk1);
  clearMat(&wrk2);
  clearMat(&a);
  clearMat(&b);
  clearMat(&c);
  
  return (0);
}


struct JacobArgs {
  Matrix M1;
  Matrix M2;
  Matrix M3;
  Matrix M4;
  Matrix M5;
  Matrix M6;
  Matrix M7;

};


typedef struct JacobArgs struct1;

float
jacobi(int nn)
{
 // printf("came her boi");
 int n,i,j,k;
 
 struct1 *x;
 x=(struct1 *)malloc(sizeof(struct1));
 x->M1 = a;
 x->M2 = b;
 x->M3 = c;
 x->M4 = p;
 x->M5 = bnd;
 x->M6 = wrk1;
 x->M7 = wrk2;
 
 // printf("came her boi2");
 //imax= p->mrows-1;
  //jmax= p->mcols-1;
  //kmax= p->mdeps-1;
      	
  //for(n=0 ; n<nn ; n++){
    gosa = 0.0;
 pthread_t threads[noOfThreads];
step = 0;
// printf("i have %d threads", noOfThreads);
  for (int th = 0; th < noOfThreads ; th++ ) {
      pthread_create(&threads[th], NULL, partialJacob, (void*)(x));
  }
for (int tj = 0; tj < noOfThreads; tj++)
     {	pthread_join(threads[tj], NULL); }

 
      partJcob(x);
    
   //} /* end n loop */
  //printf("optimized gosa: %f", gosa);
  return(gosa);
}

void* tempthread() {
  // printf("\n\n\ni am threaded");
}
void partJcob(void *x) {
  
Matrix* p;

Matrix* wrk2;

struct1 *y;
y=(struct1*) x;
  p = &(y->M4);
wrk2 = &(y->M7);

int i,j,k;
     for(i=1 ; i<imax ; i++)
      for(j=1 ; j<jmax ; j++)
        for(k=1 ; k<kmax ; k++)
          MR(p,0,i,j,k)= MR(wrk2,0,i,j,k);


}
void* partialJacob( void* x ) {

Matrix* a; 
Matrix* b;
Matrix* c;
Matrix* p;
Matrix* bnd;
Matrix* wrk1;
Matrix* wrk2;

struct1 *y;
y=(struct1*) x;
    a = &(y->M1);
    b = &(y->M2);
    c = &(y->M3);
    p = &(y->M4);
    bnd = &(y->M5);
    wrk1 = &(y->M6);
    wrk2 = &(y->M7);
         
  int i,j,k;
 // printf("came here");

  int core = step++;
  int ii = 0;

   for(ii= (core * imax) / noOfThreads ; ii <= (core + 1) * imax / noOfThreads; ii++)
  // for (i=1;i<imax;i++) 
      {
        i = (ii + 1);
    //   printf("\ni am a threaded bouy with i: %d, my core is: %d my imax is %d my step is %d\n\n", i,core,imax,step);
        if ( i > (imax - 1)) {
          break;
        }
        for(j=1 ; j<jmax ; j++) {

        for(k=1 ; k<kmax ; k++){
    //  printf("that was some fast ass shit");
          s0= MR(a,0,i,j,k)*MR(p,0,i+1,j,  k)
            + MR(a,1,i,j,k)*MR(p,0,i,  j+1,k)
            + MR(a,2,i,j,k)*MR(p,0,i,  j,  k+1)
            + MR(b,0,i,j,k)
             *( MR(p,0,i+1,j+1,k) - MR(p,0,i+1,j-1,k)
              - MR(p,0,i-1,j+1,k) + MR(p,0,i-1,j-1,k) )
            + MR(b,1,i,j,k)
             *( MR(p,0,i,j+1,k+1) - MR(p,0,i,j-1,k+1)
              - MR(p,0,i,j+1,k-1) + MR(p,0,i,j-1,k-1) )
            + MR(b,2,i,j,k)
             *( MR(p,0,i+1,j,k+1) - MR(p,0,i-1,j,k+1)
              - MR(p,0,i+1,j,k-1) + MR(p,0,i-1,j,k-1) )
            + MR(c,0,i,j,k) * MR(p,0,i-1,j,  k)
            + MR(c,1,i,j,k) * MR(p,0,i,  j-1,k)
            + MR(c,2,i,j,k) * MR(p,0,i,  j,  k-1)
            + MR(wrk1,0,i,j,k);

          ss= (s0*MR(a,3,i,j,k) - MR(p,0,i,j,k))*MR(bnd,0,i,j,k);

          gosa+= ss*ss;
         // printf("\n%f", gosa);
          MR(wrk2,0,i,j,k)= MR(p,0,i,j,k) + omega*ss;

          // checktimes++;
     //     printf("\nnexecuted %f times\n", gosa);
        }
      }
      gosaFinal = gosa + gosaFinal;
      }



}


int
newMat(Matrix* Mat, int mnums,int mrows, int mcols, int mdeps)
{
  Mat->mnums= mnums;
  Mat->mrows= mrows;
  Mat->mcols= mcols;
  Mat->mdeps= mdeps;
  Mat->m= NULL;
  Mat->m= (float*) 
    malloc(mnums * mrows * mcols * mdeps * sizeof(float));
  
  return(Mat->m != NULL) ? 1:0;
}

void
clearMat(Matrix* Mat)
{
  if(Mat->m)
    free(Mat->m);
  Mat->m= NULL;
  Mat->mnums= 0;
  Mat->mcols= 0;
  Mat->mrows= 0;
  Mat->mdeps= 0;
}

void
mat_set(Matrix* Mat, int l, float val)
{
  int i,j,k;

    for(i=0; i<Mat->mrows; i++)
      for(j=0; j<Mat->mcols; j++)
        for(k=0; k<Mat->mdeps; k++)
          MR(Mat,l,i,j,k)= val;
}

void
mat_set_init(Matrix* Mat)
{
  int  i,j,k;

  for(i=0; i<Mat->mrows; i++)
    for(j=0; j<Mat->mcols; j++)
      for(k=0; k<Mat->mdeps; k++)
        MR(Mat,0,i,j,k)= (float)(i*i)
          /(float)((Mat->mrows - 1)*(Mat->mrows - 1));
}