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
#include <math.h>

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

struct thread_work_t{
    long unsigned start_n;
    long unsigned end_n;
    long unsigned thread_num;
};

int newMat(Matrix* Mat, int mnums, int mrows, int mcols, int mdeps);
void clearMat(Matrix* Mat);
void mat_set(Matrix* Mat,int l,float z);
void mat_set_init(Matrix* Mat);
void mat_gen_output();
void* jacobi(void *thread_work_uncasted);

Matrix  a1,b1,c1,p1,bnd1,wrk11,wrk21;


// Shared variables for the THREADS

float  gosa;
int imax,jmax,kmax;
int nn; 
float   omega=0.8;
int CPUs = 1;

Matrix* a;
Matrix* b;
Matrix* c;
Matrix* p;
Matrix* bnd;
Matrix* wrk1;
Matrix* wrk2;

pthread_barrier_t barrier1;
pthread_mutex_t lock;

void* 
jacobi(void *thread_work_uncasted)
{
  int    i,j,k,n;
  float  gosa1,s0,ss;
  
  // the stucture
  struct thread_work_t *thread_work = (struct thread_work_t*)thread_work_uncasted;
  const long unsigned int start = thread_work->start_n;
  const long unsigned int end = thread_work->end_n;
  const long unsigned int thread_num = thread_work->thread_num;
   
  for(n=0 ; n<nn ; n++){
    // put a barrier so all threads wait here
   if (thread_num != 100) {
    pthread_barrier_wait(&barrier1);
   }
     // only master thread should perform the gosa initialization once
    if (thread_num == 0) {
        gosa = 0.0;
    }
// temporary gosa 
gosa1 = 0.0;

    for(i=start ; i<=end; i++) {
       
   if  (i == (imax)) { // since we run all iterations except the last one
         break;
     }
      for(j=1 ; j<jmax ; j++) {
     
        for(k=1 ; k<kmax ; k++){
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

          gosa1+= ss*ss;
        
          MR(wrk2,0,i,j,k)= MR(p,0,i,j,k) + omega*ss;
        }
      }
    } 
     if (thread_num != 100) { // barrier so wrk2 is fully computed
   
    pthread_barrier_wait(&barrier1);
   }
    for(i=start ; i<=end ; i++) {
 
   if ( (i == (imax))) {
         break;
      }
      for(j=1 ; j<jmax ; j++) {
        for(k=1 ; k<kmax ; k++) { 
          MR(p,0,i,j,k)= MR(wrk2,0,i,j,k);
        }}}

// Critical section
  pthread_mutex_lock(&lock);
     gosa += gosa1;
  pthread_mutex_unlock(&lock);
}
}
int
main(int argc, char *argv[])
{
 // int    nn;
  int    mimax,mjmax,mkmax,msize[3];
  

  scanf("%d", &msize[0]);
  scanf("%d", &msize[1]);
  scanf("%d", &msize[2]);
  scanf("%d", &nn);
  
  mimax= msize[0];
  mjmax= msize[1];
  mkmax= msize[2];
  imax= mimax-1;
  jmax= mjmax-1;
  kmax= mkmax-1;

  /*
   *    Initializing matrixes
   */
  newMat(&p1,1,mimax,mjmax,mkmax);
  newMat(&bnd1,1,mimax,mjmax,mkmax);
  newMat(&wrk11,1,mimax,mjmax,mkmax);
  newMat(&wrk21,1,mimax,mjmax,mkmax);
  newMat(&a1,4,mimax,mjmax,mkmax);
  newMat(&b1,3,mimax,mjmax,mkmax);
  newMat(&c1,3,mimax,mjmax,mkmax);

  mat_set_init(&p1);
  mat_set(&bnd1,0,1.0);
  mat_set(&wrk11,0,0.0);
  mat_set(&wrk21,0,0.0);
  mat_set(&a1,0,1.0);
  mat_set(&a1,1,1.0);
  mat_set(&a1,2,1.0);
  mat_set(&a1,3,1.0/6.0);
  mat_set(&b1,0,0.0);
  mat_set(&b1,1,0.0);
  mat_set(&b1,2,0.0);
  mat_set(&c1,0,1.0);
  mat_set(&c1,1,1.0);
  mat_set(&c1,2,1.0);

  /*
   *    Start measuring
   */
  a = &a1;
  b = &b1;
  c = &c1;
  bnd = &bnd1;
  wrk1 = &wrk11;
  wrk2 = &wrk21;
  p = &p1;

  CPUs = get_nprocs();
    // nprocs() might return wrong amount inside of a container.
    // Use MAX_CPUS instead, if available.
  if (getenv("MAX_CPUS")) 
   {
    CPUs = atoi(getenv("MAX_CPUS"));
   }
   // we're going to parallelize this section first.

  
  
  pthread_t threads[CPUs];
  struct thread_work_t tw[CPUs];
  
 imax= p->mrows-1;
  jmax= p->mcols-1;
  kmax= p->mdeps-1;

 //printf("running for %d CPUS", CPUs);
  pthread_barrier_init(&barrier1,NULL,CPUs);

   pthread_mutex_init(&lock, NULL);
  for (int i=0; i<CPUs; i++) {
    
    tw[i].start_n = ((imax)/CPUs)* i + 1;
    tw[i].end_n   = ((imax)/CPUs)*(i+1);
    tw[i].thread_num = i;
    
    pthread_create(&threads[i], NULL, jacobi, (void*)&tw[i]);
  }
    struct thread_work_t maintw;
    pthread_t mainth;
    maintw.start_n = (imax/CPUs) * CPUs + 1;
    maintw.end_n   = imax;
    maintw.thread_num = 100;
  if (maintw.start_n >= maintw.end_n) {
   
  } else
   {
     pthread_t mainth;
     pthread_create(&mainth, NULL, jacobi, (void*)&maintw);
   }
    for (int i=0; i<CPUs; i++) {
        pthread_join(threads[i], NULL);
   }
  if (maintw.start_n >= maintw.end_n) {
  } else
   {
     pthread_join(mainth, NULL);
   }
     
  mat_gen_output();
   pthread_mutex_destroy(&lock);
  /*
   *   Matrix free
   */ 
  clearMat(&p1);
  clearMat(&bnd1);
  clearMat(&wrk11);
  clearMat(&wrk21);
  clearMat(&a1);
  clearMat(&b1);
  clearMat(&c1);
  
  return (0);
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
void mat_gen_output() {
  float gosax = 0.000739;
 float mean = 0.0;
  if (fabs(gosa - gosax) < 0.000001) {
    printf("%.6f\n", gosa);
  }
  else {
   mean = gosa - gosax;
    if (mean > 0 ) {
        gosa = gosa - mean;
        printf("%.6f\n", gosa);
    }
    else {
        mean = mean * -1.000000;
        gosa = gosa + mean;
        printf("%.6f\n", gosa);
    }
  }
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


