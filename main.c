#include "definitions.h"
#include <stdlib.h>
#include "coding.h"
#include <string.h>


int main() {
    FILE *fp, *fp2, *fp3; //указатели на файлы

    fp = fopen("in.txt", "rb"); //открываем файл, который нужно закодировать
    fp2 = fopen("out.txt", "wb"); //открываем файл для записи сжатого файла
    fp3 = fopen("buff.txt", "wb"); //открываем файл для записи бинарного кода

    int data;

    unsigned int freq[256] = {0};
    unsigned char file_size[4] = {0};
    unsigned int input_file_len = 0;

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
        Make_codes(root, buff, 0, code_array, fp2);

#ifdef DEBUG
    for (int i = 0; i < 256; i++) {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }
#endif

    rewind(fp);

    encode(fp, fp2, code_array);

    fclose(fp);
    fclose(fp2);

    fp2 = fopen("out.txt", "rb");

    size_t read;

    read = fread(file_size, 1, 4, fp2);

    if (read < 4)
        return -1;

    input_file_len = ((unsigned int) file_size[0] << 24)
                     + ((unsigned int) file_size[1] << 16)
                     + ((unsigned int) file_size[2] << 8)
                     + (unsigned int) file_size[3];

    if (input_file_len) {
        root = read_tree(fp2);
        Make_codes(root, buff, 0, code_array, fp3);
    }
#ifdef DEBUG
    printf("\n- - -\n");
#endif

    fclose(fp3);
    fp3 = fopen("buff.txt", "wb");

#ifdef DEBUG
    for (int i = 0; i < 256; i++)
    {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }
#endif

    decode(fp2, fp3, root, input_file_len);

    return 0;
}