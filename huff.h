// header file for Huffman coder

#ifndef HUFF_H
#define HUFF_H

#define NUM_CHARS 256

// node in a Huffman tree is either a compound char (internal node)
// or a simple char (leaf)
typedef struct huffchar
{
  int freq;
  int is_compound;
  int seqno;
  union
  {
    struct
    {
      struct huffchar *left;
      struct huffchar *right;
    } compound;
    unsigned char c;
  } u;
} Huffchar;

typedef struct huffcoder
{
  int freqs[NUM_CHARS];
  int code_lengths[NUM_CHARS];
  char *codes[NUM_CHARS];
  struct huffchar *tree;
} HuffmanCoder;

// create a new huffcoder structure
HuffmanCoder *new_HuffmanCoder();

// count the frequency of characters in a file; set chars with zero
// frequency to one
void huffmanCoderCount(HuffmanCoder *this, char *filename);

// using the character frequencies build the tree of compound
// and simple characters that are used to compute the Huffman codes
void huffmanCoderBuildTree(HuffmanCoder *this);

// using the Huffman tree, build a table of the Huffman codes
// with the huffcoder object
void huffmancCoderBuildTable(HuffmanCoder *this);

// print the Huffman codes for each character in order
void huffmanCoderPrintCodes(HuffmanCoder *this);

// encode the input file and write the encoding to the output file
void huffmanCoderEncode(HuffmanCoder *this, char *inputFilename,
                        char *outputFilename);

// decode the input file and write the decoding to the output file
void huffmanCoderDecode(HuffmanCoder *this, char *inputFilename,
                        char *outputFilename);

Huffchar *new_compound(Huffchar *min1, Huffchar *min2, int seqno);
void huffmanCoderGetCodes(Huffchar *current, char *path, int depth, char **codes, int *codeLengths);
int hufmannCoderGetMin(Huffchar **list, int listSize);
#endif // HUFF_H
