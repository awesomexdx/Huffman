#include "definitions.h"
#include <stdlib.h>
#include "coding.h"
#include <string.h>



int huff_test(
        void)
{
    FILE *input_file, *output_file, *buff_file; //указатели на файлы

    input_file = fopen("in.txt", "rb"); //открываем файл, который нужно закодировать
    output_file = fopen("out.txt", "wb"); //открываем файл для записи сжатого файла
    buff_file = fopen("buff.txt", "wb"); //открываем файл для записи раскодированного файле

    int data;

    unsigned int freq[256] = {0};
    unsigned char file_size[4] = {0};
    unsigned int input_file_len = 0;

    /// ---***--- ///

    BitIOStruct bit_struct;

    bit_struct.file = output_file;
    bit_struct.bit_pos = 0;
    bit_struct.byte_pos = 0;

    memset(bit_struct.buff, 0x00, BUFF_SIZE);

    /// ---***--- ///

    char buff[256];

    if (input_file == NULL) {
        puts("FILE NOT OPEN!!!!!!!");
        return 0;
    }

    while ((data = fgetc(input_file)) != EOF) {
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

    fwrite(file_size, 1, 4, output_file);

    if (root)
        Make_codes(root, buff, 0, code_array, output_file, &bit_struct);

#ifdef DEBUG_OUTPUT
    for (int i = 0; i < 256; i++) {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }
#endif

    rewind(input_file);

    encode(input_file, output_file, code_array, &bit_struct);

    fclose(input_file);
    fclose(output_file);


#ifdef DEBUG_OUTPUT
    printf("\n- - -\n");
#endif

    output_file = fopen("out.txt", "rb");

    size_t read;

    read = fread(file_size, 1, 4, output_file);

    if (read < 4)
        return -1;

    input_file_len = ((unsigned int) file_size[0] << 24)
                     + ((unsigned int) file_size[1] << 16)
                     + ((unsigned int) file_size[2] << 8)
                     + (unsigned int) file_size[3];

    /// ---***--- ///

    bit_struct.file = output_file;
    bit_struct.bit_pos = 8;
    bit_struct.byte_pos = BUFF_SIZE - 1;

    memset(bit_struct.buff, 0x00, BUFF_SIZE);

    /// ---***--- ///

    if (input_file_len)
    {
        root = read_tree(output_file, &bit_struct);
    }
#ifdef DEBUG_OUTPUT
    printf("\n- - -\n");

    for (int i = 0; i < 256; i++)
    {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }
#endif

    decode(output_file, buff_file, root, input_file_len, &bit_struct);

    return 0;
}


int bit_io_test(
        void)
{
    BitIOStruct bit_struct;

    FILE *input_file, *output_file, *buff_file;

    input_file = fopen("in.txt", "rb");
    output_file = fopen("out.txt", "wb");
    buff_file = fopen("buff.txt", "wb");

    bit_struct.file = output_file;
    bit_struct.bit_pos = 0;
    bit_struct.byte_pos = 0;

    memset(bit_struct.buff, 0x00, BUFF_SIZE);

    unsigned int test_uint = 41645476; // рандомное число
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

    fwrite(bit_struct.buff, 1, bit_struct.byte_pos + (bit_struct.bit_pos ? 1 : 0), output_file);

    fclose(output_file);

    input_file = fopen("out.txt", "rb");

    bit_struct.file = input_file;
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

    //bit_io_test();

    return 0;
}