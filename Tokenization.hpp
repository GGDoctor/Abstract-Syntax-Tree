/**
 * @file Tokenization.hpp
 * @brief Defines the Tokenization class that takes in a comment-less C-style
 *        program as input and tokenizes it
 * @authors Jacob Franco, Zach Gassner, Haley Joerger, Adam Lyday 
 */

#ifndef TOKENIZATION_HPP
#define TOKENIZATION_HPP

#include "IgnoreComments.hpp"
#include <iostream>
#include <fstream> //Including fstream and string allow us to read in the input file
#include <string>
#include <vector> 
#include <cctype>
#include <algorithm>

using namespace std;

/**
 * @enum TokenType
 * @brief Enumerates the different token types 
 */
enum TokenType {
    CHAR,
    VOID,
    IDENTIFIER,
    INTEGER,
    STRING,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    DOUBLE_QUOTE,
    SINGLE_QUOTE,
    SEMICOLON,
    COMMA,
    ASSIGNMENT,
    PLUS,
    MINUS,
    DIVIDE,
    ASTERISK,
    MODULO,
    CARAT,
    LT,
    GT,
    LT_EQUAL,
    GT_EQUAL,
    BOOLEAN_AND_OPERATOR,
    BOOLEAN_OR_OPERATOR,
    BOOLEAN_NOT_OPERATOR,
    BOOLEAN_EQUAL,
    BOOLEAN_NOT_EQUAL,
    BOOLEAN_TRUE,
    BOOLEAN_FALSE,
};

/**
 * @struct Token
 * @brief Encapsulates a token type and the actual character(s) of the token
 */
struct Token {
    TokenType type;
    string character;
    int lineNumber;
};

/**
 * @class Tokenization 
 * @brief Tokenizes a C-style program
 */
class Tokenization {
public:
    /**
     * @brief Constructor
     * @param input - A string representing a C-style program 
     */
    Tokenization(const string& input);

    /**
     * @brief Output operator overload
     * @param os - The output stream operator
     * @param obj - The Tokenization object to output
     * @returns The modified output stream
     * @remark Outputs tokens according to project2 spec
     * 
     *      ex: cout << TokenizationObj;
     */
    friend ostream& operator << (ostream& os, const Tokenization& obj);

    /**
     * @brief Getter for tokens private variable
     * @returns The tokens vector
     */
    vector<Token> getTokens();

    /**
     * @brief The actual characters of some of the token types
     */
    vector<char> listOfSymbols = {
        '(', 
        ')', 
        '[', 
        ']', 
        '{', 
        '}', 
        '\"', 
        '\'', 
        ';', 
        ',', 
        '=', 
        '+', 
        '-', 
        '\\',
        '*', 
        '%', 
        '^', 
        '>', 
        '<', 
        '&', 
        '|', 
        '!'
    };

private:
    /**
     * @brief Contains the tokens from the input C program
     */
    vector<Token> tokens;
};

#endif