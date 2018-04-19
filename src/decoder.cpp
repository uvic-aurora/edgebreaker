#include <iostream>
#include <cassert>
#include <bitset>
#include <utility>
#include <map>
#include <limits>
#include "decoder.hpp"


//////////////////////////////////////////////////////////
// Public functions from Decoder class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Decoder(std::istream& ifile);
Description:
    Decoder class constructors
*******************************************************************************/
Decoder::Decoder(std::istream& ifile)
  : ifile_eb(ifile), dec(total_contexts_cnt) {
    e_vertex = 0;
    triangle_cnt = 0;
    vertex_count = 0;
    m_cnt = 0;
    h_cnt = 0;
    s_cnt = 0;
    coded_data_len = 0;
    conn_str_len = 0;
    geom_str_len = 0;
    x_coor_bits = 0;
    y_coor_bits = 0;
    z_coor_bits = 0;
    
    s_offset.push_back(0);
}


/*******************************************************************************
Function: ~Decoder()
Description:
    Decoder class deconstructors
*******************************************************************************/    
Decoder::~Decoder() {
    --e_vertex;
    --triangle_cnt;
    --vertex_count;
    --m_cnt;
    --h_cnt;
    --s_cnt;
    --coded_data_len;
    --conn_str_len;
    --geom_str_len;
    --x_coor_bits;
    --y_coor_bits;
    --z_coor_bits;
}


/*******************************************************************************
Function: mesh_decompression()
@Description:
    Function used to decompress the triangle mesh. This is the function user 
    should interact with to decompress the entire triangle mesh.
*******************************************************************************/  
void Decoder::mesh_decompression() {
    // Read the EB file from input stream          
    bool file = read_ebfile();
    if (!file) {
        std::cerr << "ERROR: Failed to read the compressed triangle mesh! \n";
        exit(1);
    }
    
    // Decompression preprocessing phase
    initializing();
    for (int node = 0; node <= e_vertex; ++node) {
        b_list.push_back(node);
    }

    assert(vertex_count == 0);
    Point delta = points_vec[vertex_count];
    vertices_vec[e_vertex].point = delta;
    vertices_vec[e_vertex].prediction = true;
    
    // // Print the predicted position and its corresponding original point
    // std::clog << "First point to predict \n";
    // std::clog << "Input point: " << delta << ", predicted position: " << delta << "\n";
    
    // Geometry reconstruct the second vertex in the mesh
    Point a = vertices_vec[e_vertex].point;
    
    delta = points_vec[++vertex_count];
    assert(vertex_count == 1);
    
    Quant_index c_x = delta.x() + a.x();
    Quant_index c_y = delta.y() + a.y();
    Quant_index c_z = delta.z() + a.z();
    
    // // Print the predicted position and its corresponding original point
    // std::clog << "Second point to predict \n";
    // std::clog << "Input point: " << c_x << " " << c_y << " " << c_z << ", predicted position: " << delta << "\n";
    
    vertices_vec[0].point = Point(c_x, c_y, c_z);
    vertices_vec[0].prediction = true;
        
    // Decompression generation phase
    table_generating();

    // Re-quantization all vertices coordinates
    for (auto vit = vertices_vec.begin(); vit != vertices_vec.end(); ++vit) {
        Point p = (*vit).point;
        
        double x = 0.0, y = 0.0, z = 0.0;
        if (step_size.x() != 1.0) {
            x = p.x() * step_size.x();
        }
        else {
            x = p.x();
        }
        
        if (step_size.y() != 1.0) {
            y = p.y() * step_size.y();
        }
        else {
            y = p.y();
        }
        
        if (step_size.z() != 1.0) {
            z = p.z() * step_size.z();
        }
        else {
            z = p.z();
        }
        
        (*vit).point = Point(x, y, z);
    }
    
    // Output the decompressed triangle mesh in OFF format to the standard output
    bool out = write_output();
    if (!out) {
        std::cerr << "ERROR: Output the decompressed triangle mesh failed!\n";
        exit(1);
    }
}

/*******************************************************************************
Function: std::vector<Vertex_index> compression_result();
Description:
    Generates the results information
*******************************************************************************/
std::vector<Vertex_index> Decoder::compression_result() {
    
    //////////////////////////////////////////////////////////
    // Calculate No. of edges and boundaries
    //////////////////////////////////////////////////////////
    
    // Create a map to store all the detected edges
    std::map<std::pair<Vertex_index, Vertex_index>, int> edge_map;
    std::pair<Vertex_index, Vertex_index> edge;
    
    for (auto it = tv_table.begin(); it != tv_table.end(); ++it) {
        Triangle_facet tri_face = *it;
        Vertex_index index_0 = tri_face.get_index(0);
        Vertex_index index_1 = tri_face.get_index(1);
        Vertex_index index_2 = tri_face.get_index(2);
        
        // Insert the first edge
        if(index_0 <= index_1) {
            edge = std::make_pair(index_0, index_1);
            if (edge_map.find(edge) != edge_map.end()) {
                // Current edge already exist
                edge_map[edge] += 1;
            }
            else {
                // Current edge is NOT exist
                edge_map[edge] = 1;
            }
            
        }
        else if (index_0 > index_1) {
            edge = std::make_pair(index_1, index_0);
            if (edge_map.find(edge) != edge_map.end()) {
                // Current edge already exist
                edge_map[edge] += 1;
            }
            else {
                // Current edge is NOT exist
                edge_map[edge] = 1;
            }
        }
        
        // Insert the second edge
        if(index_0 <= index_2) {
            edge = std::make_pair(index_0, index_2);
            if (edge_map.find(edge) != edge_map.end()) {
                // Current edge already exist
                edge_map[edge] += 1;
            }
            else {
                // Current edge is NOT exist
                edge_map[edge] = 1;
            }
        }
        else if (index_0 > index_2) {
            edge = std::make_pair(index_2, index_0);
            if (edge_map.find(edge) != edge_map.end()) {
                // Current edge already exist
                edge_map[edge] += 1;
            }
            else {
                // Current edge is NOT exist
                edge_map[edge] = 1;
            }
        }
        
        // Insert the third edge
        if(index_2 <= index_1) {
            edge = std::make_pair(index_2, index_1);
            if (edge_map.find(edge) != edge_map.end()) {
                // Current edge already exist
                edge_map[edge] += 1;
            }
            else {
                // Current edge is NOT exist
                edge_map[edge] = 1;
            }
        }
        else if (index_2 > index_1) {
            edge = std::make_pair(index_1, index_2);
            if (edge_map.find(edge) != edge_map.end()) {
                // Current edge already exist
                edge_map[edge] += 1;
            }
            else {
                // Current edge is NOT exist
                edge_map[edge] = 1;
            }
        }
    }
    
    // Check No. of edges and No. of boundaries in the mesh
    Vertex_index edges = edge_map.size();
    
    bool closed_mesh = true;
    for (auto it = edge_map.begin(); it != edge_map.end(); ++it) {
        if (it -> second != 2) {
            closed_mesh = false;
            break;
        }
    }
    
    
    //////////////////////////////////////////////////////////
    // Generates the result vector
    //////////////////////////////////////////////////////////
    
    std::vector<Vertex_index> result_vec;
    
    // Basic mesh information
    Vertex_index vertices = vertices_vec.size();
    Vertex_index facets = history_vec.size();
    
    int genus = h_table.size();
    int numBorderLoops = m_table.size();

    // Adjust boundary count for the non-closed meshes
    if (!closed_mesh) {
        numBorderLoops += 1;
    }
    
    // Coded data size
    Vertex_index conn_size = ceil(conn_str_len / 8.0);
    Vertex_index geom_size = ceil(geom_str_len / 8.0);
    Vertex_index code_size = ceil(coded_data_len / 8.0);
    
    result_vec.push_back(vertices);
    result_vec.push_back(edges);
    result_vec.push_back(facets);
    result_vec.push_back(numBorderLoops);
    result_vec.push_back(genus);
    result_vec.push_back(code_size);
    result_vec.push_back(geom_size);
    result_vec.push_back(conn_size);
    
    return result_vec;
}


/*******************************************************************************
Function: Point get_quan_step_size();
Description:
    Generates the results information
*******************************************************************************/
Point Decoder::get_quan_step_size() {
    return step_size;
}


//////////////////////////////////////////////////////////
// private functions from Decoder class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: static void get_history_code1(const std::string& binary_string, 
                                        std::vector<Triangle_type>& hist_vec);
@Description:
    Generate the compression history vector from the input file, this function 
    use code series 1 from D. King's 3.67V bit paper.
*******************************************************************************/
void Decoder::get_history_code1(const std::string& binary_string, std::vector<Triangle_type>& hist_vec) {
    // Initialize current op-code and previous op-code
    Triangle_type opcode = I, prev_opcode = I;
    // Declare three characters to store the input code
    char input_ch, char_2nd, char_3rd;
    
    for (int i = 0; i < binary_string.length(); ++i) {
        input_ch = binary_string[i];
        char_2nd = binary_string[i+1];
        char_3rd = binary_string[i+2];
        
        // C type
        if (input_ch == '0') { 
            opcode = C; 
        }
        
        // Current op-code follows a C code
        else if (input_ch == '1' && prev_opcode == C) {
            // check the second character
            if (char_2nd == '0') { 
                opcode = S; 
            }
            else if (char_2nd == '1') { 
                opcode = R; 
            }
            
            // Update the string location count
            i += 1;
        }
        
        // Current op-code NOT follows a C code
        else if (input_ch == '1' && prev_opcode != C) {
            // check the second and third characters
            if (char_2nd == '0' && char_3rd == '0') { 
                opcode = S; 
            }
            else if (char_2nd == '0' && char_3rd == '1') { 
                opcode = R; 
            }
            else if (char_2nd == '1' && char_3rd == '0') { 
                opcode = L; 
            }
            else if (char_2nd == '1' && char_3rd == '1') { 
                opcode = E; 
            }
            
            // Update the string location count
            i += 2;
        }
        
        // Add current op-code to the history vector
        hist_vec.push_back(opcode);
        
        // Update the previous op-code
        prev_opcode = opcode;
    }
}


/*******************************************************************************
Function: static void get_history_code2(const std::string& binary_string, 
                                        std::vector<Triangle_type>& hist_vec);
@Description:
    Generate the compression history vector from the input file, this function 
    use code series 2 from D. King's 3.67V bit paper.
*******************************************************************************/
void Decoder::get_history_code2(const std::string& binary_string, std::vector<Triangle_type>& hist_vec) {
    // Initialize current op-code and previous op-code
    Triangle_type opcode = I, prev_opcode = I;
    // Declare three characters to store the input code
    char input_ch, char_2nd, char_3rd;
    
    for (int i = 0; i < binary_string.length(); ++i) {
        input_ch = binary_string[i];
        char_2nd = binary_string[i+1];
        char_3rd = binary_string[i+2];
        
        // C type follows another C type
        if (input_ch == '0' && prev_opcode == C) { 
            opcode = C; 
        }
        
        // Current op-code follows a C code
        else if (input_ch == '1' && prev_opcode == C) {
            // check the second character
            if (char_2nd == '0') { 
                opcode = S; 
            }
            else if (char_2nd == '1') { 
                opcode = R; 
            }
            
            // Update the string location count
            i += 1;
        }
        
        // Current op-code NOT follows a C code (R, L, S type)
        else if (input_ch == '1' && prev_opcode != C) {
            // check the second character
            if (char_2nd == '0') { 
                opcode = R; 
            }
            else if (char_2nd == '1') {
                // check the third character
                if (char_3rd == '0') { 
                    opcode = L; 
                }
                else if (char_3rd == '1') { 
                    opcode = S; 
                }
                
                // Update the string location count
                i += 1;
            }
            
            // Update the string location count
            i += 1;
        }
        
        // Current op-code NOT follows a C code (C, E type)
        else if (input_ch == '0' && prev_opcode != C) {
            // check the second character
            if (char_2nd == '0') { 
                opcode = C; 
            }
            else if (char_2nd == '1') { 
                opcode = E; 
            }
            
            // Update the string location count
            i += 1;
        }
        
        // Add current op-code to the history vector
        hist_vec.push_back(opcode);
        
        // Update the previous op-code
        prev_opcode = opcode;
    }
}


/*******************************************************************************
Function: static void get_history_code3(const std::string& binary_string, 
                                        std::vector<Triangle_type>& hist_vec);
@Description:
    Generate the compression history vector from the input file, this function 
    use code series 3 from D. King's 3.67V bit paper.
*******************************************************************************/
void Decoder::get_history_code3(const std::string& binary_string, std::vector<Triangle_type>& hist_vec) {
    // Initialize current op-code and previous op-code
    Triangle_type opcode = I, prev_opcode = I;
    // Declare three characters to store the input code
    char input_ch, char_2nd, char_3rd;
    
    for (int i = 0; i < binary_string.length(); ++i) {
        input_ch = binary_string[i];
        char_2nd = binary_string[i+1];
        char_3rd = binary_string[i+2];
        
        // C type follows another C type
        if (input_ch == '0' && prev_opcode == C) { 
            opcode = C; 
        }
        
        // Current op-code follows a C code
        else if (input_ch == '1' && prev_opcode == C) {
            // check the second character
            if (char_2nd == '0') { 
                opcode = S; 
            }
            else if (char_2nd == '1') { 
                opcode = R; 
            }
            
            // Update the string location count
            i += 1;
        }
        
        // Current op-code NOT follows a C code (L, E type)
        else if (input_ch == '1' && prev_opcode != C) {
            // check the second character
            if (char_2nd == '0') { 
                opcode = L; 
            }
            else if (char_2nd == '1') { 
                opcode = E; 
            }
            
            // Update the string location count
            i += 1;
        }
        
        // Current op-code NOT follows a C code (C, S, R type)
        else if (input_ch == '0' && prev_opcode != C) {
            // check the second character
            if (char_2nd == '0') { 
                opcode = C; 
            }
            else if (char_2nd == '1') {
                // check the third character
                if (char_3rd == '0') { 
                    opcode = S; 
                }
                else if (char_3rd == '1') { 
                    opcode = R; 
                }
                
                // Update the string location count
                i += 1;
            }
            
            // Update the string location count
            i += 1;
        }
        
        // Add current op-code to the history vector
        hist_vec.push_back(opcode);
        
        // Update the previous op-code
        prev_opcode = opcode;
    }
}


/*******************************************************************************
Function: distinguish_opcode()
@Description:
    Distinguish between the S and M operation, this function is only called if
    the compressed triangle mesh contains the M type triangle
*******************************************************************************/
void Decoder::distinguish_opcode() {
    // Variable initialization 
    Vertex_index m_type_cnt = 0, h_type_cnt = 0, skip_h = 0, skip_m = 0; 
    
    for(auto iter = history_vec.begin(); iter != history_vec.end(); ++iter) {
        Triangle_type opcode = *iter;

        // Distinguish between S, M and M' operations
        if (opcode == S) {
            bool real_s_type = true;
            
            // Real M type
            if (!m_table.empty() && m_type_cnt < m_table.size()) {
                Vertex_index skip_m_cnt = m_table[m_type_cnt].first;
                if (skip_m == skip_m_cnt && m_type_cnt < m_table.size()) {
                    *iter = M;
                    m_type_cnt += 1;
                    skip_m = 0; 
                    real_s_type = false;
                }
            }
            
            // Real M' case
            if (!h_table.empty() && h_type_cnt < h_table.size()) {
                Vertex_index skip_h_cnt = h_table[h_type_cnt].skip_cnt();                
                if (skip_h == skip_h_cnt && h_type_cnt < h_table.size()) {
                    *iter = H;
                    h_type_cnt += 1;
                    skip_h = 0; 
                    real_s_type = false;
                }
            }
            
            // Real S type
            if (real_s_type) { 
                skip_m += 1;
                skip_h += 1;
            }
        }
    }
}


/*******************************************************************************
Function: bool vertex_coordinate_decode(int bit_cnt, int coordinate, Quant_index& coor);
@Description:
    Using arithmetic coding functions in SPL library to decompress the 
    geometry information from the input stream
*******************************************************************************/
bool Decoder::vertex_coordinate_decode(int bit_cnt, int coordinate, Quant_index& coor) {
    // check if input coordinate is valid
    assert(coordinate == 0 || coordinate == 1 || coordinate == 2);
    
    // Initialize coordinate
    coor = 0;
    
    // Total context for each coordinate
    int x_contexts = Context_selector::total_contexts(x_coor_bits, f_levels);
    int y_contexts = Context_selector::total_contexts(y_coor_bits, f_levels);
    
    // Initialize the context 
    Context_selector context(bit_cnt, f_levels); 

    // Initialize a string to store all bits in the digit
    std::string digits;
    
    // Decode the sign bit using bypass mode 
    // Use bit = 1 to represents both zero and positive value
    // Use bit = 0 to represents negative value
    int signum = dec.decodeBypass();
    if (signum < 0) {
        std::cerr << "ERROR: Decode bypass failed\n";
        return false;
    }
    
    // Decode the value part using regular mode 
    for (int i = 0; i < bit_cnt; ++i) {
        int context_id = context.get_context();            
        int bit;
        if(context_id == -1) {
            bit = dec.decodeBypass();
        }
        else {
            if (coordinate == 0) {
                // Encode x coordinate
                bit = dec.decodeRegular(context_id);
            }
            else if (coordinate == 1) {
                // Encode x coordinate
                int y_id = context_id + x_contexts;
                bit = dec.decodeRegular(y_id);
            }
            else if (coordinate == 2) {
                // Encode x coordinate
                int z_id = context_id + x_contexts + y_contexts;
                bit = dec.decodeRegular(z_id);
            }
        }
        
        if (bit < 0) {
            std::cerr << "ERROR: Decode regular failed " << i << "\n";
            return false;
        }

        // Update bit in digit
        coor = 2 * coor + bit;
        
        // Update context id for next bit
        bool enc_bit = context.encode_bit(bit);
        if (!enc_bit) {
            std::cerr << "ERROR: Update context id failed\n";
            return false;
        }
    }
    
    // Update the coordinate
    if (signum == 0) {
        // Negative digit
        coor = -1 * coor;
    }

    return true;
}


/*******************************************************************************
Function: bool read_ebfile()
@Description:
    Read the entire EB file from the given input stream
*******************************************************************************/
bool Decoder::read_ebfile() {

    //////////////////////////////////////////////////////////
    // Read the header part of the EB file
    //////////////////////////////////////////////////////////
   
    // signature
    ifile_eb.getBits(redundant_bits);
    unsigned int sig = ifile_eb.getBits(max_bits_to_put);
    if (sig != 696610198) {
        std::cerr << "ERROR: EB file in the bad condition!\n";
    }
    
    // code series
    ifile_eb.getBits(redundant_bits);
    int code_series = ifile_eb.getBits(max_bits_to_put);
    
    // hist_str_len
    ifile_eb.getBits(redundant_bits);
    Vertex_index hist_str_len = ifile_eb.getBits(max_bits_to_put);
    
    // vertices_cnt
    ifile_eb.getBits(redundant_bits);
    Vertex_index vertices_cnt = ifile_eb.getBits(max_bits_to_put);
    
    // hole_cnt
    ifile_eb.getBits(redundant_bits);
    unsigned int hole_cnt = ifile_eb.getBits(max_bits_to_put);
    
    // handle_cnt
    ifile_eb.getBits(redundant_bits);
    unsigned int handle_cnt = ifile_eb.getBits(max_bits_to_put);
    
    // offset_cnt
    ifile_eb.getBits(redundant_bits);
    unsigned int offset_cnt = ifile_eb.getBits(max_bits_to_put);
    
    // x_coor_bits
    ifile_eb.getBits(redundant_bits);
    x_coor_bits = ifile_eb.getBits(max_bits_to_put);
    
    // y_coor_bits
    ifile_eb.getBits(redundant_bits);
    y_coor_bits = ifile_eb.getBits(max_bits_to_put);
    
    // z_coor_bits
    ifile_eb.getBits(redundant_bits);
    z_coor_bits = ifile_eb.getBits(max_bits_to_put);
    
    // step_size x coordinate
    ifile_eb.getBits(redundant_bits);
    Quant_index coef_x = ifile_eb.getBits(max_bits_to_put);
    
    ifile_eb.getBits(1);
    int signum_x = ifile_eb.getBits(1);
    int exp_x = ifile_eb.getBits(max_bits_to_put);
    if (signum_x == 0) {
        // Negative exponent
        exp_x = -1 * exp_x;
    }
    
    double quan_x = pow2double(coef_x, exp_x);

    
    // step_size y coordinate
    ifile_eb.getBits(redundant_bits);
    Quant_index coef_y = ifile_eb.getBits(max_bits_to_put);
    
    ifile_eb.getBits(1);
    int signum_y = ifile_eb.getBits(1);
    int exp_y = ifile_eb.getBits(max_bits_to_put);
    if (signum_y == 0) {
        // Negative exponent
        exp_y = -1 * exp_y;
    }
    
    double quan_y = pow2double(coef_y, exp_y);
    
    
    // step_size z coordinate
    ifile_eb.getBits(redundant_bits);
    Quant_index coef_z = ifile_eb.getBits(max_bits_to_put);
    
    ifile_eb.getBits(1);
    int signum_z = ifile_eb.getBits(1);
    int exp_z = ifile_eb.getBits(max_bits_to_put);
    if (signum_z == 0) {
        // Negative exponent
        exp_z = -1 * exp_z;
    }
    
    double quan_z = pow2double(coef_z, exp_z);
        
    
    // Generate the quantization step size
    step_size = Point(quan_x, quan_y, quan_z);
    
    // Byte alignment for header part
    ifile_eb.align();
    
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
    // Read the history part of the EB file
    //////////////////////////////////////////////////////////
    
    // Generate the binary history string
    std::string history_string;
    for(auto i = 0; i < hist_str_len; ++i) {
        int bit = ifile_eb.getBits(1);
        history_string += static_cast<char>(bit + '0');
    }
    
    // Byte alignment for connectivity part
    ifile_eb.align();

    // Store the compression history into the history vector
    switch(code_series) {
        case 1:
            // Compression history is stored in binary format using code1
            get_history_code1(history_string, history_vec);
            break;
            
        case 2:
            // Compression history is stored in binary format using code2
            get_history_code2(history_string, history_vec);
            break;
            
        case 3:
            // Compression history is stored in binary format using code3
            get_history_code3(history_string, history_vec);
            break;
    }
    
    // Make sure op-code sequence vector is not empty
    assert(!history_vec.empty());
    
    
    //////////////////////////////////////////////////////////
    // Read the M table part of the EB file
    //////////////////////////////////////////////////////////
    
    if (hole_cnt != 0) {
        for (int i = 0; i < hole_cnt; ++i) {
            ifile_eb.getBits(redundant_bits);
            int skip_m_cnt = ifile_eb.getBits(max_bits_to_put);
            
            ifile_eb.getBits(redundant_bits);
            int hole_len = ifile_eb.getBits(max_bits_to_put);
            
            std::pair<int, int> m_item(skip_m_cnt, hole_len);
            m_table.push_back(m_item);
        }
        
        // Byte alignment
        ifile_eb.align();
        
        // Make sure M table is not empty
        assert(!m_table.empty());
    }
    
    
    //////////////////////////////////////////////////////////
    // Read the M' table part of the EB file
    //////////////////////////////////////////////////////////
    
    if (handle_cnt != 0) {
        for (int i = 0; i < handle_cnt; ++i) {
            ifile_eb.getBits(redundant_bits);
            int pos = ifile_eb.getBits(max_bits_to_put);
            
            ifile_eb.getBits(redundant_bits);
            int offset_val = ifile_eb.getBits(max_bits_to_put);
            
            ifile_eb.getBits(redundant_bits);
            int skip_h_cnt = ifile_eb.getBits(max_bits_to_put);
            
            Mesh_handle handle_item(pos, offset_val, skip_h_cnt);
            h_table.push_back(handle_item);
        }
        
        // Byte alignment
        ifile_eb.align();
        
        // Make sure H table is not empty
        assert(!h_table.empty());
    }
    
    // Distinguish between the real S, M and M' operations
    if (hole_cnt != 0 || handle_cnt != 0) {
        distinguish_opcode();
    }
    
    
    //////////////////////////////////////////////////////////
    // Read the offset vector part of the EB file
    //////////////////////////////////////////////////////////
    
    if (offset_cnt != 0) {
        for (int i = 0; i < offset_cnt; ++i) {
            ifile_eb.getBits(redundant_bits);
            int s_type_cnt = ifile_eb.getBits(max_bits_to_put);
            
            ifile_eb.getBits(redundant_bits);
            int offset_val = ifile_eb.getBits(max_bits_to_put);
            
            std::pair<int, int> offset_pair(s_type_cnt, offset_val);
            offset_vec.push_back(offset_pair);
        }
        
        // Byte alignment
        ifile_eb.align();
        
        // Make sure H table is not empty
        assert(!offset_vec.empty());
    }
    
    
    //////////////////////////////////////////////////////////
    // Read the geometry part of the EB file
    //////////////////////////////////////////////////////////
    
    // Start bit count for the geometry part
    Vertex_index geom_begin = ifile_eb.getReadCount();
    
    // Set the Arithmetic decoder bit stream
    dec.setInput(&ifile_eb);
    
    // Starts the arithmetic decoder
    dec.start();

    Quant_index x = 0, y = 0, z = 0;
    for (auto i = 0; i < vertices_cnt; ++i) {
        // Decode x coordinate
        bool coor_x = vertex_coordinate_decode(x_coor_bits, 0, x);
        if (!coor_x) {
            std::cerr << "ERROR: Decode x coordinate failed\n";
            return false;
        }
        
        // Decode y coordinate
        bool coor_y = vertex_coordinate_decode(y_coor_bits, 1, y);
        if (!coor_y) {
            std::cerr << "ERROR: Decode y coordinate failed\n";
            return false;
        }
        
        // Decode z coordinate
        bool coor_z = vertex_coordinate_decode(z_coor_bits, 2, z);
        if (!coor_z) {
            std::cerr << "ERROR: Decode z coordinate failed\n";
            return false;
        }
        
        points_vec.push_back(Point(x, y, z));
    }
    
    // Terminate the arithmetic decoder
    if (dec.terminate() < 0) {
        std::cerr << "ERROR: Decode terminate failed\n";
        return false;
    }
    
    // Make sure points vector size is equals to the vertices_cnt
    assert(points_vec.size() == vertices_cnt);
    
    // Resize the vertices vector size to the vertices count and initialize
    vertices_vec.resize(vertices_cnt);
    for (auto vit = vertices_vec.begin(); vit != vertices_vec.end(); ++vit) {
        (*vit).prediction = false;
    }
    
    // End bit count for the geometry part
    Vertex_index geom_end = ifile_eb.getReadCount();
    
    
    ///////////////////////////////////////////////////////////
    // Output information for comparison
    ///////////////////////////////////////////////////////////
    
    // Geometry data size in bits
    geom_str_len = geom_end - geom_begin;
    
    // Total compressed data size in bits
    conn_str_len = hist_str_len + 2 * hole_cnt * 32 + 3 * handle_cnt * 32 + 2 * offset_cnt * 32;
    
    // Output all the redundant bits
    while(!ifile_eb.isEof()) {
        ifile_eb.getBits(1);
    }
    
    // Byte alignment
    ifile_eb.align();
    
    // Total compressed data size in bits
    coded_data_len = ifile_eb.getReadCount();
    
    return true;
}


/*******************************************************************************
Function: geometry_processing(Triangle_facet tri_label);
Description:
    Use Parallelogram Prediction scheme to reconstruct the vertex location
    for the given point c. This function is used to process the all mesh's 
    vertices except the start and end vertex of the initial active gate.
    
Parameters:
    Triangle_facet: The vertices indices of current triangle
*******************************************************************************/
void Decoder::geometry_processing(Triangle_facet tri_label) {
    // Get vertices indices for points a, b, and delta
    Vertex_index index_a = tri_label.get_index(0);
    Vertex_index index_b = tri_label.get_index(1);
    Vertex_index index_delta = tri_label.get_index(2);
    assert(index_a != index_b && index_a != index_delta && index_b != index_delta);
    
    // Get points a and b
    Point a = vertices_vec[index_a].point;
    Point b = vertices_vec[index_b].point;
    assert(vertices_vec[index_a].prediction == true && vertices_vec[index_b].prediction == true);

    // Initialize predicted point
    Point predict;
    if (triangle_cnt == 1) {
        // Geometry processing for the third vertex of the mesh
        // std::clog << "Geometry prediction without third vertex \n";
        predict = geometry_predict(&a, &b, nullptr);
    }
    else {
        Point d = vertices_vec[prev_vertex_d].point;
        predict = geometry_predict(&a, &b, &d);
    }
    // std::clog << "Calculated position: " << predict << "\n";

    // Geometry prediction for the third vertex of current triangle
    if (vertices_vec[index_delta].prediction == false) {
        Point delta = points_vec[++vertex_count];
        
        // Point reconstruction
        Quant_index c_x = delta.x() + static_cast<Quant_index>(predict.x());
        Quant_index c_y = delta.y() + static_cast<Quant_index>(predict.y());
        Quant_index c_z = delta.z() + static_cast<Quant_index>(predict.z());
        
        // // Print the predicted position and its corresponding original point
        // std::clog << "Input point: " << c_x << " " << c_y << " " << c_z << ", predicted position: " << delta << "\n";
        
        vertices_vec[index_delta].point = Point(c_x, c_y, c_z);
        vertices_vec[index_delta].prediction = true;
    }
}


/*******************************************************************************
Function: initializing()
@Description:
    Function for the decompression preprocessing phase. It gets the triangle 
    count, the external vertices count and the offset table for S operation.
*******************************************************************************/
void Decoder::initializing() {
    // Variable initialization
    int m_count = 0;  // The local M operation count
    int h_count = 0;  // The local M operation count
    int s_count = 0;  // S type triangle count
    std::pair<int, int> es_pair;  // A pair stores the current e and s value
    std::stack<std::pair<int, int>> es_stack;   // A stack stores es_pair value
    
    // Make sure the es_stack is empty
    assert(es_stack.empty());

    for (auto iter = history_vec.begin(); iter != history_vec.end(); ++iter) {
        Triangle_type opcode = *iter;

        // Update the required variable based on the current op-code
        switch(opcode) {
            case C:
                e_vertex -= 1;
                break;
                
            case L:
                e_vertex += 1;
                break;
                
            case R:
                e_vertex += 1;
                break;
                
            case E:
                e_vertex += 3;
                
                if (!es_stack.empty()) {
                    // Store e, s value to e_pop and s_pop
                    es_pair = es_stack.top();

                    // Update the offset table
                    s_offset[es_pair.second] = e_vertex - es_pair.first - 2;
                    
                    // Pop the stack
                    es_stack.pop();
                }
                break;
                
            case S:
                e_vertex -= 1;
                s_count += 1;
                
                // Enlarge the offset table by one
                s_offset.push_back(0);
                
                // Push the es pair to the stack
                es_pair = std::make_pair(e_vertex, s_count);
                es_stack.push(es_pair);
                
                // Make sure the es_stack is not empty
                assert(!es_stack.empty()); 
                break;
                
            case H:
            {
                e_vertex -= 1;
                
                // Update offset vector for S type triangle
                int es_stack_size = es_stack.size();
                for (auto i = h_count; i < h_count + es_stack_size; ++i) {
                    std::pair<int, int> offset_pair = offset_vec[i];
                    s_offset[offset_pair.first] = offset_pair.second;
                }
                
                // Update local M' count
                h_count += es_stack_size;
                
                // Empty the es pair stack
                while (!es_stack.empty()) {
                    es_stack.pop();
                }
                break;
            }
            
            case M:
            {
                int length = m_table[m_count].second;
                assert(length >= 3);
                e_vertex -= (length + 1);
                m_count += 1;
                break;
            }
        }
    }
    
    // Adjust the external vertices count (the label of vertices start from 0) 
    e_vertex -= 1;    
}


/*******************************************************************************
Function: table_generating()
@Description:
    Function for the decompression generation phase. It creates the 
    Triangle-Vertices table from the compression history based on the 
    information from the preprocessing phase
*******************************************************************************/  
void Decoder::table_generating(){    
    // Variable initialization
    bool e_case = false; // Stop condition
    Vertex_index insert_data = 0; // New node that needs to insert into the circular list 
    Triangle_facet tri_label(0, 0, 0); // Triangle_facet object for TV table
    Circ_list<Vertex_index>::Const_circulator gate = b_list.begin(); // The active gate
    Vertex_index updated_d = prev_vertex_d; // The updated point d used for next triangle
    
    do {
        // Update the points for geometry reconstruction
        prev_vertex_d = updated_d;
        
        // Get the previous and next gate on the bounding loop
        Circ_list<Vertex_index>::Const_circulator gate_prev = gate; --gate_prev;
        Circ_list<Vertex_index>::Const_circulator gate_next = gate; ++gate_next;
        Circ_list<Vertex_index>::Const_circulator gate_prev_prev = gate_prev; --gate_prev_prev;
        
        // Get the current op-code from the compression history
        Triangle_type opcode = history_vec[triangle_cnt];
        
        switch(opcode) {
            case C:
                // Make sure the gate, gate_prev and (e_vertex + 1) are different from each other
                assert(*gate != *gate_prev && *gate != (e_vertex + 1) && *gate_prev != (e_vertex + 1));
        
                // TV table: (G.P, G, ++e)
                tri_label = Triangle_facet(*gate_prev, *gate, ++e_vertex);
                tv_table.push_back(tri_label);
                
                // Loop update: Insert ++e node between G.P and G
                b_list.insert(gate, e_vertex);
                
                // Make sure the gate is remain the same
                assert(*gate != e_vertex);
                
                // Update point d for the next triangle
                updated_d = tri_label.get_index(0);
                break;
                
            case L:
                // Make sure the gate, gate_prev and gate_prev_prev are different from each other
                assert(*gate != *gate_prev && *gate != *gate_prev_prev && *gate_prev != *gate_prev_prev);
        
                // TV table: (G.P, G, G.P.P)
                tri_label = Triangle_facet(*gate_prev, *gate, *gate_prev_prev);
                tv_table.push_back(tri_label);
              
                // Loop update: Delete node G.P, connect G.P.P and G directly
                b_list.erase(gate_prev);
                
                // Update point d for the next triangle
                updated_d = tri_label.get_index(0);
                break;
                
            case R:
                // Make sure the gate, gate_prev and gate_next are different from each other
                assert(*gate != *gate_prev && *gate != *gate_next && *gate_prev != *gate_next);
        
                // TV table: (G.P, G, G.N)
                tri_label = Triangle_facet(*gate_prev, *gate, *gate_next);
                tv_table.push_back(tri_label);

                // Loop update: Delete node G, connect G.P and G.N directly
                b_list.erase(gate);
                
                // Gate update: gate = gate -> next
                gate = gate_next;
                
                // Make sure the gate is update to gate_next
                assert(gate == gate_next);
                
                // Update point d for the next triangle
                updated_d = tri_label.get_index(1);
                break;
                
            case E:
                // Make sure the gate, gate_prev and gate_next are different from each other
                assert(*gate != *gate_prev && *gate != *gate_next && *gate_prev != *gate_next);
        
                // TV table: (G.P, G, G.N)
                tri_label = Triangle_facet(*gate_prev, *gate, *gate_next);
                tv_table.push_back(tri_label);

                // Loop update: Pop node G
                b_list.clear();
                
                // Make sure the current bounding list is empty
                assert(b_list.empty());
                
                // Terminate decompression for the current history stream
                e_case = true;
                break;

            case M:
                // Make sure the gate, gate_prev and (e_vertex + 1) are different from each other
                assert(*gate != *gate_prev && *gate != (e_vertex + 1) && *gate_prev != (e_vertex + 1));
        
                // TV table: (G.P, G, ++e)
                tri_label = Triangle_facet(*gate_prev, *gate, ++e_vertex);
                tv_table.push_back(tri_label);
 
                // Loop update: Insert (l + 1) nodes between G.P and G
                for(int i = e_vertex; i < (e_vertex + m_table[m_cnt].second); ++i) {
                    b_list.insert(gate, i);
                }
                b_list.insert(gate, e_vertex);
                
                // Update the external vertices count 
                e_vertex = e_vertex + m_table[m_cnt].second - 1;
                
                // M type count update
                m_cnt += 1;
                
                // Update point d for the next triangle
                updated_d = tri_label.get_index(0);
                break;
              
            case H:
            {
                // Fetch the entry from stack
                My_stack<std::pair<Circ_list<Vertex_index>, Vertex_index>>::Const_iterator gate_iter = std::next(list_stack.begin(), h_table[h_cnt].position());
                Circ_list<Vertex_index> remove_list((*gate_iter).first);
                
                int remove_list_size = remove_list.size();
                Circ_list<Vertex_index>::Const_circulator gate_d = remove_list.begin();
                
                // D = G.N, Repeat D=D.N for O[++s] times
                for (int i = 1; i <= h_table[h_cnt].offset(); ++i) {
                    ++gate_d;
                }
                
                // Preparation for the insert node
                Vertex_index insert_data = *gate_d;
                
                // Make sure the gate, gate_prev and gate_next are different from each other
                assert(*gate != *gate_prev && *gate != *gate_d && *gate_prev != *gate_d);
                
                //  TV table: (G.P, G, D)
                tri_label = Triangle_facet(*gate_prev, *gate, *gate_d);
                tv_table.push_back(tri_label);
                
                // Loop update: Insert node after G.P
                b_list.insert(gate, *gate_d);
                
                // Loop update: Splice another list into current list
                Circ_list<Vertex_index>::Const_circulator gate_d_next = gate_d;
                b_list.splice(gate, remove_list, ++gate_d_next, gate_d, remove_list_size - 1);
                b_list.insert(gate, insert_data);
                
                // Remove the entry from stack
                remove_list.clear();
                list_stack.erase(gate_iter);
                
                // M' type count update
                h_cnt += 1;
                
                // Update point d for the next triangle
                updated_d = tri_label.get_index(0);
                break; 
            }
            
            case S:
            {
                // D = G.N, Repeat D=D.N for O[++s] times
                s_cnt += 1;
                for (int i = 1; i <= s_offset[s_cnt]; ++i) {
                    ++gate_next;
                }
                
                // Preparation for right side sub list
                Vertex_index insert_data = *gate_next;
                
                // Make sure the gate, gate_prev and gate_next are different from each other
                assert(*gate != *gate_prev && *gate != *gate_next && *gate_prev != *gate_next);
                
                //  TV table: (G.P, G, D)
                tri_label = Triangle_facet(*gate_prev, *gate, *gate_next);
                tv_table.push_back(tri_label);
                
                // Geometry reconstruction for the S type triangle
                triangle_cnt += 1;
                geometry_processing(tri_label);
                
                // Update point d for right side sub mesh
                prev_vertex_d = tri_label.get_index(0);
                
                // Update point d for left side sub mesh
                Vertex_index d_left = tri_label.get_index(1);
                
                // Second (left side) sub-loop: (Connect G.P and D directly)
                Circ_list<Vertex_index> sub_list_2;
                sub_list_2.splice(sub_list_2.begin(), b_list, gate_next, gate, b_list.size() - s_offset[s_cnt] - 1);
                
                // First (right side) sub-loop: (Connect D and G directly)
                b_list.push_back(insert_data);
                
                // Store sub list and point d for the left side sub mesh
                std::pair<Circ_list<Vertex_index>, Vertex_index> list_pair(std::move(sub_list_2), d_left);
                list_stack.push(std::move(list_pair));
                
                // Generate the Triangle-Vertices table for the right side sub-loop      
                table_generating();

                // Update the triangle count for the left side sub-loop
                triangle_cnt -= 1;
                
                // Compress the left side of sub mesh
                if (!list_stack.empty()) {
                    b_list = std::move(list_stack.top().first);
                    updated_d = list_stack.top().second;
                    list_stack.pop();
                    
                    // Gate update: gate = D
                    gate = b_list.begin();
                }
                else {
                    e_case = true;
                }
                
                break; 
            }
        }
        
        // Update the triangle count 
        triangle_cnt += 1;
        
        // Geometry prediction for the third vertex of current triangle
        if (opcode != S) {
            geometry_processing(tri_label);
        }

    } while(!e_case);
}


/*******************************************************************************
Function: write_output()
@Description:
    Output the decompressed triangle mesh in OFF format to the standard output
*******************************************************************************/ 
bool Decoder::write_output() {
    std::cout << "OFF\n"
              << points_vec.size() << " " << history_vec.size() << " 0\n";

    // Geometry information of the OFF file
    std::cout.precision(std::numeric_limits<double>::max_digits10);
    for (auto iter = vertices_vec.begin(); iter != vertices_vec.end(); ++iter) {
        std::cout << (*iter).point << "\n";
    }
    
    // Connectivity information of the OFF file
    for (auto iter = tv_table.begin(); iter != tv_table.end(); ++iter) {
        std::cout << "3 " << *iter << "\n";
    }
    std::cout << "\n\n";
    
    std::cout.flush();
    if(!std::cout) {
        return false;
    }
    
    return true;
}

