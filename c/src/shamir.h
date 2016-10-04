#ifndef SHAMIR_SECRET_SHARING_H_
#define SHAMIR_SECRET_SHARING_H_

#include <gmp.h>


/**
 * Represent a share with its X-Y coordinates
 */
typedef struct share {
	mpz_t x;
	mpz_t y;
} share_t;


/**
 * @brief Splits the given \a secret into \a num_shares shares
 *
 * This function implements the \a secret splitting mechanism defined in:
 * Adi Shamir, "How to share a secret", Communications of the ACM 22(11)
 * pages 612-613 doi:10.1145/359168.359176
 *
 * The secret is split into \a num_shares that are stored as pairs of
 * (x,y) in the given \a shares. All the numbers must be less than the
 * given \a prime
 *
 * @param secret The secret to be split
 * @param num_shares The number of generated shares
 * @param threshold The minimum number of shares needed to reconstruct secret
 * @param prime A prime number used to construct a Galois field
 * @param shares The list of the shares
 * @return EXIT_SUCCESS if successful. Otherwise, EINVAL or ENOMEM
 */
int split_secret(const mpz_t secret,
                 const unsigned int num_shares,
                 const unsigned int threshold,
                 const mpz_t prime,
                 share_t * shares);

/**
 * @brief Reconstruct a secret from the given shares
 *
 * This function reconstruct the \a secret from the given \a shares_xs
 * and \a shares_ys according to Shamir's algorithm.
 *
 * @param num_shares The number of shares
 * @param shares The list of the shares
 * @param prime A prime number used to construct a Galois field
 * @param secret The reconstructed secret
 */
int reconstruct_secret(const unsigned int num_shares,
                       const share_t * shares,
                       const mpz_t prime,
                       mpz_t secret);

#endif
