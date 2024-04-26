# Challenge2
Second challenge PACS course

Francesca Barbaro  
Codice persona: 10662154  
Matricola: 220332 

In this repository you find:

### Makefile:

A Makefile is present in the Src directory, so the code is 'ready to go'.
In order to smoothly compile the code just go in the directory
and type `make`.
To run the code and visualize the result type `./main`.

### Src:

In this directory you find:
* **main.cpp**
* **Insp_131.mtx**: test matrix dowloaded form the Matrix Market
* **Matrix.mtx**: a smaller matrix in the Matrix Market format to do easyer
tests

### Include:

In this directory you find:
* **Matrix.hpp**: Header file for the template class Matrix
* **Matrix_imp.hpp**: Header file for the implementation of the class Matrix
* **random_vec.hpp**: Header file containing a template function to generate vectors of random numbers
* **chrono.hpp**: utility to make the timing test of the Matrix-Vector product

### Results:
Here the results of the timing of the Matrix-Vector product operation are displayed.  
The time is evaluated running 10.000 multiplications and then averaging the results.

| TIME             | Row-wise | Column-wise    |
|------------------|----------|----------------|
| **Uncompressed** | 3.62104  | 4.95656        |
| **Compressed**   | 0.307987 | 0.500174       |

It's possible to notice that the Compressed form works much better for the multiplcation
both for the row-wise and the column-wise ordering, in fact it takes circa 10 times less.  
This shows that contiguos memory allocation is more efficient.  
Comparing instead row and column ordering we observe that the row-wise ordering is behaving better.



### Further details on the template class Matrix:
This class allows to store a sparse matrix in a dynamic form or in a compressed 
form, it's also possible to switch from a form to the other.
* **COOmap**: the **Dynamic** (or uncomprassed) form. It's based on a map:
  (i, j) → A(i, j), where the tuple (i,j) is the key.
* **CSR** Compressed Sparse Row: a **compressed** form, where elements of the matrix are stored 
in a vector ordered by rows. There are then two vectors of indexes. The first (the inner indexes),
  of length the number of rows plus one, contains the starting index for the elements of
  each row. The second vector of indexes (the outer indexes), of length the number of nonzeroes,
  contains the corresponding column index. 
* **CSC** Compressed Sparse Column: a **Compressed** form, As before, but with the role of row and column exchanged.
  The matrix is thus stored column-wise.

You can find more documentation for the storage types at this [link](https://en.wikipedia.org/wiki/Sparse_matrix).  

**Template parameters**: 
* `T` → Type of the elements of the matrix. It can also be complex.
* `StorageOrder Store` → Enumerator to set the ordering of the matrix row-wise `StorageOrder::row` 
or column-wise `StorageOrder::column.`  

In the class Marix.hpp you find:

#### Private:
* **`nrows`**, **`ncols`** → number of rows and colums in the matrix.
* **`values`** → map for the COOmap form. The map's key is (i,j) with i for
the rows and j for the columns if the 
template parameter `StorageOrder Store` is set on `row`. In case `StorageOrder 
Store` is set on `column`, the map's key is (j, i), with j for the columns and 
i for the rows. This allows to have the element stored in the map sordet by row or by column,
* **`inner_index`**, **`outer_index`**, **`compressed_values`** → the three vectors to store the 
matrix in compressed form. `inner_index` will contain the index of the rows if
using CSR form, and the columns for CSC form. The `outher_index` contain the remaning
index.
* **`compressed`** → boolean variable true if the matrix is in compressed form.

#### Pubblic:
* Default constructor.
* Constructor taking in input the number of rows and the number of columns.
* **`read_matrix_market`** → method that can read the matrix in a Matrix Market form.
* **`is_compressed`** → method giving true if the matrix is in compressed form.
* **`operator()`** → both in his const and non const version, given two indexes 
return the value.
* **`compress`** and **`uncompress`** → methods that do what they say.
* **`resize`** → method that given a matrix in uncompressed form can reshape it.
* **`storage_val_compressed`** and **`storage_val_map`** → two methods that help the **`compress`**
and **`uncompress`** ones.

#### Friends:
* `operator*` → allows the product between a matrix M and a vector V.


### Further details on main.cpp:
In the main.cpp you find different blocks commented that does different test.
To try just decomment the one needed.
It's possible to switch between matrix selecting: `filename` or `filename_short`.  
The main print out the results of the timing test explained before.