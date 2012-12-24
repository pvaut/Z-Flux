double neville ( int, double*, double*, double );
double* divdiff ( int, double*, double* );
double nf_eval ( int, double*, double*, double );
void cubic_nak ( int, double*, double*, double*, double*, double* );
void cubic_clamped ( int, double*, double*, double*, double*,
                     double*, double, double );
double spline_eval ( int, double*, double*, double*, double*,
                     double*, double );

double spline_eval_der( int n, double *x, double *f, double *b, double *c, double *d, double t);

double spline_eval_der2( int n, double *x, double *f, double *b, double *c, double *d, double t);
