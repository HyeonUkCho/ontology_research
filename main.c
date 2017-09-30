#include <stdio.h>     // fopen, feof, fread, fclose 함수가 선언된 헤더 파일
#include <string.h>    // strlen, memset 함수가 선언된 헤더 파일
#include <stdlib.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS    // fopen 보안 경고로 인한 컴파일 에러 방지

int MAX_SIZE = 100;
int URL_LENGTH = 200;


typedef struct urlnode* urlnodeptr;
typedef struct keynode* keynodeptr;
typedef struct hashtable* hashtableptr;

typedef struct keynode{
    char keyword[100];
    keynodeptr nextkey;
    urlnodeptr nexturl;
}keynode;

typedef struct urlnode{
    char url[200];
    urlnodeptr nexturl;
}urlnode;


typedef struct hashtable{
    keynodeptr* arr;
    int tablesize;
}hashtable;

int get_digit(int size){
    int i=10;
    while(1){
        if(size/i == 0)
            break;
        i = i * 10;
    }
    return i;
}

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
       hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

hashtableptr create_hashtable(int file_size){
    hashtableptr newhash = (hashtableptr)malloc(sizeof(hashtable));
    if(newhash == NULL){
        printf("메모리 부족\n");
        exit(1);
    }
    newhash->arr = (keynodeptr*)malloc(sizeof(keynodeptr)*1000);
    newhash->tablesize = 1000;
    memset(newhash->arr,NULL,sizeof(keynodeptr)*1000);
    return newhash;
}

keynodeptr create_keynode(unsigned char* keyword){
    keynodeptr newnode = (keynodeptr)malloc(sizeof(keynode));
    if(newnode == NULL){
        printf("메모리 부족\n");
        exit(1);
    }
    strcpy(newnode->keyword,keyword);
    newnode->nextkey = NULL;
    newnode->nexturl = NULL;
    return newnode;
}

urlnodeptr create_urlnode(char url[]){
    urlnodeptr newnode = (urlnodeptr)malloc(sizeof(urlnode));
    if(newnode == NULL){
        printf("메모리 부족\n");
        exit(1);
    }
    strcpy(newnode->url,url);
    newnode->nexturl = NULL;
    return newnode;
}

int add_keynode(hashtableptr ht, unsigned char* keyword){
    int collision_check = 0;
    int addr = hash(keyword) % ht->tablesize;//get_digit(ht->tablesize);
    printf("%s addr : %d",keyword,addr);
    keynodeptr newkeynode = create_keynode(keyword);
    if(ht->arr[addr] == NULL){
        printf(" 추가\n");
        ht->arr[addr] = newkeynode;
    }else if(strcmp(ht->arr[addr]->keyword,keyword) == 0){
        printf(" 중복\n");

    }else{
        printf(" collision 발생\n");
        newkeynode->nextkey = ht->arr[addr];
        ht->arr[addr] = newkeynode;
        collision_check = 1;
    }
    return collision_check;
}

keynodeptr search_keynode(hashtableptr ht, unsigned char* keyword){
    int addr = hash(keyword) % ht->tablesize;
    keynodeptr target = ht->arr[addr];
    if(target == NULL){
        printf("not existed!\n");
        return NULL;
    }else{
        while(target != NULL){
            if(strcmp(target->keyword,keyword) == 0)
                break;
            target = target->nextkey;
        }
    }
    return target;
}

int search_urlnode(urlnodeptr rootnode, unsigned char* url){
    int i = 0;
    urlnodeptr temp = rootnode;
    while(!temp){
        if(strcmp(url,temp->url) == 0){
            i = 1;
        }else{
           temp = temp->nexturl;
        }
    }
}

int add_urlnode(hashtableptr ht,unsigned char* keyword,char* url){
    int i;
    urlnodeptr newnode = create_urlnode(url);
    keynodeptr result = search_keynode(ht,keyword);
    if(result == NULL){
        i=0;
    }else{
        if(result->nexturl == NULL){
            result->nexturl = newnode;
            i=1;
        }else{
            if(search_urlnode(result->nexturl,url) == 0){
                newnode->nexturl = result->nexturl;
                result->nexturl = newnode;
                i=1;
            }
        }
    }
    return i;
}

void print_urlnode(urlnodeptr unp){
    urlnodeptr temp = unp;
    while(temp != NULL){
        printf("%s -> ",temp->url);
        temp = temp->nexturl;
    }
    printf("\n");
}

void print_keynode(keynodeptr knp){
    keynodeptr temp = knp;
    while(temp != NULL){
        printf("%s -> ",temp->keyword);
        print_urlnode(temp->nexturl);
        temp = temp->nextkey;
    }
}

void print_hashtable(hashtableptr ht){
    int i;
    for(i=0;i<ht->tablesize;i++){
        printf("%d ",i);
        print_keynode(ht->arr[i]);
        printf("\n");
    }
}

int FILE_SIZE = 0;
int main()
{
    FILE *fp = fopen("incheon.txt", "r");

    if(fp == NULL) {
        printf("topic File is not opened");
        exit(1);
    }

    //calculate file size
    fseek(fp,0,SEEK_END);
    FILE_SIZE = ftell(fp);
    fseek(fp,0,SEEK_SET);
    printf("file size is %d\n",FILE_SIZE);

    //make hashtable
    hashtableptr ht = create_hashtable(FILE_SIZE);

    //insert topic
    int i=0;
    int cnt_collision = 0;
    while (!feof(fp)){
        unsigned char* inp = (unsigned char*)malloc(sizeof(unsigned char)*MAX_SIZE);
        fscanf(fp,"%s",inp);
        int index = hash(inp);
        //printf(" %u ",index);
        //printf("%s\n",inp);

        cnt_collision += add_keynode(ht,inp);
        free(inp);
    }

    fclose(fp);
    printf("%d\n",cnt_collision);

    fp = fopen("incheon_sample_2.txt", "r");

    if(fp == NULL) {
        printf("topic File is not opened");
        exit(1);
    }

    //insert url
    int j=0;
    while(!feof(fp)){
        unsigned char* search_word = (unsigned char*)malloc(sizeof(unsigned char)*MAX_SIZE);
        unsigned char* inp_topic = (unsigned char*)malloc(sizeof(unsigned char)*MAX_SIZE);
        unsigned char* inp_url = (unsigned char*)malloc(sizeof(unsigned char)*MAX_SIZE);
        fscanf(fp,"%s\t%s\t%s",search_word,inp_topic,inp_url);
        //printf("%d. %s\t%s\t%s\n",j++,search_word,inp_topic,inp_url);
        int check = add_urlnode(ht,inp_topic,inp_url);
        if(!check){
            printf("cannot find word\n");
        }
    }

    print_hashtable(ht);

    fseek(fp,0,SEEK_SET);

    //input search word and return url
    printf("\n==============================================================\n");
    while(!feof(fp)){
        unsigned char* search_word = (unsigned char*)malloc(sizeof(unsigned char)*MAX_SIZE);
        unsigned char* inp_topic = (unsigned char*)malloc(sizeof(unsigned char)*MAX_SIZE);
        unsigned char* inp_url = (unsigned char*)malloc(sizeof(unsigned char)*MAX_SIZE);
        fscanf(fp,"%s\t%s\t%s",search_word,inp_topic,inp_url);
        keynodeptr result = search_keynode(ht,inp_topic);
        printf("%s_url : ",search_word);
        print_urlnode(result->nexturl);
        printf("\n");
    }
    fclose(fp);
    free(ht);
    return 0;
}
