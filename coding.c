#include "coding.h"
#include <string.h>


inline SoR * new_tree_node(unsigned char data, SoR * left, SoR * right) {
    SoR * new_node = malloc(sizeof(SoR));

    new_node->data = data;
    new_node->left = left;
    new_node->right = right;

    return new_node;
}


SoR * build_tree(unsigned int * freq) {
    Queue * queue = heap_create(256);

    for (unsigned int i = 0; i < 256; i++)
    {
        if (freq[i] != 0)
            queueInsert(queue, freq[i], new_tree_node((unsigned char) i, 0, 0));
    }

    QueueItem a;
    QueueItem b;

    while (queue->nitems > 1) {
        a = getMin(queue);
        b = getMin(queue);

        queueInsert(queue, a.priority + b.priority, new_tree_node(0, a.value, b.value));
    }

    SoR * root = getMin(queue).value;

    queueFree(queue);

    return root;
}


void Make_codes(SoR *root, char * buff, size_t len, Code ** code_arr, FILE * fout)
{
    if (root->left == NULL && root->right == NULL) {
        fwrite("1", 1, 1, fout);

        fwrite(&root->data, 1, 1, fout);

        buff[len] = 0;

        code_arr[root->data] = malloc(sizeof(Code));

        code_arr[root->data]->code = malloc(len);

        code_arr[root->data]->length = len;

        strcpy(code_arr[root->data]->code, buff);
    }
    else {
        fwrite("0", 1, 1, fout);

        buff[len] = '0';
        Make_codes(root->left, buff, len + 1, code_arr, fout);

        buff[len] = '1';
        Make_codes(root->right, buff, len + 1, code_arr, fout);
    }
}


SoR * read_tree(FILE * fin) {
    static unsigned char buff;

    fread(&buff, 1, 1, fin);

    SoR * left = NULL;
    SoR * right = NULL;

    if (buff == '1') {
        fread(&buff, 1, 1, fin);

        return new_tree_node(buff, NULL, NULL);
    }
    else {
        left = read_tree(fin);
        right = read_tree(fin);

        return new_tree_node('\0', left, right);
    }
}

int getbit(int k) // функция ,которая возвращает k-й бит из кода символа
{
    ;
}

void stupid_function(size_t bitcount, FILE * outfile) // нужна таблица кодов
{
    unsigned char out[] = {0};
    size_t out_pos = 0;
    size_t byte_pos = 0;
    int bit;
    for (size_t k=0; k<bitcount; k++) // bitcount - длина кода символа
    {
        bit = getbit(k);
        out[out_pos]+=bit<<(7-byte_pos++);
        if (byte_pos==8){
            byte_pos = 0;
            out_pos++;
            if(out_pos>=8) {
                fwrite(out, 1, out_pos, outfile);
                memset(out, 0, out_pos);
                out_pos = 0;

            }
        }
    }
}