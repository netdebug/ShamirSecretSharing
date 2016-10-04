#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <gmp.h>

#include "shamir.h"


static void gen_unique_random_list(
	const size_t list_length,
	const size_t element_size,
	mpz_t * list,
	gmp_randstate_t rng_state)
{
	size_t i = 0, j = 0;
	int is_unique = 0;

	if (!list || list_length <= 0 || element_size <= 0 || !rng_state) {
		abort();
	}

	for (i = 0; i < list_length; i++) {
		mpz_init(list[i]);
		is_unique = 0;
		while (!is_unique) {
spin:			mpz_urandomb(list[i], rng_state, (unsigned long) element_size);
			mpz_add_ui(list[i], list[i], 1);
			for (j = 0; j < i; j++) {
				if (mpz_cmp(list[i], list[j]) == 0) {
					goto spin;
				}
			}
			is_unique = 1;
		}
	}
}

int split_secret(
		const mpz_t secret,
		const unsigned int num_shares,
		const unsigned int threshold,
		const mpz_t prime,
		share_t * shares)
{
	unsigned int i = 0, j = 0;
	size_t prime_size = 0;
	int retval = EXIT_SUCCESS;
	mpz_t * coefficients = NULL, * xs = NULL;
	mpz_t y, tmp, degree;
	gmp_randstate_t rng_state;

	/* Check the inputs */
	if (mpz_cmp(secret, prime) >= 0 || !shares || threshold < 1 ||
		threshold > num_shares || num_shares < 1) {
		return EINVAL;
	}

	/* Test the supplied prime */
	if (mpz_probab_prime_p(prime, 50) < 1) {
		return EINVAL;
	}

	/*
	This is the only place where we need malloc. If this code needs
	to be ported into an "malloc-less" platform, then the following
	needs to be replaced with a statically allocated coefficients
	array with pre-defined maximum threshold.
	*/
	coefficients = (mpz_t *) malloc((threshold - 1) * sizeof(mpz_t));
	xs = (mpz_t *) malloc(num_shares * sizeof(mpz_t));
	if (!coefficients || !xs) {
		return ENOMEM;
	}

	srand((unsigned int) time(NULL));
	gmp_randinit_default(rng_state);
	gmp_randseed_ui(rng_state, rand());
	prime_size = mpz_sizeinbase(prime, 2);

	/* Initialize coefficients and xs */
	gen_unique_random_list(threshold-1, prime_size-1,
		coefficients, rng_state);
	gen_unique_random_list(num_shares, prime_size-1,
		xs, rng_state);

	mpz_init(tmp);
	for (i = 0; i < num_shares; i++) {
		mpz_set(shares[i].x, xs[i]);
		mpz_init_set(y, secret);
		mpz_init_set_ui(degree, 1);
		for (j = 0; j < (threshold - 1); j++) {
			mpz_powm_sec(tmp, shares[i].x, degree, prime);
			mpz_addmul(y, coefficients[j], tmp);
			mpz_add_ui(degree, degree, 1);
		}
		mpz_clear(degree);
		mpz_mod(y, y, prime);
		mpz_set(shares[i].y, y);
		mpz_clear(y);
		if (mpz_cmp(shares[i].x, secret) == 0 ||
			mpz_cmp(shares[i].y, secret) == 0) {
			retval = EINVAL;
			break;
		}
	}
	mpz_clear(tmp);

	if (retval != EXIT_SUCCESS) {
		for (i = 0; i < num_shares; i++) {
			mpz_set_ui(shares[i].x, 0);
			mpz_set_ui(shares[i].y, 0);
		}
	}

	/* Clear data */
	gmp_randclear(rng_state);
	for (i = 0; i < num_shares; i++) {
		mpz_set_ui(xs[i], 0);
		mpz_clear(xs[i]);
	}
	for (i = 0; i < (threshold - 1); i++) {
		mpz_set_ui(coefficients[i], 0);
		mpz_clear(coefficients[i]);
	}
	free(coefficients); coefficients = NULL;
	free(xs); xs = NULL;

	return retval;
}


int reconstruct_secret(
		const unsigned int num_shares,
		const share_t * shares,
		const mpz_t prime,
		mpz_t secret)
{
	unsigned int j = 0, m = 0;
	int retval = -1;
	mpz_t product, d, r;
	mpz_t reconstructed;

	if (!shares) {
		return EINVAL;
	}

	/* Test the supplied prime */
	if (mpz_probab_prime_p(prime, 50) < 1) {
		return EINVAL;
	}

	for (j = 0; j < num_shares; j++) {
		if (mpz_cmp(shares[j].x, prime) >= 0 ||
			mpz_cmp(shares[j].y, prime) >= 0) {
			return EINVAL;
		}
	}

	mpz_init_set_ui(reconstructed, 0);

	for (j = 0; j < num_shares; j++) {
		mpz_init_set_ui(product, 1);
		for (m = 0; m < num_shares; m++) {
			mpz_init(d);
			mpz_init(r);
			if (m != j) {
				mpz_sub(d, shares[m].x, shares[j].x);
				retval = mpz_invert(d, d, prime);
				if (0 == retval) goto ERR;
				mpz_mul(r, shares[m].x, d);
				mpz_mul(product, product, r);
			}
			mpz_clear(d);
			mpz_clear(r);
		}
		mpz_addmul(reconstructed, shares[j].y, product);
		mpz_mod(reconstructed, reconstructed, prime);
		mpz_clear(product);
	}
	mpz_set(secret, reconstructed);
	mpz_set_ui(reconstructed, 0);
	mpz_clear(reconstructed);

	return EXIT_SUCCESS;
ERR:
	mpz_clear(d);
	mpz_clear(r);
	mpz_clear(product);
	mpz_clear(reconstructed);
	return EXIT_FAILURE;
}
