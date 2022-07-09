/**
 * vectordb.cpp
 * C++ source file that should contain implementation for member functions
 * - rows()
 * - get()
 * - add()
 * - update()
 * - remove()
 * 
 * You need to modify this file to implement the above-mentioned member functions
 * as specified in the hand-out (Task 3)
 */ 

#include <iostream>
#include "vectordb.hpp"
#include <vector>


using namespace std;
using namespace nwen;

VectorDbTable::VectorDbTable() {}
VectorDbTable::~VectorDbTable() {}

/**
 * @return
 */
int VectorDbTable::rows(){
    return table.size();
}

/**
 * @param unsigned int i
 * @return
 */
movie* VectorDbTable::get(unsigned int i){

    if(i > rows()){ //check its not 0
        return NULL; //return nothing
    }
    return &table[i]; //else return exisitng table
}

/**
 * @param movie &m
 * @return
 */
bool VectorDbTable::add(movie &m){ 
    for(int i = 0; i < rows(); i++){ //run through the rows of movies
        if(table[i].id == m.id){ //check if the movie id exist in the table
            return false; //it already exists so return false
        }
    }

    table.push_back(m); //add movie if not found on the table
    return true; //done true
}

/**
 * @param unsigned long int id
 * @param movie &m
 * @return
 */
bool VectorDbTable::update(unsigned long int id, movie &m){  
    std::vector<movie> tempTable; //temp vector
    bool movie_found = false; //movie exists
    for(int i = 0; i < rows(); i++){ //loop through row of movies
        if(table[i].id == id){ //check if the id matches
            tempTable.push_back(m); //add movie to temp vector
            movie_found = true; //set to true
        } 
        else{
            tempTable.push_back(table[i]); //add existing table of movies
        }
    }
    if(movie_found){
        table = tempTable; //append tempvector to main table
        return true; //done         
    }
    return false; //done as false otherwise
}

/**
 * @param unsigned long int id 
 * @return 
 */
bool VectorDbTable::remove(unsigned long int id){

    for(int i = 0; i < rows() ; i++){ //run through the row of movies
        if(table[i].id == id){ //check if the id exists in the table
            table.erase(table.begin()+i); //get rid of the movie
            return true; //done
        }
    }
    return false; //can not find movie return false
}
