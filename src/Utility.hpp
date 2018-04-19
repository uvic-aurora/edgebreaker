// Utility.hpp
// Description:
//      Declare or define the shared variables, types, functions and classes 
//      that used by both the Encoder and Decoder class.

#ifndef utility_hpp
#define utility_hpp

#include <iostream>
#include <cmath>
#include <limits>
#include <SPL/math.hpp>
#include <SPL/arithCoder.hpp>
#include <SPL/bitStream.hpp>
#include <CGAL/Cartesian.h>
#include "Context_selector.hpp"


//////////////////////////////////////////////////////////
// Symbolic type name
//////////////////////////////////////////////////////////
typedef unsigned long Vertex_index;
typedef int Quant_index;
typedef unsigned long long Ullong;


//////////////////////////////////////////////////////////
// Input/Output bit stream related variables
//////////////////////////////////////////////////////////
const int max_bits_to_put = 30; // Maximum No. of bits to output by putBit function
const int redundant_bits = 32 - max_bits_to_put; // Redundant bits to output
const int fraction_bits = 32; // No. of bits in fraction part of the fixed point representation
const int fixed_represent_bits = 64; // Total No. of bits in fixed point representation


//////////////////////////////////////////////////////////
// Triangle types
//////////////////////////////////////////////////////////
enum Triangle_type : char {
    C = 'C', // C type triangle
    L = 'L', // L type triangle
    E = 'E', // E type triangle
    R = 'R', // R type triangle
    S = 'S', // S type triangle
    M = 'M', // M type triangle
    H = 'H', // M' type triangle
    I = 'I'  // This symbol just use for triangle type initialization
};


//////////////////////////////////////////////////////////
// Arithmetic coding related declarations
//////////////////////////////////////////////////////////
const int coor_max_bits = 31;  // Maximum No. of bits in value part to be encoded
const int f_levels = 3;    // Levels of full trees in UI function

// Total number of contexts used to encode all bits 
const int max_contexts = Context_selector::total_contexts(coor_max_bits, f_levels);
const int total_contexts_cnt = 3 * max_contexts;

// Some basic types to be used with SPL BinArithEncoder/BinArithEDecoder class
typedef SPL::BinArithEncoder MyArithEncoder;
typedef SPL::BinArithDecoder MyArithDecoder;


//////////////////////////////////////////////////////////
// Some basic types to be used with CGAL Point_3 class
//////////////////////////////////////////////////////////
typedef double Real;
typedef CGAL::Cartesian<Real> Kernel;
typedef Kernel::Point_3 Point;


//////////////////////////////////////////////////////////
// Useful classes
//////////////////////////////////////////////////////////

/*******************************************************************************
Class: Mesh_handle
Description:
    A class stores each mesh handle's information. Three private members are 
    included in the Mesh_handle class: the position of the associated gate in 
    the stack, the offset between the gate and the reached point, and the No. of 
    S operation skipped.
*******************************************************************************/
class Mesh_handle {
public:
    // A constructor create a Handle object with given parameters
    // Parameters:
    //      position: Associated gate's position in the stack
    //      offset: Offset between the gate and the reached point
    //      skip_cnt: No. of S operation skipped
    Mesh_handle(Vertex_index position, Vertex_index offset, Vertex_index skip_cnt) 
        : position_(position), offset_(offset), skip_cnt_(skip_cnt) {}
    
    // Destructor
    ~Mesh_handle() {}
    
    // Get associated gate's position
    Vertex_index position() const;
    
    // Get offset value
    Vertex_index offset() const;
    
    // Get No. of S operation skipped
    Vertex_index skip_cnt() const;
    

private:
    // The position of the associated gate in the stack
    Vertex_index position_;
    
    // The offset between the gate and the reached point
    Vertex_index offset_;
    
    // No. of S operation skipped
    Vertex_index skip_cnt_;
};


/*******************************************************************************
Class: Triangle_facet
Description:
    A class stores each triangle's three vertices label/index.
*******************************************************************************/
class Triangle_facet {
public:

    // A constructor create a Triangle_facet object with given parameters
    // Parameters:
    //      a: Index of first triangle vertex
    //      b: Index of second triangle vertex
    //      c: Index of third triangle vertex
    Triangle_facet(Vertex_index a, Vertex_index b, Vertex_index c);
    
    // Copy constructor
    Triangle_facet(const Triangle_facet& tri_label) = default;
        
    // Move constructor
    Triangle_facet(Triangle_facet&& tri_label) = default;
    
    // Copy assignment operator
    Triangle_facet& operator=(const Triangle_facet& tri_label) = default;
    
    // Move assignment operator
    Triangle_facet& operator=(Triangle_facet&& tri_label) = default;
    
    // Destructor
    ~Triangle_facet() {}
    
    // Gets corresponding vertex index
    // Parameters: 
    //      n: The vertex sequence id in current triangle
    // Returns: 
    //      The corresponding vertex index
    Vertex_index get_index(int n) const;
    
    
private:
    // An array to store triangle's three indices
    Vertex_index facet[3];  
};


/*******************************************************************************
Function: std::ostream& operator<<(std::ostream& outStream, const Triangle_facet& tri_label);
Description:
    Stream Inserter for Triangle_facet class
*******************************************************************************/    
std::ostream& operator<<(std::ostream& outStream, const Triangle_facet& tri_label); 
// {
    // outStream << tri_label.get_index(0) << " " << tri_label.get_index(1) << " " 
              // << tri_label.get_index(2);
    // return outStream;
// }


//////////////////////////////////////////////////////////
// Useful functions
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: sign()
Description:
    The signum function of a real number val 
*******************************************************************************/
// template<class T>
// int sign(T val) {
    // return (T(0) < val) - (val < T(0));
// }


/*******************************************************************************
Function: Vertex_index pow2int(int exp);
Description:
    The power function for integer, the base of power is 2
    
Parameters: 
        exp: The exponent value
    
Returns:
        The result of raising base to the power exponent
*******************************************************************************/
Vertex_index pow2int(int exp);


/*******************************************************************************
Function: double pow2double(Quant_index coef, int exp);
Description:
    The power function for double, the base of power is 2
    
Parameters: 
        coef: The coefficient value
        exp: The exponent value
    
Returns:
        The result of raising base to the power exponent
*******************************************************************************/
double pow2double(Quant_index coef, int exp);


/*******************************************************************************
Function: bool step_size_convert(double coord, Quant_index& coef, int& exp);
Description:
    Convert the quantization step size from double type to scientific notation
    
Parameters: 
    coord: The step size coordinate
    coef: The coefficient part of the scientific notation
    exp: The exponential part of the scientific notation
    
Returns:
    Upon success, true is returned; otherwise, false is returned
*******************************************************************************/
bool step_size_convert(double coord, Quant_index& coef, int& exp);


/*******************************************************************************
Function: geometry_prediction(Point* a, Point* b, Point* d);
Description:
    Parallelogram prediction function. This function predicts the vertex c's 
    location using parallelogram prediction scheme

Parameters: 
        a: The first vertex from current triangle (a, b, c)
        b: The second vertex from current triangle (a, b, c)
        d: The third vertex of the previous processed triangle (a, b, d)
    
Returns:
        The predicted vertex location 
*******************************************************************************/
Point geometry_predict(Point* a, Point* b, Point* d);


// #include "Utility_impl.hpp"
#endif