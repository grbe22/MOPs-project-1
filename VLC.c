/// file name: VLM.c
/// author: Gabe Rippel (gwr3294)
/// encodes strings based on their freqeuncy.
/// doesn't work for decoding, though. That's a bit of a problem.


#include <stdio.h>
#include <string.h>
#include "node_heap.h"

#define MAXSYMS 256
#define MAX_CODE 32
#define NUL     '\0'

void reversal(char string[]) {
    int k = strlen(string);
    for (int i = 0; i < k / 2; i ++) {
        char ph = string[i];
        string[i] = string[k-i-1];
        string[k-i-1] = ph;
    }
}

void prepend(char string[], size_t length) {
    /// our objective is to add spaces to the front of the string until size == length.
    int difference = length - strlen(string) + 1;
    for (int i = strlen(string) - 1; i >= 0; i --) {
        string[i + difference] = string[i];
    }
    for (int i = 0; i < difference; i ++) {
        string[i] = ' ';
    }
}

int basic_log(int length) {
    int two = 2;
    for (int i = 1; i <= 8; i++) {
        if (two > length) {
            return (i);
        } else {
            /// :3
            two = two + two;
        }
    }
}


int main(void) {
    static Heap heap;
    heap_init(&heap);
    static Symbol symbols[MAXSYMS];

    int length_of_heap = read_symbols(MAXSYMS, symbols);
    heap_make(&heap, length_of_heap, symbols);
    
    while (heap.size > 1) {
        Node lowest = heap_remove(&heap);
        Node sec_lowest = heap_remove(&heap);
        /// yes, it does it backwards. I know.
        /// this is intentional & I'll reverse it when it's time to print.
        /// so long as the program works, right?
        for (size_t i = 0; i < lowest.num_valid; i++) {
            for (size_t j = 0; j < MAX_CODE; j ++) {
                if (lowest.syms[i].codeword[j] == NUL) {
                    lowest.syms[i].codeword[j] = '0';
                    break;
                }
            }
        }
        for (size_t i = 0; i <  sec_lowest.num_valid; i ++) {
            for (size_t j = 0; j < MAX_CODE; j++) {
                if (sec_lowest.syms[i].codeword[j] == NUL) {
                    sec_lowest.syms[i].codeword[j] = '1';
                    break;
                }
            }
        }

        Node replacement;
        replacement.frequency = lowest.frequency + sec_lowest.frequency;
        replacement.num_valid = lowest.num_valid + sec_lowest.num_valid;
        

        for (size_t i = 0; i < lowest.num_valid; i ++) {
            replacement.syms[i] = lowest.syms[i];
        }
        for (size_t j = lowest.num_valid; j < replacement.num_valid; j++) {
            replacement.syms[j] = sec_lowest.syms[j - lowest.num_valid];
        }

        heap_add(&heap, replacement);
    }
    /// now we printin'
    Node final_node = heap_remove(&heap);
    
    int code_len = 0;
    for (size_t i = 0; i < final_node.num_valid; i++) {
        if (strlen(final_node.syms[i].codeword) > code_len) {
            code_len = strlen(final_node.syms[i].codeword);
        }
    }
    
    int total_bytes = 0;
    int total_characters = 0;
    /// code_len is the size of the longest string.
    for (size_t i = 0; i < final_node.num_valid; i++) {
        Symbol man = final_node.syms[i];
        /// printf("Codeword: %s\n", man.codeword);
        /// 2/23/23: Everything except this reversal works just peachy. Why.
        /// this loop runs from start to finish.
        total_bytes += man.frequency * strlen(man.codeword);
        reversal(man.codeword);
        prepend(man.codeword, code_len);
        total_characters += man.frequency;
        if (man.symbol > 127) {
            printf("symbol: '0x%x'\tfrequency:\t\t%lu\tcodeword:%s\n", man.symbol, man.frequency, man.codeword);
        } else {
            printf("symbol: '");
            if (man.symbol == '\n') {
                printf("\\n");
            } else if (man.symbol == '\t') {
                printf("\\t");
            } else {
                printf("%c", man.symbol);
            }
            printf("'\tfrequency:\t\t%lu\tcodeword:%s\n", man.frequency, man.codeword);
        }
    }
    printf("\n");
    float avg = (float)(total_bytes) / (float)(total_characters);
    printf("Average VLC code length:\t%.4f\n", avg);
    int avg2 = basic_log(final_node.num_valid);
    printf("Fixed length code length:\t%.4f\n", (float)(avg2));
    printf("Longest variable code length:\t%d\n", code_len);
    printf("Node cumulative frequency:\t%d\n", total_characters);
    printf("Number of distinct symbols:\t%ld\n\n", final_node.num_valid);
}

