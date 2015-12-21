#include "stdafx.h"
#include "util.h"

#define LLONG long long
#define ULLONG unsigned LLONG
#define CHUNK 64

LARGE_INTEGER start;
LARGE_INTEGER end;
LARGE_INTEGER freq;

float getMax(float** x, int& iVal, int& jVal, size_t n){
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

float getMaxP(float** x, int& iVal, int& jVal, int n){
	float max = x[0][0];
	int i, j;

#pragma omp parallel for firstprivate(j) schedule(dynamic, CHUNK)
	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){
			if (x[i][j] > max){
				max = x[i][j];
				iVal = i;
				jVal = j;
			}
		}
	}
	return max;
}

void vectMul(float** matrix, const float* vector, float* result, size_t length){
	for (size_t i = 0; i < length; i++){
		float temp = 0;
		for (size_t j = 0; j < length; j++){
			temp += matrix[i][j] * vector[j];
		}
		result[i] = temp;
	}
}

void vectMulP(float** matrix, const float* vector, float* result, int length){
	int j;
#pragma omp parallel for private(j) schedule(dynamic, 64)
	for (int i = 0; i < length; i++){
		float temp = 0;
		for (j = 0; j < length; j++){
			temp += matrix[i][j] * vector[j];
		}
		result[i] = temp;
	}
}

void vectMulP2(float** matrix, const float* vector, float* result, size_t length){
	int j;
	parallel_for(size_t(0), length, [&](size_t i){
		float temp = 0;
		for (size_t j = 0; j < length; j++){
			temp += matrix[i][j] * vector[j];
		}
		result[i] = temp;
	});
}

void matMul(float** matrix1, float** matrix2, float** result, size_t size){
	for (size_t i = 0; i < size; i++){
		for (size_t j = 0; j < size; j++){
			float sum = 0;
			for (size_t k = 0; k < size; k++){
				sum += matrix1[i][k] * matrix2[k][j];
			}
			result[i][j] = sum;
		}
	}
}

void matMulP(float** matrix1, float** matrix2, float** result, int size){
	int i, j, k;
#pragma omp parallel for private(k, j) schedule(dynamic, 64)
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			float sum = 0;
			for (k = 0; k < size; k++){
				sum += matrix1[i][k] * matrix2[k][j];
			}
			result[i][j] = sum;
		}
	}
}

void matMulP2(float** matrix1, float** matrix2, float** result, size_t size){
	parallel_for(size_t(0), size, [&](size_t i){
		for (size_t j = 0; j < size; j++){
			float sum = 0;
			for (size_t k = 0; k < size; k++){
				sum += matrix1[i][k] * matrix2[k][j];
			}
			result[i][j] = sum;
		}
	});
}

LLONG getElapsedTime(LARGE_INTEGER start, LARGE_INTEGER end, int size){
	long factor = (size <= 32) ? 1000000000 : 1000000;

	LARGE_INTEGER elapsed;
	elapsed.QuadPart = (end.QuadPart - start.QuadPart) * factor;
	elapsed.QuadPart /= freq.QuadPart;

	return elapsed.QuadPart;
}

void testMax(int size){
	printf("testing finding max value in a matrix... size ==> %d\n", size);
	float** temp = fill2d(size);
	int iVal, jVal;
	float max;
	ULLONG time1 = ULLONG_MAX, time2 = ULLONG_MAX;

	char* denom = (size <= 32) ? "mcs" : "ms";

	QueryPerformanceFrequency(&freq);

	for (int i = 0; i < 5; i++){
		QueryPerformanceCounter(&start);
		max = getMax(temp, iVal, jVal, size);
		QueryPerformanceCounter(&::end);

		ULLONG elapsed = getElapsedTime(start, ::end, size);

		if (elapsed < time1){
			time1 = elapsed;
		}
	}
	printf("max value ==> %f \ti=%d\tj=%d\tseq time ==> %d", max, iVal, jVal, time1);
	cout << denom << endl;

	for (int i = 0; i < 5; i++){
		QueryPerformanceCounter(&start);
		max = getMaxP(temp, iVal, jVal, size);
		QueryPerformanceCounter(&::end);

		ULLONG elapsed = getElapsedTime(start, ::end, size);
		if (elapsed < time2){
			time2 = elapsed;
		}
	}
	printf("max value ==> %f \ti=%d\tj=%d\tomp time ==> %d", max, iVal, jVal, time2);
	cout << denom << endl;
	printf("T(seq)/T(par)=%f\n", (float)time1 / time2);
	sprtr();

	free(temp);
}

void testVectMul(const int size){
	printf("testing vector by matrix multiplication... size ==> %d\n", size);
	float** temp = fill2d(size);
	float* vector = fill1d(size);
	float* res = (float*)malloc(size*sizeof(float));
	float* res2 = (float*)malloc(size*sizeof(float));
	float* res3 = (float*)malloc(size*sizeof(float));
	char* denom = (size <= 32) ? "mcs" : "ms";

	ULLONG time1 = ULLONG_MAX, time2 = ULLONG_MAX, time3 = ULLONG_MAX;
	QueryPerformanceFrequency(&freq);
	for (int i = 0; i < 5; i++){
		QueryPerformanceCounter(&start);
		vectMul(temp, vector, res, size);
		QueryPerformanceCounter(&::end);
		ULLONG elapsed = getElapsedTime(start, ::end, size);
		if (elapsed < time1){
			time1 = elapsed;
		}
	}
	printf("vector multiplication \ttime ==> %d", time1);
	cout << denom << endl;


	for (int i = 0; i < 5; i++){
		QueryPerformanceCounter(&start);
		vectMulP(temp, vector, res2, size);
		QueryPerformanceCounter(&::end);

		ULLONG elapsed = getElapsedTime(start, ::end, size);
		if (elapsed < time2){
			time2 = elapsed;
		}
	}

	printf("vector multiplication omp time ==> %d", time2);
	cout << denom << endl;

	bool b = (areEqual(res, res2, size));
	cout << "results are equal ==> " << (b ? "true" : "false") << endl;
	cout << "time1/time2 ==>" << ((double)time1 / time2) << endl;
	sprtr();

	for (int i = 0; i < 5; i++){
		QueryPerformanceCounter(&start);
		vectMulP2(temp, vector, res3, size);
		QueryPerformanceCounter(&::end);

		ULLONG elapsed = getElapsedTime(start, ::end, size);
		if (elapsed < time3){
			time3 = elapsed;
		}
	}

	printf("vector multiplication ppl time ==> %d", time3);
	cout << denom << endl;

	b = (areEqual(res, res3, size));
	cout << "results are equal ==> " << (b ? "true" : "false") << endl;
	cout << "time1/time3 ==>" << ((double)time1 / time3) << endl;
	sprtr();

	for (int i = 0; i < size; i++){
		free(temp[i]);
	}
	free(temp);
	free(vector);
	free(res);
	free(res2);
}

void testMatMul(const int size){
	printf("testing matrix multiplcation... size ==> %d\n", size);
	float** temp1 = fill2d(size);
	float** temp2 = fill2d(size);
	float** result = malloc(size);
	float** result2 = malloc(size);
	float** result3 = malloc(size);

	char* denom = (size <= 32) ? "mcs" : "ms";

	ULLONG time1 = ULLONG_MAX, time2 = ULLONG_MAX, time3 = ULLONG_MAX;
	QueryPerformanceFrequency(&freq);
	for (int i = 0; i < 5; i++){
		QueryPerformanceCounter(&start);
		matMul(temp1, temp2, result, size);
		QueryPerformanceCounter(&::end);

		ULLONG elapsed = getElapsedTime(start, ::end, size);
		if (elapsed < time1){
			time1 = elapsed;
		}
	}
	printf("seq time ==> %d", time1);
	cout << denom << endl;

	for (int i = 0; i < 5; i++){
		QueryPerformanceCounter(&start);
		matMulP(temp1, temp2, result2, size);
		QueryPerformanceCounter(&::end);

		long elapsed = getElapsedTime(start, ::end, size);
		if (elapsed < time2){
			time2 = elapsed;
		}
	}
	printf("omp time ==> %d", time2);
	cout << denom << endl;
	printf("T(seq)/T(par)=%f\n", (float)time1 / time2);

	bool b = (areEqual(result, result2, size));
	cout << "results are equal ==> " << (b ? "true" : "false") << endl;
	//print(result2, size);
	sprtr();

	for (int i = 0; i < 5; i++){
		QueryPerformanceCounter(&start);
		matMulP2(temp1, temp2, result3, size);
		QueryPerformanceCounter(&::end);

		ULLONG elapsed = getElapsedTime(start, ::end, size);
		if (elapsed < time3){
			time3 = elapsed;
		}
	}

	printf("matrix multiplication ppl time ==> %d", time3);
	cout << denom << endl;

	b = (areEqual(result, result3, size));
	cout << "results are equal ==> " << (b ? "true" : "false") << endl;
	cout << "time1/time3 ==>" << ((double)time1 / time3) << endl;
	sprtr();

	for (int i = 0; i < size; i++){
		free(temp1[i]);
		free(temp2[i]);
		free(result[i]);
		free(result2[i]);
	}
	free(temp1);
	free(temp2);
	free(result);
	free(result2);
}

int main(){
	srand(time(0));

	for (int i = 4; i <= 2048; i *= 2){
		testMax(i);
	}
	for (int i = 4; i <= 2048; i *= 2){
		testVectMul(i);
	}
	for (int i = 4; i <= 2048; i *= 2){
		testMatMul(i);
	}


}