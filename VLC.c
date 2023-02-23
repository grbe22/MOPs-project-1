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

int main(void) {
    static Heap heap;
    heap_init(&heap);
    static Symbol symbols[MAXSYMS];

    read_symbols(MAXSYMS, symbols);
    
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
                    sec_lowest.syms[i].codeword[j] = '0';
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
        for (size_t j = 0; j < sec_lowest.num_valid; j++) {
            replacement.syms[lowest.num_valid + j - 1] = sec_lowest.syms[j];
        }

        heap_add(&heap, replacement);
    }
    /// now we printin'

    Node final_node = heap_remove(&heap);
    for (size_t i = 0; i < final_node.num_valid; i++) {
        printf("symbol: '%c'\tfrequency:\t\t'%lu'\tcodeword:\tNUL\n", final_node.syms[i].symbol, final_node.syms[i].frequency);
    }
}

