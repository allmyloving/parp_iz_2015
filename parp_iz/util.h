#include "stdafx.h"

float* fill1d(const size_t size);

float** fill2d(const size_t size);

void print(float** c, size_t size);

void print(float* p, int size);

void sprtr();

bool equals(float a, float b);

bool areEqual(float** matrix1, float** matrix2, size_t size);

bool areEqual(float* v1, float* v2, size_t size);

// Метод превращает двумерный массив в одномерный для дальнейшего использования SSE комманд
float* widen(float** input, const int size);

float* widen_transpose(float** input, const int size);