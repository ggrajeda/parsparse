#include "sparse_matrix.h"

#include <Rcpp.h>

#include <memory>
#include <string>

using namespace Rcpp;

namespace smallcount {
namespace {

static constexpr char kCooSparseMatrix[] = "COO_SparseMatrix";
static constexpr char kNonZeroCoords[] = "nzcoo";
static constexpr char kNonZeroData[] = "nzdata";

static constexpr char kSvtSparseMatrix[] = "SVT_SparseMatrix";
static constexpr char kSvt[] = "SVT";
static constexpr char kType[] = "type";
static constexpr char kInteger[] = "integer";
static constexpr int kRowIndex = 0;  // Index of row information in SVT entry
static constexpr int kValIndex = 1;  // Index of value information in SVT entry

static constexpr char kDim[] = "dim";

}  // namespace

std::unique_ptr<SparseMatrix> SparseMatrix::create(const std::string &rep) {
    if (rep == kCooRep) return std::make_unique<CooSparseMatrix>();
    if (rep == kSvtRep) return std::make_unique<SvtSparseMatrix>();
    return nullptr;
}

// -----------------------------------------------------------------------------
// COO Sparse Matrix
// -----------------------------------------------------------------------------
void CooSparseMatrix::updateMetadata(const MatrixMetadata &m) {
    metadata = m;
    coords.reserve(2 * m.nval);
    vals.reserve(m.nval);
}

void CooSparseMatrix::addEntry(const MatrixData &entry) {
    coords.push_back(entry.row);
    coords.push_back(entry.col);
    vals.push_back(entry.val);
}

SEXP CooSparseMatrix::toRcpp() {
    S4 obj(kCooSparseMatrix);
    obj.slot(kNonZeroCoords) = createCoordsMatrix(std::move(coords));
    obj.slot(kNonZeroData) = wrap(std::move(vals));
    obj.slot(kDim) = IntegerVector({nrow(), ncol()});
    return obj;
}

IntegerMatrix CooSparseMatrix::createCoordsMatrix(std::vector<int> coords) {
    IntegerVector r_coords = wrap(std::move(coords));
    r_coords.attr(kDim) = Dimension(2, r_coords.size() / 2);
    return transpose(as<IntegerMatrix>(std::move(r_coords)));
}

// -----------------------------------------------------------------------------
// SVT Sparse Matrix
// -----------------------------------------------------------------------------
void SvtSparseMatrix::updateMetadata(const MatrixMetadata &m) {
    metadata = m;
    svt = Svt(m.ncol);
}

void SvtSparseMatrix::addEntry(const MatrixData &entry) {
    int col = entry.col - 1;
    if (svt[col].empty()) {
        svt[col] = SvtEntry(2);
    }
    svt[col][kRowIndex].push_back(entry.row - 1);
    svt[col][kValIndex].push_back(entry.val);
}

SEXP SvtSparseMatrix::toRcpp() {
    S4 obj(kSvtSparseMatrix);
    obj.slot(kSvt) = R_NilValue;
    if (nval() != 0) {
        obj.slot(kSvt) = createSvtList(std::move(svt));
    }
    obj.slot(kDim) = IntegerVector({nrow(), ncol()});
    obj.slot(kType) = kInteger;
    return obj;
}

List SvtSparseMatrix::createSvtList(Svt svt) {
    int ncol = svt.size();
    List svt_list = List(ncol);
    for (int i = 0; i < ncol; i++) {
        if (!svt[i].empty()) {
            svt_list[i] = wrap(std::move(svt[i]));
        }
    }
    return svt_list;
}

}  // namespace smallcount
