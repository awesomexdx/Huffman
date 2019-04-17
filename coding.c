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


inline int writebit(
        unsigned int bit,
        BitIOStruct * bit_struct)
{
#ifdef BIT_OUTPUT
    printf("%d", bit);
#endif
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
#ifdef BIT_OUTPUT
    printf("%d", (bit_struct->buff[bit_struct->byte_pos] >> (7 - bit_struct->bit_pos)) & 0x01);
#endif
    return (bit_struct->buff[bit_struct->byte_pos] >> (7 - bit_struct->bit_pos++)) & 0x01;
}

void Make_codes(SoR *root, char * buff, size_t len, Code ** code_arr, FILE * fout,
                BitIOStruct * bit_struct)
{
    if (root->left == NULL && root->right == NULL) {
        writebit(1, bit_struct);

        for (int k = 7; k >= 0; k--)
            writebit((unsigned int)(root->data >> k) & 0x01, bit_struct);

        buff[len] = 0;

        code_arr[root->data] = malloc(sizeof(Code));

        if (code_arr[root->data] == NULL) {
            puts("OH GOD WHY ALLOCATION ERROR");
            printf("--%d\n", len);
            exit(EXIT_FAILURE);
        }

        code_arr[root->data]->code = malloc(len);

        if (code_arr[root->data]->code == NULL) {
            puts("OH GOD WHY ALLOCATION ERROR");
            printf("==%d\n", len);
            exit(EXIT_FAILURE);
        }

        code_arr[root->data]->length = len;
        strcpy(code_arr[root->data]->code, buff);
    }
    else {
        writebit(0, bit_struct);

        buff[len] = '0';
        Make_codes(root->left, buff, len + 1, code_arr, fout, bit_struct);

        buff[len] = '1';
        Make_codes(root->right, buff, len + 1, code_arr, fout, bit_struct);
    }
}


SoR * read_tree(FILE * fin,
                BitIOStruct * bit_struct) {
    static int bit;
    unsigned char buff = 0;

    bit = readbit(bit_struct);

    SoR * left = NULL;
    SoR * right = NULL;

    if (bit) {
        for (int k = 7; k >= 0; k--)
            buff += (readbit(bit_struct) << k);

        return new_tree_node(buff, NULL, NULL);
    }
    else {
        left = read_tree(fin, bit_struct);
        right = read_tree(fin, bit_struct);

        return new_tree_node('\0', left, right);
    }
}

void encode(
        FILE * in,
        FILE * out,
        Code ** code_table,
        BitIOStruct * bit_struct)
{
    unsigned char input_buff[BUFF_SIZE];

    size_t input_len = 0;

    char * code = NULL;
    unsigned int code_length = 0;

    while ((input_len = fread(input_buff, 1, BUFF_SIZE, in)) > 0) {
        for (unsigned int k = 0; k < input_len; k++) {
            code = code_table[input_buff[k]]->code;
            code_length = code_table[input_buff[k]]->length;

            for (unsigned int code_pos = 0; code_pos < code_length; code_pos++) {
                writebit((unsigned int) code[code_pos] - '0', bit_struct);
#ifdef DEBUG_OUTPUT
                printf("%c", code[code_pos] == '1' ? '1' : '0');
#endif
                }

#ifdef DEBUG_OUTPUT
            printf(" ");
#endif
        }
    }

    fwrite(bit_struct->buff, 1, bit_struct->byte_pos + (bit_struct->bit_pos > 0 ? 1 : 0), bit_struct->file);
}


void decode(
        FILE * in,
        FILE * out,
        SoR * root,
        unsigned int file_len,
        BitIOStruct * bit_struct)
{
    char output_buff[BUFF_SIZE];

    size_t output_pos = 0;

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
            bit = readbit(bit_struct);
#ifdef DEBUG_OUTPUT
            printf("%c", bit ? '1' : '0'); //тернарный оператор
#endif
            if (!bit)
                node = node->left;
            else
                node = node->right;
#ifdef DEBUG_OUTPUT
            if (node->left == NULL && node->right == NULL)
                printf(" ");
#endif
        }


    }

    fwrite(output_buff, 1, output_pos, out);
}
