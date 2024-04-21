//
// Created by bfran on 17/04/2024.
//

#include <iostream>
#include <cmath>
#include "Matrix.hpp"
#include "Matrix_imp.hpp"

using namespace algebra;

int main(){

    Matrix<double, StorageOrder::row> matrix;
    //std::string filename="lnsp_131.mtx";
    std::string filename="Matrix.mtx";
    matrix.read_matrix_market(filename);
    matrix.print();



    return 0;
};