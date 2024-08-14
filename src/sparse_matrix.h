#ifndef SMALLCOUNT_SPARSE_MATRIX_H_
#define SMALLCOUNT_SPARSE_MATRIX_H_

#include <Rcpp.h>

#include <vector>

using namespace Rcpp;

namespace smallcount {

// Pair of parallel arrays containing the row and value, respectively, of each
// non-zero entry in a given column.
using SvtEntry = std::vector<std::vector<int>>;
// Vector of SVT entries for each column.
using Svt = std::vector<SvtEntry>;

// Information about a sparse matrix.
struct MatrixMetadata {
    int nrow;  // Number of rows
    int ncol;  // Number of columns
    int nval;  // Number of non-zero values
};

// Non-zero entry in a sparse matrix.
struct MatrixData {
    int row;  // Row index
    int col;  // Column index
    int val;  // Value
};

// Abstract sparse matrix struct.
struct SparseMatrix {
    // Metadata for the number of rows, columns, and non-zero values.
    // Default initialization is an empty 1 x 1 matrix.
    MatrixMetadata metadata = {.nrow = 1, .ncol = 1, .nval = 0};

    int nrow() const { return metadata.nrow; }
    int ncol() const { return metadata.ncol; }
    int nval() const { return metadata.nval; }

    // Updates the matrix with the given metadata.
    virtual void updateMetadata(const MatrixMetadata &metadata) = 0;
    // Adds a non-zero data entry to the matrix.
    virtual void addEntry(const MatrixData &entry) = 0;
    // Consumes the matrix and converts it to an S4 object.
    virtual SEXP toRcpp() = 0;
};

// COO representation of a sparse matrix.
struct CooSparseMatrix : public SparseMatrix {
    // Coordinates of non-zero entries, contained as: {row1, col1, row2, ...}.
    std::vector<int> coords{};
    // Non-zero values.
    std::vector<int> vals{};

    void updateMetadata(const MatrixMetadata &metadata) override;
    void addEntry(const MatrixData &entry) override;
    SEXP toRcpp() override;

   private:
    // Converts the coords C++ vector to an Rcpp IntegerMatrix.
    static IntegerMatrix createCoordsMatrix(std::vector<int> coords);
};

// SVT representation of a sparse matrix.
struct SvtSparseMatrix : public SparseMatrix {
    // Sparse vector tree.
    Svt svt{};

    void updateMetadata(const MatrixMetadata &metadata) override;
    void addEntry(const MatrixData &entry) override;
    SEXP toRcpp() override;

   private:
    // Converts the SVT C++ matrix to an Rcpp List.
    static List createSvtList(Svt svt);
};

}  // namespace smallcount

#endif
