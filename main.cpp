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

    //try to resize the matrix -> ok
    std::cout<<"resize matrix"<<std::endl;
    matrix.resize(5,5);
    matrix.print();

    /*//now try to compress and decompress the matrix -> ok
    std::cout<<"compressed and uncompressed matrix"<<std::endl;
    matrix.compress();
    matrix.print();*/

    // check matrix*vector
    std::vector<double> vec={1.0,1.0,1.0,1.0,1.0};
    std::vector<double> result=matrix*vec;
    std::cout<<"product uncompressed mat*vec"<<std::endl;
    for (auto it = result.cbegin(); it != result.cend(); ++it) {
        std::cout << *it << std::endl;
    }

    matrix.compress();
    std::vector<double> result1=matrix*vec;
    std::cout<<"product compressed mat*vec"<<std::endl;
    for (auto it = result.cbegin(); it != result.cend(); ++it) {
        std::cout << *it << std::endl;
    }




    return 0;
};