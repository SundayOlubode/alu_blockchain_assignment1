// Question 2, Task 1: Block Structure

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_DATA_SIZE 256
#define HASH_SIZE 64

typedef struct Block
{
        int index;
        time_t timestamp;
        char data[MAX_DATA_SIZE];
        char previous_hash[HASH_SIZE + 1];
        char hash[HASH_SIZE + 1];
} Block;

void calculateBlockHash(Block *block, char *output);
Block *createBlock(int index, const char *data, const char *previous_hash);
void displayBlock(Block *block);

/**
 * Calculates the SHA-256 hash for a block
 * @param block Block to be hashed
 * @param output Buffer to store the resulting hash
 */
void calculateBlockHash(Block *block, char *output)
{
        char input[MAX_DATA_SIZE * 2];
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;

        // Combine all block data for hashing
        snprintf(input, sizeof(input), "%d%ld%s%s",
                 block->index, block->timestamp, block->data, block->previous_hash);

        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input, strlen(input));
        SHA256_Final(hash, &sha256);

        // Convert to hexadecimal string
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
                sprintf(output + (i * 2), "%02x", hash[i]);
        }
        output[HASH_SIZE] = '\0';
}

/**
 * Creates a new block with the given parameters
 * @param index Block index
 * @param data Block data
 * @param previous_hash Hash of the previous block
 * @return Pointer to the new block or NULL if creation fails
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

        calculateBlockHash(block, block->hash);

        return block;
}

/**
 * Displays the information of a block
 * @param block Block to display
 */
void displayBlock(Block *block)
{
        printf("\nBlock Information:\n");
        printf("----------------\n");
        printf("Index: %d\n", block->index);
        printf("Timestamp: %s", ctime(&block->timestamp));
        printf("Data: %s\n", block->data);
        printf("Previous Hash: %s\n", block->previous_hash);
        printf("Block Hash: %s\n", block->hash);
}

int main()
{
        char input_data[MAX_DATA_SIZE];

        printf("Enter data for this genesis block: ");

        if (fgets(input_data, MAX_DATA_SIZE, stdin))
        {
                input_data[strcspn(input_data, "\n")] = 0;

                Block *genesis_block = createBlock(0, input_data, "0");

                if (genesis_block)
                {
                        printf("\nSuccessfully created genesis block!");
                        displayBlock(genesis_block);

                        // Clean up
                        free(genesis_block);
                }
                else
                {
                        printf("Failed to create block!\n");
                        return 1;
                }
        }

        return 0;
}