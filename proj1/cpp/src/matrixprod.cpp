//#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <papi.h>
#include <math.h>
#include <stdlib.h>

#define SYSTEMTIME clock_t
 
void multiplyMatrixes(int mat_size, bool in_line = false) {
	SYSTEMTIME Time1, Time2;
	int i, j, k, acc;
	double* mat1 = (double*) malloc(mat_size * mat_size * sizeof(*mat1));
	double* mat2 = (double*) malloc(mat_size * mat_size * sizeof(*mat2));
	double* mat_res = (double*) malloc(mat_size * mat_size * sizeof(*mat_res));

	for (i = 0; i < mat_size; ++i) {
		for (j = 0; j < mat_size; ++j) {
			mat1[i*mat_size + j] = 1.0;
			mat2[i*mat_size + j] = 1.0 + i;
		}
	}

    Time1 = clock();

	if (in_line) {
		for (i = 0; i < mat_size; ++i) {	
			for (k = 0; k < mat_size; ++k) {	
				for (j = 0; j < mat_size; ++j) {
					mat_res[i*mat_size + j] += mat1[i*mat_size + k] * mat2[k*mat_size + j];
				}
			}
		}
	} else {
		for (i = 0; i < mat_size; ++i) {	
			for (j = 0; j < mat_size; ++j) {	
				acc = 0;
				for (k = 0; k < mat_size; ++k) {	
					acc += mat1[i*mat_size + k] * mat2[k*mat_size + j];
				}
				mat_res[i*mat_size + j] = acc;
			}
		}
	}

    Time2 = clock();
	printf("Time: %3.3f seconds\n", (double)(Time2 - Time1) / CLOCKS_PER_SEC);

	std::cout << "Result matrix:" << std::endl;
	for(i = 0; i < std::min(10, mat_size); ++i) {
		std::cout << mat_res[i] << " ";
	}
	std::cout << std::endl;

	free(mat1);
	free(mat2);
	free(mat_res);
}

float produtoInterno(float *v1, float *v2, int col) {
	int i;
	float sum=0.0;	

	for(i = 0; i < col; ++i) {
		sum += v1[i] * v2[i];
	}
	return(sum);
}

void handle_error (int ret) {
	std::cout << "PAPI error " << ret << ": " << PAPI_strerror(ret) << std::endl;
	exit(1);
}

void init_papi() {
	int ret = PAPI_library_init(PAPI_VER_CURRENT);
	if (ret != PAPI_VER_CURRENT && ret < 0) {
		std::cout << "PAPI library version mismatch!" << std::endl;
		exit(1);
	}
	if (ret < 0) {
		handle_error(ret);
	}

	std::cout << "PAPI Version Number:" 
		<< " MAJOR: " << PAPI_VERSION_MAJOR(ret)
		<< " MINOR: " << PAPI_VERSION_MINOR(ret)
		<< " REVISION: " << PAPI_VERSION_REVISION(ret) 
		<< std::endl;
}


int main (int argc, char *argv[]) {
	int mat_size, op = 1;
	int EventSet = PAPI_NULL;
  	long long values[2];
	
	if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
		std::cout << "FAIL" << std::endl;
	}

	if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
		std::cout << "ERRO: create eventset" << std::endl;
	}

	if (PAPI_add_event(EventSet,PAPI_L1_DCM ) != PAPI_OK) {
		std::cout << "ERRO: PAPI_L1_DCM" << std::endl;
	}

	if (PAPI_add_event(EventSet,PAPI_L2_DCM) != PAPI_OK) {
		std::cout << "ERRO: PAPI_L2_DCM" << std::endl;
	}

	do {
		std::cout << std::endl << "1. Multiplication" << std::endl;
		std::cout << "2. Line Multiplication" << std::endl;
		std::cout << "Selection?: ";
		std::cin >> op;

		if (op == 0) {
			break;
		}

		std::cout << "Matrix dimension? ";
   		std::cin >> mat_size;

		// Start counting
		if (PAPI_start(EventSet) != PAPI_OK) {
			std::cout << "ERRO: Start PAPI" << std::endl;
		}

		switch (op){
		case 1: 
			multiplyMatrixes(mat_size);
			break;
		case 2:
			multiplyMatrixes(mat_size, true);
			break;
		}

  		if (PAPI_stop(EventSet, values) != PAPI_OK) {
			std::cout << "ERRO: Stop PAPI" << std::endl;
		}

		std::cout << "L1 DCM: " << values[0] << std::endl;
		std::cout << "L2 DCM: " << values[1] << std::endl;

		if (PAPI_reset(EventSet) != PAPI_OK) {
			std::cout << "FAIL reset" << std::endl; 
		}
	} while (op != 0);

	if (PAPI_remove_event(EventSet, PAPI_L1_DCM) != PAPI_OK) {
		std::cout << "FAIL remove event" << std::endl; 
	}

	if (PAPI_remove_event(EventSet, PAPI_L2_DCM) != PAPI_OK) {
		std::cout << "FAIL remove event" << std::endl;
	}

	if (PAPI_destroy_eventset( &EventSet ) != PAPI_OK) {
		std::cout << "FAIL destroy" << std::endl;
	}
}
