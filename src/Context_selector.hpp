// Context_selector.hpp
// Description:
//     This head file includes the implementation of the binarization scheme 
//     which is used by the arithmetic coding

#ifndef context_hpp
#define context_hpp

#include <iostream>
#include <iterator>


/*******************************************************************************
Class: Context_selector
Description:
    The implementation of binarization scheme which is used with the arithmetic 
    encoder and decoder class
*******************************************************************************/
class Context_selector {
public:
    
    // A constructor create a Context_selector object with given parameters, 
    // and initialize the binarization function
    // Parameters: 
    //      no_of_bits: No. of bits need to be encode (i.e. n)
    //      full_trees_level: Full tree with given levels (i.e. f)
    Context_selector(int no_of_bits, int full_trees_level);
    
    // Destructor
    ~Context_selector();
    
    // Returns the total number of contexts used to encode all bits
    // Parameters: 
    //      no_of_bits: No. of bits need to be encode (i.e. n)
    //      full_trees_level: Full tree with given levels (i.e. f)
    // Formula: 
    //      total_context = 2 ^ f + n - f - 1
    // Returns:
    //      The total number of contexts used to encode all bits
    static int total_contexts(int no_of_bits, int full_trees_level);
    
    // Gets context id for the current input bit
    // Returns:
    //      The context id used to encode the current input bit. The range of 
    //      context id is: [-1, 2^f + n - f - 2]. If -1 returned, the later 
    //      input bits are encode in bypass mode
    int get_context() const;
    
    // Reads a bit that needs to encode, updates the context id for the next bit
    // Parameters: 
    //      input_bit: Input binary bit
    bool encode_bit(int input_bit);

	
private:
    // No. of bits need to be encode (i.e. n)
    int n_bits;
    
    // Full trees with f levels (i.e. f)
    int f_levels;
    
    // The adjust value for the next input bit
    int delta;
    
    // Current context id
    int context_id;
    
    // No. of bits encoded so far
    int encode_bits;
};


// #include "Context_selector_impl.hpp"
#endif