#ifndef SMALLCOUNT_FILE_READER_H_
#define SMALLCOUNT_FILE_READER_H_

#include <Rcpp.h>

#include <fstream>
#include <string>

#include "sparse_matrix.h"

using namespace Rcpp;

namespace smallcount {

// Abstract class to parse a sparse matrix from a file.
class SparseMatrixFileReader {
   public:
    // Reads a sparse matrix of type M from a file.
    template <typename M>
    SEXP readMatrix(const std::string &filepath) {
        M matrix;
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return matrix.toRcpp();
        }
        parseFile(file, matrix);
        file.close();
        return matrix.toRcpp();
    }

   protected:
    // Reads the contents of a file into a SparseMatrix object.
    virtual void parseFile(std::ifstream &file, SparseMatrix &matrix) = 0;
};

}  // namespace smallcount

#endif
