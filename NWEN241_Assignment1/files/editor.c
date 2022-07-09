 #include <stdio.h>
 #include "editor.h"
 #include <string.h>

int editor_insert_char(char editing_buffer[], int editing_buflen, char to_insert, int pos)
{
    if (pos >= editing_buflen || pos < 0) return 0; // if the pos is outside/equal the editing_buflen return 0

    //Declare variable
    char txt[editing_buflen]; //temp char array
    txt[pos] = to_insert; //insert character at pos index

    for(int i = pos; i < editing_buflen; i++){ //run from the pos index to the last character
        txt[i+1] = editing_buffer[i]; //copy the character given at i and then move every character 1 space to the right
        editing_buffer[i] = txt[i]; //appened the value from the temporary array to the main char array
    }
        return 1; //return 1 if its inside the array size

    }

    int editor_delete_char(char editing_buffer[], int editing_buflen, char to_delete, int offset)
    {
        if (offset >= editing_buflen || offset < 0) return 0; //if the offset is outside then return 0

        char txt[editing_buflen];

        for(int i = offset; i < editing_buflen; i++){ //start from the offset until the end of the array
            if(editing_buffer[i] == to_delete){ //if the char is the one to be deleted
                txt[i]=editing_buffer[i+1]; // copy the next character on the editing buffer and append on the temp array
                editing_buffer[i]=txt[i]; //appended on temp array
                for(int j = i+1; j < editing_buflen; j++){ //from the letter to be removed +1 till the end of the array
                    txt[j] = editing_buffer[j]; //appened the temp to the main array
                    editing_buffer[j-1] = txt[j]; // move everything to the left after deleting
                }
                return 1; //return 1
            }   

        }
        return 0; //return 0 if char is not found in the array
    }

    int editor_replace_str(char editing_buffer[], int editing_buflen, const char *str, const char *replacement, int offset)
    {
        unsigned int diff, newStr = strlen(replacement), strLength = strlen(str);
        //return -1 right away if it's out of bounds
        if(offset >= editing_buflen || offset < 0 || strLength == 0 || editing_buflen <= offset + strLength) return -1;

        //make sure difference is not a negative value
        if(strLength > newStr) diff = strLength - newStr; 
        else diff = newStr - strLength;
        //loop from offset to end of buffer
        for (int i = offset; i < editing_buflen; i++) {
            unsigned int j;
            for (j = 0; j < strLength; j++) //loop through original text
                if (editing_buffer[i + j] != str[j]) break; //break if the character does not match

        //only continue when original text length is larger than index
            if (strLength > j) continue; 
            //Move to the left
         if (strLength > newStr) { 
            for (j = i; j < editing_buflen; j++)
                editing_buffer[j] = editing_buffer[j + diff];
        }
        //Move to the right
                else if (strLength < newStr) { 
                    for (j = editing_buflen - 1; j > i + strLength; j--)
                        editing_buffer[j] = editing_buffer[j - diff];
                }
                for (j = 0; j < newStr; j++)
                    if(i + j < editing_buflen) editing_buffer[i + j] = replacement[j];
                return i + j - 1;
            }
            return -1; //return -1 otherwise
        }

    void editor_view(int rows, int cols, char viewing_buffer[rows][cols], const char editing_buffer[], int editing_buflen, int wrap){
            int row, col;
            char* disp[rows]; //pointer
            //setup brackets
            for(row = 0; row < rows; row++){
                disp[row]=(char*) viewing_buffer + cols * row;
                for(col = 0; col < cols; col++)
                    disp[row][col] ='\0'; //blank
            }
            //restart to 0
            row = 0;
            col = 0;
            for (unsigned int i = 0; i < editing_buflen; i++) {
                char c = editing_buffer[i];

        if (c == '\0') break; //End of editing_buffer 

        if (c == '\n') { //Go to next line and start at far left column
            row++;
            col = 0;
            continue;
        }
        //make sure to go next line if wrap and column is bigger than the last column
        if (wrap && col  >= cols - 1) {
            row++;
            col = 0;
        } 
        if (col >= cols - 1) continue;
        else if (row >= rows) break;

        disp[row][col] = c; //append character

        col++;
    }
}
