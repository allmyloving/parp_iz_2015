#include "stdafx.h"
#include "simd.h"
#include "util.h"

using namespace std;

#define MAX_SIZE 2048

float getMax(float** x, int& iVal, int& jVal, int n){
	iVal = 0;
	jVal = 0;
	float max = x[0][0];

	for (size_t i = 0; i < n; i++){
		for (size_t j = 0; j < n; j++){
			if (x[i][j] > max){
				max = x[i][j];
				iVal = i;
				jVal = j;
			}
		}
	}
	return max;
}

void vectMul(float** matrix, const float* vector, float* result, int length){
	for (size_t i = 0; i < length; i++){
		float temp = 0;
		for (size_t j = 0; j < length; j++){
			temp += matrix[i][j] * vector[j];
		}
		result[i] = temp;
	}
}

void matMul(float** matrix1, float** matrix2, float** result, size_t size){
	for (size_t i = 0; i < size; i++){
		for (size_t j = 0; j < size; j++){
			result[i][j] = 0;
			for (size_t k = 0; k < size; k++){
				result[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
}

void matMul2(float** matrix1, float** matrix2, float** result, size_t size){
	for (size_t i = 0; i < size; i++){
		result[i] = new float[size];
		for (size_t j = 0; j < size; j++){
			float temp = matrix1[i][j];
			result[i][j] = 0;
			for (size_t k = 0; k < size; k++){
				result[i][k] += temp * matrix2[j][k];
			}
		}
	}
}


void testMax(){
	const int size = MAX_SIZE;
	float** temp = fill2d(size);
	float* p = widen(temp, size);

	int iVal, jVal;
	float max;
	long start, end, time1 = LONG_MAX, time2 = LONG_MAX;
	for (int i = 0; i < 5; i++){
		start = __rdtsc();
		max = getMax(temp, iVal, jVal, size);
		end = __rdtsc();
		if (end - start < time1){
			time1 = end - start;
		}
	}
	printf("max value ==> %f \ttime ==> %d\n", max, time1);

	int size2 = size*size;
	for (int i = 0; i < 5; i++){
		start = __rdtsc();
		max = getMaxP(p, size2);
		end = __rdtsc();
		if (end - start < time2){
			time2 = end - start;
		}
	}
	printf("max value ==> %f \tsse time ==> %d\n", max, time2);
	sprtr();
	_aligned_free(temp);
	_aligned_free(p);
}

void testVectMul(const int size){
	printf("testing vector by matrix multiplication... size ==> %d\n", size);
	//const int size = MAX_SIZE;

	float** temp = fill2d(size);
	float* p = widen(temp, size);
	float* vector = fill1d(size);
	float* res = (float*)_aligned_malloc(size*sizeof(float), 16);
	float* res2 = (float*)_aligned_malloc(size*sizeof(float), 16);

	long start, end, time1 = LONG_MAX, time2 = LONG_MAX;
	for (int i = 0; i < 5; i++){
		start = __rdtsc();
		vectMul(temp, vector, res2, size);
		end = __rdtsc();
		if (end - start < time1){
			time1 = end - start;
		}
	}
	printf("vector multiplication \ttime ==> %d\n", time1);

	for (int i = 0; i < 5; i++){
		start = __rdtsc();
		vectMulP(p, vector, res, size);
		end = __rdtsc();
		if (end - start < time2){
			time2 = end - start;
		}
	}

	printf("vector multiplication sse time ==> %d\n", time2);
	cout << "results are equal ==> " << ((areEqual(res, res2, size)) ? "true" : "false") << endl;
	cout << "time1/time2 ==>" << ((double)time1 / time2) << endl;
	sprtr();

	for (int i = 0; i < size; i++){
		_aligned_free(temp[i]);
	}
	_aligned_free(temp);
	_aligned_free(p);
	_aligned_free(vector);
	_aligned_free(res);
	_aligned_free(res2);
}

void testMatMul(const int size){
	printf("testing matrix multiplication... size ==> %d\n", size);

	float** temp1 = fill2d(size);
	float** temp2 = fill2d(size);
	float* m1 = widen(temp1, size);
	float* m2 = widen_transpose(temp2, size);
	float** result = (float**)_aligned_malloc(size*sizeof(float), 16);
	float** result2 = (float**)_aligned_malloc(size*sizeof(float), 16);

	for (int i = 0; i < size; i++){
		result[i] = (float*)_aligned_malloc(size*sizeof(float), 16);
		result2[i] = (float*)_aligned_malloc(size*sizeof(float), 16);
	}

	long start, end, time1 = LONG_MAX, time2 = LONG_MAX;
	for (int i = 0; i < 5; i++){
		start = __rdtsc();//clock();
		//matMul(temp1, temp2, result2, size);
		end = __rdtsc();//clock();
		if (end - start < time1){
			time1 = end - start;
		}
	}
	printf("matrix multiplication \ttime ==> %d\n", time1);

	for (int i = 0; i < 5; i++){
		start = __rdtsc();// clock();
		float* m1 = widen(temp1, size);
		float* m2 = widen_transpose(temp2, size);
		matMulP(m1, m2, result, size);
		end = __rdtsc();//clock();
		if (end - start < time2){
			time2 = end - start;
		}
	}
	printf("matrix multiplication sse time ==> %d\n", time2);
	cout << "results are equal ==> " << ((areEqual(result, result2, size)) ? "true" : "false") << endl;
	cout << "time1/time2 ==>" << ((double)time1 / time2) << endl;
	sprtr();

	for (int i = 0; i < size; i++){
		_aligned_free(temp1[i]);
		_aligned_free(temp2[i]);
		_aligned_free(result[i]);
		_aligned_free(result2[i]);
	}
	_aligned_free(temp1);
	_aligned_free(temp2);

	_aligned_free(result);
	_aligned_free(result2);

	_aligned_free(m1);
	_aligned_free(m2);
}


int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(0));
	//testMax();
	for (int i = 64; i <= MAX_SIZE; i *= 2){
		//testVectMul(i);
		//testMatMul(i);
	}
	testMatMul(2*MAX_SIZE);
	return 0;
}