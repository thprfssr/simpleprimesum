#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>

#define INTERVAL_SIZE 1000000000

bool *SIEVER = NULL;

bool is_prime(uint64_t n)
{
	if (n <= 1) {
		return false;
	}
	for (int i = 2; i * i <= n; i++) {
		if (n % i == 0) {
			return false;
		}
	}
	return true;
}

uint64_t isqrt(uint64_t n)
{
	uint64_t r = (uint64_t) floor(sqrt((double) n));
	return r;
}

bool *get_siever(uint64_t n)
{
	uint64_t r = isqrt(n);
	bool *list = NULL;
	list = malloc(r * sizeof(bool));
	if (list == NULL) {
		printf("Error in get_siever(): Could not allocate enough memory for list! Exiting...\n");
		exit(-1);
	}

	for (int i = 0; i <= r; i++) {
		list[i] = is_prime(i);
	}

	return list;
}

bool *prime_sieve(uint64_t a, uint64_t b)
{
	printf("Sieving between %" PRIu64 " and %" PRIu64 "...\n", a, b);
	if (!(a < b)) {
		printf("Error in prime_sieve(): Lower bound must be strictly less than upper bound! Exiting...\n");
		exit(-1);
	}

	uint64_t r = isqrt(b);
	uint64_t n = b - a;
	bool *siever = SIEVER;

	bool *sief = NULL;
	sief = malloc(n * sizeof(bool));
	if (sief == NULL) {
		printf("Error in prime_sieve(): Could not allocate enough memory! Exiting...\n");
		exit(-1);
	}

	// Initialize the sief elements all to true
	for (int i = 0; i < n; i++) {
		sief[i] = true;
	}

	// Take care of the special cases of 0 and 1
	if (a == 0) {
		sief[0 - a] = false;
		sief[1 - a] = false;
	} else if (a == 1) {
		sief[1 - a] = false;
	}

	// Start sieving
	for (int p = 0; p <= r; p++) {
		if (siever[p] == false) {
			continue;
		}

		if (p < 1000) {
			printf("Sieving multiples of %i...\n", p);
		} else if (p == 1013) {
			printf("Sieving multiples of higher primes...\n");
		}

		// Eliminate all the multiples of p except for p itself
		int j = p;
		while (j < a) {
			j += p;
		}
		if (j == p) {
			j += p;
		}
		while (j < b) {
			sief[j - a] = false;
			j += p;
		}

		if (p >= b) {
			break;
		}
	}

	return sief;
}

uint64_t partial_prime_sum(uint64_t a, uint64_t b)
{
	bool *sief = prime_sieve(a, b);
	uint64_t s = 0;
	for (int i = a; i < b; i++) {
		if (sief[i - a] == true) {
			//printf("%i is prime\n", i);
			s += i;
		}
	}

	free(sief);
	printf("The sum of all the primes in [%" PRIu64 ", %" PRIu64 ") is:\n%" PRIu64 "\n", a, b, s);
	return s;
}

uint64_t total_prime_sum(uint64_t n)
{
	SIEVER = get_siever(n);

	int q = n / INTERVAL_SIZE;
	int r = n % INTERVAL_SIZE;

	uint64_t s = 0;
	for (int k = 0; k < q; k++) {
		s += partial_prime_sum(k * INTERVAL_SIZE, (k + 1) * INTERVAL_SIZE);
	}
	if (r > 0) {
		s += partial_prime_sum(q * INTERVAL_SIZE, n);
	}

	printf("The total sum of all the primes strictly less than %" PRIu64 " is:\n%" PRIu64 "\n", n, s);
	return s;
}

int main()
{
	int a = 0;
	int b = 1000000000;
	int n = 1000000000;
	total_prime_sum(n);

	return 0;
}
