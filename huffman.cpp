#include "huff.cpp"
#include <fstream>
#include <bitset>
#include <iostream>
using namespace std;

// Build frequency map
// Iterates through all bytes in the input and populates a frequency map.
void buildFrequencyMap(const vector<unsigned char>& data, unordered_map<unsigned char, int>& freqMap) {
    for(unsigned char byte : data) freqMap[byte]++;
}

// Build Huffman Tree
//ues a priority queue (min-heap) to build a tree where more frequent characters are nearer the root.
//A priority_queue is like a heap — it always keeps the "highest priority" item at the top.
// In Huffman coding, you need a min-heap to always extract the node with the smallest frequency.

Node* buildHuffmanTree(const unordered_map<unsigned char, int>& freqMap)
{
    priority_queue<Node*, vector<Node*>, Compare> pq;
    //Create a priority queue that holds Node*, uses a vector internally,
    //and prioritizes nodes based on frequency in ascending order (i.e., a min-heap)."

    for (auto& pair : freqMap)
        pq.push(new Node(pair.first, pair.second));

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* merged = new Node('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        pq.push(merged);
    }

    return pq.top(); //returning root of huffman tree.
}

//Build Huffman codes from tree
//Recursively generates Huffman binary codes for each character.
//Traverses the tree using pre-order traversal to build binary strings.
void buildCodes(Node* root, const string& str, unordered_map<unsigned char, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right)
        huffmanCode[root->byte] = str;
    buildCodes(root->left, str + "0", huffmanCode);
    buildCodes(root->right, str + "1", huffmanCode);
}

// Encode data using Huffman codes
//Converts the data into a long binary string using the Huffman map.
string encodeData(const vector<unsigned char>& data, unordered_map<unsigned char, string>& huffmanCode) {
    string encoded = "";
    for (unsigned char byte : data)
        encoded += huffmanCode[byte];
    return encoded;
}

// Decode binary Huffman string
// Traverses the tree based on bits ('0' or '1') to get the original bytes.
//encode = binary.
vector<unsigned char> decodeData(const string& binary, Node* root) {
    vector<unsigned char> output;
    Node* current = root;
    for (char bit : binary) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            output.push_back(current->byte);
            current = root;
        }
    }
    return output;
}

// This function writes a binary string (like "010101...") to a binary file, grouping it into bytes.
// It’s part of the Huffman compression process, where you’ve already encoded your data into a long string of 0s and 1s.
// Convert a string of binary digits into actual binary bytes and save them in a .bin (or similar) file.

/*
This function does the following:
Writes the length of the binary string.
Converts every 8 bits from a binary string into a byte.
Writes those bytes to a file, padding the last group if necessary.
This makes sure your Huffman encoded data is stored efficiently and can be read back for decompression.
*/

void writeBinaryFile(const string& binary, const string& filename)
{
    ofstream out(filename, ios::binary);
    size_t size = binary.size();
    out.write(reinterpret_cast<char*>(&size), sizeof(size));
    // reinterpret_cast<char*> is used to write the binary representation of size_t.

    for (size_t i = 0; i < binary.size(); i += 8)
    {
        // Every 8 characters (bits) will be turned into 1 byte.
        string byteStr = binary.substr(i, 8);
        // Get the next 8 bits from the binary string.
        if (byteStr.size() < 8)
            byteStr.append(8 - byteStr.size(), '0');
            // In case the last group has fewer than 8 bits (like 5 or 3), we add extra '0' bits to complete the byte.
        bitset<8> bits(byteStr);
        // bitset<8> converts the 8-bit string into an actual 8-bit binary number.
        out.put(static_cast<char>(bits.to_ulong()));
        // to_ulong() turns that into an unsigned long.
        // static_cast<char>() converts it to a char, which represents a byte, and it's written to the file.
    }
    out.close();
}

// Read Huffman-encoded binary file
// Reads bytes from file, converts them back into a string of bits, then truncates based on the original size stored.

/*
Open the file.
Read the size of the original binary bit string.
Read the rest of the file byte-by-byte.

Convert each byte back to its 8-bit binary string form.
Trim any padded bits at the end based on the original size.
*/

string readBinaryFile(const string& filename) {
    ifstream in(filename, ios::binary);
    size_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    string binary;
    char byte;
    while (in.get(byte))
    {
        bitset<8> bits(byte);
        binary += bits.to_string();
    }
    binary = binary.substr(0, size);
    // The last byte might have extra padding (from writeBinaryFile), so we use the original bit size (size) to trim the extra '0's.
    in.close();
    return binary;
}

//Save Huffman codebook.
//Save and load Huffman code mappings to/from a text file.
//Used for storing the mapping so decompression knows how to interpret the bits.
void saveCodebook(const unordered_map<unsigned char, string>& codebook) {
    ofstream out("codebook.txt");
    for (auto& pair : codebook)
        out << (int)pair.first << " " << pair.second << "\n";
        // (int)pair.first converts the character to its ASCII integer value.
    out.close();
}

//Load Huffman codes into codebook.
unordered_map<unsigned char, string> loadCodebook() {
    unordered_map<unsigned char, string> codebook;
    ifstream in("codebook.txt");
    int byte;
    string code;
    while (in >> byte >> code)
        codebook[(unsigned char)byte] = code;
    in.close();
    return codebook;
}

// Rebuild Huffman tree from codebook
//Reconstruct the Huffman tree using the saved codebook.
//so that whenever we want to decompress the compressed file, we can.
Node* rebuildTree(const unordered_map<unsigned char, string>& codebook) {
    Node* root = new Node('\0', 0);
    for (auto& pair : codebook)
    {
        Node* current = root;
        for (char bit : pair.second) {
            if (bit == '0') {
                if (!current->left) current->left = new Node('\0', 0);
                current = current->left;
            }
            else{
                if (!current->right) current->right = new Node('\0', 0);
                current = current->right;
            }
        }
        current->byte = pair.first;
    }
    return root;
}

// Regular Huffman compression
// Combines reading the input, building the tree and codebook, encoding the data, writing the file, and saving codebook.
// Also calculates and prints compression ratio.

void compress(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile, ios::binary);
    vector<unsigned char> data((istreambuf_iterator<char>(in)), {});
    in.close();

    size_t originalSize = data.size();
    unordered_map<unsigned char, int> freqMap;
    buildFrequencyMap(data, freqMap);
    Node* root = buildHuffmanTree(freqMap);
    unordered_map<unsigned char, string> codebook;
    buildCodes(root, "", codebook);

    string encoded = encodeData(data, codebook);
    writeBinaryFile(encoded, outputFile);
    saveCodebook(codebook);

    ifstream comp(outputFile, ios::binary | ios::ate);
    size_t compressedSize = comp.tellg();
    comp.close();

    double compressionRatio = (1.0 - ((double)compressedSize / originalSize)) * 100.0;
    cout << "Original Size      : " << originalSize << " bytes\n";
    cout << "Compressed Size    : " << compressedSize << " bytes\n";
    cout << "Compression Ratio  : " << compressionRatio << "%\n";
}

// Regular Huffman decompression
// Loads codebook, rebuilds tree, reads binary file, decodes data, and writes original content to output.
void decompress(const string& inputFile, const string& outputFile) {
    unordered_map<unsigned char, string> codebook = loadCodebook();
    Node* root = rebuildTree(codebook);
    string binary = readBinaryFile(inputFile);
    vector<unsigned char> decoded = decodeData(binary, root);

    ofstream out(outputFile, ios::binary);
    for (unsigned char byte : decoded)
        out.put(byte);
    out.close();
}
