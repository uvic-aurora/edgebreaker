#include "decoder.hpp"
#include <fstream>
#include <chrono>
#include <cmath>
#include <SPL/Timer.hpp>


/*******************************************************************************
Function: usage()
@Description:
    Print the usage info to the standard output stream
*******************************************************************************/
void usage() {
    std::cout << "=============================================================================\n"
              << "Usage:\n"
              << "decode_mesh [OPTIONS] \n"
              << "-r $result_file:  Writes the mesh compression information to the given file.\n"
              << "-h:   Prints the help info about the different command line options.  \n"
              << "\n"
              << "The input triangle mesh is stored in EB format.\n"
              << "The output decompressed triangle mesh is stored in OFF format.\n"
              << "=============================================================================\n";
}


/*******************************************************************************
Function: main()
@Description:
    Main function for the mesh decompression program
*******************************************************************************/
int main(int argc, char** argv) {
    
    //////////////////////////////////////////////////////////
    // Read the command line argument
    //////////////////////////////////////////////////////////
    
    // Variable initialization
    int output_result = 0;    // Output result to given file
    std::string result_fname;    // Output result file name
    
    // User specified quantization step size
    char opt;
    while ((opt = getopt(argc, argv, "hr:")) != EOF) {
        switch (opt) {
            case 'h':
                usage();
                return 0;
                break;  
                
            case 'r':
                output_result = 1;
                result_fname = optarg;
                break;
        }
    }
    
    //////////////////////////////////////////////////////////
    // Triangle mesh decompression 
    //////////////////////////////////////////////////////////

    auto start_t = std::chrono::high_resolution_clock::now();
    Decoder dec(std::cin);
    dec.mesh_decompression();
    auto end_t = std::chrono::high_resolution_clock::now();
    
    
    //////////////////////////////////////////////////////////
    // Generate mesh information 
    //////////////////////////////////////////////////////////
    
    if (output_result == 1) {
        std::ofstream ofile_result(result_fname);
        std::vector<Vertex_index> result_vec = dec.compression_result();
        
        double run_time = std::chrono::duration<double>(end_t - start_t).count();
        double peak_mem = SPL::getPeakMemUsage();
        
        ofile_result.precision(std::numeric_limits<double>::max_digits10);
        for (auto vit = result_vec.begin(); vit != result_vec.end(); ++vit) {
            ofile_result << *vit << " ";
        }
        
        ofile_result << run_time << " " << peak_mem << " ";
        
        Point step_size = dec.get_quan_step_size();
        ofile_result << step_size << "\n";
        
        ofile_result.flush();
        if (!ofile_result) {
            std::cerr << "ERROR: The output result file error! \n";
            return 1;
        }
    }

    
    return 0;
}