#include "mtx_file_reader.h"

#include <Rcpp.h>
#include <stdlib.h>

#include <fstream>
#include <optional>
#include <string>

#include "mtx_file.h"
#include "sparse_matrix.h"

using namespace Rcpp;

namespace smallcount {
namespace {

// Checks if a matrix entry is valid for a given matrix.
void checkValid(const SparseMatrix &matrix, const MtxLine &entry) {
    int row = entry.row;
    int col = entry.col;
    if (row <= 0 || row > matrix.nrow() || col <= 0 || col > matrix.ncol()) {
        stop("Coordinate (%d, %d) is out of bounds for a %d x %d matrix", row,
             col, matrix.nrow(), matrix.ncol());
    }
}

}  // namespace

std::optional<MtxLine> MtxFileReader::parseMtxLine(const std::string &line) {
    // Ignore comments.
    if (line[0] == '%') {
        return std::nullopt;
    }
    // Read row, column, and value information.
    MtxLine result;
    const char *c_str = line.c_str();
    char *str_end;
    result.row = strtol(c_str, &str_end, /*__base=*/10);
    result.col = strtol(str_end, &str_end, /*__base=*/10);
    result.val = strtol(str_end, &str_end, /*__base=*/10);
    if (result.row == 0 || result.col == 0 || result.val == 0) {
        warning(
            "Error processing line %d. Encountered zeroes or misconfigured "
            "entry: %d %d %d",
            line_num, result.row, result.col, result.val);
        return std::nullopt;
    }
    return result;
}

void MtxFileReader::parseFile(std::ifstream &file, SparseMatrix &matrix) {
    std::string line;
    while (std::getline(file, line)) {
        line_num++;
        auto entry = parseMtxLine(line);
        if (!entry.has_value()) {
            continue;
        }
        if (init) {
            checkValid(matrix, *entry);
            matrix.addEntry(entry->data());
            non_zero_count++;
        } else {
            matrix.updateMetadata(entry->metadata());
            init = true;
        }
    }
    if (non_zero_count != matrix.nval()) {
        warning(
            "Count of non-zero data does not match the count specified in "
            "the matrix metadata (%d != %d).",
            non_zero_count, matrix.nval());
    }
}

}  // namespace smallcount
