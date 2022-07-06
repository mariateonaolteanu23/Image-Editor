#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matrix_utils.h"
#include "utils.h"

//  allocs memory for a double matrix
double **alloc_matrix(int n, int m)
{
	double **a = (double **)malloc(sizeof(double *) * n);
	DIE(!a, "malloc a");

	for (int i = 0; i < n; ++i) {
		a[i] = (double *)malloc(sizeof(double) * m);
		DIE(!a[i], "malloc a[i]");
	}

	return a;
}

//  frees the memory allocated for a double matrix
void free_matrix(double **a, int n)
{
	for (int i = 0; i < n; ++i)
		free(a[i]);

	free(a);
	a = NULL;
}

//  loads the pixel matrix from a binary file
void b_load(FILE *file, double **a, int n, int m)
{
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			unsigned char pixel;
			fread(&pixel, sizeof(unsigned char), 1, file);
			a[i][j] = (double)pixel;
		}
	}
}

//  loads the pixel matrix from a text file
void t_load(FILE *file, double **a, int n, int m)
{
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			int pixel;
			fscanf(file, "%d", &pixel);
			a[i][j] = (double)pixel;
		}
	}
}

//  loads the color channels matrices from a binary file
void b_3_load(FILE *file, double **a, double **b, double **c, int n, int m)
{
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m * 3; j += 3) {
			unsigned char pixel1, pixel2, pixel3;
			fread(&pixel1, sizeof(unsigned char), 1, file);
			a[i][j / 3] = (double)pixel1;

			fread(&pixel2, sizeof(unsigned char), 1, file);
			b[i][j / 3] = (double)pixel2;

			fread(&pixel3, sizeof(unsigned char), 1, file);
			c[i][j / 3] = (double)pixel3;
		}
	}
}

//  loads the color channels matrices from a text file
void t_3_load(FILE *file, double **a, double **b, double **c, int n, int m)
{
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m * 3; j += 3) {
			int pixel;
			fscanf(file, "%d", &pixel);
			a[i][j / 3] = (double)pixel;

			fscanf(file, "%d", &pixel);
			b[i][j / 3] = (double)pixel;

			fscanf(file, "%d", &pixel);
			c[i][j / 3] = (double)pixel;
		}
	}
}

//  stores and computes the cropped matrix by the given selection
double **crop_matrix(double **a, int x1, int y1, int x2, int y2)
{
	double **crop = alloc_matrix(y2 - y1, x2 - x1);
	for (int i = 0; i < y2 - y1; ++i)
		for (int j = 0; j < x2 - x1; ++j)
			crop[i][j] = a[i + y1][j + x1];

	return crop;
}

//  swaps 2 elememts of a double matrix
void swap_mat_el(double *a, double *b)
{
	double temp = *a;
	*a = *b;
	*b = temp;
}

//  compute the transpose of a matrix inplace
void transpose_inplace(double **a, int x1, int y1, int n)
{
	for (int i = 0; i < n; ++i)
		for (int j = i; j < n; ++j)
			swap_mat_el(&a[i + y1][j + x1], &a[j + y1][i + x1]);
}

//  inverts a matrix's rows inplace
void invert_rows_inplace(double **a, int x1, int y1, int n)
{
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n / 2; ++j)
			swap_mat_el(&a[i + y1][j + x1], &a[i + y1][n - j - 1 + x1]);
}

//  rotates a matrix 90 degrees clockwise inplace
void rotate_90_inplace(double **a, int x1, int y1, int n)
{
	transpose_inplace(a, x1, y1, n);
	invert_rows_inplace(a, x1, y1, n);
}

//  rotates a matrix 180 degrees clockwise inplace
void rotate_180_inplace(double **a, int x1, int y1, int n)
{
	rotate_90_inplace(a, x1, y1, n);
	rotate_90_inplace(a, x1, y1, n);
}

//  rotates a matrix 270 degrees clockwise inplace
void rotate_270_inplace(double **a, int x1, int y1, int n)
{
	invert_rows_inplace(a, x1, y1, n);
	transpose_inplace(a, x1, y1, n);
}

//  copies a matrix and rotates the copy 90 degrees clockwise
double **rotate_90(double **a, int n, int m)
{
	double **rotate = alloc_matrix(m, n);

	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j)
			rotate[j][n - 1 - i] = a[i][j];

	return rotate;
}

//  copies a matrix and rotates the copy 180 degrees clockwise
double **rotate_180(double **a, int n, int m)
{
	double **rotate = alloc_matrix(n, m);

	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j)
			rotate[i][m - j - 1] = a[n - i - 1][j];

	return rotate;
}

//  copies a matrix and rotates the copy 270 degrees clockwise
double **rotate_270(double **a, int n, int m)
{
	double **rotate = alloc_matrix(m, n);

	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j)
			rotate[m - j - 1][i] = a[i][j];

	return rotate;
}

//  prints pixel matrix to text file
void t_print(FILE *file, double **a, int n, int m)
{
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			int p_a = round(a[i][j]);
			fprintf(file, "%d ", p_a);
		}
		fprintf(file, "\n");
	}
}

//  prints color channels matrices to text file
void t_3_print(FILE *file, double **a, double **b, double **c, int n, int m)
{
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			int p_a = round(a[i][j]);
			int p_b = round(b[i][j]);
			int p_c = round(c[i][j]);

			fprintf(file, "%d ", p_a);
			fprintf(file, "%d ", p_b);
			fprintf(file, "%d ", p_c);
		}
		fprintf(file, "\n");
	}
}

//  prints pixel matrix to binary file
void b_print(FILE *file, double **a, int n, int m)
{
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j) {
			unsigned char p_a = round(a[i][j]);
			fwrite(&p_a, sizeof(unsigned char), 1, file);
		}
}

//  prints color channels matrices to binary file
void b_3_print(FILE *file, double **a, double **b, double **c, int n, int m)
{
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			unsigned char p_a = round(a[i][j]);
			unsigned char p_b = round(b[i][j]);
			unsigned char p_c = round(c[i][j]);

			fwrite(&p_a, sizeof(unsigned char), 1, file);
			fwrite(&p_b, sizeof(unsigned char), 1, file);
			fwrite(&p_c, sizeof(unsigned char), 1, file);
		}
	}
}
