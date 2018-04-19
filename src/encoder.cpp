#include "encoder.hpp"
#include <cassert>
#include <stack>
#include <bitset>
#include <iterator>


//////////////////////////////////////////////////////////
// Function implementation from My_Vertex class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: int get_vertex_mark() const;
Description:
    Get current vertex mark
*******************************************************************************/
template <class Refs, class Traits, class P>
int My_Vertex<Refs, Traits, P>::get_vertex_mark() const {
    return mark;
}


/*******************************************************************************
Function: void set_vertex_mark(int mark_value);
Description:
    Set current vertex mark
*******************************************************************************/
template <class Refs, class Traits, class P>
void My_Vertex<Refs, Traits, P>::set_vertex_mark(int mark_value) {
    mark = mark_value;
}


/*******************************************************************************
Function: Vertex_index get_index() const;
Description:
    Get current vertex index
*******************************************************************************/
template <class Refs, class Traits, class P>
Vertex_index My_Vertex<Refs, Traits, P>::get_index() const {
    return index;
}


/*******************************************************************************
Function: void set_index(Vertex_index index_value);
Description:
    Set current vertex index
*******************************************************************************/
template <class Refs, class Traits, class P>
void My_Vertex<Refs, Traits, P>::set_index(Vertex_index index_value) {
    index = index_value;
}


/*******************************************************************************
Function: bool get_flag() const;
Description:
    Get current vertex prediction flag
*******************************************************************************/
template <class Refs, class Traits, class P>
bool My_Vertex<Refs, Traits, P>::get_flag() const {
    return prediction;
}


/*******************************************************************************
Function: void set_flag(bool flag);
Description:
    Set current vertex prediction flag
*******************************************************************************/
template <class Refs, class Traits, class P>
void My_Vertex<Refs, Traits, P>::set_flag(bool flag) {
    prediction = flag;
}



//////////////////////////////////////////////////////////
// Function implementation from My_Vertex class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: int get_halfedge_mark() const;
Description:
    Get the current vertex mark
*******************************************************************************/
template < class Refs, class TP, class TV, class TF>
int My_Halfedge<Refs, TP, TV, TF>::get_halfedge_mark() const {
    return mark;
}


/*******************************************************************************
Function: void set_halfedge_mark(int mark_value);
Description:
    Set the current vertex mark
*******************************************************************************/
template < class Refs, class TP, class TV, class TF>
void My_Halfedge<Refs, TP, TV, TF>::set_halfedge_mark(int mark_value) {
    mark = mark_value;
}


/*******************************************************************************
Function: Halfedge_handle prev_on_border() const;
Description:
    Get the previous halfedge on border
*******************************************************************************/
template < class Refs, class TP, class TV, class TF>
typename Refs::Halfedge_handle My_Halfedge<Refs, TP, TV, TF>::prev_on_border() const {
    return prev_halfedge_on_border;
}


/*******************************************************************************
Function: Halfedge_handle next_on_border() const;
Description:
    Get the next halfedge on border
*******************************************************************************/
template < class Refs, class TP, class TV, class TF>
typename Refs::Halfedge_handle My_Halfedge<Refs, TP, TV, TF>::next_on_border() const {
    return next_halfedge_on_border;
}


/*******************************************************************************
Function: void set_prev_on_border(Halfedge_handle prev);
Description:
    Set the previous halfedge on border
*******************************************************************************/
template < class Refs, class TP, class TV, class TF>
void My_Halfedge<Refs, TP, TV, TF>::set_prev_on_border(Halfedge_handle prev) {
    prev_halfedge_on_border = prev;
}


/*******************************************************************************
Function: void set_next_on_border(Halfedge_handle next);
Description:
    Set the next halfedge on border
*******************************************************************************/
template < class Refs, class TP, class TV, class TF>
void My_Halfedge<Refs, TP, TV, TF>::set_next_on_border(Halfedge_handle next) {
    next_halfedge_on_border = next;
}



//////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: getBorderLoops()
Description:
    The function finds all of the border loops in the polyhedron mesh. For each 
    border loop that is found, one representative border halfedge from the loop 
    is written to the iterator halfedgeIter and the loop size is written to the 
    iterator sizeIter.
*******************************************************************************/
template <class P, class I1, class I2>
int getBorderLoops(P& polyMesh, I1 halfedgeIter, I2 sizeIter) {
    // The polyhedron types
    typedef typename P::Halfedge_iterator Halfedge_iterator;
    typedef typename P::Halfedge_handle Halfedge_handle;
    
    // Initialize for boundary and holes
    std::set<Halfedge_handle> border_edges;     // Set for all border halfedges
    int border_len = 0;    // Length of visited border halfedges
    int loop_cnt = 0;   // No. of loops in the mesh
    
    // Add all border halfedges to the border_edges set
    for (auto hit = polyMesh.border_halfedges_begin(); hit != polyMesh.halfedges_end(); ++hit) {
        //Move to the border halfedge
        ++hit;   
        
        // Check if current halfedge is border halfedge
        assert((*hit).is_border());
        
        // Add border halfedge to the border_edges vector
        border_edges.insert(&*hit);
    }
    
    while (!border_edges.empty()) {
        Halfedge_handle border_start = *(border_edges.begin());
        
        // Update the beginning halfedge for current bounding loop
        *(halfedgeIter++) = border_start;
        
        // Find the next halfedge for current bounding loop
        Halfedge_handle next_border = border_start -> next();

        // Erase beginning halfedge from border_edges set
        border_edges.erase(border_start);
        
        // Update the length of current bounding loop
        border_len = 1;
        
        // Find the rest halfedges on current bounding loop
        while (next_border != border_start) {
            // Update the loop length
            ++border_len;
            
            // The halfedge that needs to be deleted from the set
            Halfedge_handle edge_delete = next_border;
            
            // Find the next halfedge for current bounding loop
            next_border = next_border -> next();
            
            // Erase the detected halfedge from border_edges set
            border_edges.erase(edge_delete);
        }
        
        // Update the loop count and the length of current loop
        ++loop_cnt;
        *(sizeIter++) = border_len;
    }

    return loop_cnt;
}


/*******************************************************************************
Function: getConnectedComponents()
Description:
    The function determines the connected components of the mesh. (Vertices are 
    in the same connected component if they are connected by a path consisting 
    of one or more edges.) For each connected component, a handle corresponding 
    to a single representative vertex in the component is written to the output 
    iterator vertexIter.
*******************************************************************************/
template <class P, class I>
int getConnectedComponents(const P& polyMesh, I vertexIter) {
    // The polyhedron types
    typedef typename P::Halfedge_const_handle Halfedge_const_handle;
    typedef typename P::Vertex_const_iterator Vertex_const_iterator;
    typedef typename P::Vertex_const_handle Vertex_const_handle;
    
    // Initialize a set for all NOT visited vertices
    std::set<Vertex_const_handle> not_visited_vertices;
    // Initialize a stack for all the not visited adjacent vertices
    std::stack<Halfedge_const_handle> edge_stack;
    // Initialize number of components
    int components = 0;
    
    // Add all vertices to the not_visited_vertices set
    for (auto vit = polyMesh.vertices_begin(); vit != polyMesh.vertices_end(); ++vit) {
        not_visited_vertices.insert(&*vit);
    }
    
    do {
        // Find the initial vertex for current component
        Vertex_const_handle vertex_start = *(not_visited_vertices.begin());

        // Update the represent vertex handles
        *(vertexIter++) = vertex_start;
        
        // Add the halfedge to the edge stack
        Halfedge_const_handle halfedge_start = vertex_start -> halfedge();
        edge_stack.push(halfedge_start);
        
        // Erase the initial vertex from the not_visited_vertices set
        not_visited_vertices.erase(vertex_start);
        
        while (!edge_stack.empty()) {            
            Halfedge_const_handle current_edge = edge_stack.top();
            int valence = current_edge -> vertex_degree();
            
            // Initialize a count for the not visited neighbour vertex
            int not_visited_neighbor = 0;
            
            // Initialize the next halfedge and next vertex
            Halfedge_const_handle next_edge = current_edge -> next_on_vertex();
            Vertex_const_handle next_vertex = next_edge -> opposite() -> vertex();
            
            // Find the not visited adjacent vertex
            for (int i = 0; i < valence; ++i) {
                // If next vertex is not visited, push it to the not visited stack
                if(not_visited_vertices.find(next_vertex) != not_visited_vertices.end()) {                    
                    // Push the current halfedge to the stack
                    edge_stack.push(next_edge -> opposite());
                    
                    // Update the not visited neighbour count
                    not_visited_neighbor = 1;
                    
                    // Erase the current vertex from set
                    not_visited_vertices.erase(next_vertex);
                    
                    // Find the not visited neighbour, break the for loop
                    break;
                }
                
                // Update the next halfedge and vertex
                next_edge = next_edge -> next_on_vertex();
                next_vertex = next_edge -> opposite() -> vertex();
            }
            
            // If current vertex don't have any not visited vertices, pop the stack
            if (not_visited_neighbor == 0) {
                edge_stack.pop();
            }
        }
        
        // Update the components count
        ++components;
        
    } while (!not_visited_vertices.empty());
    
    return components;
}


//////////////////////////////////////////////////////////
// Public functions from Encoder class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Encoder(Polyhedron mesh, std::ostream& ofile, Point quan_size,  
                  int x_bit, int y_bit, int z_bit);
Description:
    Encoder class constructors
*******************************************************************************/
Encoder::Encoder(Polyhedron mesh, std::ostream& ofile, Point quan_size, int x_bit, int y_bit, int z_bit)
  : polyMesh(mesh), ofile_eb(ofile), step_size(quan_size), x_coor_bits(x_bit), y_coor_bits(y_bit), 
    z_coor_bits(z_bit), out(geom_out), enc(total_contexts_cnt, &out) {
        
    numBorderLoops = 0;
    s_cnt = 0;
    skip_m = 0;
    skip_h = 0;
    vertices_index = 0;
    processed_cnt = 0;
    affected_s_cnt = 0;
    coded_data_len = 0;
    conn_str_len = 0;
    geom_str_len = 0;
    
    // Starts the arithmetic encoder
    enc.start();
}


/*******************************************************************************
Function: ~Encoder();
Description:
    Encoder class deconstructor
*******************************************************************************/
Encoder::~Encoder() {
    --numBorderLoops;
    --s_cnt;
    --skip_m;
    --skip_h;
    --vertices_index;
    --processed_cnt;
    --affected_s_cnt;
    --coded_data_len;
    --conn_str_len;
    --geom_str_len;
}


/*******************************************************************************
Function: void mesh_compression()
Description:
    Triangle mesh compression function
*******************************************************************************/
void Encoder::mesh_compression() {
    // Generate the coefficient and exponent for each step size coordinate
    Quant_index coef_x = 0, coef_y = 0, coef_z = 0;
    int exp_x = 0, exp_y = 0, exp_z = 0;
    
    bool convert_x = step_size_convert(step_size.x(), coef_x, exp_x);
    if (!convert_x) {
        std::cerr << "ERROR: Convert x coordinate step size error!\n";
        exit(1);
    }
    
    bool convert_y = step_size_convert(step_size.y(), coef_y, exp_y);
    if (!convert_y) {
        std::cerr << "ERROR: Convert y coordinate step size error!\n";
        exit(1);
    }
    
    bool convert_z = step_size_convert(step_size.z(), coef_z, exp_z);
    if (!convert_z) {
        std::cerr << "ERROR: Convert z coordinate step size error!\n";
        exit(1);
    }
    
    // Push the step size pair in to the vector
    std::pair<Quant_index, int> size_pair_x(coef_x, exp_x);
    std::pair<Quant_index, int> size_pair_y(coef_y, exp_y);
    std::pair<Quant_index, int> size_pair_z(coef_z, exp_z);
    
    step_size_vec.push_back(size_pair_x);
    step_size_vec.push_back(size_pair_y);
    step_size_vec.push_back(size_pair_z);
    
    // calculate the program used quantization step size
    double quan_x = pow2double(coef_x, exp_x);
    double quan_y = pow2double(coef_y, exp_y);
    double quan_z = pow2double(coef_z, exp_z);
    
    step_size = Point(quan_x, quan_y, quan_z);
    
    // std::clog.precision(std::numeric_limits<double>::max_digits10);
    // std::clog << "Program used quantization step_size is: " << step_size << "\n";
    
    // Preprocessing the input triangle mesh
    bool pre = preprocessing();
    if (!pre) {
        std::cerr << "ERROR: Mesh preprocessing falied! \n";
        exit(1);
    }
    
    // Use Edgebreaker and parallelogram prediction to compress the mesh
    compressing();
    
    // Terminate the arithmetic encoder
    if (enc.terminate()) {
        std::cerr << "ERROR: Encode terminate failed\n";
        exit(1);
    }
    
    // Byte-alignment for geometry streastream
    out.flush();

    // Write the compressed triangle mesh to the given output stream
    bool file = write_ebfile();
    if (!file) {
        std::cerr << "ERROR: Failed to write the compressed triangle mesh! \n";
        exit(1);
    }
}


/*******************************************************************************
Function: std::vector<Vertex_index> compression_result();
Description:
    Generates the results information
*******************************************************************************/
std::vector<Vertex_index> Encoder::compression_result() {
    // Generates the result vector
    std::vector<Vertex_index> result_vec;
    
    // Basic mesh information
    Vertex_index vertices = polyMesh.size_of_vertices();
    Vertex_index facets = polyMesh.size_of_facets();
    Vertex_index halfedges = polyMesh.size_of_halfedges();
    Vertex_index edges = halfedges / 2;
    int genus = 1.0 - (static_cast<double>(vertices) + facets - edges) / 2.0 - numBorderLoops / 2.0;
    
    // Coded data size
    Vertex_index conn_size = ceil(conn_str_len / 8.0);
    Vertex_index code_size = ceil(coded_data_len / 8.0);
    
    result_vec.push_back(vertices);
    result_vec.push_back(edges);
    result_vec.push_back(facets);
    result_vec.push_back(numBorderLoops);
    result_vec.push_back(genus);
    result_vec.push_back(code_size);
    result_vec.push_back(geom_str_len);
    result_vec.push_back(conn_size);
    
    return result_vec;
}


/*******************************************************************************
Function: Point get_quan_step_size();
Description:
    Generates the results information
*******************************************************************************/
Point Encoder::get_quan_step_size() {
    return step_size;
}


//////////////////////////////////////////////////////////
// Private functions from Encoder class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: static void Encoder::write_history_code1(const std::vector<Triangle_type>& 
          hist_vec, std::string& binary_string)
Description:
    Binary history string generation function (Using code series 1)
*******************************************************************************/
void Encoder::write_history_code1(const std::vector<Triangle_type>& hist_vec, std::string& binary_string) {
    // Initialize binary code
    std::string binary_code;
    // Initialize current op-code and previous op-code
    Triangle_type opcode = I, prev_code = I;
    
    // Generate the binary history string from the history vector
    for(auto it = hist_vec.begin(); it != hist_vec.end(); ++it) {
        // Get the current op-code
        opcode = *it;
        
        // Current triangle follows a C type triangle
        if (prev_code == C) {
            if (opcode == C) { 
                binary_code = "0"; 
            }
            else if (opcode == S) {
                binary_code = "10"; 
            }
            else if (opcode == R) {
                binary_code = "11"; 
            }
        }
        
        // Current triangle NOT follows a C type triangle
        else {
            if (opcode == C) {
                binary_code = "0";
            }
            else if (opcode == S) {
                binary_code = "100";
            }
            else if (opcode == R) {
                binary_code = "101";
            }
            else if (opcode == L) {
                binary_code = "110";
            }
            else if (opcode == E) {
                binary_code = "111";
            }
        }
        
        // Write current code to the history string
        binary_string += binary_code;
        
        // Update the previous op-code
        prev_code = opcode;
    }
}


/*******************************************************************************
Function: static void Encoder::write_history_code2(const std::vector<Triangle_type>& 
          hist_vec, std::string& binary_string)
Description:
    Binary history string generation function (Using code series 2)
*******************************************************************************/
void Encoder::write_history_code2(const std::vector<Triangle_type>& hist_vec, std::string& binary_string) {
    // Initialize binary code
    std::string binary_code;
    // Initialize current op-code and previous op-code
    Triangle_type opcode = I, prev_code = I;
    
    // Generate the binary history string from the history vector
    for(auto it = hist_vec.begin(); it != hist_vec.end(); ++it) {
        // Get the current op-code
        opcode = *it;
        
        // Current triangle follows a C type triangle
        if (prev_code == C) {
            if (opcode == C) {
                binary_code = "0";
            }
            else if (opcode == S) {
                binary_code = "10";
            }
            else if (opcode == R) {
                binary_code = "11";
            }
        }
        
        // Current triangle NOT follows a C type triangle
        else {
            if (opcode == C) {
                binary_code = "00";
            }
            else if (opcode == S) {
                binary_code = "111";
            }
            else if (opcode == R) {
                binary_code = "10";
            }
            else if (opcode == L) {
                binary_code = "110";
            }
            else if (opcode == E) {
                binary_code = "01";
            }
        }
        
        // Write current code to the history string
        binary_string += binary_code;
        
        // Update the previous op-code
        prev_code = opcode;
    }
}


/*******************************************************************************
Function: static void Encoder::write_history_code3(const std::vector<Triangle_type>& 
          hist_vec, std::string& binary_string)
Description:
    Binary history string generation function (Using code series 3)
*******************************************************************************/
void Encoder::write_history_code3(const std::vector<Triangle_type>& hist_vec, std::string& binary_string) {
    // Initialize binary code
    std::string binary_code;
    // Initialize current op-code and previous op-code
    Triangle_type opcode = I, prev_code = I;
    
    // Generate the binary history string from the history vector
    for(auto it = hist_vec.begin(); it != hist_vec.end(); ++it) {
        // Get the current op-code
        opcode = *it;
        
        // Current triangle follows a C type triangle
        if (prev_code == C) {
            if (opcode == C) {
                binary_code = "0";
            }
            else if (opcode == S) {
                binary_code = "10";
            }
            else if (opcode == R) {
                binary_code = "11";
            }
        }
        
        // Current triangle NOT follows a C type triangle
        else {
            if (opcode == C) {
                binary_code = "00";
            }
            else if (opcode == S) {
                binary_code = "010";
            }
            else if (opcode == R) {
                binary_code = "011";
            }
            else if (opcode == L) {
                binary_code = "10";
            }
            else if (opcode == E) {
                binary_code = "11";
            }
        }
        
        // Write current code to the history string
        binary_string += binary_code;
        
        // Update the previous op-code
        prev_code = opcode;
    }
}


/*******************************************************************************
Function: static int code_series_determine(const std::vector<Triangle_type>& hist_vec);
Description:
    Decide which code series to used in the final EB file, and call the 
    corresponding write history code function.
*******************************************************************************/
int Encoder::code_series_determine(const std::vector<Triangle_type>& hist_vec) {
    // Initialize current op-code and previous op-code
    Triangle_type opcode = I, prev_code = I;
    
    // Initialize the cost variables
    int sa = 0, ra = 0, rn = 0, l = 0, e = 0, t = hist_vec.size();
    
    for(auto it = hist_vec.begin(); it != hist_vec.end(); ++it) {
        // Get the current op-code
        opcode = *it;
        
        // Update the cost variables
        if (opcode == L) {
            l += 1;
        }
        else if (opcode == E) {
            e += 1;
        }
        else if (opcode == S && prev_code == C) {
            sa += 1;
        }
        else if (opcode == R && prev_code == C) {
            ra += 1;
        }
        else if (opcode == R && prev_code != C) {
            rn += 1;
        }
        
        // Update the previous op-code
        prev_code = opcode;
    }
    
    // Calculate cost for each binary code series
    int cost1 = 2 * t - sa - ra;
    int cost2 = 2 * t - rn - e;
    int cost3 = 2 * t - l - e;
    
    // Initialize code series used to encode the history 
    int code = 0;
    
    // Determine the code series
    if (cost1 <= cost2 && cost1 <= cost3) {
        code = 1;
    }
    else if (cost2 <= cost1 && cost2 <= cost3) {
        code = 2;
    }
    else if (cost3 <= cost1 && cost3 <= cost2) {
        code = 3;
    }

    return code;
}


/*******************************************************************************
Function: bool preprocessing();
Description:
    Main mesh preprocessing function
*******************************************************************************/
bool Encoder::preprocessing(){
    // Test to see if the mesh contains the single component
    std::vector<Vertex_const_handle> componentVertices;
    int component_cnt = getConnectedComponents<Polyhedron, std::back_insert_iterator<std::vector<Vertex_const_handle>>>
    (polyMesh, std::back_inserter(componentVertices));
    
    if(component_cnt != 1) {
        std::cerr << "ERROR: Input triangle mesh should contain ONLY one component!\n";
        exit(1);
    }
    
    // Normalize the mesh's border
    if (!(polyMesh.normalized_border_is_valid())) {
        polyMesh.normalize_border();
    }
    
    
    //////////////////////////////////////////////////////////
    // Mesh preprocessing
    //////////////////////////////////////////////////////////
    
    // Initialize all vertices' mark, index and flag
    for (auto iter = polyMesh.vertices_begin(); iter != polyMesh.vertices_end(); ++iter) {
        (*iter).set_vertex_mark(0);
        (*iter).set_index(0);
        (*iter).set_flag(false);
        
        // Get current point
        Point p = iter -> point();
        
        // Get the quantization index the current point
        Quant_index p_x = 0, p_y = 0, p_z = 0;      
        Quant_index max_x_val = pow2int(x_coor_bits - 2);
        Quant_index max_y_val = pow2int(y_coor_bits - 2);
        Quant_index max_z_val = pow2int(z_coor_bits - 2);
        
        // x coordinate
        double point_x = SPL::signum(p.x()) * std::floor(std::abs(p.x()) / step_size.x() + 0.5);
        if (point_x > std::numeric_limits<Quant_index>::max()) {
            std::cerr << "ERROR: X coordinate quantization index overflow!\n";
            return false;
        }
        else {
            if (std::abs(point_x) > max_x_val) {
                std::cerr << "ERROR: X coordinate quantization index overflow!\n";
                return false;
            }
            else {
                p_x = static_cast<Quant_index>(point_x);
            }
        }
        
        // y coordinate
        double point_y = SPL::signum(p.y()) * std::floor(std::abs(p.y()) / step_size.y() + 0.5);
        if (point_y > std::numeric_limits<Quant_index>::max()) {
            std::cerr << "ERROR: Y coordinate quantization index overflow!\n";
            return false;
        }
        else {
            if (std::abs(point_y) > max_y_val) {
                std::cerr << "ERROR: Y coordinate quantization index overflow!\n";
                return false;
            }
            else {
                p_y = static_cast<Quant_index>(point_y);
            }
        }
        
        // z coordinate
        double point_z = SPL::signum(p.z()) * std::floor(std::abs(p.z()) / step_size.z() + 0.5);
        if (point_z > std::numeric_limits<Quant_index>::max()) {
            std::cerr << "ERROR: Z coordinate quantization index overflow!\n";
            return false;
        }
        else {
            if (std::abs(point_z) > max_z_val) {
                std::cerr << "ERROR: Z coordinate quantization index overflow!\n";
                return false;
            }
            else {
                p_z = static_cast<Quant_index>(point_z);
            }
        }
        
        iter -> point() = Point(p_x, p_y, p_z);
    }
    
    // Check for duplicated vertices after quantization
    std::set<Point> mesh_vertices;
    for (auto vit = polyMesh.vertices_begin(); vit != polyMesh.vertices_end(); ++vit) {
        Point p = vit -> point();
        if (mesh_vertices.find(p) != mesh_vertices.end()) {
            std::cerr << "ERROR: The quantized mesh contains duplicated vertices!\n";
            exit(1);
        }
        else {
            mesh_vertices.insert(p);
        }
    }

    // Initialize all border vertices' mark
    for (auto hit = polyMesh.border_halfedges_begin(); hit != polyMesh.halfedges_end(); ++hit) {
        (*hit).vertex() -> set_vertex_mark(2);
        ++hit;  // Move to the border half edge
    }
    
    // Initialize all halfedges' mark
    for (auto hit = polyMesh.halfedges_begin(); hit != polyMesh.halfedges_end(); ++hit) {
        (*hit).set_halfedge_mark(0);
    }
        
    // Mesh preprocessing for the closed triangle mesh
    if (polyMesh.is_closed()) {
        // Get the initial gate for mesh compression
        gate = polyMesh.halfedges_begin();
        
        // Make sure the vertices_index is zero
        assert(vertices_index == 0);
        
        // Update the vertex mark and index for the end vertex of the initial gate
        Vertex_handle v1 = gate -> vertex();
        v1 -> set_vertex_mark(1);
        v1 -> set_index(vertices_index);
        
        // Update the vertex mark and index for the start vertex of the initial gate
        Vertex_handle v2 = gate -> opposite() -> vertex();
        v2 -> set_vertex_mark(1);
        ++vertices_index;
        v2 -> set_index(vertices_index);
        
        // Update the .N and .P relationship for the gate
        gate -> set_prev_on_border(gate -> opposite());
        gate -> set_next_on_border(gate -> opposite());
        
        // update the halfedge mark for the initial gate
        gate -> set_halfedge_mark(1);
        
        // Make sure the vertices_index is 1 after the gate initialization
        assert(vertices_index == 1);
    }
    
    // Mesh preprocessing for the NOT closed triangle mesh
    else {       
        boundary_detection();
    }
    
    // Resize the history vector to the No. of triangles in the mesh
    history_vec.resize(polyMesh.size_of_facets());

    // Geometry processing the start vertex of the initial gate
    Point c = gate -> opposite() -> vertex() -> point();
    
    // // Print the original points and its corresponding predicted position
    // std::clog << "First point to predict \n";
    // std::clog << "Input point: " << c << ", predicted position: " << c << "\n";

    // Arithmetic encoding the vertex coordinates
    bool encode_x = vertex_coordinate_encode(c.x(), x_coor_bits, 0);
    if (!encode_x) {
        std::cerr << "ERROR: Encode x coordinate failed! \n";
        return false;
    }
    
    bool encode_y = vertex_coordinate_encode(c.y(), y_coor_bits, 1);
    if (!encode_y) {
        std::cerr << "ERROR: Encode y coordinate failed! \n";
        return false;
    }
    
    bool encode_z = vertex_coordinate_encode(c.z(), z_coor_bits, 2);   
    if (!encode_z) {
        std::cerr << "ERROR: Encode z coordinate failed! \n";
        return false;
    }    
    
    // Update the vertex's flag
    Vertex_handle vc = gate -> opposite() -> vertex();
    (*vc).set_flag(true);
    
    // Geometry processing the end vertex of the initial gate
    Point a = gate -> opposite() -> vertex() -> point();
    c = gate -> vertex() -> point();
    
    // Get the integer difference between the actual and predicted position
    Quant_index x = c.x() - a.x();
    Quant_index y = c.y() - a.y();
    Quant_index z = c.z() - a.z();
    
    // // Print the original points and its corresponding predicted position
    // std::clog << "Second point to predict \n";
    // std::clog << "Input point: " << c << ", predicted position: " << x << " " << y << " " << z << "\n";

    // Arithmetic encoding the vertex coordinates
    encode_x = vertex_coordinate_encode(x, x_coor_bits, 0);
    if (!encode_x) {
        std::cerr << "ERROR: Encode x coordinate failed! \n";
        return false;
    }
    
    encode_y = vertex_coordinate_encode(y, y_coor_bits, 1);
    if (!encode_y) {
        std::cerr << "ERROR: Encode y coordinate failed! \n";
        return false;
    }
    
    encode_z = vertex_coordinate_encode(z, z_coor_bits, 2);
    if (!encode_z) {
        std::cerr << "ERROR: Encode z coordinate failed! \n";
        return false;
    } 
    
    // Update the third vertex's flag
    vc = gate -> vertex();
    (*vc).set_flag(true);
    
    return true;
}


/*******************************************************************************
Function: boundary_detection()
Description:
    Detect the boundary of the mesh (longest bounding loop) 
*******************************************************************************/
void Encoder::boundary_detection() {
    // Initialize border_vec vector and loop_sizes vector
    std::vector<Halfedge_handle> border_vec;
    std::vector<int> loop_sizes;
    
    // Get the number of border loops from the mesh
    numBorderLoops = getBorderLoops<Polyhedron, std::back_insert_iterator<std::vector<Halfedge_handle>>, 
    std::back_insert_iterator<std::vector<int>>>(polyMesh, std::back_inserter(border_vec),std::back_inserter(loop_sizes));
    
    // Initialize boundary length
    int boundary_len = 0;
    
    // Mesh contains only one bounding loop
    if(numBorderLoops == 1) {
        // Get the initial gate for compression
        gate = border_vec[0] -> opposite();
        boundary_len = loop_sizes[0];
    }
    
    // Mesh contains more than one bounding loop
    else {
        int loop_id = 0;
        boundary_len = loop_sizes[loop_id];
    
        // Find the longest boundary loop from boundary vector
        for (auto it = loop_sizes.begin(); it != loop_sizes.end(); ++it) {
            if (*it > boundary_len) {
                boundary_len = *it;
                loop_id = it - loop_sizes.begin();
            }
        }
        
        // Get the initial gate for compression
        gate = border_vec[loop_id] -> opposite();
        
        // Make sure the boundary length we get is the largest length from vector
        assert(boundary_len == loop_sizes[loop_id]);
        
        // Update the .N and .P relationship and mark for all holes
        for(int i = 0; i < loop_sizes.size(); ++i) {
            if (i == loop_id) {
                continue;
            }
            
            // Update the hole's start half edge
            Halfedge_handle hole_start = border_vec[i];
            hole_start -> opposite() -> set_halfedge_mark(2);
            hole_start -> opposite() -> set_prev_on_border(hole_start -> next() -> opposite());
            hole_start -> opposite() -> set_next_on_border(hole_start -> prev() -> opposite());
            
            // Update the rest hole's half edges
            Halfedge_handle hole_next = hole_start -> next();
            while (hole_next != hole_start) {
                hole_next -> opposite() -> set_halfedge_mark(2);
                hole_next -> opposite() -> set_prev_on_border(hole_next -> next() -> opposite());
                hole_next -> opposite() -> set_next_on_border(hole_next -> prev() -> opposite());
                
                hole_next = hole_next -> next();
            }
        }
    }
    
    // Make sure initial gate is not border halfedge
    assert(!((*gate).is_border()));
    
    // Initialize index value for the boundary vertices
    int index = boundary_len - 1;
    
    // Update the .N and .P relationship and mark for the start boundary halfedge
    Halfedge_handle boundary_start = gate -> opposite();
    gate -> set_halfedge_mark(1);
    gate -> set_prev_on_border(boundary_start -> next() -> opposite());
    gate -> set_next_on_border(boundary_start -> prev() -> opposite());
    
    boundary_start -> vertex() -> set_vertex_mark(1);
    boundary_start -> vertex() -> set_index(index);
    --index;
    
    // Update the .N and .P relationship and mark for the rest boundary halfedges
    Halfedge_handle next_border = boundary_start -> next();
    while (next_border != boundary_start) {
        next_border -> opposite() -> set_halfedge_mark(1);
        next_border -> opposite() -> set_prev_on_border(next_border -> next() -> opposite());
        next_border -> opposite() -> set_next_on_border(next_border -> prev() -> opposite());
        
        next_border -> vertex() -> set_vertex_mark(1);
        next_border -> vertex() -> set_index(index);
        
        next_border = next_border -> next();
        --index;
        
        // Update the vertices index count
        ++vertices_index;
    }
    
    // Make sure the vertices_index is boundary_len - 1
    assert(vertices_index == (boundary_len - 1));
}


/*******************************************************************************
Function: bool geometry_processing();
Description:
    Use Parallelogram Prediction scheme to predict the vertex location
    for the given point c. This function is used to process the all mesh's 
    vertices except the start and end vertex of the initial active gate.
*******************************************************************************/
bool Encoder::geometry_processing() {
    // Initialize the point a, b, and c
    Point a = gate -> opposite() -> vertex() -> point();
    Point b = gate -> vertex() -> point();
    Point c = gate -> next() -> vertex() -> point();
    
    // Get vertex handle for point c
    Vertex_handle vc = gate -> next() -> vertex();
    
    // Get the predict position
    Point predict;
    if (processed_cnt == 0) {
        // Geometry processing for the third vertex of the mesh
        // std::clog << "Geometry prediction without third vertex \n";
        predict = geometry_predict(&a, &b, nullptr);
    }
    else {
        Point d = gate -> opposite() -> next() -> vertex() -> point();
        predict = geometry_predict(&a, &b, &d);
    }
    // std::clog << "Calculated position: " << predict << "\n";

    // Geometry prediction for the third vertex of current triangle
    if (!(*vc).get_flag()) {
        // Get the integer difference between the actual and predicted position
        Quant_index x = c.x() - static_cast<Quant_index>(predict.x());
        Quant_index y = c.y() - static_cast<Quant_index>(predict.y());
        Quant_index z = c.z() - static_cast<Quant_index>(predict.z());
        
        // // Print the original points and its corresponding predicted position
        // std::clog << "Input point: " << c << ", predicted position: " << x << " " << y << " " << z << "\n";
        
        // Arithmetic encoding the vertex coordinates
        bool encode_x = vertex_coordinate_encode(x, x_coor_bits, 0);
        if (!encode_x) {
            std::cerr << "ERROR: Encode x coordinate failed! \n";
            return false;
        }
        
        bool encode_y = vertex_coordinate_encode(y, y_coor_bits, 1);
        if (!encode_y) {
            std::cerr << "ERROR: Encode y coordinate failed! \n";
            return false;
        }
        
        bool encode_z = vertex_coordinate_encode(z, z_coor_bits, 2);
        if (!encode_z) {
            std::cerr << "ERROR: Encode z coordinate failed! \n";
            return false;
        }
        
        // Update vertex's flag
        (*vc).set_flag(true);
    }
    
    return true;
}


/*******************************************************************************
Function: Triangle_type tri_type_identify()
Description:
    Identify the topological relationship of current triangle with the remaining
    part of the mesh's boundary
*******************************************************************************/
Triangle_type Encoder::tri_type_identify() {
    // Initialize triangle type
    Triangle_type tri_type = I;
    
    // Get the third vertex mark value
    int mark_value = gate -> next() -> vertex() -> get_vertex_mark();
    
    // Make sure the vertex mark is correct
    assert(mark_value == 0 || mark_value == 1 || mark_value == 2 || mark_value == 3);
    
    if (mark_value == 0) {
        // Third vertex is not visited yet
        tri_type = C;
    }
    
    else if (mark_value == 2) {
        // Third vertex is on the boundary of hole
        tri_type = M;
    }
    
    else {
        if (gate -> next() == gate -> next_on_border()) {
            if (gate -> prev() == gate -> prev_on_border()) {
                // v precedes and follows g
                tri_type = E;
            }
            else {
                // v immediately follows g
                tri_type = R;
            }
        }
        else {
            if (gate -> prev() == gate -> prev_on_border()) {
                // v immediately precedes g
                tri_type = L;
            }
            else {
                // //  v is else where on the boundary
                if (mark_value == 3) {
                    tri_type = H;
                }
                else {
                    tri_type = S;
                }
            }
        }
    }
    
    return tri_type;
}


/*******************************************************************************
Function: process_c_type_triangle()
Description:
    C type triangle processing function
*******************************************************************************/
void Encoder::process_c_type_triangle() {
    // Update the third vertex's index and mark
    gate -> next() -> vertex() -> set_index(++vertices_index);
    gate -> next() -> vertex() -> set_vertex_mark(1);
    
    // Update halfedges' mark
    gate -> set_halfedge_mark(0);
    gate -> prev() -> opposite() -> set_halfedge_mark(1);
    gate -> next() -> opposite() -> set_halfedge_mark(1);
    
    // Fixed the first link: g.p.o.P=g.P; g.P.N=g.p.o;
    gate -> prev() -> opposite() -> set_prev_on_border(gate -> prev_on_border());
    gate -> prev_on_border() -> set_next_on_border(gate -> prev() -> opposite());
    
    // Fixed the second link: g.p.o.N=g.n.o; g.n.o.P=g.p.o;
    gate -> prev() -> opposite() -> set_next_on_border(gate -> next() -> opposite());
    gate -> next() -> opposite() -> set_prev_on_border(gate -> prev() -> opposite());
    
    // Fixed the third link: g.n.o.N=g.N; g.N.P=g.n.o 
    gate -> next() -> opposite() -> set_next_on_border(gate -> next_on_border());
    gate -> next_on_border() -> set_prev_on_border(gate -> next() -> opposite());
    
    // Gate update
    gate = gate -> next() -> opposite();
}


/*******************************************************************************
Function: process_e_type_triangle()
Description:
    E type triangle processing function
*******************************************************************************/
void Encoder::process_e_type_triangle() {
    // Update halfedges' mark
    gate -> set_halfedge_mark(0);
    gate -> prev() -> set_halfedge_mark(0);
    gate -> next() -> set_halfedge_mark(0);
    
    // Pop the corresponding offset value for the normal S operation
    if (offset_vec.size() > affected_s_cnt) {
        offset_vec.pop_back();
    }
    
    // Current sub mesh is compressed completely, set active gate to null
#ifdef BUG
    gate = nullptr;
#else
    gate = Halfedge_handle();
#endif
}


/*******************************************************************************
Function: process_l_type_triangle()
Description:
    L type triangle processing function
*******************************************************************************/
void Encoder::process_l_type_triangle() {
    // Update halfedges' mark
    gate -> set_halfedge_mark(0);
    gate -> prev_on_border() -> set_halfedge_mark(0);
    gate -> next() -> opposite() -> set_halfedge_mark(1);
    
    // Fixed the first link: g.P.P.N=g.n.o; g.n.o.P=g.P.P; 
    gate -> prev_on_border() -> prev_on_border() -> set_next_on_border(gate -> next() -> opposite());
    gate -> next() -> opposite() -> set_prev_on_border(gate -> prev_on_border() -> prev_on_border());
    
    // Fixed the second link: g.n.o.N=g.N; g.N.P=g.n.o;
    gate -> next() -> opposite() -> set_next_on_border(gate -> next_on_border());
    gate -> next_on_border() -> set_prev_on_border(gate -> next() -> opposite());
    
    // Gate update
    gate = gate -> next() -> opposite();
}


/*******************************************************************************
Function: process_r_type_triangle()
Description:
    R type triangle processing function
*******************************************************************************/
void Encoder::process_r_type_triangle() {
    // Update halfedges' mark
    gate -> set_halfedge_mark(0);
    gate -> next_on_border() -> set_halfedge_mark(0);
    gate -> prev() -> opposite() -> set_halfedge_mark(1);
    
    // Fixed the first link: g.N.N.P=g.p.o; g.p.o.N=g.N.N; 
    gate -> next_on_border() -> next_on_border() -> set_prev_on_border(gate -> prev() -> opposite());
    gate -> prev() -> opposite() -> set_next_on_border(gate -> next_on_border() -> next_on_border());
    
    // Fixed the second link: g.p.o.P=g.P; g.P.N=g.p.o;
    gate -> prev() -> opposite() -> set_prev_on_border(gate -> prev_on_border());
    gate -> prev_on_border() -> set_next_on_border(gate -> prev() -> opposite());
    
    // Gate update
    gate = gate -> prev() -> opposite();
}


/*******************************************************************************
Function: process_s_type_triangle()
Description:
    S type triangle processing function
*******************************************************************************/
void Encoder::process_s_type_triangle() {
    // Update halfedges' mark
    gate -> set_halfedge_mark(0);
    gate -> prev() -> opposite() -> set_halfedge_mark(1);
    gate -> next() -> opposite() -> set_halfedge_mark(1);
    
    // Initial candidate for gate_b
    Halfedge_handle gate_b = gate -> next();
    while(gate_b -> get_halfedge_mark() != 1) {
        gate_b = gate_b -> opposite() -> prev();
    }
    
    // Fixed the first link: g.P.N=g.p.o; g.p.o.P=g.P; 
    gate -> prev_on_border() -> set_next_on_border(gate -> prev() -> opposite());
    gate -> prev() -> opposite() -> set_prev_on_border(gate -> prev_on_border());
    
    // Fixed the second link: g.p.o.N=b.N; b.N.P=g.p.o; 
    gate -> prev() -> opposite() -> set_next_on_border(gate_b -> next_on_border());
    gate_b -> next_on_border() -> set_prev_on_border(gate -> prev() -> opposite());
    
    // Fixed the third link: b.N=g.n.o; g.n.o.P=b; 
    gate_b -> set_next_on_border(gate -> next() -> opposite());
    gate -> next() -> opposite() -> set_prev_on_border(gate_b);
    
    // Fixed the fourth link: g.n.o.N=g.N; g.N.P=g.n.o;
    gate -> next() -> opposite() -> set_next_on_border(gate -> next_on_border());
    gate -> next_on_border() -> set_prev_on_border(gate -> next() -> opposite());
    
    // Update the skip S count for both M and M' type of triangle
    skip_m += 1;
    skip_h += 1;
    
    // Update S type triangle count
    s_cnt += 1;
    
    // Calculate the offset value for current S operation
    int offset_value = 1;
    Halfedge_handle offset_gate = gate -> next() -> opposite() -> next_on_border();
    while (offset_gate != gate -> next() -> opposite()) {
        offset_value += 1;
        offset_gate = offset_gate -> next_on_border();
    }
    
    // Subtract the 2 vertices of gate from the offset value
    offset_value -= 2;
    
    // Add current offset value to the stack
    std::pair<int, int> offset_pair(s_cnt, offset_value);
    offset_vec.push_back(offset_pair);
    
    // Update the vertices and halfedges mark for the left side sub mesh
    Halfedge_handle sub_gate = gate -> prev() -> opposite();
    sub_gate -> set_halfedge_mark(3);
    
    Halfedge_handle next_border = sub_gate -> next_on_border();
    while (next_border != sub_gate) {
        next_border -> set_halfedge_mark(3);
        next_border -> vertex() -> set_vertex_mark(3);
        next_border = next_border -> next_on_border();
    }
    
    // Store the gate for the left side sub mesh
    assert(sub_gate -> get_halfedge_mark() == 3);
    s_gate.push(sub_gate);
    
    // Compress the right side of sub mesh
    gate = gate -> next() -> opposite();
    compressing();
    
    // Compress the left side of sub mesh
    if (!s_gate.empty()) {
        gate = s_gate.top();
        s_gate.pop();
        
        // Update the vertices and halfedges mark for the popped left side sub mesh
        gate -> set_halfedge_mark(1);
        gate -> vertex() -> set_vertex_mark(1);
        
        Halfedge_handle sub_border = gate -> next_on_border();
        while (sub_border != gate) {
            sub_border -> set_halfedge_mark(1);
            sub_border -> vertex() -> set_vertex_mark(1);
            sub_border = sub_border -> next_on_border();
        }    
    }
}


/*******************************************************************************
Function: process_m_type_triangle()
Description:
    M type triangle processing function
*******************************************************************************/
void Encoder::process_m_type_triangle() {
    // Update halfedges' mark
    gate -> set_halfedge_mark(0);
    gate -> prev() -> opposite() -> set_halfedge_mark(1);
    gate -> next() -> opposite() -> set_halfedge_mark(1);
    
    // Initial candidate for gate_b
    Halfedge_handle gate_b = gate -> next();
    while(gate_b -> get_halfedge_mark() != 2) {
        gate_b = gate_b -> opposite() -> prev();
    }

    // Traversal of the new edge
    int hole_len = 0;
    do {
        // Update the holes' boundary vertices index and mark
        gate_b -> vertex() -> set_index(++vertices_index);
        gate_b -> vertex() -> set_vertex_mark(1);
        
        // Update halfedges' mark
        gate_b -> set_halfedge_mark(1);
        
        // Update the hole length
        hole_len += 1;
        
        // Move to next edge around hole
        gate_b = gate_b -> next_on_border();
    } while(gate_b -> vertex() != gate -> next() -> vertex());
    
    // Fixed the first link: g.P.N=g.p.o; g.p.o.P=g.P; 
    gate -> prev_on_border() -> set_next_on_border(gate -> prev() -> opposite());
    gate -> prev() -> opposite() -> set_prev_on_border(gate -> prev_on_border());
    
    // Fixed the second link: g.p.o.N=b.N; b.N.P=g.p.o; 
    gate -> prev() -> opposite() -> set_next_on_border(gate_b -> next_on_border());
    gate_b -> next_on_border() -> set_prev_on_border(gate -> prev() -> opposite());
    
    // Fixed the third link: b.N=g.n.o; g.n.o.P=b; 
    gate_b -> set_next_on_border(gate -> next() -> opposite());
    gate -> next() -> opposite() -> set_prev_on_border(gate_b);
    
    // Fixed the fourth link: g.n.o.N=g.N; g.N.P=g.n.o;
    gate -> next() -> opposite() -> set_next_on_border(gate -> next_on_border());
    gate -> next_on_border() -> set_prev_on_border(gate -> next() -> opposite());
    
    // Update the M table
    std::pair<int, int> m_item = std::make_pair(skip_m, hole_len);
    m_table.push_back(m_item);
    
    // Update the skip S count
    skip_m = 0;
    
    // Gate update
    gate = gate -> next() -> opposite();
}


/*******************************************************************************
Function: process_h_type_triangle()
Description:
    M' type triangle processing function. The code listed here is based on our 
    best guess, the pseudo-code is missing from the paper.
*******************************************************************************/
void Encoder::process_h_type_triangle() {
    // Update halfedges' mark
    gate -> set_halfedge_mark(0);
    gate -> prev() -> opposite() -> set_halfedge_mark(1);
    gate -> next() -> opposite() -> set_halfedge_mark(1);
    
    // Initial candidate for gate_b
    Halfedge_handle gate_b = gate -> next();
    while(gate_b -> get_halfedge_mark() != 3) {
        gate_b = gate_b -> opposite() -> prev();
    }
    
    // Update vertex and halfedge mark on the merging border loop
    gate_b -> set_halfedge_mark(1);
    gate_b -> vertex() -> set_vertex_mark(1);
    
    Halfedge_handle merge_border = gate_b -> next_on_border();
    while (merge_border != gate_b) {
        merge_border -> set_halfedge_mark(1);
        merge_border -> vertex() -> set_vertex_mark(1);
        
        merge_border = merge_border -> next_on_border();
    }
    
    // Initialize offset value for M' case
    Vertex_index position = 0, offset = 0;
    
    // Find the associate gate in the stack and remove
    if (s_gate.find(gate_b) != s_gate.end()) {
        // gate_b is the associate gate
        My_findable_stack<Halfedge_handle>::Const_iterator pos_iter = s_gate.find(gate_b);
        position = std::distance(s_gate.begin(), pos_iter);
        
        // Erase the associate gate from stack
        s_gate.erase(pos_iter);
    }
    else {
        Halfedge_handle next_merge_border = gate_b -> next_on_border();
        while (s_gate.find(next_merge_border) == s_gate.end()) {
            // Current border halfedge is not the associate gate
            next_merge_border = next_merge_border -> next_on_border();
        }
        
        // Find the associate gate in the stack
        My_findable_stack<Halfedge_handle>::Const_iterator pos_iter = s_gate.find(next_merge_border);
        position = std::distance(s_gate.begin(), pos_iter);
        
        // Update the offset value
        while (next_merge_border != gate_b) {
            ++offset;
            next_merge_border = next_merge_border -> next_on_border();
        }
        
        // Erase the associate gate from stack
        s_gate.erase(pos_iter);
    }
    
    // Update the H table
    Mesh_handle handle_item(position, offset, skip_h);
    h_table.push_back(handle_item);
    
    // Fixed the first link: g.P.N=g.p.o; g.p.o.P=g.P; 
    gate -> prev_on_border() -> set_next_on_border(gate -> prev() -> opposite());
    gate -> prev() -> opposite() -> set_prev_on_border(gate -> prev_on_border());
    
    // Fixed the second link: g.p.o.N=b.N; b.N.P=g.p.o; 
    gate -> prev() -> opposite() -> set_next_on_border(gate_b -> next_on_border());
    gate_b -> next_on_border() -> set_prev_on_border(gate -> prev() -> opposite());
    
    // Fixed the third link: b.N=g.n.o; g.n.o.P=b; 
    gate_b -> set_next_on_border(gate -> next() -> opposite());
    gate -> next() -> opposite() -> set_prev_on_border(gate_b);
    
    // Fixed the fourth link: g.n.o.N=g.N; g.N.P=g.n.o;
    gate -> next() -> opposite() -> set_next_on_border(gate -> next_on_border());
    gate -> next_on_border() -> set_prev_on_border(gate -> next() -> opposite());
    
    // Update the skip S count
    skip_h = 0;
    
    // Calculate No. of S triangles that affected by current handle operation
    affected_s_cnt = offset_vec.size();
    
    // Gate update
    gate = gate -> next() -> opposite();
}


/*******************************************************************************
Function: void compressing()
Description:
    Triangle mesh compression function. 
*******************************************************************************/
void Encoder::compressing() {
    // Initialize stop condition
    bool e_case = false;
    
    do {
        // Stop condition for compression recursive process
#ifdef BUG
        if (gate == nullptr) {
            return;
        }
#else
        if (gate == Halfedge_handle()) {
            return;
        }
#endif
        
        // Identify the triangle type and add to history vector
        Triangle_type tri_type = tri_type_identify();
        
        // Add current triangle type into op-code sequence vector
        if (tri_type == M || tri_type == H) { 
            // use S op-code to represent M and M' type in history vector
            history_vec[processed_cnt] = S;
        }
        else { 
            history_vec[processed_cnt] = tri_type;
        }
        
        // Geometry processing for the third vertex of current triangle
        bool geom = geometry_processing();
        if (!geom) {
            std::cerr << "ERROR: Geometry processing failed! \n";
            exit(1);
        }
        
        // Update the processed triangle count
        ++processed_cnt;

        // Gate update and corresponding operation for each type of triangle
        switch(tri_type) {
            case C:
                process_c_type_triangle();
                break;
                
            case L:
                process_l_type_triangle();
                break;
                
            case R:
                process_r_type_triangle();
                break;
                
            case E:
                process_e_type_triangle();
                
                // No more triangle in current mesh, stop the compression
                e_case = true;                
                break;
                
            case S:
                process_s_type_triangle();
                break;
                
            case M:
                process_m_type_triangle();
                break;
            
            // Attention: We use char H to represent the M' type triangle
            case H:
                process_h_type_triangle();
                break;
        }

    } while (!e_case);
}


/*******************************************************************************
Function: write_ebfile()
Description:
    Write the entire EB file to the output stream
*******************************************************************************/
bool Encoder::write_ebfile() {
    // Make sure the vertices_index is equals to No. of mesh vertices - 1
    assert(vertices_index == (polyMesh.size_of_vertices() - 1));
    
    // Update the hole information count
    unsigned int hole_cnt = 0;
    if (!m_table.empty()) {
        hole_cnt = m_table.size();
    }
    
    // Update the handle information count
    unsigned int handle_cnt = 0;
    if (!h_table.empty()) {
        handle_cnt = h_table.size();
    }
    
    // Determine the code series
    int code_series = code_series_determine(history_vec);
    assert(code_series == 1 || code_series == 2 || code_series == 3);
    
    // Generate the binary history string
    std::string history_string;
    if(code_series == 1) {
        write_history_code1(history_vec, history_string);
    }
    else if(code_series == 2) {
        write_history_code2(history_vec, history_string);
    }
    else if(code_series == 3) {
        write_history_code3(history_vec, history_string);
    }
    
    // Update the offset information count
    unsigned int offset_cnt = 0;
    if (!offset_vec.empty()) {
        offset_cnt = offset_vec.size();
    }
    
    //////////////////////////////////////////////////////////
    // Write the header part of the EB file
    //////////////////////////////////////////////////////////
    
     // Signature
    unsigned int sig = 696610198;
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(sig, max_bits_to_put);
    
    // code_series
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(code_series, max_bits_to_put);
    
    // hist_str_len
    assert(history_string.length() > 0);
    Vertex_index hist_str_len = history_string.length();
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(hist_str_len, max_bits_to_put);
    
    // vertices_cnt
    Vertex_index vertices_cnt = polyMesh.size_of_vertices();
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(vertices_cnt, max_bits_to_put);
    
    // hole_cnt
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(hole_cnt, max_bits_to_put);
    
    // handle_cnt
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(handle_cnt, max_bits_to_put);
    
    // offset_cnt
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(offset_cnt, max_bits_to_put);
    
    // x_coor_bits
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(x_coor_bits, max_bits_to_put);
    
    // y_coor_bits
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(y_coor_bits, max_bits_to_put);
    
    // z_coor_bits
    ofile_eb.putBits(0, redundant_bits);
    ofile_eb.putBits(z_coor_bits, max_bits_to_put);
    
    // Write step_size
    for (auto it = step_size_vec.begin(); it != step_size_vec.end(); ++it) {
        Quant_index coef = (*it).first;
        ofile_eb.putBits(0, redundant_bits);
        ofile_eb.putBits(coef, max_bits_to_put);
        
        int exp = (*it).second;
        
        // Get the signum of the exponent
        int signum = SPL::signum(exp);
        if(signum == 0) {
            signum = 1;
        }
        else if(signum == -1) {
            signum = 0;
        }
        
        ofile_eb.putBits(0, 1);
        ofile_eb.putBits(signum, 1);
        ofile_eb.putBits(std::abs(exp), max_bits_to_put);
    }  
    
    // Byte alignment for header part
    ofile_eb.align();
    
    // // Print out the header information
    // std::clog << "==========================================\n";
    // std::clog.precision(std::numeric_limits<double>::max_digits10);
    // std::clog << "signature: " << sig << "\n";
    // std::clog << "code_series: " << code_series << "\n";
    // std::clog << "hist_str_len: " << hist_str_len << "\n";
    // std::clog << "vertices_cnt: " << vertices_cnt << "\n";
    // std::clog << "hole_cnt: " << hole_cnt << "\n";
    // std::clog << "handle_cnt: " << handle_cnt << "\n";
    // std::clog << "offset_cnt: " << offset_cnt << "\n";
    // std::clog << "x_coor_bits: " << x_coor_bits << "\n";
    // std::clog << "y_coor_bits: " << y_coor_bits << "\n";
    // std::clog << "z_coor_bits: " << z_coor_bits << "\n";
    // std::clog << "step_size: " << step_size << "\n";
    
    
    //////////////////////////////////////////////////////////
    // Write the history part of the EB file
    //////////////////////////////////////////////////////////
    
    
    for (char& c : history_string) {
        int bit = c - '0';
        ofile_eb.putBits(bit, 1);
    }

    // Byte alignment for connectivity part
    ofile_eb.align();
    
    
    //////////////////////////////////////////////////////////
    // Write the M table part of the EB file
    //////////////////////////////////////////////////////////
    
    if (hole_cnt != 0) {
        for (auto it = m_table.begin(); it != m_table.end(); ++it) {
            int skip_m_cnt = (*it).first;
            ofile_eb.putBits(0, redundant_bits);
            ofile_eb.putBits(skip_m_cnt, max_bits_to_put);
            
            int hole_len = (*it).second;
            ofile_eb.putBits(0, redundant_bits);
            ofile_eb.putBits(hole_len, max_bits_to_put);
        }
        
        // Byte alignment
        ofile_eb.align();
    }
    
    
    //////////////////////////////////////////////////////////
    // Write the M' table part of the EB file
    //////////////////////////////////////////////////////////
    
    if (handle_cnt != 0) {
        for (auto it = h_table.begin(); it != h_table.end(); ++it) {
            int pos = (*it).position();
            ofile_eb.putBits(0, redundant_bits);
            ofile_eb.putBits(pos, max_bits_to_put);
            
            int offset_val = (*it).offset();
            ofile_eb.putBits(0, redundant_bits);
            ofile_eb.putBits(offset_val, max_bits_to_put);
            
            int skip_h_cnt = (*it).skip_cnt();
            ofile_eb.putBits(0, redundant_bits);
            ofile_eb.putBits(skip_h_cnt, max_bits_to_put);
        }
        
        // Byte alignment 
        ofile_eb.align();
    }
    
    
    //////////////////////////////////////////////////////////
    // Write the affected offset part of the EB file
    //////////////////////////////////////////////////////////
    
    if (offset_cnt != 0) {
        for (auto it = offset_vec.begin(); it != offset_vec.end(); ++it) {
            int s_type_cnt = (*it).first;
            ofile_eb.putBits(0, redundant_bits);
            ofile_eb.putBits(s_type_cnt, max_bits_to_put);
            
            int offset_val = (*it).second;
            ofile_eb.putBits(0, redundant_bits);
            ofile_eb.putBits(offset_val, max_bits_to_put);
        }
        
        // Byte alignment 
        ofile_eb.align();
    }
    
    
    ///////////////////////////////////////////////////////////
    // Write the geometry part of the EB file
    ///////////////////////////////////////////////////////////

    for (char& c : geom_out.str()) {
        int geom_char = c;
        
        for (int i = 0; i < 8; ++i) {
            // Get current bit for encoding (From MSB to LSB)
            int bit = (geom_char >> (8 - i - 1)) & 1; 
            ofile_eb.putBits(bit, 1);
        }
    }
    
    // Flush any pending output
    ofile_eb.flush();
    
    if (!ofile_eb.isOkay()) {
        std::cerr << "ERROR: Failed to write the compressed mesh! \n";
        return false;
    }
    
    
    ///////////////////////////////////////////////////////////
    // Output information for comparison
    ///////////////////////////////////////////////////////////
    
    // Geometry data size in byte
    geom_str_len = geom_out.str().length();
    
    // Total compressed data size in bits
    conn_str_len = hist_str_len + 2 * hole_cnt * 32 + 3 * handle_cnt * 32 + 2 * offset_cnt * 32;
    
    // Total compressed data size in bits
    coded_data_len = ofile_eb.getWriteCount();
    
    
    return true;
}


/*******************************************************************************
Function: bool vertex_coordinate_encode(Quant_index predict_error, int bit_cnt, int coordinate);
Description:
    Arithmetic encoding function
*******************************************************************************/
bool Encoder::vertex_coordinate_encode(Quant_index predict_error, int bit_cnt, int coordinate) {
    // check if input coordinate is valid
    assert(coordinate == 0 || coordinate == 1 || coordinate == 2);
    
    // Total context for each coordinate
    int x_contexts = Context_selector::total_contexts(x_coor_bits, f_levels);
    int y_contexts = Context_selector::total_contexts(y_coor_bits, f_levels);
    
    // Initialize the context 
    Context_selector context(bit_cnt, f_levels); 
    
    // Encode the sign bit using bypass mode 
    // Use bit = 1 to represents both zero and positive value
    // Use bit = 0 to represents negative value
    int signum = SPL::signum(predict_error);
    if(signum == 0) {
        signum = 1;
    }
    else if(signum == -1) {
        signum = 0;
    }
    
    int enc_sign = enc.encodeBypass(signum);
    if (enc_sign) {
        std::cerr << "ERROR: Encode bypass failed\n";
        return false;
    }
    
    // Encode the value part using regular mode 
    for (int i = 0; i < bit_cnt; ++i) {
        // Get current bit for encoding (From MSB to LSB)
        int bit = (std::abs(predict_error) >> (bit_cnt - i - 1)) & 1; 
        
        int context_id = context.get_context();
        int retVal;
        if(context_id == -1) {
            retVal = enc.encodeBypass(bit);
        }
        else {
            if (coordinate == 0) {
                // Encode x coordinate
                retVal = enc.encodeRegular(context_id, bit);
            }
            else if (coordinate == 1) {
                // Encode x coordinate
                int y_id = context_id + x_contexts;
                retVal = enc.encodeRegular(y_id, bit);
            }
            else if (coordinate == 2) {
                // Encode x coordinate
                int z_id = context_id + x_contexts + y_contexts;
                retVal = enc.encodeRegular(z_id, bit);
            }
        }
        
        if (retVal) {
            std::cerr << "ERROR: Encode regular failed\n";
            return false;
        }
        
        // Update context id for next bit
        bool enc_bit = context.encode_bit(bit);
        if (!enc_bit) {
            std::cerr << "ERROR: Update context id failed\n";
            return false;
        }
    }
    
    return true;
}

