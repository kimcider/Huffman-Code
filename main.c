//
//  main.c
//  cse3081_mp3_20170024
//
//  Created by 김병천 on 2022/11/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prj.h"
void print_leaf(node_pointer node,int depth);//추후삭제요망
int cnt=0;
int main(int argc, const char * argv[]) {
    //usage check
    if(argc!=3){
        printf("usage: mp3_20170024 -c input.txt\n");
        printf("usage: mp3_20170024 -d input.txt\n");
        exit(-1);
    }
    
    //option check
    char option_string[50];
    strcpy(option_string,argv[1]);
    if(!(strcmp(option_string,"-d")==0||strcmp(option_string,"-c")==0)){
        printf("you can only use '-c' or 'd'\n");
        exit(-1);
    }
    if(strcmp(option_string,"-c")==0)option=1;
    if(strcmp(option_string,"-d")==0)option=2;
    
    //input file construct
    strcpy(input_file_name,argv[2]);
    ifp=fopen(input_file_name,"r");
    if(ifp==NULL){
        printf("file open fail\n");
        exit(-1);
    }
    
    //output file construct
    strcpy(output_file_name, input_file_name);
    if(option==1)strcat(output_file_name,".zz");
    else if(option==2) strcat(output_file_name,".yy");
    ofp=fopen(output_file_name,"w");
    if(ofp==NULL){
        printf("file open fail\n");
        exit(-1);
    }
    
    //start program
    if(option==1){
        compression();
    }else{
        decompression();
    }
    
}

void compression(void){
    char input;
    counter_type input_ary[128];
    node_pointer root;
    node_pointer node_list[128];
    int node_counter=0;
    node_pointer end_point;
    //construct tree
    {
        for(int i=0;i<128;i++){
            input_ary[i]=0;
            node_list[i]=NULL;
        }
        while(fscanf(ifp, "%c",&input)!=EOF){
            input_ary[input]++;
        }
        
        //construct first_node;
        for(int i=0;i<128;i++){
            if(input_ary[i]!=0){
                node_list[node_counter]=node_init(i,input_ary[i]);
                node_counter++;
            }
        }
        
        //code_list할당
        code_list=malloc(sizeof(codeword)*node_counter+1);
        number_of_codeword=node_counter+1;
        end_point=malloc(sizeof(struct node));
        
        end_point->count=0;
        end_point->l_child=NULL;
        end_point->r_child=NULL;
        end_point->is_end=1;
        end_point->is_character=1;
        
        int min=0;
        for(int i=1;i<node_counter;i++){
            if(node_list[i]->count<node_list[min]->count)min=i;
        }
        node_list[min]=merge_two_node(end_point, node_list[min]);
        
        //merge단계
        while(1){
            if(node_counter==1)break;
            
            int min_index,min2_index;
            
            if(node_list[0]->count<=node_list[1]->count){
                min_index=0;
                min2_index=1;
            }else{
                min_index=1;
                min2_index=0;
            }
            
            //choice most minimum and second minimum node
            for(int i=2;i<node_counter;i++){
                if(node_list[i]->count<node_list[min2_index]->count){
                    if(node_list[i]->count<node_list[min_index]->count){
                        min2_index=min_index;
                        min_index=i;
                    }else{
                        min2_index=i;
                    }
                }
            }
            node_pointer upper_node=merge_two_node(node_list[min_index],node_list[min2_index]);
            //앞의 리스트에 둘을 합한 노드를, 뒤의 리스트의 본래 리스트의 맨 마지막의 노드를 배치
            //최후에 node_list[0]에 root노드가 남게됨.
            if(min_index<min2_index){
                node_list[min_index]=upper_node;
                node_list[min2_index]=node_list[node_counter-1];//이조건 좀 다시보기
            }else{
                node_list[min2_index]=upper_node;
                node_list[min_index]=node_list[node_counter-1];
            }
            node_list[node_counter-1]=NULL;
            node_counter--;
        }
        
        root=node_list[0];
    }
    {//codeword 만들어주기
        create_codeword(root, 1, 0);
    }

    //codeword number 출력하기
    fwrite(&codeword_counter,sizeof(int),1,ofp);
    //codeword들 출력하기
    fwrite(code_list,sizeof(codeword),codeword_counter,ofp);
  

    
    //codeword기반으로 데이터출력하기
    fclose(ifp);
    ifp=fopen(input_file_name,"r");
    if(ifp==NULL){
        printf("file open fail\n");
        exit(-1);
    }
    code_type word=0;
    int word_counter=0;
    int end_index=-999;
    while(fscanf(ifp, "%c",&input)!=EOF){
        int index=-999;
        for(int i=0;i<number_of_codeword;i++){
            if(code_list[i].c==input){
                index=i;
                break;
            }
        }
        int flag=0;
        for(int j=CHECK_SIZE-1;j>=0;j--){
            int result=code_list[index].code>>j&1;
            if(flag==1){
                word=(word<<1)+result;
                word_counter++;
                if(word_counter==CHECK_SIZE){
                    word_counter=0;
                    fwrite(&word,sizeof(code_type),1,ofp);
                    word=0;
                }
            }
            if(result==1)flag=1;
        }
    }
    //다끝나고 end_pointer 출력해주고
    for(int i=0;i<number_of_codeword;i++){
        if(code_list[i].is_end==1){
            end_index=i;
            break;
        }
    }
    int flag=0;
    for(int j=CHECK_SIZE-1;j>=0;j--){
        int result=code_list[end_index].code>>j&1;
        if(flag==1){
            word=(word<<1)+result;
            word_counter++;
            if(word_counter==CHECK_SIZE){
                word_counter=0;
                fwrite(&word,sizeof(code_type),1,ofp);
                word=0;
            }
        }
        if(result==1)flag=1;
    }
    //CHECK_SIZE비트에 맞춰서 나머지 출력해주고
    while(1){
        if(word_counter<CHECK_SIZE){
            word=word<<1;
            word_counter++;
        }else break;
    }
    
    
    fwrite(&word,sizeof(code_type),1,ofp);
    fclose(ifp);
    fclose(ofp);
    
    free(code_list);
    free(end_point);
    free_node(root);

}

node_pointer node_init(char c, counter_type num){
    node_pointer node=malloc(sizeof(struct node));
    node->l_child=NULL;
    node->r_child=NULL;
    node->c=c;
    node->count=num;
    node->is_character=1;
    return node;
}

node_pointer merge_two_node(node_pointer node1,node_pointer node2){
    node_pointer upper_node=malloc(sizeof(struct node));
    upper_node->l_child=node1;
    upper_node->r_child=node2;
    upper_node->c=0;
    upper_node->is_character=0;
    upper_node->is_end=0;
    upper_node->count=node1->count+node2->count;
    return upper_node;
}


void create_codeword(node_pointer node,code_type code, int depth){
    if(node->is_character==0){
        code_type l_code=(code<<1)+0;
        code_type r_code=(code<<1)+1;
        create_codeword(node->l_child, l_code, depth+1);
        create_codeword(node->r_child, r_code, depth+1);
    }else{
        codeword temp;
        temp.code=code;
        temp.is_end=node->is_end;
        temp.c=node->c;
        temp.depth=depth;
        
        code_list[codeword_counter]=temp;
        codeword_counter++;
    }
}

void decompression(void){
    {
        //printf("decompression 시작\n\n");
        ifp=fopen(input_file_name,"rb");
        int temp_num;
        fread(&temp_num,sizeof(int),1,ifp);
        //printf("잘써졌나 - num: %d\n",temp_num);
        codeword code_list[temp_num];
        fread(code_list,sizeof(codeword),temp_num,ifp);
        
        //트리복구
        node_pointer root=malloc(sizeof(struct node));
        {
            root->l_child=NULL;
            root->r_child=NULL;
            root->is_end=0;
            root->is_character=0;
            
            root->count=999999999999;
            for(int i=0;i<temp_num;i++){
                reconstruct_tree(root,code_list[i]);
            }
        }
        //print_leaf(root, 0);
        ofp=fopen(output_file_name,"w");
        code_type word=0;
        node_pointer temp=root;
        int cnt=0;
        while(fread(&word,sizeof(code_type),1,ifp)==1){
            int char_flag=0;
            for(int j=CHECK_SIZE-1;j>=0;j--){
                int result=(word>>j)&1;
                if(result==0){
                    temp=temp->l_child;
                    if(temp->is_character==1)char_flag=1;
                }else if(result==1){
                    temp=temp->r_child;
                    if(temp->is_character==1)char_flag=1;
                }
                
                if(char_flag==1){
                    if(temp->is_end==1){
                        fclose(ifp);
                        fclose(ofp);
                        return;
                    }else{
                        fprintf(ofp,"%c",temp->c);
                        temp=root;
                        char_flag=0;
                    }
                }else continue;
            }
        }
    }
}


void reconstruct_tree(node_pointer node,codeword code){
    int flag=0;
    for(int j=CHECK_SIZE-1;j>=0;j--){
        int result=code.code>>j&1;
        if(flag==1){
            if(result==1){
                if(node->r_child==NULL){
                    node_pointer temp=malloc(sizeof(struct node));
                    temp->l_child=NULL;
                    temp->r_child=NULL;
                    temp->count=0;
                    temp->is_character=0;
                    temp->is_end=0;
                    node->r_child=temp;
                }
                node=node->r_child;
            }
            else{
                if(node->l_child==NULL){
                    node_pointer temp=malloc(sizeof(struct node));
                    temp->l_child=NULL;
                    temp->r_child=NULL;
                    temp->count=0;
                    temp->is_character=0;
                    temp->is_end=0;
                    node->l_child=temp;
                 
                }
                node=node->l_child;
            }
        }
        if(result==1){
            flag=1;
        }
    }
    node->c=code.c;
    node->is_character=1;
    node->is_end=code.is_end;
}

void free_node(node_pointer node){
    if(node->r_child!=NULL) free_node(node->r_child);
    if(node->l_child!=NULL)free_node(node->l_child);
    
    free(node);
    return;
}

