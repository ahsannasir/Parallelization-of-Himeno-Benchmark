#include <complex>
#include <iostream>
#include <pthread.h>
#include <sys/sysinfo.h>

using namespace std;

int max_row, max_column, max_n;
int step_i = 0;
int noOfThreads;
char **mat;
void* mandelbrot(void* arg){
	int core = step_i++;
	for(int r = core * max_row/noOfThreads; r <  (core + 1) * max_row/ noOfThreads; ++r){
//                std::cout << '\n'<< '\n'<< '\n'<< '\n';
                for(int c = 0; c < max_column; ++c){
                        complex<float> z;
                        int n = 0;
  //                      std::cout << '\n';
                        while(abs(z) < 2 && ++n < max_n){
                                z = pow(z, 2) + decltype(z)(
                                        (float)c * 2 / max_column - 1.5,
                                        (float)r * 2 / max_row - 1
                                );
  //                              std::cout << "value of the z"<< z ;
  //                              std::cout << "value of n" << n ;
                        }
                        mat[r][c]=(n == max_n ? '#' : '.');
                }
        }
}

int main(){
//	int max_row, max_column, max_n;
	cin >> max_row;
	cin >> max_column;
	cin >> max_n;

	mat = (char**)malloc(sizeof(char*)*max_row);

	for (int i=0; i<max_row;i++)
		mat[i]=(char*)malloc(sizeof(char)*max_column);
	noOfThreads = get_nprocs();
	pthread_t threads[noOfThreads];
  // Creating  threads, each evaluating its own part
  	for (int i = 0; i < noOfThreads; i++) {
      	int* p;
      	pthread_create(&threads[i], NULL, mandelbrot, (void*)(p));
  	}
  	for (int i = 0; i < noOfThreads; i++)
      		pthread_join(threads[i], NULL);

/*
	for(int r = 0; r < max_row; ++r){
                std::cout << '\n'<< '\n'<< '\n'<< '\n';
		for(int c = 0; c < max_column; ++c){
			complex<float> z;
			int n = 0;	
			std::cout << '\n';
			while(abs(z) < 2 && ++n < max_n){
				z = pow(z, 2) + decltype(z)(
					(float)c * 2 / max_column - 1.5,
					(float)r * 2 / max_row - 1
				);
				std::cout << "value of the z"<< z ;
		  		std::cout << "value of n" << n ;
			}	
			mat[r][c]=(n == max_n ? '#' : '.');
		}
	}
*/
	for(int r = 0; r < max_row; ++r){
		for(int c = 0; c < max_column; ++c)
			std::cout << mat[r][c];
		cout << '\n';
	}	
}

