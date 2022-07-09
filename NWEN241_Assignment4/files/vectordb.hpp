/**
 * vectordb.hpp
 * C++ header file that should contain declaration for
 * - VectorDbTable class
 * 
 * You need to modify this file to declare VectorDbTable class 
 * as specified in the hand-out (Task 2)
 */ 
 
#include <vector>
#include "abstractdb.hpp"

using namespace std;
namespace nwen {  

    class VectorDbTable : public AbstractDbTable {      
            
    public:
        VectorDbTable();
        ~VectorDbTable();

        int rows();
        movie* get(unsigned int i);
        bool add(movie &m);
        bool update(unsigned long int id, movie &m);
        bool remove(unsigned long int id);
        
     private:
        std::vector<movie> table;
        
    
    };
}