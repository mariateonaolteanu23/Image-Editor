#ifndef MATRIX_UTTILS_
#define MATRIX_UTTILS_

double **alloc_matrix(int n, int m);

void free_matrix(double **a, int n);

void b_load(FILE *file, double **a, int n, int m);

void t_load(FILE *file, double **a, int n, int m);

void b_3_load(FILE *file, double **a, double **b, double **c, int n, int m);

void t_3_load(FILE *file, double **a, double **b, double **c, int n, int m);

void rotate_90_inplace(double **a, int x1, int y1, int n);

void rotate_180_inplace(double **a, int x1, int y1, int n);

void rotate_270_inplace(double **a, int x1, int y1, int n);

double **rotate_90(double **a, int n, int m);

double **rotate_180(double **a, int n, int m);

double **rotate_270(double **a, int n, int m);

double **crop_matrix(double **a, int x1, int y1, int x2, int y2);

void t_print(FILE *file, double **a, int n, int m);

void t_3_print(FILE *file, double **a, double **b, double **c, int n, int m);

void b_print(FILE *file, double **a, int n, int m);

void b_3_print(FILE *file, double **a, double **b, double **c, int n, int m);

#endif /* MATRIX_UTTILS_ */
