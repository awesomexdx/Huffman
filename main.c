#include "definitions.h"
#include <stdlib.h>
#include "coding.h"
#include <string.h>

#define BUFF_SIZE   1024


void encode(
        FILE * in,
        FILE * out,
        Code ** code_table)
{
    char input_buff[BUFF_SIZE];
    char output_buff[BUFF_SIZE] = { 0 };

    size_t input_len = 0;
    size_t byte_pos = 0;
    size_t bit_pos = 0;

    char * code = NULL;
    unsigned int code_length = 0;

    while ((input_len = fread(input_buff, 1, BUFF_SIZE, in)) > 0) {
        for (unsigned int k = 0; k < input_len; k++) {
            code = code_table[input_buff[k]]->code;
            code_length = code_table[input_buff[k]]->length;

            for (unsigned int code_pos = 0; code_pos < code_length; code_pos++) {
                if (code[code_pos] == '1')
                    output_buff[byte_pos] |= 0x80 >> bit_pos; // 1000 0000
                bit_pos++;

                printf("%c", code[code_pos] == '1' ? '1' : '0');

                if (bit_pos == 8) {
                    bit_pos = 0;
                    byte_pos++;

                    if (byte_pos == BUFF_SIZE) {
                        fwrite(output_buff, 1, BUFF_SIZE, out);
                        memset(output_buff, 0, BUFF_SIZE);
                        byte_pos = 0;
                    }
                }
            }

            printf(" ");
        }
    }

    fwrite(output_buff, 1, byte_pos + (bit_pos > 0 ? 1 : 0), out);
}


int main()
{
    FILE *fp, *fp2, *fp3; //указатели на файлы

    fp = fopen("in.txt", "rb"); //открываем файл, который нужно закодировать
    fp2 = fopen("out.txt", "wb"); //открываем файл для записи сжатого файла
    fp3 = fopen("buff.txt", "wb"); //открываем файл для записи бинарного кода

    int data;

    unsigned int freq[256] = { 0 };

    if (fp == NULL)
    {
        puts("FILE NOT OPEN!!!!!!!");
        return 0;
    }

    while ((data = fgetc(fp)) != EOF)
    {
        freq[data]++;
    }

    SoR * root = build_tree(freq);

    Code * code_array[256] = { NULL };

    char buff[256];

    Make_codes(root, buff, 0, code_array, fp2);

    for (int i = 0; i < 256; i++)
    {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }

    rewind(fp);

    encode(fp, fp2, code_array);

    fclose(fp);
    fclose(fp2);
/*
    fp2 = fopen("out.txt", "rb");

    root = read_tree(fp2);

    Make_codes(root, buff, 0, code_array, fp3);

    printf("- - -\n");

    for (int i = 0; i < 256; i++)
    {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }
*/
    return 0;
}