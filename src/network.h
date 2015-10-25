#include <stdlib.h>
#include <stdio.h>
#include "mandelbrot.h"

#define network_quit() network_quit_h(__FILE__, __LINE__);

int network_init();
int network_accept();

size_t network_read (void *buffer, size_t size, size_t count);
size_t network_write(void *buffer, size_t size, size_t count);

size_t network_read_mp(struct fractal_params *fractal);
size_t network_write_q(mpq_t *q);
size_t network_write_mp(mp_t *v);

void network_stop();
void network_close();
void network_finish();
void network_quit_h (char *file, int line);
