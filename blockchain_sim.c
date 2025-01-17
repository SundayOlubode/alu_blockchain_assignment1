// Question 1, Task 2: Blockchain Simulation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_DATA_SIZE 256
#define HASH_SIZE 64

/**
 * Structure representing a block in the blockchain
 */
typedef struct Block
{
	int index;
	time_t timestamp;
	char data[MAX_DATA_SIZE];
	char previous_hash[HASH_SIZE + 1];
	char hash[HASH_SIZE + 1];
	struct Block *next;
} Block;

/**
 * Structure representing the blockchain
 */
typedef struct
{
	Block *head;
	int length;
} Blockchain;

/**
 * Calculates SHA-256 hash for a block
 * @param block Pointer to the block to hash
 * @param output Buffer to store the resulting hash string
 */
void calculateHash(Block *block, char *output)
{
	char input[MAX_DATA_SIZE * 2];
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;

	// Combine block data for hashing
	snprintf(input, sizeof(input), "%d%ld%s%s",
		 block->index, block->timestamp, block->data, block->previous_hash);

	// Calculate hash
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, input, strlen(input));
	SHA256_Final(hash, &sha256);

	// Convert to hex string
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(output + (i * 2), "%02x", hash[i]);
	}
	output[HASH_SIZE] = '\0';
}

/**
 * Creates a new block with given parameters
 * @param index Block index
 * @param data Block data
 * @param previous_hash Hash of the previous block
 * @return Pointer to the new block
 */
Block *createBlock(int index, const char *data, const char *previous_hash)
{
	Block *block = (Block *)malloc(sizeof(Block));
	if (!block)
		return NULL;

	block->index = index;
	block->timestamp = time(NULL);
	strncpy(block->data, data, MAX_DATA_SIZE - 1);
	block->data[MAX_DATA_SIZE - 1] = '\0';
	strncpy(block->previous_hash, previous_hash, HASH_SIZE);
	block->previous_hash[HASH_SIZE] = '\0';
	block->next = NULL;

	calculateHash(block, block->hash);
	return block;
}

/**
 * Validates the entire blockchain
 * @param chain Pointer to the blockchain
 * @return 1 if valid, 0 if invalid
 */
int validateChain(Blockchain *chain)
{
	if (!chain->head)
		return 1; // Empty chain is valid

	Block *current = chain->head->next;
	Block *previous = chain->head;
	char calculated_hash[HASH_SIZE + 1];

	while (current)
	{
		// Verify previous hash link
		calculateHash(previous, calculated_hash);
		if (strcmp(current->previous_hash, calculated_hash) != 0)
		{
			return 0;
		}

		// Verify current block's hash
		calculateHash(current, calculated_hash);
		if (strcmp(current->hash, calculated_hash) != 0)
		{
			return 0;
		}

		previous = current;
		current = current->next;
	}

	return 1;
}

/**
 * Adds a new block to the blockchain
 * @param chain Pointer to the blockchain
 * @param data Data to store in the new block
 * @return 1 if successful, 0 if failed
 */
int addBlock(Blockchain *chain, const char *data)
{
	if (!chain)
		return 0;

	if (!chain->head)
	{
		// Create genesis block
		chain->head = createBlock(0, data, "0");
		chain->length = 1;
		return chain->head != NULL;
	}

	Block *current = chain->head;
	while (current->next)
	{
		current = current->next;
	}

	Block *new_block = createBlock(current->index + 1, data, current->hash);
	if (!new_block)
		return 0;

	current->next = new_block;
	chain->length++;

	// Validate chain after adding new block
	return validateChain(chain);
}

/**
 * Displays the contents of a block
 * @param block Pointer to the block to display
 */
void displayBlock(Block *block)
{
	printf("\nBlock #%d\n", block->index);
	printf("Timestamp: %ld\n", block->timestamp);
	printf("Data: %s\n", block->data);
	printf("Previous Hash: %s\n", block->previous_hash);
	printf("Hash: %s\n", block->hash);
}

/**
 * Displays the entire blockchain
 * @param chain Pointer to the blockchain
 */
void displayBlockchain(Blockchain *chain)
{
	Block *current = chain->head;
	while (current)
	{
		displayBlock(current);
		current = current->next;
	}
}

/**
 * Frees all memory allocated for the blockchain
 * @param chain Pointer to the blockchain
 */
void freeBlockchain(Blockchain *chain)
{
	Block *current = chain->head;
	while (current)
	{
		Block *temp = current;
		current = current->next;
		free(temp);
	}
	chain->head = NULL;
	chain->length = 0;
}

int main()
{
	Blockchain chain = {NULL, 0};
	char data[MAX_DATA_SIZE];

	printf("Simple Blockchain Simulation\n\n");

	// Create Genesis block
	printf("Creating Genesis Block...\n");
	if (!addBlock(&chain, "Genesis Block"))
	{
		printf("Failed to create genesis block!\n");
		return 1;
	}
	sleep(1);
	printf("Genesis Block created successfully!\n");

	// Add blocks with user input
	printf("\nKindly add additional 3 new blocks!\n\n");
	for (int i = 1; i < 4; i++)
	{
		printf("Enter data for block %d: ", i);
		if (fgets(data, MAX_DATA_SIZE, stdin))
		{
			// Remove newline
			data[strcspn(data, "\n")] = 0;

			if (!addBlock(&chain, data))
			{
				printf("Failed to add block!\n");
				break;
			}

			printf("Block added successfully!\n");
			printf("Chain validation: %s\n",
			       validateChain(&chain) ? "VALID" : "INVALID");
		}
	}

	// Display the entire blockchain
	printf("\nFinal Blockchain State:");
	displayBlockchain(&chain);

	// Cleanup
	freeBlockchain(&chain);
	return 0;
}