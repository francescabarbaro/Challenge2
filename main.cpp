//
// Created by bfran on 17/04/2024.
//

#include <iostream>
#include <cmath>
#include "Matrix.hpp"
#include "Matrix_imp.hpp"

using namespace algebra;

int main(){

    Matrix<double, StorageOrder::column> matrix;

    //try to read the matrix -> ok
    std::string filename="lnsp_131.mtx";
    std::string filename_short="Matrix.mtx";
    matrix.read_matrix_market(filename_short);
    matrix.print();



    return 0;
};