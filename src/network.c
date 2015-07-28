#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <gmp.h>
#include "mandelbrot.h"
#include "network.h"
#include "log.h"

const int port = 3987;
int sockfd;

int clientfd;

int network_init()
{
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd == -1)
		return 0;

	bzero(&servaddr, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	    LOG(PRIO_ERROR, "setsockopt(SO_REUSEADDR) failed\n");
    
	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		close(sockfd);
		return 0;
	}
	if (listen(sockfd, 1024) == -1)
	{
		close(sockfd);
		return 0;
	}
	
	return 1;
}

int network_accept()
{
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	
	clientfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
	
	if (clientfd == -1)
	{
		LOG(PRIO_ERROR, "Failed to accept client\n");
		return 0;
	}
	
	return 1; 
}

size_t network_read(void *buffer, size_t size, size_t count)
{
	size_t btot = size * count;
	ssize_t br = 0;
	ssize_t n;
	
	LOG(PRIO_VVVERBOSE, "Read %d bytes\n", btot);
	
	while (br < btot)
	{
		n = read(clientfd, (unsigned char *)buffer + br, btot - br);
		br += n;		
		if (n == 0)
		{
			LOG(PRIO_ERROR, "Read zero bytes (%d/%d)\n", br, btot);
			return 0;
		}
		else if(n < 0)
		{
			LOG(PRIO_ERROR, "Read error (%d/%d)\n", br, btot);
			return 0;
		}
	}
	return br / size;
}

size_t network_write(void *buffer, size_t size, size_t count)
{
	size_t btot = size * count;
	ssize_t bw = 0;
	ssize_t n;
	while (bw < btot)
	{
		n = write(clientfd, (unsigned char *)buffer + bw, btot - bw);
		bw += n;		
		if (n < 1)
		{
			LOG(PRIO_ERROR, "Wrote zero bytes (%d/%d)\n", bw, btot);
			return 0;
		}
		else if(n < 0)
		{
			LOG(PRIO_ERROR, "Write error (%d/%d)\n", bw, btot);
			return 0;
		}
	}
	return bw / size;
}

size_t network_write_q(mpq_t *q)
{
	size_t totcount = 0;
	size_t count = 0;
	int64_t s_count;

	int32_t sign = mpz_sgn(mpq_numref(*q));

	unsigned char *buff = NULL;

	mpz_export(buff, &count, -1, 1, 0, 0, mpq_numref(*q));
		
	// Write sign
	if (!network_write(&sign, 4, 1)) 
	{
		free(buff);
		return 0;
	}
	totcount += 4;

	s_count = (int64_t)count; 
	// Write size of buffer as long
	if (!network_write(&s_count, 8, 1))
	{
		free(buff);
		return 0;
	}
	totcount += 8;

	// Write actual buffer with values
	if (!network_write(buff, 1, count))
	{
		free(buff);
		return 0;
	}
	totcount += count;
	free(buff); buff = NULL;

	mpz_export(buff, &count, -1, 1, 0, 0, mpq_denref(*q));

	s_count = (int64_t)count;
	// Write size of buffer as long
	if (!network_write(&s_count, 8, 1))
	{
		free(buff);
		return 0;
	}
	totcount += 8;

	if (!network_write(buff, count, 1))
	{
		free(buff);
		return 0;
	}
	totcount += count;
	free(buff);

	return totcount;
}

size_t network_read_mp(struct fractal_params *fractal)
{
	mpq_t vals[4];
	mpq_inits(vals[0], vals[1], vals[2], vals[3], NULL);
	
	for (int i = 0; i < 4; i++)
	{
		unsigned char *buff;
		int32_t sign, count;

		//Read numerator
		if (!network_read(&sign, 4, 1) ||
			!network_read(&count, 4, 1))
		{
			LOG(PRIO_ERROR, "Failed to read rationals\n");
			return 0;
		}
		buff = malloc(count);
		if (!buff)
		{
			LOG(PRIO_ERROR, "Failed to allocate buffer\n");
			return 0;
		}
		if (network_read(buff, 1, count) != count)
		{
			free(buff);
			return 0;
		}
		mpz_import(mpq_numref(vals[i]), count, -1, 1, 0, 0, buff);
		if (sign == -1)
			mpz_neg(mpq_numref(vals[i]), mpq_numref(vals[i]));

		free(buff); buff = NULL;

		//Read denominator
		if (!network_read(&count, 4, 1))
		{
			LOG(PRIO_ERROR, "Failed to read rationals\n");
			return 0;
		}
		buff = malloc(count);
		if (!buff)
		{
			LOG(PRIO_ERROR, "Failed to allocate buffer\n");
			return 0;
		}
		if (network_read(buff, 1, count) != count)
		{
			free(buff);
			return 0;
		}
		mpz_import(mpq_denref(vals[i]), count, -1, 1, 0, 0, buff);

		free(buff); buff = NULL;
	}

#ifdef MP_RATIONALS
	mp_init_set(fractal->min_re, vals[0]);
	mp_init_set(fractal->max_re, vals[1]);
	mp_init_set(fractal->min_im, vals[2]);
	mp_init_set(fractal->max_im, vals[3]);
#else
	mpf_set_q(fractal->min_re, vals[0]);
	mpf_set_q(fractal->max_re, vals[1]);
	mpf_set_q(fractal->min_im, vals[2]);
	mpf_set_q(fractal->max_im, vals[3]);
#endif

	mpq_clears(vals[0], vals[1], vals[2], vals[3], NULL);

	return 1;
}

void network_stop()
{
	close(sockfd);
}

void network_finish()
{
	shutdown(clientfd, SHUT_WR);
	
	char buff[1024];
	while (1)
	{
		int res = read(clientfd, buff, 1024);
		if (res < 0)
		{
			LOG(PRIO_ERROR, "Unexpected error during network shutdown\n");
			return;
		}
		if (res == 0) // Loop until res == 0
			break;
	}    
}

void network_close()
{
    close(clientfd);
}

void network_quit_h(char *file, int line)
{
	LOG_h(PRIO_ERROR, file, line, "Called network_quit\n");
	network_close();
	LOG(PRIO_INFO, "Child exiting\n");
	exit(1);
}
