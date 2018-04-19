#include <cassert>
#include <limits>
#include "Utility.hpp"


//////////////////////////////////////////////////////////
// Functions from Mesh_handle class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Vertex_index position() const;
Description:
    Get associated gate's position
*******************************************************************************/
Vertex_index Mesh_handle::position() const {
    return position_;
}
    
    
/*******************************************************************************
Function: Vertex_index offset() const;
Description:
    Get offset value
*******************************************************************************/
Vertex_index Mesh_handle::offset() const {
    return offset_;
}

    
/*******************************************************************************
Function: Vertex_index skip_cnt() const;
Description:
    Get No. of S operation skipped
*******************************************************************************/
Vertex_index Mesh_handle::skip_cnt() const {
    return skip_cnt_;
}
    

/*******************************************************************************
Function: std::ostream& operator<<(std::ostream& outStream, const Mesh_handle& h);
Description:
    Stream Inserter for Mesh_handle class
*******************************************************************************/    
std::ostream& operator<<(std::ostream& outStream, const Mesh_handle& h) {
    outStream << h.position() << " " << h.offset() << " " << h.skip_cnt();
    return outStream;
}



//////////////////////////////////////////////////////////
// Functions from Triangle_facet class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Triangle_facet(Vertex_index a, Vertex_index b, Vertex_index c);
Description:
    Triangle_facet class constructor 
*******************************************************************************/
Triangle_facet::Triangle_facet(Vertex_index a, Vertex_index b, Vertex_index c) {
    facet[0] = a;
    facet[1] = b;
    facet[2] = c;
}


/*******************************************************************************
Function: Vertex_index get_index(int n) const;
Description:
    Gets corresponding vertex index
*******************************************************************************/
Vertex_index Triangle_facet::get_index(int n) const {
    assert(n == 0 || n == 1 || n == 2);
    return facet[n];
}


/*******************************************************************************
Function: std::ostream& operator<<(std::ostream& outStream, const Triangle_facet& tri_label);
Description:
    Stream Inserter for Triangle_facet class
*******************************************************************************/    
std::ostream& operator<<(std::ostream& outStream, const Triangle_facet& tri_label) {
    outStream << tri_label.get_index(0) << " " << tri_label.get_index(1) << " " 
              << tri_label.get_index(2);
    return outStream;
}



//////////////////////////////////////////////////////////
// Useful functions
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Vertex_index pow2int(int exp);
Description:
    The power function for integer, the base of power is 2
*******************************************************************************/
Vertex_index pow2int(int exp) {
    // Initialize the result
    Vertex_index result = 1;
    
    // Make sure the input exponential value is always positive
    assert(exp >= 0 && exp < 31);
    
    if (exp == 0) {
        return result;
    }
    else {
        for (int i = 0; i < exp; ++i) {
            result *= 2;
        }
    }
    
    return result;
}


/*******************************************************************************
Function: double pow2double(Quant_index coef, int exp);
Description:
    The power function for double, the base of power is 2
*******************************************************************************/
double pow2double(Quant_index coef, int exp) {
    // Initialize the result
    double result = coef;
    
    if(exp < 0) {
        // The exponent is negative
        for(int i = 0; i < std::abs(exp); ++i) {
            result *= 2.0;
        }
    }
    else if (exp >= 0) {
        // The exponent is positive
        for(int i = 0; i < exp; ++i) {
            result /= 2.0;
        }
    }
    
    return result;
}


/*******************************************************************************
Function: bool step_size_convert(double coord, Quant_index& coef, int& exp);
Description:
    Convert the quantization step size from double type to scientific notation
*******************************************************************************/
bool step_size_convert(double coord, Quant_index& coef, int& exp) {
    // Variable initialize
    int count = 0;
    
    for (;;) {
        if (coord >= 1.0 && coord < 2.0) {
            break;
        }
        else if (coord < 1.0) {
            coord *= 2.0;
            ++count;
        }
        else {
            coord /= 2.0;
            --count;
        }
    }
    
    // Update the variable
    exp = count + max_bits_to_put - 1;
    
    double coef_double = coord * pow2int(max_bits_to_put - 1);
    if (coef_double > std::numeric_limits<Quant_index>::max()) {
        std::cerr << "ERROR: Coefficient overflow!\n";
        return false;
    }
    else {
        coef = coef_double;
        return true;
    }
}


/*******************************************************************************
Function: geometry_prediction(Point* a, Point* b, Point* d);
Description:
    Parallelogram prediction function. This function predicts the vertex c's 
    location using parallelogram prediction scheme
*******************************************************************************/
Point geometry_predict(Point* a, Point* b, Point* d) {
    if (a == nullptr && b == nullptr && d == nullptr) {
        // No point is known before the prediction
        return CGAL::ORIGIN;
    }
    else if (b == nullptr && d == nullptr) {
        // Only point a is known before the prediction
        return *a;
    }
    else if (d == nullptr) {
        // Points a and b are known before the prediction
        int x = ((a -> x()) + (b -> x())) / 2;
        int y = ((a -> y()) + (b -> y())) / 2;
        int z = ((a -> z()) + (b -> z())) / 2;

        return Point(x, y, z);
    }
    
    // Points a, b, d are all known before the prediction
    int x = ((a -> x()) + (b -> x()) - (d -> x()));
    int y = ((a -> y()) + (b -> y()) - (d -> y()));
    int z = ((a -> z()) + (b -> z()) - (d -> z()));
    
    return Point(x, y, z);
}


