// code for a huffman coder

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "huff.h"

// create a new huffcoder structure
struct huffcoder *huffcoder_new()
{
  HuffmanCoder *coder = malloc(sizeof(HuffmanCoder));

  for (int i = 0; i < NUM_CHARS; i++)
  {
    coder->freqs[i] = 0;
    coder->code_lengths[i] = 0;
  }
  coder->tree = malloc(sizeof(Huffchar));
  return coder;
}

// count the frequency of characters in a file; set chars with zero
// frequency to one
void huffmanCoderCount(HuffmanCoder *this, char *filename)
{
  FILE *file = fopen(filename, "r");
  unsigned char current = fgetc(file);
  while (!feof(file))
  {
    this->freqs[current]++;
    current = fgetc(file);
  }
  fclose(file);
  for (int i = 0; i < NUM_CHARS; i++)
  {
    if (this->freqs[i] == 0)
    {
      this->freqs[i] = 1;
    }
  }
}

// using the character frequencies build the tree of compound
// and simple characters that are used to compute the Huffman codes
void huffmanCoderBuildTree(HuffmanCoder *this)
{
  int seqno = NUM_CHARS;
  Huffchar **nodes = malloc(sizeof(Huffchar *) * NUM_CHARS);
  for (int i = 0; i < NUM_CHARS; i++)
  {
    nodes[i] = malloc(sizeof(Huffchar));
    nodes[i]->freq = this->freqs[i];
    nodes[i]->is_compound = 0;
    nodes[i]->seqno = i;
    nodes[i]->u.c = i;
  }

  int listSize = NUM_CHARS;
  while (listSize > 1)
  {
    int minFreq = huffmanCoderGetMin(nodes, listSize);
    Huffchar *min1 = nodes[minFreq];
    nodes[minFreq] = nodes[listSize - 1];
    minFreq = huffmanCoderGetMin(nodes, listSize - 1);
    Huffchar *min2 = nodes[minFreq];
    nodes[minFreq] = nodes[listSize - 2];
    Huffchar *compound = new_compound(min1, min2, seqno);
    seqno++;
    listSize--;
    nodes[listSize - 1] = compound;
  }
  this->tree = nodes[0];
}

Huffchar *new_compound(Huffchar *min1, Huffchar *min2, int seqno)
{
  Huffchar *newhuff = malloc(sizeof(Huffchar));
  newhuff->freq = min1->freq + min2->freq;
  newhuff->seqno = seqno;
  newhuff->is_compound = 1;
  newhuff->u.compound.left = min1;
  newhuff->u.compound.right = min2;
  return newhuff;
}

int huffmanCoderGetMin(Huffchar **list, int listSize)
{
  int min = list[0]->freq;
  int minIndex = 0;
  for (int i = 0; i < listSize; i++)
  {
    if (list[i]->freq < min || (list[i]->freq == min && list[i]->seqno < list[minIndex]->seqno))
    {
      min = list[i]->freq;
      minIndex = i;
    }
  }
  return minIndex;
}

// using the Huffman tree, build a table of the Huffman codes
// with the huffcoder object
void huffmanCoderBuildTable(struct huffcoder *this)
{
  assert(this->tree != NULL);
  char path[NUM_CHARS];
  int depth = 0;
  huffmanCoderGetCodes(this->tree, path, depth, this->codes, this->code_lengths);
}

void huffmanCoderGetCodes(Huffchar *current, char *path, int depth, char **codes, int *codeLengths)
{
  if (current->is_compound)
  {
    path[depth] = '0';
    huffmanCoderGetCodes(current->u.compound.left, path, depth + 1, codes, codeLengths);
    path[depth] = '1';
    huffmanCoderGetCodes(current->u.compound.right, path, depth + 1, codes, codeLengths);
  }
  else
  {
    unsigned char c = current->u.c;
    codeLengths[c] = depth;
    path[depth] = '\0';
    codes[c] = malloc(sizeof(char) * depth);
    codes[c] = strcpy(codes[c], path);
  }
}

// print the Huffman codes for each character in order;
// you should not modify this function
void huffmanCoderPrintCodes(struct huffcoder *this)
{
  for (int i = 0; i < NUM_CHARS; i++)
  {
    // print the code
    printf("char: %d, freq: %d, code: %s\n", i, this->freqs[i], this->codes[i]);
  }
}

// encode the input file and write the encoding to the output file
void huffmanCoderEncode(struct huffcoder *this, char *inputFilename, char *outputFilename)
{
  FILE *input = fopen(inputFilename, "r");
  FILE *output = fopen(outputFilename, "w");
  unsigned char in = fgetc(input);
  while (!feof(input))
  {
    fputs(this->codes[in], output);
    in = fgetc(input);
  }
  fputs(this->codes[4], output);
  fclose(output);
  fclose(input);
}

// decode the input file and write the decoding to the output file
void huffmanCoderDecode(struct huffcoder *this, char *inputFilename, char *outputFilename)
{
  FILE *in = fopen(inputFilename, "r");
  FILE *out = fopen(outputFilename, "w");

  unsigned char c = fgetc(in);
  while (!feof(in))
  {
    Huffchar *node = this->tree;
    while (node->is_compound)
    {
      if (c == '0')
      {
        node = node->u.compound.left;
      }
      else if (c == '1')
      {
        node = node->u.compound.right;
      }
      c = fgetc(in);
    }
    if (!feof(in))
    {
      fputc(node->u.c, out);
    }
  }
  fclose(in);
  fclose(out);
}
