#include <Rcpp.h>

#include <filesystem>
#include <string>

#include "file_reader.h"

using namespace Rcpp;

// Only used for debugging file reads.
// TODO: Delete this function.
// [[Rcpp::export]]
CharacterVector getWorkingDirectory() {
    return CharacterVector(std::filesystem::current_path());
}

// Reads a SparseMatrix R object from a file using the internal representation
// specified (either "coo" or "svt").
// [[Rcpp::export]]
SEXP readSparseMatrix(std::string filepath, std::string rep) {
    return smallcount::SparseMatrixFileReader::read(filepath, rep);
}
