/// file: node_heap.c
/// original author: ben k steele, bksvcs@rit.edu
/// modified & implemented by gabe rippel, gwr3294@rit.edu
///
/// create an application-specific, minimum-ordered heap, whose
/// implementation is tailored to the variable-length coding application.

#include <string.h>
#include <node_heap.h>

/// NUL is the 0, or null character, in ASCII code representation.
///
#define NUL '\0'

/// Number of bits in an extended ASCII code representation.
///
#define NUM_BITS 8

/// The maximum number of distinct symbols in a byte.
///
#define MAX_SYMS 256

/// MAX_CODE is the worst possible code length.
///
#define MAX_CODE 32


/// read_symbols reads characters from standard input, calculates
/// each symbol's frequency of appearance, and counts the number of
/// distinct symbols it sees.
/// @param maxcount the unsigned dimension of the syms array
/// @param syms array of Symbol structures initialized and filled
/// @pre  maxcount is a the capacity of the syms array.
/// @post  syms array is initialized and filled with histogram information.
/// @post  syms is ordered so that all used entries are before unused.
/// @post  unused syms array entries are initialized as unused.
///
int read_symbols(size_t maxcount, Symbol syms[]) {
    int pos = 0;
    char present;
    while ((present = fgetc(stdin)) != null) {
        
        /// If the current character has been seen before
        bool exists = false;

        /// Where in syms[] the character has been seen, if available.
        int existsWhere;
        
        for (size_t i = 0; i < pos; i ++) {
            if (syms[i].symbol == present) {
                exists = true;
                existsWhere = i;
                break;
            }
        }
        if (exists) {
            syms[existsWhere].frequency ++;
        } else {
            syms[pos].frequency = 1;
            if (present == '\n') {
                syms[pos].frequency = 0;
            }
            syms[pos].symbol = present;
            pos++;
        }
    }
    return pos;
}


/// heap_init initializes the heap storage with all unused Node entries.
/// heap is a <em>pointer</em>, a reference to a heap structure.
//@param heap a valid pointer to a Heap structure
/// @pre  heap is a valid pointer and heap->capacity is uninitialized.
/// @post  heap->array has been initialized with unused Node entries.
/// @post  heap->size == 0.
/// @post  heap->capacity is initialized to dimension of heap->array (q.v).
///

void heap_init(Heap * heap) {
    heap -> size = 0;
    heap -> capacity = MAX_SYMS;
    for (int i = 0; i < MAX_SYMS; i++) {
        heap -> array[i].frequency = 0;
        heap -> array[i].num_valid = 0;
        for (int j = 0; j < MAX_SYMS; j++) {
            heap -> array[i].syms[j].frequency = 0;
            heap -> array[i].syms[j].bit = 0;
            heap -> array[i].syms[j].symbol = NUL;
            for (int k = 0; k < MAX_CODE; k++) {
                heap -> array[i].syms[j].codeword[k] = NUL;
            }
    }
}


/// parent returns the parent value of a heap.
/// takes an int, and returns one less cut in half.
/// returns -1 if the value has no parent.
int parent(int i) {
    if (i == 0) {
        return(-1);
    }
    return ((i - 1) / 2);
}

/// childl returns the leftmost child.
/// it's just double plus one.
/// returns -1 if the value is out of range.
int childl(int i) {
    i = i * 2 + 1;
    if (i > MAX_SYMS) {
        return (-1);
    }
    return (i);
}

/// childr is childl but it makes i one larger. yeah.
/// returns -1 if the value is out of range.
int childr(int i) {
    i = i * 2 + 2;
    if (i > MAX_SYMS) {
        return (-1);
    }
    return (i);
}



/// heap_make fills heap with symbols from symlist and <em>heapifies</em> it.
/// heap is a <em>pointer</em>, a reference to an initialized heap.
/// length is the length of the symlist.
/// <p>algorithm:
/// Take each symbol from symlist and put it into the
/// next note in the heap, initializing the node's frequency
/// to that of the symbol, and setting the node's num_valid to 1.
/// @param heap pointer to an initialized and unused heap
/// @param length unsigned length of symlist
/// @param symlist array of Symbol structures
/// @pre  entries in symlist[0, ..., length-1] are valid Symbol objects.
/// @pre  heap is an initialized, unused heap of capacity >= length.
/// @pre  length <= heap->capacity. symlist is array of Symbol structs.
/// @post  heap->array is filled with Node entries in min-heap order.
/// @post  heap->size == length.
/// @post  heap->array[i].syms[0] is distinct symbol for i in [0, ..., length-1].
///
void heap_make( Heap * heap, size_t length, Symbol symlist[] ) {
    /// converts symlist to a populated array.
    heap -> size = length;
    read_symbols(MAX_COUNT, symlist[]);
    /// this loop will determine the location we're seeking.
    for (int i = 0; i < length; i ++) {
        Symbol holster = symlist[i];
        heap -> array[i].syms[0] = holster;
        int place_one = i;
        int place = parent(i);
        /// logic here?
        /// we try the element we just added against its parent. if it's less, we swap positions. that's what holster's for.
        /// otherwise, the process is complete and we can break out of the loop.
        while (place != -1) {
            if (heap -> array[place_one].syms[0].frequency < heap -> array[place].syms[0].frequency) {
                hoslter = array[place].syms[0].frequency;
                heap -> array[place_one].syms[0] = heap -> array[place].syms[0];
                heap -> array[place].syms[0] = holster;
                place_one = place;
                place = parent(place);
            } else {
                break;
            }
        }
    }
}


/// heap_remove removes and returns a node structure.
/// heap is a <em>pointer</em>, a reference to an initialized heap.
/// @param heap a pointer to the heap data structure
/// @return a Node structure that is the saved top entry
/// @pre  heap->size > 0 (fatal error to remove from an empty heap)
/// @post  heap->size has decreased by 1.
/// @post  remaining heap is in proper heap order.
///
/// <p>Algorithm:
/// <ul><li>
/// Save the top of the heap at index 0.
/// </li><li>
/// Reduce the size of the heap by
/// copying the last heap entry into the top location and
/// replacing the last location's entry with values representing an
/// unused Node object.
/// </li><li>
/// Restore the heap order by sifting the top entry down the heap.
/// </li><li>
/// Return the saved top entry.
/// </li></ul>
///
Node heap_remove( Heap * heap ) {
    /// this doesn't seem to complicated unless i'm absolutely butchering the idea behind it.
    Symbol lowest = heap -> array[0].syms[0];
    /// possible - I don't know how data is stored. heap -> size could be 1 greater than what I want.
    heap -> array[0].syms[0] = heap -> array[heap -> size].syms[0];
    heap -> size --;
    int position = 0;
    while (position < heap-> size) {
        Symbol this = heap -> array[position].syms[0];
        int left = childl(position);
        int right = childr(position);
        Symbol left_node = heap -> array[left].syms[0];
        Symbol right_node = heap -> array[right].syms[0];
        Symbol comparison;
        int left = 0;
        if (left_node.frequency < right_node.frequency) {
            comparison = left_node;
        } else {
            comparison = right_node;
            left = 1;
        }
        if (comparison.frequency < this.frequency) {
            heap -> array[position].syms[0] = comparison;
            if (left == 1) {
                heap -> array[right].syms[0] = this;
                position = right;
            } else {
                heap -> array[left].syms[0] = this;
                position  = left;
            }
        }  else {
            break;
        }
    }
}


/// 2/21/23, 10:23 AM
/// I'm done with the rough draft. I have no idea how to check this, and this editor is terrible for showing off
/// basic errors. I've spent ~ 10 hours on this already.
/// I'm in hell.
/// as soon as I figure out how to run this I can start the glorious debugging process.
/// also I have no idea where I can compare the frequencies assuming there's multiple symbols in each node.
/// that's a problem for later.
/// I've already got BASICALLY a 30% on this project so I could call it quits right now.