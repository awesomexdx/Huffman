#include "definitions.h"
#include <stdlib.h>
#include "coding.h"
#include <string.h>



int huff_test(
        void)
{
    FILE *fp, *fp2, *fp3; //указатели на файлы

    fp = fopen("in.txt", "rb"); //открываем файл, который нужно закодировать
    fp2 = fopen("out.txt", "wb"); //открываем файл для записи сжатого файла
    fp3 = fopen("buff.txt", "wb"); //открываем файл для записи бинарного кода (что это значит???)

    int data;

    unsigned int freq[256] = {0};
    unsigned char file_size[4] = {0};
    unsigned int input_file_len = 0;

    /// ---***--- ///

    BitIOStruct bit_struct;

    bit_struct.file = fp2;
    bit_struct.bit_pos = 0;
    bit_struct.byte_pos = 0;

    memset(bit_struct.buff, 0x00, BUFF_SIZE);

    /// ---***--- ///

    char buff[256];

    if (fp == NULL) {
        puts("FILE NOT OPEN!!!!!!!");
        return 0;
    }

    while ((data = fgetc(fp)) != EOF) {
        freq[data]++;
    }

    SoR *root = build_tree(freq);

    Code *code_array[256] = {NULL};

    for (int k = 0; k < 256; k++)
        input_file_len += freq[k];

    // This ensures independence from unsigned int size
    // on different platforms
    file_size[0] = (unsigned char) ((input_file_len >> 24) & 0xFF);
    file_size[1] = (unsigned char) ((input_file_len >> 16) & 0xFF);
    file_size[2] = (unsigned char) ((input_file_len >> 8) & 0xFF);
    file_size[3] = (unsigned char) ((input_file_len) & 0xFF);

    fwrite(file_size, 1, 4, fp2);

    if (root)
        Make_codes(root, buff, 0, code_array, fp2, &bit_struct);

#ifdef DEBUG_OUTPUT
    for (int i = 0; i < 256; i++) {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }
#endif

    rewind(fp);

    encode(fp, fp2, code_array, &bit_struct);

    fclose(fp);
    fclose(fp2);


#ifdef DEBUG_OUTPUT
    printf("\n- - -\n");
#endif

    fp2 = fopen("out.txt", "rb");

    size_t read;

    read = fread(file_size, 1, 4, fp2);

    if (read < 4)
        return -1;

    input_file_len = ((unsigned int) file_size[0] << 24)
                     + ((unsigned int) file_size[1] << 16)
                     + ((unsigned int) file_size[2] << 8)
                     + (unsigned int) file_size[3];

    /// ---***--- ///

    bit_struct.file = fp2;
    bit_struct.bit_pos = 8;
    bit_struct.byte_pos = BUFF_SIZE - 1;

    memset(bit_struct.buff, 0x00, BUFF_SIZE);

    /// ---***--- ///

    if (input_file_len)
    {
        root = read_tree(fp2, &bit_struct);
    }
#ifdef DEBUG_OUTPUT
    printf("\n- - -\n");

    for (int i = 0; i < 256; i++)
    {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }
#endif

    decode(fp2, fp3, root, input_file_len, &bit_struct);

    return 0;
}


int bit_test(
        void)
{
    BitIOStruct bit_struct;

    FILE *fp, *fp2, *fp3;

    fp = fopen("in.txt", "rb");
    fp2 = fopen("out.txt", "wb");
    fp3 = fopen("buff.txt", "wb");

    bit_struct.file = fp2;
    bit_struct.bit_pos = 0;
    bit_struct.byte_pos = 0;

    memset(bit_struct.buff, 0x00, BUFF_SIZE);

    unsigned int test_uint = 41645476;
    unsigned int new_uint = 0;

    unsigned int bit;

    for (int k = 8 * sizeof(unsigned int) - 1; k >= 0; k--)
    {
        bit = (test_uint >> k) & 0x01;
#ifdef DEBUG_OUTPUT
        printf("%d", bit);
#endif
        writebit(bit, &bit_struct);
    }

#ifdef DEBUG_OUTPUT
    printf("\n");
#endif

    fwrite(bit_struct.buff, 1, bit_struct.byte_pos + (bit_struct.bit_pos ? 1 : 0), fp2);

    fclose(fp2);

    fp = fopen("out.txt", "rb");

    bit_struct.file = fp;
    bit_struct.bit_pos = 8;
    bit_struct.byte_pos = BUFF_SIZE - 1;

    memset(bit_struct.buff, 0x00, BUFF_SIZE);

    for (int k = 8 * sizeof(unsigned int) - 1; k >= 0; k--)
    {
        bit = (unsigned int) readbit(&bit_struct);
#ifdef DEBUG_OUTPUT
        printf("%d", bit);
#endif
        new_uint += (bit << k);
    }

#ifdef DEBUG_OUTPUT
    printf("\n");

    printf("Numbers are %s equal", new_uint == test_uint ? "" : "not");
#endif

    return 0;
}

int main(
        void)
{
    huff_test();

    //bit_test();

    return 0;
}