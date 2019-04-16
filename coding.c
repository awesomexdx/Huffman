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

    SoR * root = queue->nitems > 0 ? getMin(queue).value : NULL;

    queueFree(queue);

    return root;
}


typedef struct {
    FILE * file;
    unsigned int bit_pos;
    unsigned int byte_pos;

    unsigned char buff[BUFF_SIZE];

} BitIOStruct;


inline int writebit(
        unsigned int bit,
        BitIOStruct * bit_struct)
{
    bit_struct->buff[bit_struct->byte_pos] += (bit << (7 - bit_struct->bit_pos++));

    if (bit_struct->bit_pos > 7) {
        bit_struct->bit_pos = 0;
        bit_struct->byte_pos++;
        if (bit_struct->byte_pos == BUFF_SIZE)
        {

            bit_struct->byte_pos = 0;

            fwrite(bit_struct->buff, 1, BUFF_SIZE, bit_struct->file);

            memset(bit_struct->buff, 0, BUFF_SIZE);
        }

    }

    return 0;
}

inline int readbit(
        BitIOStruct * bit_struct)
{
    if (bit_struct->bit_pos > 7) {
        bit_struct->bit_pos = 0;
        bit_struct->byte_pos++;
        if (bit_struct->byte_pos == BUFF_SIZE)
        {
            bit_struct->byte_pos = 0;

            fread(bit_struct->buff, 1, BUFF_SIZE, bit_struct->file);
        }

    }

    return (bit_struct->buff[bit_struct->byte_pos] >> (7 - bit_struct->bit_pos++)) & 0x01;
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
#ifdef DEBUG
                printf("%c", code[code_pos] == '1' ? '1' : '0');
#endif
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
#ifdef DEBUG
            printf(" ");
#endif
        }
    }

    fwrite(output_buff, 1, byte_pos + (bit_pos > 0 ? 1 : 0), out);
}


void decode(
        FILE * in,
        FILE * out,
        SoR * root,
        unsigned int file_len)
{
    char input_buff[BUFF_SIZE];
    char output_buff[BUFF_SIZE];

    size_t input_len = 0;
    size_t output_pos = 0;

    size_t byte_pos = 0;
    size_t bit_pos = 0;

    SoR * node = root;

    int bit = 0;

    //while (() > 0);

    for (unsigned int decoded_len = 0; decoded_len < file_len; )
    {
        if (node->left == NULL && node->right == NULL)
        {
            output_buff[output_pos++] = node->data;
            node = root;

            decoded_len++;

            if (output_pos == BUFF_SIZE) {
                fwrite(output_buff, 1, BUFF_SIZE, out);
                output_pos = 0;
            }
        }
        else
        {
            if (bit_pos > 7)
            {
                bit_pos = 0;
                byte_pos++;
            }

            if (byte_pos == input_len)
            {
                input_len = fread(input_buff, 1, BUFF_SIZE, in);
                byte_pos = 0;
            }

            bit = (input_buff[byte_pos] >> (7 - bit_pos++)) & 0x01; // 0000 0001

            printf("%c", bit ? '1' : '0'); //тернарный оператор

            if (!bit)
                node = node->left;
            else
                node = node->right;
#ifdef DEBUG
            if (node->left == NULL && node->right == NULL)
                printf(" ");
#endif
        }


    }

    fwrite(output_buff, 1, output_pos, out);
}
