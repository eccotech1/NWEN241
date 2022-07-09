/**
 * Username: competelri
 * ID: 300573032
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/**
 * album structure. See handout for details.
 */ 
struct album {
	unsigned long id;
	char title[100];
	unsigned short year;
	char artist[100];
};


/**
 * db_table structure. See handout for details.
 */   
struct db_table {
	struct album *table;
	unsigned int rows_total;
	unsigned int rows_used;
};


/**
 * Function prototype for Task 1. See handout for details.
 */ 
int db_show_row(const struct db_table *db, unsigned int row){

	if(row < db -> rows_used && row >= 0){ //only if the row is valid/found
		printf("%6lu:%20.20s:%20.20s:%4u\n", db -> table[row].id, db -> table[row].title, db -> table[row].artist, db -> table[row].year); //print statement
		return 1; 
	}
	return 0; //else return 0 if not found/valid
}

/**
 * Function prototype for Task 2. See handout for details.
 */ 
int db_add_row(struct db_table *db, struct album *a){


	if(db -> rows_used == db -> rows_total){ //check if the all available rows is filled
		db -> table = (struct album*)realloc(db -> table, db -> rows_total + 5 * sizeof(struct album)); //realloc the memory
		db -> rows_total += 5; //add 5 rows
	}
	//check if the rows used is less than the total
	if(db->rows_used < db->rows_total){
		db->table[db->rows_used] = *a; //assigned the used row to the pointer
		db->rows_used += 1; //increase the amount of rows used
		return 1;
		
	}
	return 0; //return 0 if not
}

/**
 * Function prototype for Task 3. See handout for details.
 */ 
int db_remove_row(struct db_table *db, unsigned long id){
	int moveUp = -1; //moveup one row

	for(int i = 0; i < db -> rows_used; i++){
		if(db -> table[i].id == id){ //if id is found
			moveUp = i; //appened id to moveUp
		}
	}
	if(moveUp != -1){ //check if moveUp is equal to id

		for(int i = moveUp; i< db -> rows_used-1; i++){ //run through all the rows used
			db -> table[i] = db ->table[i+1]; //copy data to the next row
		}
		db -> rows_used -= 1; //remove one used row

		if(db -> rows_total - db ->rows_used > 4){ //check if its greater than 4
			//realloc memory with 5 less than initial
			db -> rows_total -= 5; //remove 5 rows
			db -> table = (struct album*)realloc(db -> table, db -> rows_total * sizeof(struct album));

		}
		return 1;
	}
	return 0; //return 0 otherwise
}


