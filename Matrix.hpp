//
// Created by bfran on 17/04/2024.
//

#ifndef CHALLENGE2_SPARSEMATRIX_H
#define CHALLENGE2_SPARSEMATRIX_H

#include <iostream>
#include <cmath>
#include <map>
#include <vector>
#include <array>


namespace algebra {

    //Enumerator to define the storage order
    enum StorageOrder { row, column };

    //Template class Matrix
    template <class T, StorageOrder Store = StorageOrder::row>
    class Matrix {
    private:
        // Number of rows and columns
        std::size_t nrows;
        std::size_t ncols;

        // Elements of the matrix
        std::map<std::array<std::size_t, 2>, T> values;

        // Type of storage
        bool compressed = false;

        // Compressed Sparse Row format
        std::vector<std::size_t> inner_index;
        std::vector<std::size_t> outer_index;
        std::vector<T> compressed_values;


    public:
        // Constructors
        Matrix(std::size_t nrows, std::size_t ncols) : nrows(nrows), ncols(ncols) {};
        Matrix() = default;

        // To check is the marix is compressed or uncompressed
        bool is_compressed() {
            return compressed;
        } ;

        // search an element in compress matrix
        T find_compressed(std::size_t row, std::size_t col);

        // Operator to access elements (non-const version)
        T &operator()(std::size_t i, std::size_t j) ;
        // Operator to access elements (const version)
        const T &operator()(std::size_t i, std::size_t j) const ;

        // Method to compress the matrix
        void compress();
        void uncompress();

        //method to resize the matrix
        void resize(std::size_t new_rows, std::size_t new_cols);




    };// class Matrix


} // algebra

#endif //CHALLENGE2_SPARSEMATRIX_H
