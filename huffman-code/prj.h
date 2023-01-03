//
//  prj.h
//  cse3081_mp3_20170024
//
//  Created by 김병천 on 2022/11/21.
//

#ifndef prj_h
#define prj_h
#define INFINITE 9999

typedef int code_type;
typedef unsigned long long counter_type;
#define CHECK_SIZE sizeof(code_type)*8

struct node{
    struct node* l_child;
    struct node* r_child;
    unsigned long long count;
    char c;
    char is_character;
    char is_end;
};
typedef struct node* node_pointer;

typedef struct{
    char c;
    char is_end;
    char depth;
    code_type code;
}codeword;
codeword* code_list;

int number_of_codeword=-999;
int codeword_counter=0;
int option=-1;
FILE* ifp;
FILE* ofp;
char input_file_name[100];
char output_file_name[100];


void compression(void);
void decompression(void);
void reconstruct_tree(node_pointer node,codeword code);
node_pointer node_init(char c, counter_type num);
node_pointer merge_two_node(node_pointer node1,node_pointer node2);
void create_codeword(node_pointer node,code_type code, int depth);

void free_node(node_pointer node);
#endif /* prj_h */
