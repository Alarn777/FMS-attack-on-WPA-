//
// Created by Michael Rokitko on 24/11/2018.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 256

int modulo(int x,int N){
    return (x % N + N) %N;
}

void swap(unsigned char* from,unsigned char *to){
    unsigned char temp = *to;
    *to = *from;
    *from = temp;
}

unsigned char* crypto(unsigned char *key,unsigned char* S,int keyLen){
    for(int i = 0;i <  255;i++) {
        S[i] = i;
    }
    int j = 0;
    for(int i = 0;i <  255;i++) {
        j = (j + S[i] + key[i % keyLen]) % 256;
        swap(&S[i], &S[j]);
    }
    return S;
}

unsigned char generate(unsigned char* S,unsigned char* text,int textLen){
    unsigned char res;
    int i = 0,j = 0;
    for (int count = 0;count < textLen; count++) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        swap(&S[i], &S[j]);
        res = S[(S[i] + S[j]) % 256];
        text[count] = text[count] ^ res;
    }
}

void rc4(unsigned char* key,int keyLen,unsigned char* S,unsigned char* text,int textLen){
    S = crypto(key,S,keyLen);
    generate(S,text,textLen);

}


void crack(unsigned char *key,unsigned char* S,unsigned char* buffer,int* counts,int current_attack_bit){
    for(int i = 0;i <  255;i++) {
        S[i] = i;
    }

    unsigned char key_arr[8];
    key_arr[0] = buffer[0];
    key_arr[1] = buffer[1];
    key_arr[2] = buffer[2];

    key_arr[3] = key[0];
    key_arr[4] = key[1];
    key_arr[5] = key[2];
    key_arr[6] = key[3];
    key_arr[7] = key[4];

    int j = 0;
        for(int i = 0;i  < (current_attack_bit + 3);i++) {
            j = modulo((j + S[i] + key_arr[i % 8]),256);
            swap(&S[i], &S[j]);
    }
    int m = 0;
    for (int k = 0; k < SIZE; k++) {
        if(S[k] == (0xAA ^ buffer[3])) {
//            printf("S: %.2x\n", S[k]);
            m = k;
        }
    }

    int key1 = modulo((m - j - S[current_attack_bit + 3]),256);
    if(key1 < 0)
        return;
    counts[key1]++;
}


int main(){

    int counts[SIZE];
    unsigned char readBuffer[5], key[5],S[SIZE];
    int best, max;
    for(int c = 0; c < 5; c++) {
        key[c] = 0;
    }
    for(int i = 0; i < SIZE; i++) {
        counts[i] = 0;
    }
    for (int j = 0; j < 5; ++j) {
        printf("-------------For key[%d] best guesses are---------------\n",j);


        FILE *fp;
        fp = fopen("traffic.rcf", "r");
        if (fp == NULL) {
            printf("\nError opening file");
            exit(1);
        }

        //each time we have to count again
        for(int i = 0; i < SIZE; i++) {
            counts[i] = 0;
        }

        while (fread(readBuffer, sizeof(unsigned char), 5, fp) != 0)
        if (readBuffer[0] == j+3 && readBuffer[1] == 0xFF)
            crack(key,S,readBuffer,counts,j);
    best = 0;
    max = 0;
    for (int i = 0; i < SIZE; ++i) {
        if (counts[i] >= max) {
            max = counts[i];
            printf("Max count is: %d -> ",max);
            best = i;
            printf("Best guess hex is: %.2x, best guess char: %c \n",i,i);

        }
    }
        key[j] = best;
        fclose(fp);
        printf("--------------------------------------------------------\n");
    }

    //in my specific case next line is needed to open the file beacuse the algorithm gives suggestet characters for a key only
    key[4] = 'm';

    for (int k = 0; k < 5; ++k) {
        printf("%.2x",key[k]);
    }
    printf("\n");

    for (int k = 0; k < 5; ++k) {
        printf("%c",key[k]);
    }


    FILE *out = fopen("out.rcf","w");
    FILE *in;
    in = fopen("traffic.rcf", "r");
    if (in == NULL) {
        printf("\n\nError opening file");
        exit(1);
    }

    key[4] = 'm';
    while (fread(readBuffer, sizeof(unsigned char), 128, in) != 0)
    {
        unsigned char temp[128];
        for (int i = 0; i < 128; ++i) {
            temp[i] = readBuffer[i];
        }
        rc4(key,40,S,temp,5);
        fwrite(temp, sizeof(unsigned char), 128,out);
    }
    return 0;
}

