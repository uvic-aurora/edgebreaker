// Decoder.hpp
// Description:
//     The Decoder class uses both the Edgebreaker algorithm and the 
//     parallelogram predict scheme to decompress the 3D triangle mesh.

#ifndef Decoder_hpp
#define Decoder_hpp

#include "Utility.hpp"
#include "Circ_list.hpp"
#include "My_stack.hpp"
#include "Context_selector.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <stack>
#include <unistd.h>


/*******************************************************************************
Structure: Mesh_vertex
Description:
    This structure represents each reconstructed mesh's vertex. Two members are 
    included in this Mesh_vertex structure. One is the vertex coordinates, the 
    other is a flag shows whether the current vertex has been predicted or not.
*******************************************************************************/
struct Mesh_vertex {
    // The vertex coordinates 
    Point point;
    
    // A flag shows whether the vertex has been predicted or not
    bool prediction;
};


/*******************************************************************************
Class: Decoder
Description:
    A class uses for triangle mesh decompression.
    This class uses the Edgebreaker algorithm and the parallelogram predict 
    scheme to decompress the triangle mesh, writes the decompressed triangle
    mesh in OFF format to the standard output stream.
    
Preconditions:
    The input compressed triangle mesh must stored in EB file format
*******************************************************************************/
class Decoder {
public:
    // A constructor create a Decoder object with given parameter
    // Parameters: 
    //      ifile: EB file input stream
    Decoder(std::istream& ifile);
    
    // Destructor
    ~Decoder();
    
    // Op-code sequence vector generation function. The codeword from code 
    // series 1 of King's 3.67V bit paper is used
    // Parameters: 
    //      binary_string: Binary op-code string use the codeword from code series 1
    //      hist_vec: The op-code sequence vector, stores all the detected 
    //                triangle type.
    static void get_history_code1(const std::string& binary_string, 
                                  std::vector<Triangle_type>& hist_vec);
    
    // Op-code sequence vector generation function. The codeword from code 
    // series 2 of King's 3.67V bit paper is used
    // Parameters: 
    //      binary_string: Binary op-code string use the codeword from code series 2
    //      hist_vec: The op-code sequence vector, stores all the detected 
    //                triangle type.
    static void get_history_code2(const std::string& binary_string, 
                                  std::vector<Triangle_type>& hist_vec);
    
    // Op-code sequence vector generation function. The codeword from code 
    // series 3 of King's 3.67V bit paper is used
    // Parameters: 
    //      binary_string: Binary op-code string use the codeword from code series 3
    //      hist_vec: The op-code sequence vector, stores all the detected 
    //                triangle type. 
    static void get_history_code3(const std::string& binary_string, 
                                  std::vector<Triangle_type>& hist_vec);
    
    // 3D Triangle mesh decompression function. The function performs the whole 
    // decompression procedure for the compressed triangle mesh. The decompressed 
    // triangle mesh will be output to the standard output stream in OFF file format
    void mesh_decompression();
    
    
private:
    // Operation distinguish function. This function distinguishes among the 
    // S, M and M' type of operations. This function is called if and only if 
    // the compressed triangle mesh contains handle or hole
    void distinguish_opcode();
    
    // Arithmetic decoding function for geometry processing. This function uses 
    // the arithmetic decode functions from the SPL library to decodes the 
    // prediction errors for each vertex's 3 coordinates
    // Returns:
    //      The decoded vertex coordinate
    
    // Arithmetic decode function. It uses the arithmetic coding scheme to decode 
    // the integer prediction error of each vertex. The arithmetic decode function 
    // from the SPL library is called inside the function
    // Returns: 
    //      The integer prediction error of current vertex's given coordinate
    int vertex_coordinate_decode();
    
    // EB file reading function. This function reads the entire EB file from the 
    // given input stream, generates the op-code sequence vector, generates M 
    // table and M' table
    // Preconditions:
    //      The compressed triangle mesh must stored in EB file format
    void read_ebfile();
    
    // Geometry processing related function. It reconstructs the third vertex of 
    // the current input triangle.
    // Parameters: 
    //      tri_label: A Triangle_facet object which contains the given 
    //                 triangle's three vertices label/index
    void geometry_processing(Triangle_facet tri_label);
    
    // Decompression preprocessing phase function. This function gets the total 
    // triangle count, the external vertices count, and generates the offset 
    // table for S operation
    void initializing();
    
    // Decompression generation phase function. This function creates 
    // the triangle-vertices table based on the information gets from the 
    // preprocessing phase
    void table_generating();
    
    // OFF file written function. This function writes the decompressed triangle 
    // mesh in OFF format to the standard output
    void write_output();
    
    
    // The op-code sequence vector, stores all the detected triangle type
    std::vector<Triangle_type> history_vec;
    
    // The points list vector, stores the prediction error for each vertex
    std::vector<Point> points_vec;
    
    // The reconstructed vertices vector, stores all the reconstructed vertices
    std::vector<Mesh_vertex> vertices_vec;
    
    // The M table uses to store the information of each hole
    // Explain for std::pair type:
    //      First: the number of S operation skipped
    //      Second: the length of current hole
    std::vector<std::pair<Vertex_index, Vertex_index>> m_table;
    
    // The M' table uses to store the information of each handle
    std::vector<Mesh_handle> h_table;
    
    // The quantization step size
    Point step_size;
    
    // The previous reconstructed vertex index from triangle (a, b, d) 
    Vertex_index prev_vertex_d;
    
    // A circular doubly linked list contains all boundary vertices' index
    Circ_list<Vertex_index> b_list;
    
    // Triangle-vertices incident table
    std::vector<Triangle_facet> tv_table;
    
    // S operation offset table
    std::vector<Vertex_index> s_offset;
    
    // A stack to store the sub boundary list generated by S type triangle
    // Explain for std::pair type:
    //      The point reconstruction is relied on the previous reconstructed 
    //      vertex. Since the previous reconstructed vertex is updated each time 
    //      when we process the right side sub-mesh of the S type triangle, 
    //      the program needs to remember the previous reconstructed vertex for 
    //      current S type gate when later processes the left side sub-mesh
    //      First: the left side sub-mesh of current S type triangle
    //      Second: the previous reconstructed vertex index for the first triangle 
    //              of the left side sub-mesh
    My_stack<std::pair<Circ_list<Vertex_index>, Vertex_index>> list_stack;
    
    // Input stream to read the EB file
    std::istream& ifile_eb;
    
    // External vertices count
    Vertex_index e_vertex;
    
    // A count represents the No. of triangle processed so far
    Vertex_index triangle_cnt;
        
    // M operation count
    Vertex_index m_cnt;
    
    // M' operation count
    Vertex_index h_cnt;
    
    // S operation count
    Vertex_index s_cnt;
    
    // A string stream stores the encoded integer geometry prediction errors of 
    // each mesh's vertex
    std::stringstream geom_in;
    
    // Input bit stream
	SPL::InputBitStream in;
    
    // An arithmetic decoder object to decodes the geometry prediction errors
    MyArithDecoder dec;
};


#include "decoder.cpp"
#endif