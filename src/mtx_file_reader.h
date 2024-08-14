#ifndef SMALLCOUNT_MTX_FILE_READER_H_
#define SMALLCOUNT_MTX_FILE_READER_H_

#include <Rcpp.h>

#include <string>

#include "file_reader.h"
#include "mtx_file.h"
#include "sparse_matrix.h"

using namespace Rcpp;

namespace smallcount {

// File reader to construct sparse matrices from .mtx files.
class MtxFileReader : public SparseMatrixFileReader {
   public:
    static SEXP readCooSparseMatrix(const std::string &filepath) {
        MtxFileReader reader;
        return reader.readMatrix<CooSparseMatrix>(filepath);
    }

    static SEXP readSvtSparseMatrix(const std::string &filepath) {
        MtxFileReader reader;
        return reader.readMatrix<SvtSparseMatrix>(filepath);
    }

   private:
    bool init = false;  // Whether the matrix has been initialized with metadata
    int line_num = 0;   // Line number in the file
    int non_zero_count = 0;  // Count of non-zero entries in the matrix

    // Static class. Should not be instantiated.
    MtxFileReader() = default;

    // Reads a line of an .mtx file.
    std::optional<MtxLine> parseMtxLine(const std::string &line);

    // Reads the contents of an .mtx file into a SparseMatrix object.
    void parseFile(std::ifstream &file, SparseMatrix &matrix) override;
};

}  // namespace smallcount

#endif
