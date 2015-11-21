#include "stdafx.h"
#include "util.h"

#define EPSILON 0.01

float* fill1d(const size_t size){
	float* arr = (float*)_aligned_malloc(size*sizeof(float), 16);
	for (int i = 0; i < size; i++){
		arr[i] = rand() / (RAND_MAX - 1.);
	}
	return arr;
}

float** fill2d(const size_t size){
	float** matrix = (float**)_aligned_malloc(size*sizeof(float), 16);

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
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			if (!equals(matrix1[i][j], matrix2[i][j])){
				return false;
			}
		}
	}
	return true;
}

bool areEqual(float* v1, float* v2, size_t size){
	for (int i = 0; i < size; i++){
		if (!equals(v1[i], v2[i])){
			return false;
		}
	}
	return true;
}

// Метод превращает двумерный массив в одномерный для дальнейшего использования SSE комманд
float* widen(float** input, const int size){
	float* result = (float*)_aligned_malloc(size*size*sizeof(float), 16);

	for (int i = 0, k = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			result[k++] = input[i][j];
		}
	}
	return result;
}

float* widen_transpose(float** input, const int size){
	float* result = (float*)_aligned_malloc(size*size*sizeof(float), 16);

	for (int i = 0, k = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			result[k++] = input[j][i];
		}
	}
	return result;
}