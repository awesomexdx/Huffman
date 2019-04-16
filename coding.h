#pragma once

#ifndef HUFFMAN_CODING_H
#define HUFFMAN_CODING_H


#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"

void encode(
        FILE * in,
        FILE * out,
        Code ** code_table);


void decode(
        FILE * in,
        FILE * out,
        SoR * root);


SoR * new_tree_node(unsigned char data, SoR * left, SoR * right);


SoR * build_tree(unsigned int * freq);


void Make_codes(SoR *root, char * buff, size_t len, Code ** code_arr, FILE * fout);


SoR * read_tree(FILE * fin);

#endif
