#include <complex>
#include <iostream>
#include <assert.h>
#include <sys/sysinfo.h>
#include <pthread.h>


using namespace std;
int max_row, max_column, max_n;

char* mat;
typedef struct
{
  int from, to;
} ThreadData;
int cpus;


void* fillMatrix(void* arg)
{
	ThreadData* range = (ThreadData*) arg; //turn void argument to struct

	for(int r = range->from; r < range->to; ++r) {
		for(int c = 0; c < max_column; ++c) {
			complex <float> z;
			int n = 0;
			while(abs(z) < 2 && ++n < max_n)
				z = pow(z, 2) + decltype(z)(
					(float)c * 2 / max_column - 1.5,
					(float)r * 2 / max_row - 1
				);
			mat[(r * (max_column)) + c] = (n == max_n ? '#' : '.');
		}
	}

	pthread_exit(0);
}

int main(){

	 cpus = get_nprocs();
	if (getenv("MAX_CPUS"))		//getting MAX number of CPUS
  	{
    		cpus = atoi(getenv("MAX_CPUS"));
  	}

    	assert(cpus > 0 && cpus <= 64);
    	//fprintf(stderr, "Running on %d CPUS\n", cpus);
 // cpus = 32;
	cin >> max_row >> max_column >> max_n;

	pthread_t td[cpus]; //create array of threads
  ThreadData range[cpus];
	//Initialize matrix
	mat = (char*) calloc((max_row * max_column), sizeof(char*));

	//for (int i = 0; i < max_row; i++)
//		mat[i] = (char*) calloc(max_column, sizeof(char));

  int loopSize = (max_row + cpus) / cpus;

  // prepare range of for loops beforehand
	for(int i = 0; i < cpus; i++)
	{
		range[i].from = (i * loopSize);
		range[i].to = (i + 1) * loopSize;

		//if 2nd term exceeds max_row change it
		if(range[i].to > max_row) range[i].to = max_row;
	}

	//create and start threads with function fillMatrix, with arguments address of range[i]
	for(int i = 0; i < cpus; i++)
    pthread_create(&td[i], NULL, fillMatrix, range + i);

  //wait for threads to finish
  for(int i = 0; i < cpus; i++)
    pthread_join(td[i], NULL);

  //print Matrix
	for(int r = 0; r < max_row; ++r) {
		for(int c = 0; c < max_column; ++c)
			cout << mat[(r * max_column) + c];
		cout << '\n';
	}
}

