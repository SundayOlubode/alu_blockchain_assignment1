// Question 1, Task 1: Implement the computeSHA256 function

/*
 * SHA-256 (Secure Hash Algorithm 256-bit) Process:
 *
 * 1. Preprocessing:
 *    - Pad the input message so its length is exactly 64 bytes less than a multiple of 64
 *    - Append the original message length as a 64-bit number
 *
 * 2. Initialize Hash Values (h0 through h7):
 *    - First 32 bits of the fractional parts of the square roots of the first 8 primes (2 to 19)
 *    - These become the initial hash state
 *
 * 3. Process each 512-bit (64-byte) chunk:
 *    a) Create message schedule (w[0..63]):
 *       - First 16 words are formed directly from the chunk
 *       - Remaining 48 words are generated using:
 *         w[i] = w[i-16] + s0 + w[i-7] + s1
 *         where s0 and s1 are specialized rotation functions
 *
 *    b) Initialize working variables (a through h) with current hash values
 *
 *    c) Main compression loop (64 rounds):
 *       - Each round updates these variables using:
 *         - Majority function: maj(a,b,c) = (a AND b) XOR (a AND c) XOR (b AND c)
 *         - Choice function: ch(e,f,g) = (e AND f) XOR (NOT e AND g)
 *         - Two sigma rotations (Σ0 and Σ1)
 *         - Round constants (first 32 bits of cube roots of first 64 primes)
 *
 *    d) Add compressed chunk to current hash value
 *
 * 4. Produce final hash:
 *    - Concatenate the final values of h0 through h7
 *    - Result is a 256-bit (32-byte) hash value
 *
 * Security Features:
 * - One-way function: Computationally infeasible to reverse
 * - Avalanche effect: Small input changes cause large hash changes
 * - Collision resistance: Extremely unlikely to find two inputs with same hash
 */

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

#define MAX_INPUT_LENGTH 1024
#define SHA256_DIGEST_LENGTH 32

/*
 * Function to compute SHA-256 hash of input string
 *@param input: The input string
 *@param hash: The output hash
 */
void computeSHA256(const char *input, unsigned char hash[SHA256_DIGEST_LENGTH])
{
	// Initialize SHA-256 context
	SHA256_CTX sha256Context;
	SHA256_Init(&sha256Context);

	// Process the input string
	SHA256_Update(&sha256Context, input, strlen(input));

	// Generate final hash
	SHA256_Final(hash, &sha256Context);
}

/*
 * Converts hash to string
 *@param hash: The hash to convert
 *@param outputString: The output string
 */
void hashToString(const unsigned char hash[SHA256_DIGEST_LENGTH], char outputString[65])
{
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputString + (i * 2), "%02x", hash[i]);
	}
	outputString[64] = '\0';
}

int main()
{
	char input[MAX_INPUT_LENGTH];
	unsigned char hash[SHA256_DIGEST_LENGTH];
	char hashString[65];

	// Get input from user
	printf("Enter text to hash (max %d characters):\n", MAX_INPUT_LENGTH - 1);
	if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL)
	{
		printf("Error reading input\n");
		return 1;
	}

	// Remove trailing newline
	size_t len = strlen(input);
	if (len > 0 && input[len - 1] == '\n')
	{
		input[len - 1] = '\0';
	}

	// Test with "Blockchain Cryptography"
	printf("\nTesting with \"Blockchain Cryptography\":\n");
	computeSHA256("Blockchain Cryptography", hash);
	hashToString(hash, hashString);
	printf("SHA-256 Digest: %s\n", hashString);

	// Process user input
	printf("\nProcessing your input: \"%s\"\n", input);
	computeSHA256(input, hash);
	hashToString(hash, hashString);
	printf("Input SHA-256 Digest: %s\n", hashString);

	return 0;
}