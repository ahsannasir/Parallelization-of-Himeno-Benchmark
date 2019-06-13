#include <complex>
#include <iostream>

using namespace std;

int main(){
	int max_row, max_column, max_n;
	cin >> max_row;
	cin >> max_column;
	cin >> max_n;

 char *mat;
	mat = (char*) calloc((max_row * max_column), sizeof(char*));
	complex<float> z;
			int n = 0;

	for(int r = 0; r < max_row; ++r){
		for(int c = 0; c < max_column; ++c){
		z = 0; n = 0;
			while(((z.real() * z.real()) + (z.imag() * z.imag()) < 4  && ++n < max_n)
				z = (z * z) + decltype(z)(
					(float)c * 2 / max_column - 1.5,
					(float)r * 2 / max_row - 1
				);
			mat[(r * (max_column)) + c]=(n == max_n ? '#' : '.');
		}
	}

	for(int r = 0; r < max_row; ++r){
		for(int c = 0; c < max_column; ++c)
			std::cout << mat[(r * (max_column)) + c];
		cout << '\n';
	}	
}

