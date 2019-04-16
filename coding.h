#pragma once

#ifndef HUFFMAN_CODING_H
#define HUFFMAN_CODING_H


#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"

SoR * new_tree_node(unsigned char data, SoR * left, SoR * right);


SoR * build_tree(unsigned int * freq);


void Make_codes(SoR *root, char * buff, size_t len, Code ** code_arr, FILE * fout);


SoR * read_tree(FILE * fin);

#endif
