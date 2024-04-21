//
// Created by bfran on 18/04/2024.
//

#ifndef CHALLENGE2_MATRIX_IMP_H
#define CHALLENGE2_MATRIX_IMP_H

#include "Matrix.hpp"

// Implementation of the template class Matrix
// This is done in a header file since it's a template
namespace algebra {


    /**
     * @brief method to read the matrix from the Matrix Market
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     * @param filename string containing the name of the file to be read
     */
    template<typename T, StorageOrder Store>
    void Matrix<T,Store>::read_matrix_market(const std::string& filename) {
        //Clear previous matrix (if needed)
        values.clear();
        inner_index.clear();
        outer_index.clear();
        compressed_values.clear();

        //open the file
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::string line;

        //avoid first row
        while (file.peek() == '%') {
            file.ignore(2048, '\n');
        }

        std::size_t num_rows, num_cols, non_zero_elem;

        file >> num_rows >> num_cols >> non_zero_elem;


        for (std::size_t i = 0; i < non_zero_elem; ++i) {
            std::size_t row, col;
            T value;
            file >> row >> col >> value;

            // the map is ordered by row index
            values[{row - 1, col - 1}] = value;
        }
        return;
    }

    // definition operator () non const version
    /**
     * @brief non const version of the operator(), if the matrix is uncompressed it can add a value
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     * @param i index of the row
     * @param j index of the column
     * @return the value of the matrix at the indexes in input
     */
    template<typename T, StorageOrder Store>
    T  & Matrix<T, Store>:: operator()(std::size_t i, std::size_t j) {
        // Check if the indexes are present in the matrix
        if (i >= nrows || j >= ncols)
            std::cerr << "Indexes out of bounds" << std::endl;

        if (!compressed) {
            auto key = std::array<std::size_t, 2>{i, j};
            return values[key]; //insert the element if it doesn't exists
        } else {
            return find_compressed(i,j);
        }

    } //non const verison operator ()

    //definition operato ()  const version
    /**
     * @brief const version of the operator()
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     * @param i index of the row
     * @param j index of the column
     * @return the value of the matrix at the indexes in input, if already exists, 0 otherwise
     */
    template<typename T, StorageOrder Store>
    const T  & Matrix<T, Store>:: operator()(std::size_t i, std::size_t j) const {
        // Check if the indexes are present in the matrix
        if (i >= nrows || j >= ncols){
            std::cerr << "Indexes out of bounds" << std::endl;
            }

        if (!compressed) {
            auto key = std::array<std::size_t, 2>{i, j};
            auto it = values.find(key);
            if (it != values.end()) {
                return it->second; // Return the existing value
            } else {
                std::cout << "Element at position (" << i << ", " << j << ") does not exist." << std::endl;
                return 0;
            }
        } else {
            return find_compressed(i,j);
        }
    }//const version operator ()

    // definition compress method
    /**
     * @brief method to compress an uncompressed matrix
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     */
    template<typename T, StorageOrder Store>
    void Matrix<T, Store>::compress() {
        if (compressed) {
            std::cout << "The matrix is already in compressed form" << std::endl;
            return;
        }
        else {
            if constexpr (Store == StorageOrder::row) {

                // save the needed memory space
                inner_index.resize(nrows+1);
                outer_index.reserve(values.size());
                compressed_values.reserve(values.size());

                inner_index[0] = 0; //by definition

                // loop over the rows
                for(std::size_t i=0;i<nrows;i++){

                    // get the iterators at the begin and end of the row
                    std::array<std::size_t,2> low{i,0}, upp{i,ncols-1};
                    auto lower=values.lower_bound(low);
                    auto upper=values.upper_bound(upp);

                    // non-zero elements of the row
                    std::size_t non_zeros=0;


                    for(auto k=lower;k!=upper;k++){

                        outer_index.emplace_back(k->first[1]);
                        compressed_values.emplace_back(k->second);

                        non_zeros++;
                    }

                    inner_index[i+1]=inner_index[i]+non_zeros;
                }



            }
            if constexpr (Store == StorageOrder::column){
                // save the needed memory space
                inner_index.resize(ncols+1);
                outer_index.reserve(values.size());
                compressed_values.reserve(values.size());

                inner_index[0] = 0; //by definition

                // loop over the columns
                for(std::size_t i=0;i<ncols;i++){

                    std::array<std::size_t,2> low{0,i}, upp{nrows-1,i};
                    auto lower=values.lower_bound(low);
                    auto upper=values.upper_bound(upp);

                    //  non-zero elements of the column
                    std::size_t non_zero=0;

                    for(auto k=lower;k!=upper;k++){
                        // I store the row of the element and its value in the right vector
                        outer_index.emplace_back(k->first[0]);
                        compressed_values.emplace_back(k->second);
                        // increase the count of nonzero elements in the line
                        non_zero++;
                    }
                    // definition of next value of first_indexes according to the CSR
                    inner_index[i+1]=inner_index[i]+non_zero;
                }

                //TODO: handle the map ordering, for now we are assuming the matrix is correctly ordered

            };
            // Clear the map
            values.clear();

            // Update compressed flag
            compressed = true;
        };
    }//compress

    // definition uncompress method
    /**
     * @brief method to uncompress a compressed matrix
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     */
    template<typename T, StorageOrder Store>
    void Matrix<T, Store>::uncompress() {
        //check if the matrix is already in uncompressed form
        if (!compressed) {
            std::cout << "The matrix is already in uncompressed form" << std::endl;
            return;
        }

        // clear the pre-existing map (should be already empty)
        values.clear();

        if (Store == StorageOrder::row) {
            for (std::size_t i = 0; i < nrows; ++i) {
                for (std::size_t j = inner_index[i]; j < inner_index[i + 1]; ++j) {
                    values[{i, outer_index[j]}] = compressed_values[j];
                }
            }
        }
        else {
            for (std::size_t j = 0; j < ncols; ++j) {
                for (std::size_t i = inner_index[j]; i < inner_index[j + 1]; ++i) {
                    values[{outer_index[i], j}] = compressed_values[i];
                }
            }
        }


        // Clear compressed data vectors
        inner_index.clear();
        outer_index.clear();
        compressed_values.clear();

        // Update compressed flag
        compressed = false;
    }//uncompress



    //definition resize method
    /**
     * @brief method to change the size of an existing matrix
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     * @param new_rows number of wanted rows
     * @param new_cols number of wanted columns
     */
    template<typename T, StorageOrder Store>
    void Matrix<T, Store>::resize(std::size_t new_rows, std::size_t new_cols) {
        // Update the number of rows and columns
        nrows = new_rows;
        ncols = new_cols;

        // Check if the matrix is compressed, if so, uncompress it
        if (compressed) {
            Matrix<T,Store>::uncompress();
        }

        // If the new size is smaller than the current size, remove elements outside the new size
        auto it = values.begin();
        while (it != values.end()) {
            if (it->first[0] >= new_rows || it->first[1] >= new_cols) {
                it = values.erase(it);
            } else {
                ++it;
            }
        }
    } //Resize

    /**
     * @brief method to find, if exist, the element given the indexes of a compressed matrix
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     * @param row index for the row
     * @param col index for the columns
     * @return corresponding value at the input indexes
     */
    template<typename T, StorageOrder Store>
    T Matrix<T, Store>::find_compressed(std::size_t row, std::size_t col) {
        if constexpr (Store == StorageOrder::row) { // Compressed Sparse Row
            for (std::size_t i = inner_index[row]; i < inner_index[row + 1]; ++i) {
                if (outer_index[i] == col) {
                    return compressed_values[i];
                }
            }
        } else { // Compressed Sparse Column
            for (std::size_t i = inner_index[col]; i < inner_index[col + 1]; ++i) {
                if (outer_index[i] == row) {
                    return compressed_values[i];
                }
            }
        }
        std::cerr << "Error: Matrix is in compressed form and element at position (" << row << ", " << col
                  << ") does not exist." << std::endl;
    }//find_compressed

    /**
     * @brief method to print the  matrix
     * @tparam T type of the elements in the matrix
     * @tparam Store the storage order of the matrix, it can be row or column
     */
    template<typename T, StorageOrder Store>
    void Matrix<T,Store>::print() const {
        if(compressed){
            Matrix<T,Store>::uncompress();
        }

        for (auto it=values.cbegin(); it!=values.cend(); ++it)
        {
            std::cout << it->first[0] << " " << it->first[1] << ": " << it->second << std::endl;
        }
    }

    }//algebra




#endif //CHALLENGE2_MATRIX_IMP_H
