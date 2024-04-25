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
        T find_compressed(std::size_t , std::size_t ) const;

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
            if(vec.size()!=mat.ncols){
                std::cerr<<"Error: matrix-vector dimensions are not compatible"<<std::endl;
                throw std::runtime_error("Incorrect dimensions");
            }

            // vector containing the result of the product
            std::vector<T> result(mat.nrows);

            if(mat.compressed){ //matrix compressed
                if constexpr (Store==StorageOrder::row){ //ordered by row
                    // loop over the rows
                    for (std::size_t i = 0; i < mat.nrows; i++)
                    {
                        T sum=0;

                        // loop over non-zero values of the rows
                        for (std::size_t j = mat.inner_index[i]; j < mat.inner_index[i+1]; j++)
                        {
                            // operation
                            sum+=mat.compressed_values[j]*vec[mat.outer_index[j]];
                        }
                        // put the sum in the right place of the result
                        result[i]=sum;
                    }

                }
                else if constexpr (Store == StorageOrder::column){ // compressed and ordered by column
                    // loop over the rows
                    for(std::size_t i=0;i<mat.ncols;i++){

                        // loop over non-zero values of the columns
                        for (std::size_t j = mat.inner_index[i]; j < mat.inner_index[i+1]; j++)
                        {
                            // operation, updating the result at each cycle
                            result[mat.outer_index[j]] += mat.compressed_values[j]*vec[i];
                        }
                    }
                }

            } else if (!mat.compressed){ //matrix not compressed
                if constexpr (Store == StorageOrder::row){ //ordered by row
                    // loop over rows
                    for (std::size_t i = 0; i < mat.nrows; i++)
                    {
                        T sum=0;

                        // get the iterators at the beginning and end of the row
                        std::array<std::size_t,2> low{i,0}, upp{i,mat.ncols-1};
                        auto lower=mat.values.lower_bound(low);
                        auto upper=mat.values.upper_bound(upp);

                        // loop over non-zero values of rows
                        for (auto j = lower; j != upper; j++)
                        {
                            // operation
                            sum+=j->second*vec[j->first[1]];
                        }
                        // put the sum in the right place of the result
                        result[i]=sum;
                    }
                }
                else if constexpr (Store == StorageOrder::column){ //uncompressed and ordered by column
                    // loop over columns
                    for(std::size_t i=0;i<mat.ncols;i++){

                        // get the iterators at the beginning and end of the column
                        std::array<std::size_t,2> low{i,0}, upp{i,mat.nrows-1};
                        auto lower=mat.values.lower_bound(low);
                        auto upper=mat.values.upper_bound(upp);

                        // loop over non-zero values of columns
                        for(auto iter=lower;iter!=upper;iter++){
                            // operation, updating the result at each cycle
                            result[iter->first[1]]+=vec[iter->first[0]]*iter->second;
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
