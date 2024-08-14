#include <Rcpp.h>

#include <filesystem>
#include <string>

#include "mtx_file_reader.h"

using namespace Rcpp;

// Only used for debugging file reads.
// TODO: Delete this function.
// [[Rcpp::export]]
CharacterVector getWorkingDirectory() {
    return CharacterVector(std::filesystem::current_path());
}

// Reads a COO_SparseMatrix object from an .mtx file.
// [[Rcpp::export]]
SEXP readCooFromMtx(std::string filepath) {
    return smallcount::MtxFileReader::readCooSparseMatrix(filepath);
}

// Reads an SVT_SparseMatrix object from an .mtx file.
// [[Rcpp::export]]
SEXP readSvtFromMtx(std::string filepath) {
    return smallcount::MtxFileReader::readSvtSparseMatrix(filepath);
}
