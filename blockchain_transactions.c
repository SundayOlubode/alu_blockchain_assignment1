// Question 2, Task 3: Simulating Transactions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_DATA_SIZE 256
#define HASH_SIZE 64
#define MAX_TRANSACTIONS 10
#define MAX_SENDER_SIZE 50
#define MAX_RECEIVER_SIZE 50
#define TRANS_STR_SIZE 150
#define INPUT_BUFFER_SIZE 1024

/* Structure Definitions */
typedef struct Transaction
{
        char sender[MAX_SENDER_SIZE];
        char receiver[MAX_RECEIVER_SIZE];
        double amount;
        time_t timestamp;
} Transaction;

typedef struct Block
{
        int index;
        time_t timestamp;
        char data[MAX_DATA_SIZE];
        Transaction transactions[MAX_TRANSACTIONS];
        int transaction_count;
        char previous_hash[HASH_SIZE + 1];
        char hash[HASH_SIZE + 1];
        struct Block *next;
} Block;

typedef struct Blockchain
{
        Block *head;
        int length;
} Blockchain;

/* Function Prototypes */
void calculateHash(Block *block, char *output);
Block *createBlock(int index, const char *data, const char *previous_hash);
void displayBlock(Block *block);
Blockchain *createBlockchain(void);
int addBlock(Blockchain *chain, const char *data);
int validateBlockchain(Blockchain *chain);
void displayBlockchain(Blockchain *chain);
void freeBlockchain(Blockchain *chain);
int addTransaction(Block *block, const char *sender, const char *receiver, double amount);
void displayTransactions(Block *block);

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
 * Calculates SHA-256 hash for a block including transaction data
 * @param block Block to be hashed
 * @param output Buffer to store the resulting hash
 */
void calculateHash(Block *block, char *output)
{
        char input[INPUT_BUFFER_SIZE];
        char trans_data[INPUT_BUFFER_SIZE / 2] = "";
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;

        // Create transaction string for hashing
        for (int i = 0; i < block->transaction_count; i++)
        {
                char trans_str[TRANS_STR_SIZE];
                snprintf(trans_str, TRANS_STR_SIZE, "%s%s%.2f",
                         block->transactions[i].sender,
                         block->transactions[i].receiver,
                         block->transactions[i].amount);
                if (strlen(trans_data) + strlen(trans_str) < sizeof(trans_data) - 1)
                {
                        strcat(trans_data, trans_str);
                }
        }

        // Combine all block data including transactions for hashing
        snprintf(input, sizeof(input), "%d%ld%s%s%s",
                 block->index, block->timestamp, block->data,
                 block->previous_hash, trans_data);

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
        block->transaction_count = 0;
        strncpy(block->data, data, MAX_DATA_SIZE - 1);
        block->data[MAX_DATA_SIZE - 1] = '\0';
        strncpy(block->previous_hash, previous_hash, HASH_SIZE);
        block->previous_hash[HASH_SIZE] = '\0';
        block->next = NULL;

        calculateHash(block, block->hash);
        return block;
}

/**
 * Adds a new block to the blockchain
 * @param chain Pointer to the blockchain
 * @param data Data for the new block
 * @return 1 if successful, 0 if failed
 */
int addBlock(Blockchain *chain, const char *data)
{
        if (!chain)
                return 0;

        if (!chain->head)
        {
                chain->head = createBlock(0, data, "0");
                if (!chain->head)
                        return 0;
                chain->length = 1;
                return 1;
        }

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
 * Adds a new transaction to a block
 * @param block Target block
 * @param sender Transaction sender
 * @param receiver Transaction receiver
 * @param amount Transaction amount
 * @return 1 if successful, 0 if failed
 */
int addTransaction(Block *block, const char *sender, const char *receiver, double amount)
{
        if (!block || block->transaction_count >= MAX_TRANSACTIONS)
                return 0;

        Transaction *trans = &block->transactions[block->transaction_count];
        strncpy(trans->sender, sender, MAX_SENDER_SIZE - 1);
        trans->sender[MAX_SENDER_SIZE - 1] = '\0';

        strncpy(trans->receiver, receiver, MAX_RECEIVER_SIZE - 1);
        trans->receiver[MAX_RECEIVER_SIZE - 1] = '\0';

        trans->amount = amount;
        trans->timestamp = time(NULL);

        block->transaction_count++;

        calculateHash(block, block->hash);
        return 1;
}

/**
 * Displays transactions in a block
 * @param block Block containing transactions
 */
void displayTransactions(Block *block)
{
        if (block->transaction_count == 0)
        {
                printf("No transactions in this block\n");
                return;
        }

        printf("\nTransactions:\n");
        for (int i = 0; i < block->transaction_count; i++)
        {
                printf("Transaction #%d:\n", i + 1);
                printf("  From: %s\n", block->transactions[i].sender);
                printf("  To: %s\n", block->transactions[i].receiver);
                printf("  Amount: %.2f\n", block->transactions[i].amount);
                printf("  Time: %s", ctime(&block->transactions[i].timestamp));
        }
}

/**
 * Displays information of a single block including transactions
 * @param block Block to display
 */
void displayBlock(Block *block)
{
        printf("\nBlock #%d\n", block->index);
        printf("Timestamp: %s", ctime(&block->timestamp));
        printf("Data: %s\n", block->data);
        printf("Previous Hash: %s\n", block->previous_hash);
        printf("Hash: %s\n", block->hash);
        displayTransactions(block);
}

/**
 * Displays the entire blockchain
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

/**
 * Safely gets string input from user
 * @param prompt The prompt to show user
 * @param buffer Buffer to store input
 * @param size Size of buffer
 */
void getStringInput(const char *prompt, char *buffer, size_t size)
{
        printf("%s", prompt);
        if (fgets(buffer, size, stdin))
        {
                buffer[strcspn(buffer, "\n")] = 0;
        }
}

/**
 * Safely gets double input from user
 * @param prompt The prompt to show user
 * @return The double value entered
 */
double getDoubleInput(const char *prompt)
{
        char buffer[64];
        double value;

        while (1)
        {
                printf("%s", prompt);
                if (fgets(buffer, sizeof(buffer), stdin))
                {
                        if (sscanf(buffer, "%lf", &value) == 1)
                        {
                                return value;
                        }
                }
                printf("Invalid input. Please enter a valid number.\n");
        }
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
        char sender[MAX_SENDER_SIZE];
        char receiver[MAX_RECEIVER_SIZE];
        double amount;
        int choice;

        do
        {
                printf("\nBlockchain Menu:\n");
                printf("1. Add new block\n");
                printf("2. Add transaction to latest block\n");
                printf("3. Display blockchain\n");
                printf("4. Validate blockchain\n");
                printf("5. Exit\n");
                printf("Enter choice: ");

                char choice_str[10];
                if (fgets(choice_str, sizeof(choice_str), stdin))
                {
                        choice = atoi(choice_str);
                }
                else
                {
                        choice = 0; // Invalid input
                }

                switch (choice)
                {
                case 1:
                        getStringInput("Enter data for new block: ", input, MAX_DATA_SIZE);
                        if (addBlock(chain, input))
                                printf("Block added successfully!\n");
                        else
                                printf("Failed to add block!\n");
                        break;

                case 2:
                        if (!chain->head)
                        {
                                printf("Create a block first!\n");
                                break;
                        }

                        Block *latest = chain->head;
                        while (latest->next)
                                latest = latest->next;

                        getStringInput("Enter sender: ", sender, MAX_SENDER_SIZE);
                        getStringInput("Enter receiver: ", receiver, MAX_RECEIVER_SIZE);
                        amount = getDoubleInput("Enter amount: ");

                        if (addTransaction(latest, sender, receiver, amount))
                                printf("Transaction added successfully!\n");
                        else
                                printf("Failed to add transaction!\n");
                        break;

                case 3:
                        displayBlockchain(chain);
                        break;

                case 4:
                        if (validateBlockchain(chain))
                                printf("Blockchain is valid!\n");
                        else
                                printf("Blockchain is invalid!\n");
                        break;

                case 5:
                        printf("Exiting...\n");
                        break;

                default:
                        printf("Invalid choice! Please enter a number between 1 and 5.\n");
                }
        } while (choice != 5);

        freeBlockchain(chain);
        return 0;
}