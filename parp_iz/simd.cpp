#include "stdafx.h"
#include "simd.h"
#include "stdint.h"

float getMaxP(const float* x, const int size){
	__m128 *px = (__m128*) x;
	__m128 max = _mm_setzero_ps();

	for (int i = 0; i < size / 4; i++){
		max = _mm_max_ps(max, px[i]);
	}

	float maxF = max.m128_f32[0];
	for (int i = 1; i < 4; i++){
		if (max.m128_f32[i] > maxF){
			maxF = max.m128_f32[i];
		}
	}
	return maxF;
}

void print_m128(__m128 item){
	for (int i = 0; i < 4; i++){
		cout << item.m128_f32[i] << " ";
	}
	//cout << endl;
}

void vectMulP(const float* matrix, const float* vector, float* result, int size){
	__m128 add, mul, v_row, m_row;

	for (int i = 0, k = 0; i < size; i++, k += size){
		add = _mm_setzero_ps();
		
		for (int j = 0; j < size / 4; j++){
			v_row = _mm_load_ps(&vector[j * 4]);
			m_row = _mm_load_ps(&matrix[j * 4 + k]);
			mul = _mm_mul_ps(v_row, m_row);

			add = _mm_add_ps(add, mul);
		}

		add = _mm_hadd_ps(add, add);
		add = _mm_hadd_ps(add, add);
		_mm_store_ss(&result[i], add);
	}
}

void matMulP(const float* matrix1, const float* matrix2, float** result, int size){
	__m128 add, mul, m1_row, m2_row;

	for (int h = 0; h < size; h++){
		for (int i = 0; i < size; i++){
			add = _mm_setzero_ps();

			for (int j = 0; j < size / 4; j++){
				m1_row = _mm_load_ps(&matrix1[j * 4 + h*size]);
				m2_row = _mm_load_ps(&matrix2[j * 4 + i*size]);

				mul = _mm_mul_ps(m1_row, m2_row);
				add = _mm_add_ps(add, mul);
			}
			add = _mm_hadd_ps(add, add);
			add = _mm_hadd_ps(add, add);
			_mm_store_ss(&result[h][i], add);
		}
	}
}