#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "ppm_render.h"

#define MAX_ITER_VALUE 1200

typedef struct {
	double re;
	double im;
} complex_d;

static size_t mandelbrot_set[HEIGHT][WIDTH];
const complex_d center = {-0.743643887037, 0.131825904205};
const complex_d window_size = {4.0 / 40000.0, 2.0 / 40000.0};

complex_d multiply_complex(complex_d z1, complex_d z2);
complex_d divide_complex(complex_d z1, complex_d z2);
complex_d add_complex(complex_d z1, complex_d z2);
complex_d sub_complex(complex_d z1, complex_d z2);
double get_complex_module(complex_d c);

void create_mandelbrot_set(size_t width, size_t height, complex_d c, complex_d win_size);

int main(int argc, char **argv) {
	create_mandelbrot_set(WIDTH, HEIGHT, center, window_size);
	for(size_t h = 0;h < HEIGHT;h++) {
		for(size_t w = 0;w < WIDTH;w++) {
			hsv_d code = {0};
			if(mandelbrot_set[h][w] >= MAX_ITER_VALUE) {
				code.s = 0;
			} else {
				code.v = 255;
				size_t buff;
				buff = mandelbrot_set[h][w] * 256 / 256;
				if(buff >= 256)
					buff = 255;
				code.s = buff;
				code.v = buff;
				buff = mandelbrot_set[h][w] * 256 / 1024;
				if(buff >= 256)
					buff = 255;
				code.h = buff;
			}
			ppm_set_pixel_hsv(h, w, code);
		}
	}
	ppm_render_image(NULL);

	return 0;
}

complex_d multiply_complex(complex_d z1, complex_d z2) {
	return (complex_d){z1.re * z2.re - z1.im * z2.im, z1.re * z2.im + z1.im * z2.re};
}
complex_d divide_complex(complex_d z1, complex_d z2) {
	double buff = z2.re * z2.re + z2.im * z2.im;
	return (complex_d){(z1.re * z2.re + z1.im * z2.im) / buff, (z1.im * z2.re - z1.re * z2.im) / buff};
}
complex_d add_complex(complex_d z1, complex_d z2) {
	return (complex_d){z1.re + z2.re, z1.im + z2.im};
}
complex_d sub_complex(complex_d z1, complex_d z2) {
	return (complex_d){z1.re - z2.re, z1.im - z2.im};
}
double get_complex_module(complex_d c) {
	return sqrt(c.re * c.re + c.im * c.im);
}

void create_mandelbrot_set(size_t width, size_t height, complex_d c, complex_d win_size) {
	double x_min = c.re - win_size.re / 2.0;
	double x_max = c.re + win_size.re / 2.0;
	double y_min = c.im - win_size.im / 2.0;
	double y_max = c.im + win_size.im / 2.0;
	printf("creating set between (%lf, %lf) and (%lf, %lf)\n", x_min, x_max, y_min, y_max);
	for(size_t h = 0;h < height;h++) {
		for(size_t w = 0;w < width;w++) {
			double x = (double)w / (double)width * (x_max - x_min) + x_min;
			double y = (double)(height - h - 1) / (double)height * (y_max - y_min) + y_min;
			complex_d C = {x,y};
			complex_d Z = C;
			size_t i;
			bool diverged = false;
			for(i = 0;i < MAX_ITER_VALUE && diverged == false;i++) {
				Z = multiply_complex(Z, Z);
				Z = add_complex(Z, C);
				if(get_complex_module(Z) > sqrt(2)) {
					diverged = true;
				}
			}
			mandelbrot_set[h][w] = i;
		}
	}
}

