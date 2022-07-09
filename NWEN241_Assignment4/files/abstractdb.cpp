/**
 * abstractdb.cpp
 * C++ source file that should contain implementation for member functions
 * - loadCSV()
 * - saveCSV()
 * 
 * You need to modify this file to implement the above-mentioned member functions
 * as specified in the hand-out (Tasks 4 and 5)
 */ 



#include <iostream>
#include <fstream>
#include <typeinfo>
#include <vector>
#include <sstream>
#include "abstractdb.hpp"
#include "vectordb.cpp"
#include <string>

using namespace nwen;
using namespace std;


/**
 * @param string fileName
 * @return
 */
bool AbstractDbTable::saveCSV(string fileName){
    ofstream MyFile(fileName);
    if(!MyFile.is_open()){ //check if file can be opened
        cerr << "Failed to Open File" << endl; //print this 
        return false; //can not save
    }
    for(int i = 0; i < rows(); i++){  //loop through the rows of the table
        try{
            movie* mov = get(i); //movie object
            //append movie found on every row within the file
            MyFile << mov->id << ",\"" << mov->title << "\"," << mov->year << ",\"" << mov->director << "\"\n";
        }catch(exception &e){ //check if writing to file fails
            MyFile.close(); //close file
            cerr << "Failed to Write File" << endl; //print this
            return false; //failure
        } 
    }
    MyFile.close(); //close file
    return true; //done
};


/**
 * @param string infn
 * @return
 */
bool AbstractDbTable::loadCSV(string infn){
    ifstream MyFile(infn); //file name
    string rNum; //row index
    movie mov; //movie obj

    if (!MyFile.is_open()){ //fail to open
        cerr << "Failed to Load File" << endl;
        return false; 
    }
    
    while(getline(MyFile, rNum)){ //run through the rows of the file
        string tokens[4]; //tokens: id, title, year, director 
        string token; //each token
        int increment = 0; //counter
        istringstream linestream(rNum); //line
        while(getline(linestream, token, ',')){ //check for commas while looping through the lines
            
            if(increment > 3){ //if there are more than 4 tokens
                MyFile.close(); //close file
                return false; //cannot load
            }
            tokens[increment++] = token; //append each token as it increments through tokens
        }
        try{
            mov.id = stoul(tokens[0], nullptr, 10); //discard space char and grab the id
            strcpy(mov.title, tokens[1].c_str()); //copy the title of the movie
            mov.year = (unsigned int) stoul(tokens[2], nullptr, 10); //discard space char
            strcpy(mov.director, tokens[3].c_str()); //copy director
            add(mov); //add the movie obj
        } catch(exception &e){ //can not read row of movie
            MyFile.close(); //close file 
            return false; //return false due to thrown exception
        }
        if(tokens[1].size() >= 45 || tokens[3].size() >= 45){ //check if title and director name are too long of a label
            MyFile.close();
            return false;
        }
        if(increment < 4){ //check if there are only 3 tokens in the row
            MyFile.close(); //close file
            return false; //failed to load
        }

    }
    MyFile.close(); //close file
    return true; //done
}

