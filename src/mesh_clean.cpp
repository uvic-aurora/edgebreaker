// mesh_clean.hpp
// Description:
//     This program use to clean the duplicated vertices in the given triangle mesh.


#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>


// Some basic types to be used with CGAL Polyhedron_3 class
typedef double Real;
typedef CGAL::Cartesian<Real> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Kernel::Point_3 Point;

// The polyhedron types
typedef Polyhedron::Vertex_iterator Vertex_iterator;
typedef Polyhedron::Vertex_handle Vertex_handle;
typedef Polyhedron::Vertex_const_iterator Vertex_const_iterator;
typedef Polyhedron::Halfedge_handle Halfedge_handle;


/*******************************************************************************
Function: usage()
@Description:
    Print the usage info on the screen
*******************************************************************************/
void usage() {
    std::cout << "============================================================================\n"
              << "Usage:\n"
              << "mesh_clean triangle_mesh [OPTIONS]\n"
              << "triangle_mesh:   The input triangle mesh for cleaning\n"
              << "-s $scaling:     Scaling factor used to scaling each vertex's coordinates.\n"
              << "                 If not specified, default to 1 (no scaling).\n"
              << "\n"
              << "Input triangle_mesh mush stored in OFF format and in triangle type.\n"
              << "============================================================================\n\n";
}


/*******************************************************************************
Function: main()
@Description:
    Main function for the mesh comparison program
*******************************************************************************/
int main(int argc, char** argv) {
    
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
    // Read the command line argument
    //////////////////////////////////////////////////////////
    
    // Initialize the scaling factor
    bool scaling = false;
    double scaling_factor = 1.0;   
    
    // User specified quantization step size
    char opt;
    while ((opt = getopt(argc, argv, "s:")) != EOF) {
        switch (opt) {
            case 's':
                scaling_factor = atof(optarg);
                scaling = true;
                break;
        }
    }
    
    // Check the input parameters
    if (scaling_factor <= 0) {
        std::cerr << "ERROR: Input parameters cannot be zero or negative!\n";
        return 2;
    }
    
    
    //////////////////////////////////////////////////////////
    // Delete the duplicated vertices
    //////////////////////////////////////////////////////////
    
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
        // Erase the halfedges that connected to the duplicated vertices 
        for (auto it = vertex_to_erase.begin(); it != vertex_to_erase.end(); ++it) {
            Halfedge_handle erase_halfedge = (*(*it)).halfedge();
            Halfedge_handle return_edge = mesh.erase_center_vertex(erase_halfedge);
            
            edge_to_erase.insert(return_edge);
        }
        
        for (auto hit = edge_to_erase.begin(); hit != edge_to_erase.end(); ++hit) {
            mesh.erase_facet(*hit);
        }
        
        // std::clog << "Remove all of the duplicated vertices.\n";
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
    // Output the cleaned mesh
    //////////////////////////////////////////////////////////
    
    std::cout.precision(std::numeric_limits<double>::digits10);
    std::cout << mesh;
    
    std::cout.flush();
    if (!std::cout) {
        std::cerr << "ERROR: Write cleaned triangle mesh failed!\n";
        return 1;
    }
    
    
    return 0;
}