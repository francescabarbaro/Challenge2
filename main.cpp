//
// Created by bfran on 17/04/2024.
//

#include <iostream>
#include "Matrix.hpp"
#include "Matrix_imp.hpp"
#include "chrono.hpp"

using namespace algebra;

int main(){

    Matrix<double, StorageOrder::row> matrix;

    //try to read the matrix -> ok
    std::string filename="lnsp_131.mtx";
    std::string filename_short="Matrix.mtx";
    matrix.read_matrix_market(filename_short);
    matrix.print();

    //try to resize the matrix -> ok
    std::cout<<"resize matrix"<<std::endl;
    matrix.resize(5,5);
    matrix.print();

    //now try to compress and decompress the matrix -> ok
    std::cout<<"compressed and uncompressed matrix"<<std::endl;
    matrix.compress();
    matrix.print();


    //check matrix vector compressed -> row ok -> col ok

    matrix.compress();
    std::cout<<"is compress: "<<matrix.is_compressed()<<std::endl;
    std::vector<double> vec={-1.0,-1.0,-1.0,-1.0,-1.0};

    std::vector<double> result1=matrix*vec;
    std::cout<<"product compressed mat*vec"<<std::endl;
    for (auto it = result1.cbegin(); it != result1.cend(); ++it) {
        std::cout << *it << std::endl;
    }

    // check matrix*vector uncompressed -> row ok -> col ok
    matrix.uncompress();
    std::cout<<"is compress: "<<matrix.is_compressed()<<std::endl;
    std::vector<double> result=matrix*vec;
    std::cout<<"product uncompressed mat*vec"<<std::endl;
    for (auto it = result.cbegin(); it != result.cend(); ++it) {
        std::cout << *it << std::endl;
    }


    //Timing the multiplication
    Matrix<double, StorageOrder::row> matrix_unc(131,131);
    matrix_unc.read_matrix_market(filename);
    matrix_unc.uncompress();

    Matrix<double, StorageOrder::row> matrix_comp(131,131);
    matrix_comp.read_matrix_market(filename);
    matrix_comp.compress();



    Timings::Chrono timer;

    double total_time_uncompressed = 0.0;
    double total_time_compressed = 0.0;

    double num_runs = 100;

    std::vector<double> vec_long(131, 1.0) ;

    for (std::size_t i = 0; i < num_runs; ++i) {


        timer.start();
        auto res_unc = matrix_unc * vec_long;
        timer.stop();
        total_time_uncompressed += timer.wallTime();

        timer.start();
        auto res_compressed = matrix_comp * vec_long;
        timer.stop();
        total_time_compressed += timer.wallTime();
    }

    // Calculate average times
    double avg_time_uncompressed = total_time_uncompressed / num_runs;
    double avg_time_compressed = total_time_compressed / num_runs;

    // Report average times
    std::cout << "Average time for multiplication with matrix in uncompressed form : " << avg_time_uncompressed
              << std::endl;
    std::cout << "Average time for multiplication with matrix in compressed form : "
              << avg_time_compressed << std::endl;

    //TODO: togliere i cout nelle funzioni e metterli come debug


    return 0;
}