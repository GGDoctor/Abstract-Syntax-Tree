/**
 * @file main.cpp
 * @brief Main function for Project 3 - Recursive Descent Parser
 * @authors Jacob Franco, Zach Gassner, Haley Joerger, Adam Lyday 
 */

#include "IgnoreComments.hpp"
#include "Tokenization.hpp"
#include "RecursiveDescentParser.hpp"
#include "symboltable.hpp"
#include <iostream>
#include <sstream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2 ) {
        cerr << "Error! There is not a correct number of command line arguments.\n";
        return 1;
    }

    string fileName = argv[1];
    IgnoreComments ignoreComments(fileName);

    // outputs the input program without comments
    // cout << ignoreComments << '\n'; 

    // converting the output stream to a string for Tokenization constructor
    stringstream ss;
    ss << ignoreComments;
    Tokenization tokenization(ss.str());
    vector<Token> tokens = tokenization.getTokens();

    // outputs tokens according to project 2 spec
    // cout << tokenization << '\n';

    //SymbolTable symbolTable;
    //symbolTable.populateSymbolTable(tokens);
    // Print the symbol table
    //symbolTable.print();
        
    RecursiveDescentParser recursiveDescentParser(tokens);
    // cout << recursiveDescentParser;

    // converting the output stream to a string for SymbolTable constructor
    /*
    stringstream ss2;
    ss2 << recursiveDescentParser;
    */
    SymbolTable symbolTable(recursiveDescentParser);
    cout << symbolTable;

    return 0;
}