//#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <papi.h>
#include <math.h>

#define SYSTEMTIME clock_t

void multiplyMatrixRegular(double* mat1, double* mat2, double* mat_res, int mat_size, int x0, int x1) {
	int i, j, k;
	double acc;
	for (i = x0; i < x1; ++i) {	
		for (j = x0; j < x1; ++j) {	
			acc = 0;
			for (k = x0; k < x1; ++k) {	
				acc += mat1[i*mat_size + k] * mat2[k*mat_size + j];
			}
			mat_res[i*mat_size + j] = acc;
		}
	}
}

void multiplyMatrixLines(double* mat1, double* mat2, double* mat_res, int mat_size, int x0, int x1) {
	int i, j, k;
	for (i = x0; i < x1; ++i) {	
		for (k = x0; k < x1; ++k) {	
			for (j = x0; j < x1; ++j) {	
				mat_res[i*mat_size + j] += mat1[i*mat_size + k] * mat2[k*mat_size + j];
			}
		}
	}
}
 
double multiplyMatrix(int mat_size, bool is_line = false) {
	SYSTEMTIME Time1, Time2;
	double* mat1 = (double*) malloc(mat_size * mat_size * sizeof(*mat1));
	double* mat2 = (double*) malloc(mat_size * mat_size * sizeof(*mat2));
	double* mat_res = (double*) malloc(mat_size * mat_size * sizeof(*mat_res));

	for (int i = 0; i < mat_size; ++i) {
		for (int j = 0; j < mat_size; ++j) {
			mat1[i*mat_size + j] = 1.0;
			mat2[i*mat_size + j] = 1.0 + i;
		}
	}

    Time1 = clock();

	if (is_line) {
		multiplyMatrixLines(mat1, mat2, mat_res, mat_size, 0, mat_size);
	} else {
		multiplyMatrixRegular(mat1, mat2, mat_res, mat_size, 0, mat_size);
	}

    Time2 = clock();

	free(mat1);
	free(mat2);
	free(mat_res);

	return ((double)(Time2 - Time1) / CLOCKS_PER_SEC);
}

float internalProduct(float *v1, float *v2, int col) {
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

void printUsage(char* path) {
	printf("usage: %s <algorithm> <matrix size>\n", path);
	printf("algorithms:\n");
	printf("\t1: regular\n");
	printf("\t2: line by line\n");
	printf("\t3: clusters\n");
}

int main(int argc, char *argv[]) {
	int EventSet = PAPI_NULL;
	double time;
  	long long values[2];

	if (argc < 3) {
		printUsage(argv[0]);
		exit(1);
	}

	int algorithm = atoi(argv[1]);
	int matrix_size = atoi(argv[2]);

	if (algorithm < 1 || algorithm > 3) {
		printf("Invalid algorithm!\n");
		printUsage(argv[0]);
		exit(2);
	}

	if (matrix_size <= 0) {
		printf("Matrix size must be positive!\n");
		printUsage(argv[0]);
		exit(3);
	}
	
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

	if (PAPI_start(EventSet) != PAPI_OK) {
		std::cout << "ERRO: Start PAPI" << std::endl;
	}

	switch (algorithm){
	case 1: 
		time = multiplyMatrix(matrix_size);
		break;
	case 2:
		time = multiplyMatrix(matrix_size, true);
		break;
	case 3:
		printf("Not implemented yet!\n");
		exit(4);
		break;
	}

	if (PAPI_stop(EventSet, values) != PAPI_OK) {
		std::cout << "ERRO: Stop PAPI" << std::endl;
	}

	printf("%3.3f %lld %lld\n", time, values[0], values[1]);

	if (PAPI_reset(EventSet) != PAPI_OK) {
		std::cout << "FAIL reset" << std::endl; 
	}

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