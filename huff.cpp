#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <unordered_map>
#include <string>
#include <vector>
#include <queue>

struct Node {
    unsigned char byte;  // The byte value (could be any byte of the file)
    int freq;            // Frequency of the byte
    Node *left, *right;  // Pointers to left and right child nodes

    // Constructor for creating a Node with a byte and frequency
    Node(unsigned char b, int f) : byte(b), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Function declarations.
void compress(const std::string& inputFile, const std::string& outputFile);
void decompress(const std::string& inputFile, const std::string& outputFile);


#endif
