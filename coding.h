#pragma once

/*#ifndef HUFFMAN_CODING_H
#define HUFFMAN_CODING_H*/


#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"

void encode(
        FILE * in,
        FILE * out,
        Code ** code_table,
        BitIOStruct * bit_struct);


void decode(
        FILE * in,
        FILE * out,
        SoR * root,
        unsigned int file_len,
        BitIOStruct * bit_struct);


SoR * new_tree_node(unsigned char data, SoR * left, SoR * right);


SoR * build_tree(unsigned int * freq);


void Make_codes(SoR *root, char * buff, size_t len, Code ** code_arr, FILE * fout,
                BitIOStruct * bit_struct);


SoR * read_tree(FILE * fin,
                BitIOStruct * bit_struct);


int readbit(
        BitIOStruct * bit_struct);

int writebit(
        unsigned int bit,
        BitIOStruct * bit_struct);

//#endif
