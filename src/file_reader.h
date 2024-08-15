#ifndef SMALLCOUNT_FILE_READER_H_
#define SMALLCOUNT_FILE_READER_H_

#include <Rcpp.h>

#include <fstream>
#include <memory>
#include <string>

#include "mtx_file_reader.h"
#include "sparse_matrix.h"

using namespace Rcpp;

namespace smallcount {

// Abstract class to parse a sparse matrix from a file.
class SparseMatrixFileReader {
   public:
    // Reads a sparse matrix from a file using the internal representation
    // specified (either "coo" or "svt").
    static SEXP read(const std::string &filepath, const std::string &rep) {
        std::unique_ptr<SparseMatrix> matrix = SparseMatrix::create(rep);
        if (matrix == nullptr) {
            stop(
                "Invalid matrix representation specified: \"%s\". Only \"coo\" "
                "and \"svt\" supported.",
                rep);
        }
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return matrix->toRcpp();
        }
        MtxFileReader::read(file, *matrix);
        file.close();
        return matrix->toRcpp();
    }
};

}  // namespace smallcount

#endif
