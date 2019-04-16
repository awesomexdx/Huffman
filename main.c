#include "definitions.h"
#include <stdlib.h>
#include "coding.h"
#include <string.h>


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

    fp2 = fopen("out.txt", "rb");

    root = read_tree(fp2);

    Make_codes(root, buff, 0, code_array, fp3);

    printf("\n- - -\n");

    fclose(fp3);
    fp3 = fopen("buff.txt", "wb");

    for (int i = 0; i < 256; i++)
    {
        if (code_array[i] != NULL)
            printf("Char %c %d Code %s\n", i, i, code_array[i]->code);
    }

    decode(fp2, fp3, root);

    return 0;
}