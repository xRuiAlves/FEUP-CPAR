//#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <papi.h>
#include <math.h>

#define SYSTEMTIME clock_t

enum MatrixAlgorithm { regular, line_by_line, blocks };

void multiplyMatrixRegular(double* mat1, double* mat2, double* mat_res, int mat_size) {
	int i, j, k;
	double acc;
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

void multiplyMatrixLines(double* mat1, double* mat2, double* mat_res, int mat_size) {
	int i, j, k;
	for (i = 0; i < mat_size; ++i) {	
		for (k = 0; k < mat_size; ++k) {	
			for (j = 0; j < mat_size; ++j) {	
				mat_res[i*mat_size + j] += mat1[i*mat_size + k] * mat2[k*mat_size + j];
			}
		}
	}
}

void multiplyMatrixBlocks(double* mat1, double* mat2, double* mat_res, int mat_size, int block_size) {
	int block_i, block_j, block_k, i, j, k;

	for (block_i = 0; block_i < mat_size; block_i += block_size) {
        for (block_j = 0; block_j < mat_size; block_j += block_size) {
            for (block_k = 0; block_k < mat_size; block_k += block_size) {
                for (i = 0; i < block_size; ++i) {
                    for (k = 0; k < block_size; ++k) {
                        for (j = 0; j < block_size; ++j) {
							mat_res[(block_i + i)*mat_size + block_j + j] += 
								mat1[(block_i + i)*mat_size + block_k + k] * 
								mat2[(block_k + k)*mat_size + block_j + j];
						}
					}
				}

			}
		}
	}                   
}

double multiplyMatrix(int mat_size, MatrixAlgorithm algorithm, int block_size = 0) {
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

	switch(algorithm) {
	case regular:
		multiplyMatrixRegular(mat1, mat2, mat_res, mat_size);
		break;
	case line_by_line:
		multiplyMatrixLines(mat1, mat2, mat_res, mat_size);
		break;
	case blocks:
		multiplyMatrixBlocks(mat1, mat2, mat_res, mat_size, block_size);
		break;
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
	printf("usage: %s <algorithm> <matrix size> [<block_size>]\n", path);
	printf("algorithms:\n");
	printf("\t1: regular\n");
	printf("\t2: line by line\n");
	printf("\t3: block by block (requires block size input)\n");
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
		time = multiplyMatrix(matrix_size, MatrixAlgorithm::regular);
		break;
	case 2:
		time = multiplyMatrix(matrix_size, MatrixAlgorithm::line_by_line);
		break;
	case 3: 
		{
			if (argc < 4) {
				printUsage(argv[0]);
				exit(4);
			}
			int block_size = atoi(argv[3]);
			time = multiplyMatrix(matrix_size, MatrixAlgorithm::blocks, block_size);
			break;
		}
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
