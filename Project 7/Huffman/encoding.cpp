// TODO: remove and replace this file header comment
// This is the CPP file you will edit and turn in.
// Remove starter comments and add your own
// comments on each function and on complex code sections.

#include "encoding.h"
#include "priorityqueue.h"
#include "queue.h"
#include "filelib.h"

void createEncodingMap(HuffmanNode* node, Map<char, string>& res, string bits);
void convertTreeIntoString(HuffmanNode* node, string& res);
void convertStringIntoTree(Queue<string>& token, HuffmanNode*& res);

Map<char, int> buildFrequencyTable(istream& input)
{
    // This funciton takes in an input istream and return a map
    // associating each character with its frequency
    Map<char, int> freqTable;
    char ch;
    while (input.get(ch)) {
        if (!freqTable.containsKey(ch)) {
            freqTable[ch] = 1;
        } else {
            freqTable[ch]++;
        }
    }
    return freqTable;
}

HuffmanNode* buildEncodingTree(Map<char, int>& freqTable)
{
    // This function takes in a frequency map and returns a
    // Huffman encoding tree
    // First, take all characters in the freqTable and convert them into Huffman nodes,
    // and put them into a priority queue
    Vector<char> charVector = freqTable.keys();
    Vector<int> countVector = freqTable.values();

    PriorityQueue<HuffmanNode*> pq;
    for (int i=0; i<charVector.size(); i++) {
        HuffmanNode* node = new HuffmanNode(charVector[i]);
        pq.enqueue(node, countVector[i]);
    }

    // Next, every time we take out 2 elements from the front priority queue
    // and create a parent node whose priority is equal to the sum of the priority of the 2 selected nodes.
    // Then we put them back into the priority queue.
    // Do this iteratively to build the Huffman encoding tree
    while (pq.size() > 1) {
        double priority1 = pq.peekPriority();
        HuffmanNode* node1 = pq.dequeue();
        double priority2 = pq.peekPriority();
        HuffmanNode* node2 = pq.dequeue();
        HuffmanNode* mergedNode = new HuffmanNode(node1, node2);
        double mergedNodePriority = priority1 + priority2;
        pq.enqueue(mergedNode, mergedNodePriority);
    }

    // When there is only 1 merged Huffman node left in the priority queue
    // This is the Huffman encoding tree we want.
    HuffmanNode* encondingTree = pq.dequeue();

    return encondingTree;
}

string flattenTreeToHeader(HuffmanNode* t)
{
    // This function takes in a Huffman encoding tree and
    // transform it into a string which is used to recreate the tree later in the program
    HuffmanNode* root = t;
    string flattenedTree = "";
    convertTreeIntoString(root, flattenedTree);

    return flattenedTree;
}

HuffmanNode* recreateTreeFromHeader(string str)
{
    // This function takes in a encoding-tree-transformed string and
    // recreate it into the original Huffman encoding tree
    // First, find all leaf nodes from the given string
    HuffmanNode* root = nullptr;
    Queue<string> token;
    int i=0;
    while (i<str.size()) {
        if (str[i] == '.') {
            string leaf = str.substr(i, 2);
            token.enqueue(leaf);
            i += 2;
        } else {
            string interior = str.substr(i, 1);
            token.enqueue(interior);
            i++;
        }
    }
    convertStringIntoTree(token, root);

    return root;
}

Map<char, string> buildEncodingMap(HuffmanNode* encodingTree)
{
    // This function takes in a built encoding tree and
    // turn it into a map associating each character with a series of '0's and '1's
    HuffmanNode* root = encodingTree;
    Map<char, string> encodingMap;
    createEncodingMap(root, encodingMap, "");

    return encodingMap;
}

void freeTree(HuffmanNode* t)
{
    // This function is used to clean up the memory for an encoding tree
    // that is no longer needed
    delete t;
}

void compress(istream& input, HuffmanOutputFile& output)
{
    // This function takes an input stream, compress it and
    // put the compressed content into the output file
    Map<char, int> frequencyTable = buildFrequencyTable(input);
    HuffmanNode* encodingTree = buildEncodingTree(frequencyTable);
    Map<char, string> encodingMap = buildEncodingMap(encodingTree);
    string flattenedTree = flattenTreeToHeader(encodingTree);

    // Write the flattened tree header into the output file
    output.writeHeader(flattenedTree);
    // Rewind back to the start of the input file and write bits to output
    // according to each character's assigned code in the map
    rewindStream(input);
    char ch;
    while (input.get(ch)) {
        string bits = encodingMap[ch];
        for (int i=0; i<bits.size(); i++) {
            output.writeBit(bits[i] - '0');
        }
    }
}

void decompress(HuffmanInputFile& input, ostream& output)
{
    // This function reconstructs the original file from the compressed version
    string header = input.readHeader();
    HuffmanNode* encodingTree = recreateTreeFromHeader(header);

    HuffmanNode* current = encodingTree;
    while (true) {
        int bit = input.readBit();
        if (bit == -1) {
            break;
        } else if (bit == 0) {
            current = current->zero;
        } else {
            current = current->one;
        }
        if (current->isLeaf()) {
            output.put(current->ch);
            current = encodingTree;
        }
    }
}

//************************************Below are helper functions*********************************
void createEncodingMap(HuffmanNode* node, Map<char, string>& res, string bits) {
    // Traverse the tree and store all characters along with their
    // bit representations into the map
    if (node->zero->isLeaf() && node->one->isLeaf()) {
        bits += '0';
        res[node->zero->ch] = bits;
        bits[bits.size()-1] = '1';
        res[node->one->ch] = bits;
    } else if (node->zero->isLeaf()) {
        bits += '0';
        res[node->zero->ch] = bits;
        bits[bits.size()-1] = '1';
        createEncodingMap(node->one, res, bits);
    } else if (node->one->isLeaf()) {
        bits += '1';
        res[node->one->ch] = bits;
        bits[bits.size()-1] = '0';
        createEncodingMap(node->zero, res, bits);
    } else {
        bits += '0';
        createEncodingMap(node->zero, res, bits);

        bits[bits.size()-1] = '1';
        createEncodingMap(node->one, res, bits);
    }
}

void convertTreeIntoString(HuffmanNode* node, string& res) {
    // Traverse the tree and convert all leaves and interior nodes into a string
    if (node->zero->isLeaf() && node->one->isLeaf()) {
        res += '(';
        res += '.';
        res += node->zero->ch;
        res += '.';
        res += node->one->ch;
        res += ')';
    } else if (node->zero->isLeaf()) {
        res += '(';
        res += '.';
        res += node->zero->ch;
        convertTreeIntoString(node->one, res);
        res += ')';
    } else if (node->one->isLeaf()) {
        res += '(';
        convertTreeIntoString(node->zero, res);
        res += '.';
        res += node->one->ch;
        res += ')';
    } else {
        res += '(';
        convertTreeIntoString(node->zero, res);
        convertTreeIntoString(node->one, res);
        res += ')';

    }
}

void convertStringIntoTree(Queue<string>& token, HuffmanNode*& res) {
    // Take in the tokenized string and
    // generate the original encoding tree and put it into res
    if (!token.isEmpty()) {
        if (token.peek()[0] == '.') {
            res = new HuffmanNode(token.dequeue()[1]);
        } else {
            token.dequeue();
            res = new HuffmanNode(nullptr, nullptr);
            convertStringIntoTree(token, res->zero);
            convertStringIntoTree(token, res->one);
            token.dequeue();
        }
    }
}
