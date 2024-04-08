/** 
 * @file IgnoreComments.hpp
 * @brief Defines the IgnoreComments class that ignores comments for a C-style program
 * @authors Jacob Franco, Zach Gassner, Haley Joerger, Adam Lyday
 */

#ifndef IGNORE_COMMENTS_HPP
#define IGNORE_COMMENTS_HPP

#include <iostream>
#include <fstream> //Including fstream and string allow us to read in the input file
#include <string>

using namespace std;

/**
 * @class IgnoreComments
 * @brief Ignores comments from a C-style program 
 */
class IgnoreComments {
public:
    /**
     * @brief Constructor
     * @param fileName - The name of a file that contains a C-style program
     */
    IgnoreComments(const string& fileName);

    /**
     * @brief Output operator overload
     * @param os - The output stream operator
     * @param obj - The IgnoreComments object to output
     * @returns The modified output stream
     * @remark Outputs the C program with comments ignored
     * 
     *      ex: cout << IgnoreCommentsObj;
     */
    friend ostream& operator << (ostream& os, const IgnoreComments& obj);

    /** 
     * @enum State
     * @brief Enumerates the states of the DFA
     */
    enum State {
        ANYTHING,
        SLASH,
        ASTERICK,
        SINGLE_LINE_COMMENT, // A comment pertaining '//'
        MULTI_LINE_COMMENT,  // A comment pertaining '/* bla */
        SINGLE_QUOTE,
        DOUBLE_QUOTE,
        ERROR
    };

private:
    /**
     * @brief This string contains the input program with comments ignored
     */
    string result;
};

#endif