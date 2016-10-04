#include <stdio.h>
#include <stdlib.h>

#include "shamir.h"
#include "test.h"

#define N (7)

static char prime_str[] = "99841919439086972575966613294336707043187599755217949770531510008839389836622547768737989201532734716581829276354290861030105561174914071295723476589851843080570268095259917196351155871135120258740477256817507765273392027358601370895330743206712568620809026896843546323934476302562679731624877889014673367723";

int main()
{
	int retval = 0;
	unsigned int i  = 0;
	mpz_t secret, prime, reconstructed;
	share_t shares[N];

	mpz_init_set_str(secret, "1234", 10);
	mpz_init_set_str(prime, prime_str, 10);

	for (i = 0; i < N; i++) {
		mpz_init(shares[i].x);
		mpz_init(shares[i].y);
	}

	retval = split_secret(secret, N, 4, prime, shares);
	massert(retval == EXIT_SUCCESS);

	for (i = 1; i < N; i++) {
		mpz_init(reconstructed);
		retval = reconstruct_secret(i, (const share_t *) shares,
			prime, reconstructed);
		if (i >= 4) {
			massert(retval == EXIT_SUCCESS && \
				mpz_cmp(reconstructed, secret) == 0);
		} else {
			massert(retval == EXIT_SUCCESS && \
				mpz_cmp(reconstructed, secret) != 0);
		}
		mpz_clear(reconstructed);
	}

	for (i = 0; i < N; i++) {
		mpz_clear(shares[i].x);
		mpz_clear(shares[i].y);
	}
	mpz_clear(secret);
	mpz_clear(prime);
	return EXIT_SUCCESS;
}
