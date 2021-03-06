// Encoder.hpp
// Description:
//     The Encoder class uses both the Edgebreaker algorithm and the 
//     parallelogram predict scheme to compress the 3D triangle mesh.

#ifndef Encoder_hpp
#define Encoder_hpp

#include "Utility.hpp"
#include "My_stack.hpp"
#include "Context_selector.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <iterator>
#include <cstdlib>
#include <list>
#include <algorithm>
#include <unistd.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>


//////////////////////////////////////////////////////////
// Class inheritance from the CGAL library
//////////////////////////////////////////////////////////

/*******************************************************************************
Class: My_Vertex
Description:
    A vertex type to be used with Polyhedron_3 class that includes some 
    self defined information. Two elements are added to the original vertex 
    type: vertex index and vertex mark. Mark value is used for triangle type 
    distinguish, index value is used for geometry prediction.
*******************************************************************************/
template <class Refs, class Traits, class P>
class My_Vertex : public CGAL::HalfedgeDS_vertex_base<Refs, Traits, P> {
public:
    // Constructor
	My_Vertex() : CGAL::HalfedgeDS_vertex_base<Refs, Traits, P>() {}
    My_Vertex(const Kernel::Point_3& p) : 
        CGAL::HalfedgeDS_vertex_base<Refs, Traits, P>(p) {}
    
    // Get current vertex mark
	int get_mark() const;
    
    // Set current vertex mark
    void set_mark(int mark_value);
    
    // Get current vertex index
    int get_index() const;
    
    // Set current vertex index
    void set_index(int index_value);
    
    // Get current vertex prediction flag
	bool get_flag() const;
    
    // Set current vertex prediction flag
    void set_flag(bool flag);

    
private:    
    // The mark value of the vertex
    int mark; 
    
    // The index of the vertex
    Vertex_index index;
    
    // A flag shows whether the vertex has been predicted or not
    bool prediction;
};


/*******************************************************************************
Class: My_Halfedge
Description:
    A halfedge type to be used with Polyhedron_3 class that includes some 
    self defined information. Mark value is added to the halfedge class, it is 
    used to indicate whether the current halfedge is on the bounding loop. 
    Two halfedge handle members are added to the class, they are used to 
    represent the predecessor and successor halfedge on the bounding loop.
*******************************************************************************/
template < class Refs, class TP, class TV, class TF>
class My_Halfedge : public CGAL::HalfedgeDS_halfedge_base<Refs, TP, TV, TF> {
public:
    typedef typename Refs::Halfedge_handle Halfedge_handle;
    
    // Constructor
	My_Halfedge() : CGAL::HalfedgeDS_halfedge_base<Refs, TP, TV, TF>() {}
    
    // Get the current vertex mark
	int get_mark() const;
    
    // Set the current vertex mark
    void set_mark(int mark_value);
    
    // Get the previous halfedge on border
	Halfedge_handle prev_on_border() const;
    
    // Get the next halfedge on border
	Halfedge_handle next_on_border() const;
    
    // Set the previous halfedge on border
    void set_prev_on_border(Halfedge_handle prev);
    
    // Set the next halfedge on border
    void set_next_on_border(Halfedge_handle next);

    
private:
    // The mark value of the vertex
    int mark; 
    
    // The previous halfedge on border
    Halfedge_handle prev_halfedge_on_border;

    // The next halfedge on border
    Halfedge_handle next_halfedge_on_border;
};


/*******************************************************************************
Class: MyItems
Description:
    An items type to be used with Polyhedron_3 class that makes use of the 
    above My_Vertex and My_Halfedge type
*******************************************************************************/
struct MyItems : public CGAL::Polyhedron_items_3 {
	template <class Refs, class Traits>
	struct Vertex_wrapper {
		typedef My_Vertex<Refs, CGAL::Tag_true, typename Traits::Point_3> Vertex;
	};
    
    template < class Refs, class Traits>
    struct Halfedge_wrapper {
        typedef My_Halfedge<Refs, CGAL::Tag_true, CGAL::Tag_true, CGAL::Tag_true> Halfedge;
    };
};

// The polyhedron types
typedef CGAL::Polyhedron_3<Kernel, MyItems> Polyhedron;


/*******************************************************************************
Function: getBorderLoops()
Description:
    The function finds all of the border loops in the polyhedron mesh. For each 
    border loop that is found, one representative border halfedge from the loop 
    is written to the iterator halfedgeIter and the loop size is written to the 
    iterator sizeIter.
    
Preconditions:
    The border must be normalized
    
Parameters:
    mesh: The polyhedron mesh
    halfedgeIter: An iterator for writing each representative halfedge
    sizeIter: An iterator for writing the size of each loop

Return:
    int: Number of border loops
*******************************************************************************/
template <class P, class I1, class I2>
int getBorderLoops(const P& polyMesh, I1 halfedgeIter, I2 sizeIter);


/*******************************************************************************
Function: getConnectedComponents()
Description:
    The function determines the connected components of the mesh. (Vertices are 
    in the same connected component if they are connected by a path consisting 
    of one or more edges.) For each connected component, a handle corresponding 
    to a single representative vertex in the component is written to the output 
    iterator vertexIter.

Parameters:
    mesh: The polyhedron mesh
    vertexIter: An output iterator for writing vertex handles

Return:
    int: Number of connected components
*******************************************************************************/
template <class P, class I>
int getConnectedComponents(const P& polyMesh, I vertexIter);


/*******************************************************************************
Class: Encoder
Description:
    A class uses for triangle mesh compression.
    This class first pre-processes the input triangle mesh, finds the initial 
    halfedge/gate for mesh compression. Then it uses the Edgebreaker algorithm 
    and the parallelogram predict scheme to compress the triangle mesh 
    and generates the final EB file. The output EB file is written to the given 
    output stream.
    
Preconditions:
    The input polygon mesh must in the triangle type
*******************************************************************************/
class Encoder {
public:
    // Typedef for halfedge and vertex
    typedef Polyhedron::Halfedge_handle Halfedge_handle;
    typedef Polyhedron::Halfedge_iterator Halfedge_iterator;
    typedef Polyhedron::Halfedge_const_iterator Halfedge_const_iterator;
    typedef Polyhedron::Vertex_handle Vertex_handle;
    typedef Polyhedron::Vertex_const_handle Vertex_const_handle;
    typedef Polyhedron::Vertex_iterator Vertex_iterator;
    typedef Polyhedron::Vertex_const_iterator Vertex_const_iterator;

    // A constructor create a Encoder object with given parameters
    // Parameters: 
    //      mesh: Input triangle mesh
    //      ofile: EB file output stream
    //      quan_size: Quantization step size
    Encoder(Polyhedron mesh, std::ostream& ofile, Point quan_size);
    
    // Destructor
    ~Encoder();
    
    // 3D triangle mesh compression function. Inside the function, it first 
    // pre-processes the input triangle mesh, then it uses Edgebreaker algorithm
    // and parallelogram predict scheme to compresses the pre-processed triangle
    // mesh. The compressed triangle mesh will be output to the standard output 
    // stream in EB file format
    void mesh_compression();
    
    // Binary history string generation function. The codeword from code series 
    // 1 of King's 3.67V bit paper is used
    // Parameters: 
    //      hist_vec: The op-code sequence vector, stores all the detected 
    //                triangle type
    //      binary_string: The binary op-code string use the codeword from 
    //                     code series 1 
    static void write_history_code1(const std::vector<Triangle_type>& hist_vec, 
                                    std::string& binary_string);
    
    // Binary history string generation function. The codeword from code series 
    // 2 of King's 3.67V bit paper is used
    // Parameters: 
    //      hist_vec: The op-code sequence vector, stores all the detected 
    //                triangle type
    //      binary_string: The binary op-code string use the codeword from 
    //                     code series 2
    static void write_history_code2(const std::vector<Triangle_type>& hist_vec, 
                                    std::string& binary_string);
    
    // Binary history string generation function. The codeword from code series 
    // 3 of King's 3.67V bit paper is used
    // Parameters: 
    //      hist_vec: The op-code sequence vector, stores all the detected 
    //                triangle type
    //      binary_string: The binary op-code string use the codeword from 
    //                     code series 3
    static void write_history_code3(const std::vector<Triangle_type>& hist_vec, 
                                    std::string& binary_string);
    
    // Code series determine function. This function decides codeword from which 
    // code series is used in the binary history string
    // Parameters: 
    //      hist_vec: The op-code sequence vector, stores all the detected 
    //                triangle type
    // Returns: 
    //      The code series that adopted to encode the op-code sequence vector
    static int code_series_determine(const std::vector<Triangle_type>& hist_vec);
    
    
private:  
    // Mesh preprocessing function. If the input mesh is closed, an arbitrary 
    // halfedge is chosen as the initial gate. If mesh contains boundary, it
    // calls the boundary_detection() to detect the mesh's boundary, updates 
    // boundary vertices and halfedges, and then chooses the initial gate for  
    // mesh compression
    void preprocessing();
 
    // Mesh boundary detection function. This function detects the boundary of 
    // the mesh (longest bounding loop). The function is called if and only if 
    // the input triangle mesh is NOT closed
    void boundary_detection();
    
    // Geometry processing related function. It processes all mesh's vertices.
    // Parallelogram prediction scheme is used inside the function.
    // The geometry prediction is processed at the same time with the 
    // connectivity processing. Inside the function, it uses the active gate to 
    // get the four points for parallelogram prediction.
    void geometry_processing();
    
    // Triangle type distinguish function. This function identifies the  
    // topological relation between the current triangle which incident upon 
    // the active gate and the boundary of remaining part of the mesh
    // Returns:
    //      Current triangle's topological relation. This topological relationship
    //      is represented by a character from the set {C, L, R, E, S, M, H}
    Triangle_type tri_type_identify();
    
    // C type triangle processing function. It updates vertices' and halfedges' 
    // mark, updates predecessor and successor relation on the current bounding 
    // loop. The active gate is also updates to the given position
    void process_c_type_triangle();
    
    // E type triangle processing function. It updates vertices' and halfedges' 
    // mark, updates predecessor and successor relation on the current bounding 
    // loop. The active gate is also updates to the given position
    void process_e_type_triangle();
    
    // L type triangle processing function. It updates vertices' and halfedges' 
    // mark, updates predecessor and successor relation on the current bounding 
    // loop. The active gate is also updates to the given position
    void process_l_type_triangle();
    
    // R type triangle processing function. It updates vertices' and halfedges' 
    // mark, updates predecessor and successor relation on the current bounding 
    // loop. The active gate is also updates to the given position
    void process_r_type_triangle();
    
    // S type triangle processing function. It updates vertices' and halfedges' 
    // mark, updates predecessor and successor relation on the current bounding 
    // loop. The active gate is also updates to the given position
    void process_s_type_triangle();
    
    // M' type triangle processing function. It updates vertices' and halfedges' 
    // mark, updates predecessor and successor relation on the current bounding 
    // loop. The active gate is also updates to the given position
    void process_h_type_triangle();
    
    // M type triangle processing function. It updates vertices' and halfedges' 
    // mark, updates predecessor and successor relation on the current bounding 
    // loop. The active gate is also updates to the given position
    void process_m_type_triangle();
    
    // Triangle mesh compression function. It performs the entire compression 
    // procedure using Edgebreaker algorithm and parallelogram prediction scheme 
    // for the input triangle mesh
    void compressing();
    
    // Arithmetic encode function. It uses the arithmetic coding scheme to encode 
    // the integer prediction error of each vertex. The arithmetic encode function 
    // from the SPL library is called inside the function
    // Parameters: 
    //      predict_error: The integer prediction error of current vertex's 
    //                     given coordinate
    void vertex_coordinate_encode(int predict_error);
    
    // EB file written function. This function writes the compressed triangle 
    // mesh in EB format to the standard output
    void write_ebfile();
    
    
    // Input triangle mesh
    Polyhedron polyMesh;
    
    // The active gate (a halfedge that compression procedure works with)
    Halfedge_handle gate;
    
    // The quantization step size
    Point step_size;
    
    // The op-code sequence vector, stores all the detected triangle type 
    std::vector<Triangle_type> history_vec;
    
    // The M table uses to store the information of each hole
    // Explain for std::pair type:
    //      First: the number of S operation skipped
    //      Second: the length of current hole
    std::vector<std::pair<Vertex_index, Vertex_index>> m_table;
    
    // The M' table uses to store the information of each handle
    std::vector<Mesh_handle> h_table;
    
    // A findable stack stores the active gate of the left side sub-mesh of each
    // S type triangle
    My_findable_stack<Halfedge_handle> s_gate;
    
    // No. of S type triangle be skipped by the M op-code
    Vertex_index skip_m;
    
    // No. of S type triangle be skipped by the M' op-code
    Vertex_index skip_h;
    
    // Index of each vertex
    Vertex_index vertices_index;
    
    // Count for processed triangle
    Vertex_index processed_cnt;
    
    // Output stream to write the EB file
    std::ostream& ofile_eb;
    
    // A string stream stores the encoded integer geometry prediction errors of 
    // each mesh's vertex
    std::stringstream geom_out;
    
    // Output bit stream
    SPL::OutputBitStream out;
    
    // An arithmetic encoder object to encodes the geometry prediction errors
    MyArithEncoder enc;
};


#include "encoder.cpp"
#endif