//
// Created by bfran on 25/04/2024.
//

#ifndef CHALLENGE2_RANDOM_VEC_HPP
#define CHALLENGE2_RANDOM_VEC_HPP

#include <iostream>
#include <vector>
#include <random>

/**
 * @brief Template function that generagte a random vector with numbers between a selected range
 * @tparam T type of the elements in the matrix
 * @tparam Store the storage order of the matrix, it can be row or column
 * @param count dimension of the vector
 * @param lower_bound maximum value that the elements of the vector can reach, (prefixed 100)
 * @param upper_bound minimum value that the elements of the vector can reach, (prefixed -100)
 * @return a vector filled with random numbers
 */
template <class T>
std::vector<T> generate_random_vector(std::size_t count, double lower_bound = -100, double upper_bound = 100){
// Seed the random number generator
std::random_device rd;
std::mt19937 gen(rd());

// Create a uniform distribution for random doubles
std::uniform_real_distribution<T> dist(lower_bound, upper_bound);

// Create a vector to store the random doubles
std::vector<T> vec(count);

// Fill the vector with random doubles
for (size_t i = 0; i < count; ++i) {
vec[i] = dist(gen);
}

return vec;
}

#endif //CHALLENGE2_RANDOM_VEC_HPP
