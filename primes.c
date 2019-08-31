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

/* Allocates memory for a sief */
bool *create_sief(int size)
{
	bool *sief = NULL;
	sief = malloc(size * sizeof(bool));
	if (sief == NULL) {
		printf("Error in prime_sieve(): Could not allocate enough memory! Exiting...\n");
		exit(-1);
	}

	return sief;
}

/* Sets all the sief elements to true. */
void sief_set_true(bool *sief, int size)
{
	for (int i = 0; i < size; i++) {
		sief[i] = true;
	}
}

bool *prime_sieve(uint64_t a, uint64_t b, bool *sief, int size)
{
	printf("Sieving between %" PRIu64 " and %" PRIu64 "...\n", a, b);

	/* First check that several assumptions hold. If not, die. */
	if (!(a < b)) {
		printf("Error in prime_sieve(): Lower bound must be strictly less than upper bound! Exiting...\n");
		exit(-1);
	}
	if (!(b - a <= size)) {
		printf("Error in prime_sieve(): Interval size must be less than or equal to the sieve size! Exiting...\n");
		exit(-1);
	}
	if (sief == NULL) {
		printf("Error in prime_sieve(): Sief is NULL! Exiting...\n");
		exit(-1);
	}

	uint64_t r = isqrt(b);
	uint64_t n = b - a;
	bool *siever = SIEVER;

	// Initialize the sief elements all to true
	sief_set_true(sief, size);

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

		/* Exit the loop if we go past b. */
		if (p >= b) {
			break;
		}
	}

	return sief;
}

uint64_t partial_prime_sum(uint64_t a, uint64_t b, bool *sief, int size)
{
	sief = prime_sieve(a, b, sief, size);
	uint64_t s = 0;
	for (uint64_t i = a; i < b; i++) {
		if (sief[i - a] == true) {
			//printf("%i is prime\n", i);
			s += i;
		}
	}

	printf("The sum of all the primes in [%" PRIu64 ", %" PRIu64 ") is:\n%" PRIu64 "\n", a, b, s);
	return s;
}

uint64_t total_prime_sum(uint64_t n)
{
	SIEVER = get_siever(n);
	bool *sief = create_sief(INTERVAL_SIZE);

	uint64_t q = n / INTERVAL_SIZE;
	uint64_t r = n % INTERVAL_SIZE;

	uint64_t s = 0;
	for (uint64_t k = 0; k < q; k++) {
		s += partial_prime_sum(k * INTERVAL_SIZE, (k + 1) * INTERVAL_SIZE, sief, INTERVAL_SIZE);
		printf("The running sum up to %" PRIu64 " is:\n%" PRIu64 "\n", (k + 1) * INTERVAL_SIZE, s);
	}
	if (r > 0) {
		s += partial_prime_sum(q * INTERVAL_SIZE, n, sief, INTERVAL_SIZE);
	}

	printf("The total sum of all the primes strictly less than %" PRIu64 " is:\n%" PRIu64 "\n", n, s);
	return s;
}

uint64_t total_interval_prime_sum(uint64_t a, uint64_t b)
{
	SIEVER = get_siever(b);
	bool *sief = create_sief(INTERVAL_SIZE);

	uint64_t n = b - a;
	uint64_t q = n / INTERVAL_SIZE;
	uint64_t r = n % INTERVAL_SIZE;

	uint64_t s = 0;
	for (uint64_t k = 0; k < q; k++) {
		s += partial_prime_sum(k * INTERVAL_SIZE + a, (k + 1) * INTERVAL_SIZE + a, sief, INTERVAL_SIZE);
		printf("The running sum within the interval [%" PRIu64 ", %" PRIu64 ") is:\n%" PRIu64 "\n", a, (k + 1) * INTERVAL_SIZE + a, s);
	}
	if (r > 0) {
		s += partial_prime_sum(q * INTERVAL_SIZE + a, b, sief, INTERVAL_SIZE);
	}

	printf("The total sum of all the primes in the interval [%" PRIu64 ", %" PRIu64 ") is:\n%" PRIu64 "\n", a, b, s);
	return s;
}

int main(int argc, char *argv[])
{
	uint64_t n = strtoull(argv[1], NULL, 10);
	total_prime_sum(n);

	return 0;
}
