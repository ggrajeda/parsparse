#ifndef SMALLCOUNT_MTX_FILE_H_
#define SMALLCOUNT_MTX_FILE_H_

#include "sparse_matrix.h"

namespace smallcount {

// Line of data/metadata in an .mtx file.
// Typically corresponds to a non-zero entry in a sparse matrix.
// Contains matrix metadata for the first line of the .mtx file.
struct MtxLine {
    // Entry row index (or total number of rows).
    int row;
    // Entry column index (or total number of columns).
    int col;
    // Entry value (or total number of non-zero values).
    int val;

    MatrixData data() { return {.row = row, .col = col, .val = val}; }

    MatrixMetadata metadata() {
        return {.nrow = row, .ncol = col, .nval = val};
    }
};

}  // namespace smallcount

#endif
