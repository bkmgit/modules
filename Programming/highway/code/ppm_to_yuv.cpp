/*
 *  Highway implementation of converting an image from PPM format to
 *  YUV format. The code demonstrates the benefits of fine grained
 *  SIMD.
 *
 *  References:
 *  https://en.wikipedia.org/wiki/Netpbm#PPM_example
 *  https://en.wikipedia.org/wiki/BMP_file_format
 *  https://en.wikipedia.org/wiki/YCbCr#ITU-R_BT.709_conversion
 *
 * @author Benson Muite
 */

// g++ ppm_to_yuv.cpp -o ppm_to_yuv -I ../../../../highway/ -L ../../../../highway-install/lib64/ -lhwy_contrib  -lhwy

#include<cmath>     // std::round
#include<fstream>   //
#include<iostream>  // std::cout
#include<sstream>   // std::istringstream
#include<stdlib.h>
#include<string>
#include<sys/time.h>

#include "hwy/contrib/thread_pool/thread_pool.h"

int main(int argc, char *argv[]) {

  if (argc <= 1) {
    std::cout << "Usage:" << std::endl;
    std::cout << "./ppm_to_yuv ppm_file" << std::endl;
    std::cout << "Where ppm_file is the name of the file in ppm" << std::endl;
    std::cout << "format to be converted to YUV format." << std::endl;
  } else {
    std::string infile_name(argv[1]);
    std::ifstream infile(infile_name);  // open file
    if(!infile) {
       std::cerr << "Error opening " << infile_name << std::endl;
       return 1;
    }
    // Read in data
    // Still need to make this more robust to variations in the format,
    // in particular comments at end of lines, specification and
    // dimensions on one line
    std::string line;
    std::getline(infile, line);
    if(line.substr(0,2) != "P3") {
      std::cerr << "File has wrong encoding." << std::endl;
      return 1;
    }
    while(std::getline(infile, line) && (line.substr(0,1) == '#')) {
    }
    size_t im_width;
    size_t im_height;
    std::istringstream size_in(line);
    size_in >> im_width >> im_height;
    std::getline(infile, line);
    size_t max_color_val;
    std::istringstream mcv(line);
    mcv >> max_color_val;
    float *im_in = NULL;
    im_in = new float[3 * im_width * im_height];
    // May want to use a template for this as can use uint8_t if shifted
    int16_t *im_out = NULL;
    im_out = new int16_t[3 * im_width * im_height];
    size_t j = 0;
    while(std::getline(infile, line)) {
      std::istringstream vals(line);
      while(!vals.eof()) {
        vals >> im_in[j++];
      }
    }
    infile.close();

    // Convert data
    // Constant luminance transformation
    // Non constant luminance would require use of
    // specialized operations to be more efficient
    /// YUV444
    for(size_t j = 0; j < im_height * im_width; j++) {
      float y = (0.2627 * im_in[(j * 3)     ]) +
                (0.6780 * im_in[(j * 3) + 1 ]) +
                (0.0593 * im_in[(j * 3) + 2 ]);
      float u = (-1.0 * (0.2627 / 1.8814)  * im_in[(j * 3)     ]) +
                (-1.0 * (0.6780 / 1.8814)  * im_in[(j * 3) + 1 ]) +
                (((1.0 - 0.0593) / 1.8814) * im_in[(j * 3) + 2 ]);
      float v = (((1.0 - 0.2627) / 1.4746) * im_in[(j * 3)     ]) +
                (-1.0 * (0.6780 / 1.4746)  * im_in[(j * 3) + 1 ]) +
                (-1.0 * (0.0593 / 1.4746)  * im_in[(j * 3) + 2 ]);
      im_out[(j * 3)     ] = static_cast<int16_t>(std::roundf(y));
      im_out[(j * 3) + 1 ] = static_cast<int16_t>(std::roundf(u));
      im_out[(j * 3) + 2 ] = static_cast<int16_t>(std::roundf(v));
    }


    // Write out converted file
    std::string outfile_name(infile_name.substr(0,infile_name.find(".ppm")).append(".yuv"));
    std::ofstream outfile(outfile_name);
    if(!outfile) {
       std::cerr << "Error opening " << outfile_name << std::endl;
       return 1;
    }
    outfile << im_width << " " << im_height << std::endl;
    for(size_t j = 0; j < im_height * im_width; j++) {
       outfile << im_out[ j * 3 ] << " "
               << im_out[ j * 3 + 1 ] << " "
               << im_out[ j * 3 + 2 ] << std::endl;
    }
    outfile.close();
    delete [] im_in;
    delete [] im_out;
    return 0;
  }
}
