// Question 2, Task 2: Creating a Blockchain

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
        struct Block *next;
} Block;

typedef struct Blockchain
{
        Block *head;
        int length;
} Blockchain;

void calculateHash(Block *block, char *output);
Block *createBlock(int index, const char *data, const char *previous_hash);
void displayBlock(Block *block);
Blockchain *createBlockchain(void);
int addBlock(Blockchain *chain, const char *data);
int validateBlockchain(Blockchain *chain);
void displayBlockchain(Blockchain *chain);
void freeBlockchain(Blockchain *chain);

/**
 * Calculates SHA-256 hash for a block
 * @param block Block to be hashed
 * @param output Buffer to store the resulting hash
 */
void calculateHash(Block *block, char *output)
{
        char input[MAX_DATA_SIZE * 2];
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;

        snprintf(input, sizeof(input), "%d%ld%s%s",
                 block->index, block->timestamp, block->data, block->previous_hash);

        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input, strlen(input));
        SHA256_Final(hash, &sha256);

        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
                sprintf(output + (i * 2), "%02x", hash[i]);
        }
        output[HASH_SIZE] = '\0';
}

/**
 * Creates a new block
 * @param index Block index
 * @param data Block data
 * @param previous_hash Hash of previous block
 * @return Pointer to new block or NULL if creation fails
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
 * Creates a new blockchain
 * @return Pointer to new blockchain or NULL if creation fails
 */
Blockchain *createBlockchain(void)
{
        Blockchain *chain = (Blockchain *)malloc(sizeof(Blockchain));
        if (chain)
        {
                chain->head = NULL;
                chain->length = 0;
        }
        return chain;
}

/**
 * Adds a new block to the blockchain
 * @param chain Pointer to the blockchain
 * @param data Data to be stored in the new block
 * @return 1 if successful, 0 if failed
 */
int addBlock(Blockchain *chain, const char *data)
{
        if (!chain)
                return 0;

        // Create genesis block if chain is empty
        if (!chain->head)
        {
                chain->head = createBlock(0, data, "0");
                if (!chain->head)
                        return 0;
                chain->length = 1;
                return 1;
        }

        // Find last block
        Block *current = chain->head;
        while (current->next)
        {
                current = current->next;
        }

        Block *newBlock = createBlock(chain->length, data, current->hash);
        if (!newBlock)
                return 0;

        current->next = newBlock;
        chain->length++;

        return 1;
}

/**
 * Validates the integrity of the blockchain
 * @param chain Pointer to the blockchain
 * @return 1 if valid, 0 if invalid
 */
int validateBlockchain(Blockchain *chain)
{
        if (!chain || !chain->head)
                return 1;

        Block *current = chain->head->next;
        Block *previous = chain->head;
        char calculated_hash[HASH_SIZE + 1];

        while (current)
        {
                calculateHash(previous, calculated_hash);
                if (strcmp(current->previous_hash, calculated_hash) != 0)
                {
                        return 0;
                }

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
 * Displays information of a single block
 * @param block Block to display
 */
void displayBlock(Block *block)
{
        printf("\nBlock #%d\n", block->index);
        printf("Timestamp: %s", ctime(&block->timestamp));
        printf("Data: %s\n", block->data);
        printf("Previous Hash: %s\n", block->previous_hash);
        printf("Hash: %s\n", block->hash);
}

/**
 * Displays all blocks in the blockchain
 * @param chain Pointer to the blockchain
 */
void displayBlockchain(Blockchain *chain)
{
        if (!chain || !chain->head)
        {
                printf("Blockchain is empty\n");
                return;
        }

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
        if (!chain)
                return;

        Block *current = chain->head;
        while (current)
        {
                Block *temp = current;
                current = current->next;
                free(temp);
        }
        free(chain);
}

int main()
{
        Blockchain *chain = createBlockchain();
        if (!chain)
        {
                printf("Failed to create blockchain!\n");
                return 1;
        }

        printf("Creating the genesis block...\n");
        sleep(1);

        if (!addBlock(chain, "Genesis Block"))
        {
                printf("Failed to create genesis block!\n");
                return 1;
        }

        printf("Genesis block created successfully!\n");

        char input[MAX_DATA_SIZE];
        int choice;

        do
        {
                printf("\nBlockchain Menu:\n");
                printf("1. Add new block\n");
                printf("2. Display blockchain\n");
                printf("3. Validate blockchain\n");
                printf("4. Exit\n");
                printf("Enter choice: ");
                scanf("%d", &choice);
                getchar();

                switch (choice)
                {
                case 1:
                        printf("Enter data for new block: ");
                        fgets(input, MAX_DATA_SIZE, stdin);
                        input[strcspn(input, "\n")] = 0;

                        if (addBlock(chain, input))
                                printf("Block added successfully!\n");
                        else
                                printf("Failed to add block!\n");
                        break;

                case 2:
                        displayBlockchain(chain);
                        break;

                case 3:
                        if (validateBlockchain(chain))
                                printf("Blockchain is valid!\n");
                        else
                                printf("Blockchain is invalid!\n");
                        break;

                case 4:
                        printf("Exiting...\n");
                        break;

                default:
                        printf("Invalid choice!\n");
                }
        } while (choice != 4);

        freeBlockchain(chain);
        return 0;
}