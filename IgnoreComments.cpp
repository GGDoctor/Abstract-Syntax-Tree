/**
 * @file IgnoreComments.cpp
 * @brief Implementation file for IgnoreComments class
 * @authors Jacob Franco, Zach Gassner, Haley Joerger, Adam Lyday
 */

#include "IgnoreComments.hpp"

/**
 * @brief Constructor
 * @param fileName - The name of a file that contains a C-style program
 * @remark Opens the file and runs it through a DFA to construct object with 
 *          comments ignored
 */
IgnoreComments::IgnoreComments(const string& fileName) {
    ifstream inputFile(fileName.c_str());

    if (!inputFile) {
        cerr << "Error. Unable to open the file :( \n";
    }

    State state = ANYTHING;
    int line = 1; 
    int mult_line = 0;
    char currentChar;

    while (inputFile.get(currentChar)) {

        switch (state) {

            //Handle Anything 
            case ANYTHING:
                if (currentChar == '/') {
                    state = SLASH;
                    char nextChar = inputFile.peek();
                    result += (nextChar == '/' || nextChar == '*') ? ' ' : currentChar;
                } else if (currentChar == '"') {
                    state = State::DOUBLE_QUOTE;
                    result += currentChar;
                } else if (currentChar == '\'') {
                    state = State::SINGLE_QUOTE;
                    result += currentChar;
                } else if (currentChar == '\n'){ //Check for new line
                    result += currentChar;
                    line += 1; 
                } else if (currentChar == '*'){  // Check if */ appears before /* 
                    char nextChar = inputFile.peek();
                    state = (nextChar == '/') ? ERROR : ANYTHING;
                    result += currentChar;
                } else {
                    result += currentChar;
                }
                break;
            
            // Handle Slash
            case SLASH:
                if (currentChar == '/') {
                    state = SINGLE_LINE_COMMENT;
                    result += ' ';
                } else if (currentChar == '*') {
                    state = MULTI_LINE_COMMENT;
                    result += ' ';
                } else {
                    state = ANYTHING;
                    result += currentChar;
                }
                break;

            // Handle Star 
            case ASTERICK:
                if (currentChar == '/') {
                    state = ANYTHING;
                    result += ' ';
                } else if (currentChar == '\n'){
                    line++; 
                    state = MULTI_LINE_COMMENT;
                    result += '\n';
                } else if (currentChar != '*') {
                    state = MULTI_LINE_COMMENT;
                    result += ' ';
                } 
                break;

            // Handle single line 
            case SINGLE_LINE_COMMENT:
                if (currentChar == '\n') {
                    line++; 
                    state = ANYTHING;
                    result += currentChar;
                } else {
                    result += ' ';
                }
                break;

            // Handle Multiple Line 
            case MULTI_LINE_COMMENT:
                if (currentChar == '*') {
                    state = ASTERICK;
                    result += ' ';
                } else {
                    line  += currentChar == '\n' ? 1 : 0 ;  
                    result += currentChar == '\n' ? '\n' : ' ';
                    mult_line += currentChar == '\n' ? 1 : 0 ;
                    
                } 
                break;

            // Handle Single quote
            case SINGLE_QUOTE: 
                if (currentChar == '\'') {
                    state = ANYTHING;
                }
                result += currentChar;
                break;

            case DOUBLE_QUOTE:
                if (currentChar == '"') {
                    state = ANYTHING;
                } 
                result += currentChar;
                break;

            case ERROR:
                break;
        }
    }

    if (state == ERROR) {

        result = "ERROR: Program contains C-style, unterminated comment on line "
                + to_string(line);

    } 
    
    if (state == MULTI_LINE_COMMENT) {

        result = "ERROR: Program contains C-style, unterminated comment on line "
                + to_string(line - mult_line);
                
    }

    inputFile.close(); 
}

/**
 * @brief Output operator overload
 * @param os - The output stream operator
 * @param obj - The IgnoreComments object to output
 * @returns The modified output stream
 * @remark Outputs the C program with comments ignored
 * 
 *      ex: cout << IgnoreCommentsObj;
 */
ostream& operator << (ostream& os, const IgnoreComments& obj) {
    os << obj.result;
    return os;
}