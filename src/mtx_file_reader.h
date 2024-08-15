#ifndef SMALLCOUNT_MTX_FILE_READER_H_
#define SMALLCOUNT_MTX_FILE_READER_H_

#include <Rcpp.h>

#include <string>

#include "mtx_file.h"
#include "sparse_matrix.h"

using namespace Rcpp;

namespace smallcount {

// File reader to construct sparse matrices from .mtx files.
class MtxFileReader {
   public:
    // Reads the contents of an .mtx file into a SparseMatrix object.
    static void read(std::ifstream &file, SparseMatrix &matrix) {
        MtxFileReader reader;
        reader.parseFile(file, matrix);
    }

   private:
    bool init = false;  // Whether the matrix has been initialized with metadata
    int line_num = 0;   // Line number in the file
    int non_zero_count = 0;  // Count of non-zero entries in the matrix

    // Static class. Should not be instantiated.
    MtxFileReader() = default;

    // Reads a line of an .mtx file.
    std::optional<MtxLine> parseMtxLine(const std::string &line);

    void parseFile(std::ifstream &file, SparseMatrix &matrix);
};

}  // namespace smallcount

#endif
