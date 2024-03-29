#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>
#include <gmp.h>

#define INTERVAL_SIZE 1000000000


/* NOTE: I ran this program for N = 1T, and the sum that I got was the
 * following: s = 9 394 869 854 112 614. Notice that this number is less than
 * 2^64 (the modulo ring for uint64_t). Hence, we can't guarantee that this is
 * the true sum; we can only say that this is the sum modulo 2^64.
 *
 * 2^64 = 18 446 744 073 709 551 616
 *
 * The uint64_t allows us to represent numbers with at most 19 decimal digits.
 * And the obtained sum is almost 19 digits long. Hence, we have strong reason
 * to believe that the running sum overflowed somewhere during the program's
 * run time.
 *
 * I recommend to switch to GMP to handle arbitrary precision integers.
 */

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

char* partial_prime_sum(uint64_t a, uint64_t b, bool *sief, int size)
{
	sief = prime_sieve(a, b, sief, size);
	//uint64_t s = 0;
	mpz_t s; // Declare a GMP Integer.
	mpz_init(s); // Initialize s and set its value to zero.
	for (uint64_t i = a; i < b; i++) {
		if (sief[i - a] == true) {
			//printf("%i is prime\n", i);
			mpz_add_ui(s, s, i);
			//s += i;
		}
	}

	char *str = NULL;
	str = mpz_get_str(str, 10, s); // Convert s into a string in base ten.
	mpz_clear(s); // Clear s and free its memory
	if (str == NULL) {
		printf("Error in partial_prime_sum(): str is NULL! Exiting...\n");
		exit(-1);
	}

	printf("The sum of all the primes in [%" PRIu64 ", %" PRIu64 ") is:\n%s\n", a, b, str);
	return str;
}

void total_prime_sum(uint64_t n)
{
	SIEVER = get_siever(n);
	bool *sief = create_sief(INTERVAL_SIZE);

	uint64_t q = n / INTERVAL_SIZE;
	uint64_t r = n % INTERVAL_SIZE;

	//uint64_t s = 0;
	mpz_t s; // Declare a GMP Integer.
	mpz_init(s); // Initialize s and set its value to zero
	for (uint64_t k = 0; k < q; k++) {
		char *str = partial_prime_sum(k * INTERVAL_SIZE, (k + 1) * INTERVAL_SIZE, sief, INTERVAL_SIZE);
		mpz_t u;
		mpz_init(u);
		mpz_set_str(u, str, 10); // Set u to be equal to the value of the number in the string, in base ten
		mpz_add(s, s, u);
		mpz_clear(u);
		//s += partial_prime_sum(k * INTERVAL_SIZE, (k + 1) * INTERVAL_SIZE, sief, INTERVAL_SIZE);
		gmp_printf("The running sum up to %" PRIu64 " is:\n%Zd\n", (k + 1) * INTERVAL_SIZE, s);
	}
	if (r > 0) {
		//s += partial_prime_sum(q * INTERVAL_SIZE, n, sief, INTERVAL_SIZE);
		char *str = partial_prime_sum(q * INTERVAL_SIZE, n, sief, INTERVAL_SIZE);
		mpz_t u;
		mpz_init(u);
		mpz_set_str(u, str, 10);
		mpz_add(s, s, u);
		mpz_clear(u);
	}

	char *str = NULL;
	str = mpz_get_str(str, 10, s);
	if (str == NULL) {
		printf("Error in total_prime_sum(): str is NULL! Exiting...\n");
		exit(-1);
	}
	mpz_clear(s);

	printf("The total sum of all the primes strictly less than %" PRIu64 " is:\n%s\n", n, str);
	//return s;
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
