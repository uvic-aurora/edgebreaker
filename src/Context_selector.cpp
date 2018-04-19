#include "Context_selector.hpp"
#include <cassert>
#include <cmath>


/*******************************************************************************
Function: Context_selector(int no_of_bits, int full_trees);
Description:
    Context_selector class constructor
*******************************************************************************/
Context_selector::Context_selector(int no_of_bits, int full_trees_level) 
: n_bits(no_of_bits), f_levels(full_trees_level), delta(0), context_id(0), encode_bits(0) {
    // make sure 1 <= f_levels <= n_bits
    assert(f_levels >= 1 && f_levels <= n_bits);
    
    // UI function is n levels full tree
    if (n_bits == f_levels) {
        context_id = (1 << (n_bits - 1));
        delta = context_id / 2;
    }
    
    // UI function has (n - f) levels linear tree and f levels full tree
    else {
        context_id = (1 << f_levels) + n_bits - f_levels - 1;
        
        if (n_bits - f_levels == 1) {
            // Current UI function only has one level linear tree
            delta = context_id / 2;
        }
        else {
            delta = 1;
        }
    }
    
    // Update encode bit 
    ++encode_bits;
}


/*******************************************************************************
Function: ~Context_selector();
Description:
    Context_selector class deconstructor
*******************************************************************************/
Context_selector::~Context_selector() {
    --delta;
    --context_id;
    --encode_bits;
}


/*******************************************************************************
Function: static int total_contexts(int no_of_bits, int full_trees_level);
Description:
    Returns the total number of contexts used to encode all bits
*******************************************************************************/
int Context_selector::total_contexts(int no_of_bits, int full_trees_level) {
    return (1 << full_trees_level) + no_of_bits - full_trees_level - 1;
}


/*******************************************************************************
Function: int get_context() const;
Description:
    Gets Context_selector id for the current input bit
*******************************************************************************/
int Context_selector::get_context() const {
    return (context_id - 1);
}


/*******************************************************************************
Function: bool encode_bit(int input_bit);
Description:
    Reads the bit that needs to encode, updates the context id for the next bit
*******************************************************************************/
bool Context_selector::encode_bit(int input_bit) {
    // Make sure input bit is binary
    if (input_bit != 0 && input_bit != 1) {
        return false;
    }
    
    // Bypass mode
    if (context_id == 0) {
        return true;
    }
    
    // Regular mode
    if (input_bit == 0) {
        // Context go to left child
        context_id -= delta;
    }
    else if (input_bit == 1) {
        // Context go to right child
        context_id += delta;
        
        if (n_bits - f_levels >= encode_bits) {
            // Enter bypass mode
            context_id = 0;
        }
    }
    
    // Update adjust value for the next input bit
    if (delta != 1) { delta /= 2; }
    else {
        if (context_id  == (1 << f_levels)) {
            // Current node is L node (node with its left child is full tree) 
            delta = context_id / 2;
        }
        else {
            // Context id still in the range of linear tree or encode the last bit
            delta = 1;
        }
    }
    
    // Update encode bit 
    ++encode_bits;
    
    return true;
}
    
    
