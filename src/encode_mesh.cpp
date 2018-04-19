#include "encoder.hpp"
#include <fstream>
#include <chrono>
#include <cmath>
#include <CGAL/Bbox_3.h>
#include <SPL/Timer.hpp>


// Some basic types to be used with CGAL Polyhedron_3 class
typedef CGAL::Bbox_3 Bbox_3;


/*******************************************************************************
Function: usage()
Description:
    Print the usage info to the standard output stream
*******************************************************************************/
void usage() {
    std::cout << "=============================================================================\n"
              << "Usage:\n"
              << "encode_mesh [OPTIONS] \n"
              << "-x $quan_x:  The x coordinate quantization step size. If not specified,\n"
              << "             default to (xmax - xmin) / 256.\n"
              << "-y $quan_y:  The y coordinate quantization step size. If not specified,\n"
              << "             default to (ymax - ymin) / 256.\n"
              << "-z $quan_z:  The z coordinate quantization step size. If not specified,\n"
              << "             default to (zmax - zmin) / 256.\n"
              << "-b $no_bits: How many bits user wants to use to encode the mesh's vertex. \n"
              << "             If not specified, default to 16 bits.\n"
              << "-h:          Print the help info about the different command line options.  \n"
              << "-r $results: Write results information in order to the file called $results.  \n"
              << "-d:          Check if any duplicated vertex in the input triangle mesh. If input \n"
              << "             mesh contains duplicated vertices, remove them \n"
              << "-s $scaling: Scaling factor used to scaling each vertex's coordinates.\n"
              << "             If not specified, default to 1 (no scaling).\n"
              << "-c:          Calculate the coding efficiency for the compressed EB file. \n"
              << "\n"
              << "The input triangle mesh is stored in OFF format.\n"
              << "The output compressed triangle mesh is stored in EB format.\n"
              << "=============================================================================\n";
}


/*******************************************************************************
Function: bounding_box(Polyhedron poly_mesh)
Description:
    Calculate the bounding box of the input triangle mesh
    
Parameter:
    poly_mesh: The input triangle mesh
    
Return:
    A vector which contains the bounding box information of the input mesh. The
    sequence of bounding box info: xmin -> xmax -> ymin -> ymax -> zmin -> zmax
*******************************************************************************/
#ifdef BUG
Bbox_3 bounding_box(Polyhedron poly_mesh) {
#else
Bbox_3 bounding_box(const Polyhedron& poly_mesh) {
#endif
    
    // Initialize the bounding box object
    Bbox_3 bbox;
    
    // Compute the bounding box of the mesh.
    Point v_begin = poly_mesh.vertices_begin() -> point();
    bbox = Bbox_3(v_begin.x(), v_begin.y(), v_begin.z(), v_begin.x(), v_begin.y(), v_begin.z());
    
    for (auto vit = poly_mesh.vertices_begin(); vit != poly_mesh.vertices_end(); ++vit) {
        Point p = vit -> point();
        bbox = bbox + Bbox_3(p.x(), p.y(), p.z(), p.x(), p.y(), p.z());
    }
    
    return bbox;
}



/*******************************************************************************
Function: main()
@Description:
    Main function for the mesh compression program
*******************************************************************************/
int main(int argc, char** argv) {
    
    //////////////////////////////////////////////////////////
    // Read the command line argument
    //////////////////////////////////////////////////////////
    
    // Variable initialization
    double quan_x = 0.0, quan_y = 0.0, quan_z = 0.0;    // Step size coordinate
    int no_bits = 0;    // No. of bits to encode
    int output_result = 0;    // Output result to given file
    std::string result_fname;    // Output result file name
    bool remove_vertices = false;    // Remove duplicated vertices
    bool scaling = false;   // Scaling the vertices
    bool efficiency = false;   // Code efficiency
    double scaling_factor = 1;  // The scaling factor
    
    // User specified quantization step size
    char opt;
    while ((opt = getopt(argc, argv, "x:y:z:b:hr:ds:c")) != EOF) {
        switch (opt) {
            case 'x':
            {
                double input_x = atof(optarg);
                if (input_x <= 0.0) {
                    std::cerr << "ERROR: Quantization step size for x coordinate cannot be negative or zero!\n";
                    return 2;
                }
                else {
                    quan_x = input_x;
                }
                break;
            }
                
            case 'y':
            {
                double input_y = atof(optarg);
                if (input_y <= 0.0) {
                    std::cerr << "ERROR: Quantization step size for y coordinate cannot be negative or zero!\n";
                    return 2;
                }
                else {
                    quan_y = input_y;
                }
                break;
            }

            case 'z':
            {
                double input_z = atof(optarg);
                if (input_z <= 0.0) {
                    std::cerr << "ERROR: Quantization step size for z coordinate cannot be negative or zero!\n";
                    return 2;
                }
                else {
                    quan_z = input_z;
                }
                break;
            }
              
            case 'b':
            {
                int bits = atoi(optarg);
                if (bits <= 0) {
                    std::cerr << "ERROR: No. of bits to encoded cannot be negative or zero!\n";
                    return 2;
                }
                else {
                    no_bits = bits;
                }
                break;
            }
                
            case 'h':
                usage();
                return 0;
                break;  
                
            case 'r':
                output_result = 1;
                result_fname = optarg;
                break;
                
            case 'd':
                remove_vertices = true;
                break;
                
            case 's':
            {
                double scale = atof(optarg);
                if (scale <= 0.0) {
                    std::cerr << "ERROR: The scaling factor cannot be zero or negative!\n";
                    return 2;
                }
                else {
                    scaling_factor = scale;
                    scaling = true;
                }
                break;
            }
            
            case 'c':
                efficiency = true;
                break;
        }
    }
    
    
    //////////////////////////////////////////////////////////
    // Read the input mesh and check its type
    //////////////////////////////////////////////////////////
    
    Polyhedron mesh;

    // Read the input mesh from standard input in OFF format.
    if (!(std::cin >> mesh)) {
        usage();
        std::cerr << "ERROR: Cannot read input mesh\n";
        return 1;
    }

    if (!(mesh.is_pure_triangle())) {
        usage();
        std::cerr << "ERROR: Input mesh must be pure triangle\n";
        return 1;
    }
    
    
    //////////////////////////////////////////////////////////
    // Delete the duplicated vertices
    //////////////////////////////////////////////////////////
    
    // The polyhedron types
    typedef Polyhedron::Vertex_iterator Vertex_iterator;
    typedef Polyhedron::Vertex_handle Vertex_handle;
    typedef Polyhedron::Halfedge_handle Halfedge_handle;
    
    // Variable initialization
    std::set<Point> mesh_vertices;
    std::set<Halfedge_handle> edge_to_erase;
    std::set<Vertex_handle> vertex_to_erase;
    bool duplicated_vertex = false;
    
    // Find the duplicated vertices 
    for (auto vit = mesh.vertices_begin(); vit != mesh.vertices_end(); ++vit) {
        Point p = (*vit).point();
        if (mesh_vertices.find(p) != mesh_vertices.end()) {
            vertex_to_erase.insert(&*vit);
            duplicated_vertex = true;
        }
        else {
            mesh_vertices.insert(p);
        }
    }
    
    if (duplicated_vertex) {
        if (!remove_vertices) {
            // The input mesh contains duplicated vertices and user not remove them
            std::cerr << "ERROR: Input mesh contains duplicated vertices!\n";
            std::cerr << "The duplicated vertices can be removed by the -d option\n";
            return 1;
        }
        else {
            // Erase the halfedges that connected to the duplicated vertices 
            for (auto it = vertex_to_erase.begin(); it != vertex_to_erase.end(); ++it) {
                Halfedge_handle erase_halfedge = (*(*it)).halfedge();
                Halfedge_handle return_edge = mesh.erase_center_vertex(erase_halfedge);
                
                edge_to_erase.insert(return_edge);
            }
            
            // Erase the facets that connected to the duplicated vertices 
            for (auto hit = edge_to_erase.begin(); hit != edge_to_erase.end(); ++hit) {
                mesh.erase_facet(*hit);
            }
            
            std::clog << "Remove all of the duplicated vertices.\n";
        }
    }
    
    
    //////////////////////////////////////////////////////////
    // Scale the mesh's vertices 
    //////////////////////////////////////////////////////////
    
    if (scaling) {
        // std::clog << "Scaling all of the mesh's vertices by: " << scaling_factor << "\n";
        
        for (auto iter = mesh.vertices_begin(); iter != mesh.vertices_end(); ++iter) {
            Point p = iter -> point();
            double p_x = p.x() * scaling_factor;
            double p_y = p.y() * scaling_factor;
            double p_z = p.z() * scaling_factor;
            
            iter -> point() = Point(p_x, p_y, p_z);
        }
    }
    
    
    //////////////////////////////////////////////////////////
    // Quantization step size and output file 
    //////////////////////////////////////////////////////////

    // Calculate the bounding box of the mesh
    Bbox_3 bbox = bounding_box(mesh);
    
    // std::clog << "bounding box: "
              // << "[" << bbox.xmin() << "," << bbox.xmax() << "] x "
              // << "[" << bbox.ymin() << "," << bbox.ymax() << "] x "
              // << "[" << bbox.zmin() << "," << bbox.zmax() << "]"
              // << "\n";
    
    // Quantization step size based on No. of bits user wants to encode
    if (no_bits != 0) {
        quan_x = (bbox.xmax() - bbox.xmin()) / (2 << no_bits);
        quan_y = (bbox.ymax() - bbox.ymin()) / (2 << no_bits);
        quan_z = (bbox.zmax() - bbox.zmin()) / (2 << no_bits); 
    }
    
    // Calculate the default quan_x
    if (quan_x == 0.0) {
        if (bbox.xmax() != bbox.xmin()) {
            quan_x = (bbox.xmax() - bbox.xmin()) / (2 << 16);
        }
        else {
            quan_x = bbox.xmax();
        }
    }
    
    // Calculate the default quan_y
    if (quan_y == 0.0) {
        if (bbox.ymax() != bbox.ymin()) {
            quan_y = (bbox.ymax() - bbox.ymin()) / (2 << 16);
        }
        else {
            quan_y = bbox.ymax();
        }
    }
    
    // Calculate the default quan_z
    if (quan_z == 0.0) {
        if (bbox.zmax() != bbox.zmin()) {
            quan_z = (bbox.zmax() - bbox.zmin()) / (2 << 16);
        }
        else {
            quan_z = bbox.zmax();
        }
    }

    // In case the coordinate after calculation is still zero
    if (quan_x == 0.0) {
        quan_x = 1.0;
    }
    if (quan_y == 0.0) {
        quan_y = 1.0;
    }
    if (quan_z == 0.0) {
        quan_z = 1.0;
    }

    // Quantization step size value
    Point quan_size(quan_x, quan_y, quan_z);
    
    // std::clog.precision(std::numeric_limits<double>::max_digits10);
    // std::clog << "Quantization step size: " << quan_size << "\n";
    
    
    //////////////////////////////////////////////////////////
    // Arithmetic encoder related setting
    //////////////////////////////////////////////////////////
    
    // No. of bits required by each coordinate
    int x_coor_bits = 0, y_coor_bits = 0, z_coor_bits = 0;
    
    // x coordinate
    if (std::abs(bbox.xmax()) > std::abs(bbox.xmin())) {
        int max_x_int = std::ceil(std::abs(bbox.xmax()) / quan_x + 0.5);
        x_coor_bits = std::ceil(log2(max_x_int));
    }
    else if (std::abs(bbox.xmax()) <= std::abs(bbox.xmin())) {
        int max_x_int = std::ceil(std::abs(bbox.xmin()) / quan_x + 0.5);
        x_coor_bits = std::ceil(log2(max_x_int));
    }
    
    // y coordinate
    if (std::abs(bbox.ymax()) > std::abs(bbox.ymin())) {
        int max_y_int = std::ceil(std::abs(bbox.ymax()) / quan_y + 0.5);
        y_coor_bits = std::ceil(log2(max_y_int));
    }
    else if (std::abs(bbox.ymax()) <= std::abs(bbox.ymin())) {
        int max_y_int = std::ceil(std::abs(bbox.ymin()) / quan_y + 0.5);
        y_coor_bits = std::ceil(log2(max_y_int));
    }
    
    // z coordinate
    if (std::abs(bbox.zmax()) > std::abs(bbox.zmin())) {
        int max_z_int = std::ceil(std::abs(bbox.zmax()) / quan_z + 0.5);
        z_coor_bits = std::ceil(log2(max_z_int));
    }
    else if (std::abs(bbox.zmax()) <= std::abs(bbox.zmin())) {
        int max_z_int = std::ceil(std::abs(bbox.zmin()) / quan_z + 0.5);
        z_coor_bits = std::ceil(log2(max_z_int));
    }
    
    x_coor_bits += 2;
    y_coor_bits += 2;
    z_coor_bits += 2;
    
     // std::clog << "x_coor_bits: " << x_coor_bits 
               // << ", y_coor_bits: " << y_coor_bits 
               // << ", z_coor_bits: " << z_coor_bits << "\n";
     
    
    //////////////////////////////////////////////////////////
    // Triangle mesh compression 
    //////////////////////////////////////////////////////////
        
    auto start_t = std::chrono::high_resolution_clock::now();
    Encoder enc(mesh, std::cout, quan_size, x_coor_bits, y_coor_bits, z_coor_bits);
    enc.mesh_compression();
    auto end_t = std::chrono::high_resolution_clock::now();
    
    
    //////////////////////////////////////////////////////////
    // Generate mesh information 
    //////////////////////////////////////////////////////////
    
    // ---------------------------------------------------------------
    // the following items are written (in order) to the result file:
    //    1) number of vertices in mesh
    //    2) number of edges in mesh
    //    3) number of faces in mesh
    //    4) number of boundary loops
    //    5) genus of mesh
    //    6) number of total bytes of coded data
    //    7) number of bytes of coded geometry data
    //    8) number of bytes of coded connectivity data
    //    9) time in seconds needed for encoding
    //    10) maximum amount of memory used by encoding program
    //    11) actual quantization step size
    // ---------------------------------------------------------------
    
    if (output_result == 1) {
        std::ofstream ofile_result(result_fname);
        std::vector<Vertex_index> result_vec = enc.compression_result();
        
        double run_time = std::chrono::duration<double>(end_t - start_t).count();
        double peak_mem = SPL::getPeakMemUsage();
        
        ofile_result.precision(std::numeric_limits<double>::max_digits10);
        for (auto vit = result_vec.begin(); vit != result_vec.end(); ++vit) {
            ofile_result << *vit << " ";
        }
        
        ofile_result << run_time << " " << peak_mem << " ";
        
        Point step_size = enc.get_quan_step_size();
        ofile_result << step_size << "\n";
        
        ofile_result.flush();
        if (!ofile_result) {
            std::cerr << "ERROR: The output result file error! \n";
            return 1;
        }
    }
    
    
    //////////////////////////////////////////////////////////
    // Calculate the coding efficiency
    //////////////////////////////////////////////////////////
    
    if (efficiency) {
        std::vector<Vertex_index> result_vec = enc.compression_result();
        
        // Overall coding efficiency
        double overall_eff = result_vec[5] * 8.0 / result_vec[0];
        std::clog << "Overall coding efficiency is: " << overall_eff << " bits per vertex \n";
        
        // Coding efficiency for geometry part
        double geom_eff = result_vec[6] * 8.0 / result_vec[0];
        std::clog << "Coding efficiency for geometry part is: " << geom_eff << " bits per vertex \n";
        
        // Coding efficiency for connectivity part
        double conn_eff = result_vec[7] * 8.0 / result_vec[0];
        std::clog << "Coding efficiency for connectivity part is: " << conn_eff << " bits per vertex \n";
    }
    

    return 0;
}
