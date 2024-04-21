//
// Created by bfran on 18/04/2024.
//

#ifndef CHALLENGE2_MATRIX_IMP_H
#define CHALLENGE2_MATRIX_IMP_H

#include "Matrix.hpp"

// Implementation of the template class Matrix
// This is done in a header file since it's a template
namespace algebra {

    // Method to read matrix from Matrix Market format
    template<typename T, StorageOrder Store>
    void Matrix<T,Store>::read_matrix_market(const std::string& filename) {
        //Clear previous matrix
        values.clear();
        inner_index.clear();
        outer_index.clear();
        compressed_values.clear();

        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::string line;

        while (file.peek() == '%') {
            file.ignore(2048, '\n');
        }
        // std::istringstream iss(line);
        std::size_t num_rows, num_cols, num_elements;

        file >> num_rows >> num_cols >> num_elements;


        for (std::size_t i = 0; i < num_elements; ++i) {
            std::size_t row, col;
            T value;
            file >> row >> col >> value;


            // we always use the format (row, col) -> value
            // only the comparison operator is different
            values[{row - 1, col - 1}] = value;
        }
        return;
    }

    // definition operator () non const version
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
    template<typename T, StorageOrder Store>
    void Matrix<T, Store>::resize(std::size_t new_rows, std::size_t new_cols) {
        // Update the number of rows and columns
        nrows = new_rows;
        ncols = new_cols;

        // Check if the matrix is compressed, if so, uncompress it
        if (compressed) {
            // TODO: uncompress();
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

    template<typename T, StorageOrder Store>
    void Matrix<T,Store>::print() const {

        for (auto it=values.cbegin(); it!=values.cend(); ++it)
        {
            std::cout << it->first[0] << " " << it->first[1] << ": " << it->second << std::endl;
        }
    }

    }//algebra




#endif //CHALLENGE2_MATRIX_IMP_H
