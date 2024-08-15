#include "hdf5_file_reader.h"

#include <cstdint>
#include <string>
#include <vector>

#include "H5Cpp.h"
#include "Rcpp.h"
#include "sparse_matrix.h"

using namespace Rcpp;

namespace smallcount {
namespace {

static constexpr char kNonZeroRows[] = "matrix/indices";
static constexpr char kNonZeroData[] = "matrix/data";
static constexpr char kColumnIndices[] = "matrix/indptr";

static constexpr char kShape[] = "matrix/shape";

// Helper function to map C++ types to HDF5 PredTypes.
template <typename T>
H5::PredType h5Type();
template <>
H5::PredType h5Type<uint32_t>() {
    return H5::PredType::NATIVE_UINT32;
}
template <>
H5::PredType h5Type<uint64_t>() {
    return H5::PredType::NATIVE_UINT64;
}

template <typename T>
std::vector<T> readDataset(H5::H5File &file, const std::string &dataset_name) {
    H5::DataSet dataset = file.openDataSet(dataset_name);
    H5::DataSpace dataspace = dataset.getSpace();

    int num_entries = dataspace.getSimpleExtentNpoints();
    std::vector<T> data(num_entries);
    dataset.read(data.data(), h5Type<T>());

    dataset.close();
    return data;
}

MatrixMetadata createMetadata(H5::H5File &file, int num_entries) {
    const auto shape = readDataset<uint64_t>(file, kShape);
    if (shape.size() != 2) {
        stop("Dataset \"%s\" has %zu entries instead of 2.", kShape,
             shape.size());
    }
    return MatrixMetadata{.nrow = static_cast<int>(shape[0]),
                          .ncol = static_cast<int>(shape[1]),
                          .nval = static_cast<int>(num_entries)};
}

}  // namespace

void Hdf5FileReader::read(H5::H5File &file, SparseMatrix &matrix) {
    const auto nz_rows = readDataset<uint32_t>(file, kNonZeroRows);
    const auto nz_data = readDataset<uint32_t>(file, kNonZeroData);
    const auto col_inds = readDataset<uint32_t>(file, kColumnIndices);
    if (nz_rows.size() != nz_data.size()) {
        stop(
            "Datasets \"%s\" and \"%s\" specify a different number of "
            "non-zero entries (%zu vs. %zu).",
            kNonZeroRows, kNonZeroData, nz_rows.size(), nz_data.size());
    }

    auto metadata = createMetadata(file, nz_data.size());
    matrix.updateMetadata(metadata);

    int col = 0;
    int num_cols = col_inds.size() - 1;
    for (int i = 0; i < nz_rows.size(); i++) {
        while (col < num_cols && (col_inds[col + 1] <= i)) {
            col++;
        }
        if (col == num_cols) break;
        matrix.addEntry(MatrixData{.row = static_cast<int>(nz_rows[i] + 1),
                                   .col = col + 1,
                                   .val = static_cast<int>(nz_data[i])});
    }
}

}  // namespace smallcount
