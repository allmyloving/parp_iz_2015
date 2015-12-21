#include "stdafx.h"
#include "util.h"

#define EPSILON 0.01

float** malloc(int size){
	float** result = (float**)malloc(size*sizeof(float));
	for (int i = 0; i < size; i++){
		result[i] = (float*)malloc(size*sizeof(float));
	}
	return result;
}

float* fill1d(const size_t size){
	float* arr = (float*)malloc(size*sizeof(float));
	for (size_t i = 0; i < size; i++){
		arr[i] = rand() / (RAND_MAX - 1.);
	}
	return arr;
}

float** fill2d(const size_t size){
	float** matrix = (float**)malloc(size*sizeof(float));

	for (size_t i = 0; i < size; i++){
		matrix[i] = fill1d(size);
	}
	return matrix;
}

void print(float** c, size_t size){
	for (size_t i = 0; i < size; i++) {
		for (size_t j = 0; j < size; j++) {
			std::cout << c[i][j] << ' ';
		}
		std::cout << std::endl;
	}
}

void print(float* p, int size){
	for (int i = 0; i < size; i++){
		cout << p[i] << " ";
	}
	cout << endl;
}

void sprtr(){
	printf("-------------------------\n");
}


bool equals(float a, float b){
	return fabs(a - b) < EPSILON;
}

bool areEqual(float** matrix1, float** matrix2, size_t size){
	for (size_t i = 0; i < size; i++){
		for (size_t j = 0; j < size; j++){
			//cout << matrix1[i][j] << " equals " << matrix2[i][j] << " ==> " << equals(matrix1[i][j], matrix2[i][j]) << endl;
			if (!equals(matrix1[i][j], matrix2[i][j])){
				return false;
			}
		}
	}
	return true;
}

bool areEqual(float* v1, float* v2, size_t size){
	for (size_t i = 0; i < size; i++){
		if (!equals(v1[i], v2[i])){
			return false;
		}
	}
	return true;
}