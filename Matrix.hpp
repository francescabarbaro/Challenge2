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
        void read_matrix_market(const std::string& );


        /**
         * @brief method To check is the matrix is compressed or uncompressed
         * @return true if compress form, false otherwise
         */
        bool is_compressed() { return compressed; } ;

        // search an element in compress matrix
        T find_compressed(std::size_t , std::size_t );

        // Operator to access elements (non-const version)
        T &operator()(std::size_t i, std::size_t j) ;
        // Operator to access elements (const version)
        const T &operator()(std::size_t, std::size_t) const ;

        // Method to compress the matrix
        void compress();
        void uncompress();

        // methods used to compress and decompress
        void storage_val_compressed(size_t, size_t);
        void storage_val_map(size_t);

        //method to resize the matrix
        void resize(std::size_t, std::size_t);

        //friend operator* to compute the matrix vector multiplication
        friend std::vector<T> operator*(const Matrix<T,Store>& mat,const std::vector<T>& vec){

            // check if the dimension of the vector is compatible with the matrix dimensions
            if(vec.size()!=mat.ncols)
                std::cerr<<"Error: matrix-vector dimensions are not compatible"<<std::endl;

            // vector containing the result of the product
            std::vector<T> result(mat.nrows);

            // differentiation between the StorageOrder
            if constexpr(Store==StorageOrder::row){
                // differentiation between compressed or not matrix, to know where to look for the data
                if(mat.compressed){
                    // loop over the rows
                    for (std::size_t i = 0; i < mat.nrows; i++)
                    {
                        // value for the i-th element of the result vector
                        T element=0;
                        // loop over non-zero values of row i, for the row-vector vector multiplication
                        for (std::size_t j = mat.inner_index[i]; j < mat.inner_index[i+1]; j++)
                        {
                            // for every non-zero value, I sum the right increment
                            element+=mat.compressed_values[j]*vec[mat.outer_index[j]];
                        }
                        // assignment of value
                        result[i]=element;
                    }
                }
                else{
                    // loop over rows
                    for (std::size_t i = 0; i < mat.nrows; i++)
                    {
                        // value for the i-th element of the result vector
                        T element=0;
                        // defintion of lower and upper, iterators for firt element of the row and first iterator after the last element of the row
                        std::array<std::size_t,2> pos={i,0};
                        auto lower=mat.values.lower_bound(pos);
                        pos={i,mat.ncols-1};
                        auto upper=mat.values.upper_bound(pos);
                        // loop over non-zero values of i-th row, with iterators, for the row vector multiplication
                        for (auto j = lower; j != upper; j++)
                        {
                            // for every non-zero-value, I sum the increment
                            element+=j->second*vec[j->first[1]];
                        }
                        // assignment of the value
                        result[i]=element;
                    }
                }
            }
            else{
                if(!mat.compressed){
                    // loop over columns
                    for(std::size_t i=0;i<mat.ncols;i++){
                        // defintion of lower and upper, iterators for first element of the column and first iterator after the last element of the column
                        std::array<std::size_t,2> pos={i,0};
                        auto lower=mat.values.lower_bound(pos);
                        pos={i,mat.nrows-1};
                        auto upper=mat.values.upper_bound(pos);
                        // loop over non-zero elements of the i-th column
                        // I sum the i-th column vector times the i-th element of the vector element-wise, but still following the given formula
                        for(auto iter=lower;iter!=upper;iter++){
                            // summing each increment of the i-th column vector
                            result[iter->first[0]]+=vec[iter->first[1]]*iter->second;
                        }
                    }
                }
                else{
                    // loop over columns
                    for(std::size_t i=0;i<mat.ncols;i++){
                        // loop over non-zero elements of column i
                        // I sum the i-th column vector times the i-th element of the vector element-wise, but still following the given formula
                        for (std::size_t j = mat.inner_index[i]; j < mat.inner_index[i+1]; j++)
                        {
                            // summing each increment of the i-th column vector
                            result[mat.outer_index[j]]+=mat.compressed_values[j]*vec[i];
                        }
                    }
                }
            }
            // return the result vector
            return result;
        };// friend operator* matrix-vector



        void print()  ;




    };// class Matrix


} // algebra

#endif //CHALLENGE2_SPARSEMATRIX_H
