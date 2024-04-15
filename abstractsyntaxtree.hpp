#ifndef ABSTRACT_SYNTAX_TREE_HPP
#define ABSTRACT_SYNTAX_TREE_HPP

#include <iostream>
#include <vector>
#include <list>
#include "Tokenization.hpp" // Include the header where Token is defined
#include "RecursiveDescentParser.hpp"

using namespace std;

struct AbstractTableEntry {
    AbstractTableEntry() : identifierName(""), identifierType(""), datatype(""), 
                    datatypeIsArray(false), datatypeArraySize(0), scope(0) { }
    string identifierName;
    string identifierType;
    string datatype;
    bool datatypeIsArray;
    int datatypeArraySize;
    int scope;
};

struct AbstractParamListEntry {
    AbstractParamListEntry() : identifierName(""), datatype(""), 
                       datatypeIsArray(false), datatypeArraySize(0), 
                       scope(0) { }
    string paramListName;
    string identifierName;
    string datatype;
    bool datatypeIsArray;
    int datatypeArraySize;
    int scope;
};

enum AbstractStateDFA {
    ABSTRACT_OTHER_STATE,
    ABSTRACT_FUNCTION,
    ABSTRACT_PROCEDURE,
    ABSTRACT_VARIABLE,
};

// Symbol table class
class AbstractSyntaxTree {
public:
    /**
     * @brief Constructs SymbolTable object
     * @param cst - The string version of the concrete syntax tree that was
     *          generated in RecursiveDescentParser
     */
    AbstractSyntaxTree(RecursiveDescentParser concreteSyntaxTree);

    /**
     * @brief Output operator overload
     * @param os - The output stream operator
     * @param obj - The SymbolTable object to output
     * @returns The modified output stream
     * @remark Outputs symbol table according to project spec
     * 
     *      ex: cout << SymbolTableObj;
     */
    friend ostream& operator << (ostream& os, const AbstractSyntaxTree& obj);
private:
    void parseParams(const vector<string>& params, int scope, const string& 
                                                                paramListName);
    list<AbstractTableEntry> table;
    list<AbstractParamListEntry> paramTable;
};

#endif /* ABSTRACT_SYNTAX_TREE_HPP */

// /**
//  * @file RecursiveDescentParser.hpp
//  * @brief Defines the RecursiveDescentParser class that takes in vector of 
//  *        tokens and creates a Left Child Right Sibling (LCRS) binary tree
//  *        that represents the Concrete Syntax Tree (CST) 
//  * @authors Jacob Franco, Zach Gassner, Haley Joerger, Adam Lyday 
//  */

// #ifndef RECURSIVE_DESCENT_PARSER_HPP
// #define RECURSIVE_DESCENT_PARSER_HPP

// #include "IgnoreComments.hpp"
// #include "Tokenization.hpp"
// #include <iostream>
// #include <vector>

// using namespace std;

// /**
//  * @enum State
//  * @brief Enumerates the different states of the DFA 
//  */
// enum State {
//     OTHER,
//     FUNCTION_DECLARATION,
//     VARIABLE_DECLARATION,
//     VARIABLE_ASSIGNMENT,
//     LOOP,
//     CONDITIONAL
// };

// /**
//  * @class LCRS
//  * @brief Represents a LCRS binary tree 
//  */
// class LCRS {
// public:
//     /**
//      * @brief Constructor
//      * @param token - A token from the input vector of tokens 
//      */
//     LCRS(const Token& token) : token(token), leftChild(nullptr), rightSibling(nullptr) { }

//     /**
//      * @brief Breadth-first search function
//      * @returns A string with the resulting BFS
//      */
//     string BFS() const;

// private:
//     /**
//      * @brief The data for a LCRS node 
//      */
//     Token token;

//     /**
//      * @brief The LCRS node's LC
//      */
//     LCRS* leftChild;

//     /**
//      * @brief The LCRS node's RS 
//      */
//     LCRS* rightSibling;

//     /**
//      * @brief Returns the DFA state 
//      * @param token - The token to get the DFA state for
//      */
//     State getStateDFA(Token token);

//     /**
//      * @brief So RecursiveDescentParser can access LCRS private variables 
//      */
//     friend class RecursiveDescentParser;
//     friend class SymbolTable;
// };


// /**
//  * @class RecursiveDescentParser
//  * @brief Creates a CST using Recursive Descent Parsing
//  */
// class RecursiveDescentParser {
// public:
//     /**
//      * @brief Constructor
//      * @param tokens - A vector of tokens from a C-style program 
//      */
//     RecursiveDescentParser(const vector<Token>& tokens);

//     /**
//      * @brief Getter for CST 
//      */
//     LCRS* getConcreteSyntaxTree();

//     /**
//      * @brief Output operator overload
//      * @param os - The output stream operator
//      * @param obj - The RecursiveDescentParser object to output
//      * @returns The modified output stream
//      * @remark Outputs CST according to project spec
//      * 
//      *      ex: cout << RecursiveDescentParserObj;
//      */
//     friend ostream& operator << (ostream& os, const RecursiveDescentParser& obj);

// private:
//     /**
//      * @brief The CST 
//      */
//     LCRS* concreteSyntaxTree;
// };

// #endif