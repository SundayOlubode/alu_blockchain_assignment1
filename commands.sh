gcc -o sha256 sha256.c -lssl -lcrypto
gcc -o blockchain_sim blockchain_sim.c -lssl -lcrypto
gcc -o block block.c -lssl -lcrypto
gcc -o blockchain blockchain.c -lssl -lcrypto
gcc -o blockchain_transactions blockchain_transactions.c -lssl -lcrypto
gcc -o blockchain_persistence blockchain_persistence.c -lssl -lcrypto