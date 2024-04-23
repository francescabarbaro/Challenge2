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
#include <fstream>


namespace algebra {

    //Enumerator to define the storage order
    enum StorageOrder { row, column };

    //Template class Matrix
    /**
     * @brief Template class Matrix to store matrix in compressed or uncompressed from
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     */
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

        // Method to read matrix from Matrix Market format
        void read_matrix_market(const std::string& filename);


        /**
         * @brief method To check is the matrix is compressed or uncompressed
         * @return true if compress form, false otherwise
         */
        bool is_compressed() { return compressed; } ;

        // search an element in compress matrix
        T find_compressed(std::size_t row, std::size_t col);

        // Operator to access elements (non-const version)
        T &operator()(std::size_t i, std::size_t j) ;
        // Operator to access elements (const version)
        const T &operator()(std::size_t i, std::size_t j) const ;

        // Method to compress the matrix
        void compress();
        void uncompress();

        // methods used to compress and decompress
        void storage_val_compressed(size_t, size_t);
        void storage_val_map(size_t);

        //method to resize the matrix
        void resize(std::size_t new_rows, std::size_t new_cols);

        //friend operator* to compute the matrix vector multiplication
        friend std::vector<T> operator*(const Matrix<T,Store>& mat,const std::vector<T>& vec);



        void print()  ;




    };// class Matrix


} // algebra

#endif //CHALLENGE2_SPARSEMATRIX_H
