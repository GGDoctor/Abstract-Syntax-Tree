#include "abstractsyntaxtree.hpp"
#include <string>
#include <sstream>
#include <cassert>

/**
 * @brief Constructs SymbolTable object
 * @param concreteSyntaxTree - The concrete syntax tree that was generated in
 *                                                      RecursiveDescentParser
 */
AbstractSyntaxTree::AbstractSyntaxTree(RecursiveDescentParser concreteSyntaxTree) {
    // converts concreteSyntaxTree to string
    stringstream ss;
    ss << concreteSyntaxTree;
    string cst = ss.str();

    // makes 2d vector of strings from the cst string
    //
    // outter vector is vector of vectors where each inner vector represents one
    // line from the cst string
    //
    // inner vector has each word from that line
    vector<vector<string>> linesAndWords;
    string line;

    while (getline(ss, line)) {
        istringstream lineStream(line);
        vector<string> words;
        string word;

        while (lineStream >> word) {
            words.push_back(word);
        }

        linesAndWords.push_back(words);
    }

    LCRS *lcrs = concreteSyntaxTree.getConcreteSyntaxTree();
    vector<string> slice;
    // used to keep track of when a new scope is neccesary -- when to scope++
    int leftBraceCounter = 0;
    int scope = 1;
    for (int i = 0; i < linesAndWords.size(); i++) {
        int lineNumber = lcrs->token.lineNumber;
        AbstractTableEntry tableEntry;
        AbstractStateDFA state = linesAndWords[i][0] == "function" ? ABSTRACT_FUNCTION : 
                         linesAndWords[i][0] == "procedure"  ? ABSTRACT_PROCEDURE : 
                        (linesAndWords[i][0] == "int" || 
                         linesAndWords[i][0] == "char" ||
                         linesAndWords[i][0] == "bool") ? 
                         ABSTRACT_VARIABLE : ABSTRACT_OTHER_STATE;

        switch (state) {
            case ABSTRACT_FUNCTION:
                tableEntry.identifierName = linesAndWords[i][2];
                tableEntry.identifierType = linesAndWords[i][0];
                tableEntry.datatype = linesAndWords[i][1];
                tableEntry.datatypeIsArray = false;
                tableEntry.datatypeArraySize = 0;
                tableEntry.scope = scope;
                table.push_back(tableEntry);
                assert(linesAndWords[i][3] == "(");
                slice.assign(linesAndWords[i].begin() + 4, linesAndWords[i].end());
                parseParams(slice, scope, tableEntry.identifierName);
                break;
            case ABSTRACT_PROCEDURE:
                tableEntry.identifierName = linesAndWords[i][1];
                tableEntry.identifierType = linesAndWords[i][0];
                tableEntry.datatype = "NOT APPLICABLE";
                tableEntry.datatypeIsArray = false;
                tableEntry.datatypeArraySize = 0;
                tableEntry.scope = scope;
                table.push_back(tableEntry);
                assert(linesAndWords[i][2] == "(");
                slice.assign(linesAndWords[i].begin() + 3, linesAndWords[i].end());
                parseParams(slice, scope, tableEntry.identifierName);
                break;
            case ABSTRACT_VARIABLE:
                tableEntry.identifierType = "datatype";
                tableEntry.datatype = linesAndWords[i][0];
                tableEntry.scope = leftBraceCounter == 0 ? 0 : scope;

                for (int j = 1; j < linesAndWords[i].size();) {
                    if (linesAndWords[i][j] == ";")
                        break;
                    tableEntry.identifierName = linesAndWords[i][j];
                    tableEntry.datatypeIsArray = false;
                    tableEntry.datatypeArraySize = 0;

                    // array
                    if (linesAndWords[i][j + 1] == "[") {
                        tableEntry.datatypeIsArray = true;
                        tableEntry.datatypeArraySize = stoi(linesAndWords[i][j + 2]);
                        
                        for (const auto& symbol : table) {
                            if (symbol.identifierName == tableEntry.identifierName&&
                                (symbol.scope == 0 || symbol.scope == tableEntry.scope)) {
                                string globallyLocally = symbol.scope == 0 ? 
                                                "globally" : "locally";
                                cerr << "Error on line " << lineNumber << ": variable \""
                                    << tableEntry.identifierName << "\" is already "
                                    << "defined " << globallyLocally << '\n';
                                exit(0);
                            }
                        }

                        for (const auto& symbol : paramTable) {
                            if (symbol.identifierName == tableEntry.identifierName &&
                                symbol.scope == tableEntry.scope) {
                                cerr << "Error on line " << lineNumber << ": variable \""
                                    << tableEntry.identifierName << "\" is already "
                                    << "defined locally\n";
                                exit(0);
                            }
                        }

                        table.push_back(tableEntry);
                        j += 5;
                    } else {

                        for (const auto& symbol : table) {
                            if (symbol.identifierName == tableEntry.identifierName &&
                                (symbol.scope == 0 || symbol.scope == tableEntry.scope)) {
                                string globallyLocally = symbol.scope == 0 ? 
                                                "globally" : "locally";
                                cerr << "Error on line " << lineNumber << ": variable \""
                                    << tableEntry.identifierName << "\" is already "
                                    << "defined " << globallyLocally << '\n';
                                exit(0);
                            }
                        }

                        for (const auto& symbol : paramTable) {
                            if (symbol.identifierName == tableEntry.identifierName &&
                                symbol.scope == tableEntry.scope) {
                                cerr << "Error on line " << lineNumber << ": variable \""
                                    << tableEntry.identifierName << "\" is already "
                                    << "defined locally\n";
                                exit(0);
                            }
                        }

                        table.push_back(tableEntry);
                        j += 2;
                    }
                }
                break;
            case ABSTRACT_OTHER_STATE:
                if (linesAndWords[i][0] == "{")
                    leftBraceCounter++;

                if (linesAndWords[i][0] == "}") {
                    leftBraceCounter--;

                    if (leftBraceCounter == 0)
                        scope++;
                }
                break;
        }

        while (lcrs->rightSibling)
            lcrs = lcrs->rightSibling;
        if (lcrs->leftChild)
            lcrs = lcrs->leftChild;
    }
}

/**
 * @brief Output operator overload
 * @param os - The output stream operator
 * @param obj - The SymbolTable object to output
 * @returns The modified output stream
 * @remark Outputs symbol table according to project spec
 *
 *      ex: cout << SymbolTableObj;
 */
ostream &operator<<(ostream &os, const AbstractSyntaxTree &obj) {
    AbstractParamListEntry previous;

    for (const auto &entry : obj.table) {
        os << "IDENTIFIER_NAME: " << entry.identifierName << '\n';
        os << "IDENTIFIER_TYPE: " << entry.identifierType << '\n';
        os << "DATATYPE: " << entry.datatype << '\n';
        os << "DATATYPE_IS_ARRAY: ";
        if (entry.datatypeIsArray)
            os << "yes\n";
        else
            os << "no\n";
        os << "DATATYPE_ARRAY_SIZE: " << entry.datatypeArraySize << '\n';
        os << "SCOPE: " << entry.scope << '\n';
        os << '\n';
    }

    for (const auto &entry : obj.paramTable) {
        if (previous.paramListName != entry.paramListName) {
            os << "PARAMETER LIST FOR: " << entry.paramListName << '\n';
            previous = entry;
        }
        os << "IDENTIFIER_NAME: " << entry.identifierName << '\n';
        os << "DATATYPE: " << entry.datatype << '\n';
        os << "DATATYPE_IS_ARRAY: ";
        if (entry.datatypeIsArray)
            os << "yes\n";
        else
            os << "no\n";
        os << "DATATYPE_ARRAY_SIZE: " << entry.datatypeArraySize << '\n';
        os << "SCOPE: " << entry.scope << '\n';
        os << '\n';
    }

    return os;
}

void AbstractSyntaxTree::parseParams(const vector<string> &params, int scope, 
                                            const string &paramListName) {
    if (params[0] == "void")
        return;
    AbstractParamListEntry paramListEntry;
    paramListEntry.paramListName = paramListName;
    paramListEntry.scope = scope;

    for ( int j = 0; j < params.size(); ) {
        if (params[j] == ")")
            break;
        paramListEntry.identifierName = params[j + 1];
        paramListEntry.datatype = params[j];
        paramListEntry.datatypeIsArray = false;
        paramListEntry.datatypeArraySize = 0;

        // array
        if (params[j + 2] == "[") {
            paramListEntry.datatypeIsArray = true;
            paramListEntry.datatypeArraySize = stoi(params[j + 3]);
            paramTable.push_back(paramListEntry);
            j += 6;
        }
        else { // regular variable
            paramTable.push_back(paramListEntry);
            j += 3;
        }
    }

    /*
    // Loop through the parameters
    for (int i = 0; i < params.size(); ++i) {
        // Skip commas
        if (params[i] == ",")
            continue;

        // Skip the '[' character for arrays
        if (params[i] == "[") {
            // The array size is the next parameter
            paramListEntry.datatypeArraySize = stoi(params[i + 1]);
            continue;
        }

        // Skip the ']' character for arrays
        if (params[i] == "]")
            continue;

        // Determine if the parameter is a datatype or identifier
        if (params[i + 1] == "," || params[i + 1] == "[" || params[i + 1] == ")") {
            // Parameter is an identifier
            paramListEntry.identifierName = params[i];
            paramTable.push_back(paramListEntry);
        } else {
            // Parameter is a datatype
            paramListEntry.datatype = params[i];
        }
    }
    */
}

// /**
//  * @file RecursiveDescentParser.cpp
//  * @brief Implementation file for the RecursiveDescentParser class
//  * @authors Jacob Franco, Zach Gassner, Haley Joerger, Adam Lyday
//  */

// #include "RecursiveDescentParser.hpp"
// #include <queue>

// /**
//  * @param token - A token object
//  * @returns The actual character(s) of the token
//  */
// auto toTokenCharacter(Token token) {
//     return token.character;
// }

// /**
//  * @brief Returns string version of token type
//  * @param token - A Token object
//  * @returns A string of the token type instead of 0, 1, 2, ...
//  */
// auto toTokenType(Token token) {
//     switch (token.type) {
//         case CHAR:
//             return "CHAR";

//         case VOID:
//             return "VOID";

//         case IDENTIFIER:
//             return "IDENTIFIER";

//         case INTEGER:
//             return "INTEGER";

//         case STRING:
//             return "STRING";

//         case LEFT_PARENTHESIS:
//             return "LEFT_PARENTHESIS";

//         case RIGHT_PARENTHESIS:
//             return "RIGHT_PARENTHESIS";

//         case LEFT_BRACKET:
//             return "LEFT_BRACKET";

//         case RIGHT_BRACKET:
//             return "RIGHT_BRACKET";

//         case LEFT_BRACE:
//             return "LEFT_BRACE";

//         case RIGHT_BRACE:
//             return "RIGHT_BRACE";

//         case DOUBLE_QUOTE:
//             return "DOUBLE_QUOTE";

//         case SINGLE_QUOTE:
//             return "SINGLE_QUOTE";

//         case SEMICOLON:
//             return "SEMICOLON";

//         case COMMA:
//             return "COMMA";

//         case ASSIGNMENT:
//             return "ASSIGNMENT";

//         case PLUS:
//             return "PLUS";

//         case MINUS:
//             return "MINUS";

//         case DIVIDE:
//             return "DIVIDE";

//         case ASTERISK:
//             return "ASTERISK";

//         case MODULO:
//             return "MODULO";

//         case CARAT:
//             return "CARAT";

//         case LT:
//             return "LT";

//         case GT:
//             return "GT";

//         case LT_EQUAL:
//             return "LT_EQUAL";

//         case GT_EQUAL:
//             return "GT_EQUAL";
    
//         case BOOLEAN_AND_OPERATOR:
//             return "BOOLEAN_AND_OPERATOR";

//         case BOOLEAN_OR_OPERATOR:
//             return "BOOLEAN_OR_OPERATOR";

//         case BOOLEAN_NOT_OPERATOR:
//             return "BOOLEAN_NOT_OPERATOR";

//         case BOOLEAN_EQUAL:
//             return "BOOLEAN_EQUAL";

//         case BOOLEAN_NOT_EQUAL:
//             return "BOOLEAN_NOT_EQUAL";

//         case BOOLEAN_TRUE:
//             return "BOOLEAN_TRUE";

//         case BOOLEAN_FALSE:
//             return "BOOLEAN_FALSE";

//     }
// }

// /**
//  * @brief Breadth-first search function
//  * @returns A string with the resulting BFS
//  */
// string LCRS::BFS() const {
//     /**
//      * @remark pair a LCRS node and its level in the tree 
//      */
//     queue<std::pair<const LCRS*, int>> q;
//     q.push({this, 0});
//     string result = "";
//     int currentLevel = -1;

//     while (!q.empty()) {
//         const LCRS* current = q.front().first;
//         int level = q.front().second;
//         q.pop();

//         if (level != currentLevel) {
//             if (currentLevel != -1) 
//                 result += '\n';
            
//             currentLevel = level;
//         }

//         result += toTokenCharacter(current->token) + ' ';

//         if (current->leftChild)
//             q.push({current->leftChild, level + 1});
//         if (current->rightSibling)
//             q.push({current->rightSibling, level});

//     }

//     return result += '\n';
// }

// /**
//  * @brief Returns the DFA state 
//  * @param token - The token to get the DFA state for
//  */
// State getStateDFA(Token token) {
//     if (token.type == IDENTIFIER) {
//         if (token.character == "function" || token.character == "procedure") 
//             return FUNCTION_DECLARATION;
        
//         if (token.character == "int" || token.character == "string" ||
//             token.character == "char" || token.character == "bool")
//             return VARIABLE_DECLARATION;

//         if (token.character == "if")
//             return CONDITIONAL;

//         if (token.character == "for" || token.character == "while")
//             return LOOP;

//         if (token.character == "=")
//             return VARIABLE_ASSIGNMENT;
//     }
        
//     return OTHER;
// }

// /**
//  * @brief Constructor
//  * @param tokens - A vector of tokens from a C-style program 
//  */
// RecursiveDescentParser::RecursiveDescentParser(const vector<Token>& tokens) {
//     /**
//      * used to keep track of left parenthesis in the cases of math/bool expression 
//      */
//     int leftParenCounter = 0;

//     LCRS* lcrs = tokens.size() > 0 ? new LCRS(tokens[0]) : nullptr;
//     LCRS* temp = lcrs;
//     State state = getStateDFA(tokens[0]);

//     for (int i = 1; i < tokens.size(); i++) {
//         switch (state) {
//             case OTHER:
//                 if (tokens[i - 1].type == SEMICOLON || 
//                     tokens[i - 1].type == RIGHT_BRACE ||
//                     tokens[i - 1].type == LEFT_BRACE ||
//                     tokens[i - 1].character == "else") {
//                     temp->leftChild = new LCRS(tokens[i]);
//                     temp = temp->leftChild;
//                     state = getStateDFA(tokens[i]);
//                 } else {
//                     temp->rightSibling = new LCRS(tokens[i]);
//                     temp = temp->rightSibling;
//                 }
//                 break;

//             case FUNCTION_DECLARATION:
//                 if (tokens[i - 1].type == RIGHT_PARENTHESIS) {
//                     temp->leftChild = new LCRS(tokens[i]);
//                     temp = temp->leftChild;
//                     state = getStateDFA(tokens[i]);
//                 } else {
//                     temp->rightSibling = new LCRS(tokens[i]);
//                     temp = temp->rightSibling;
//                 }
//                 break;

//             case VARIABLE_DECLARATION:
//                 if (tokens[i - 1].type == SEMICOLON) {
//                     temp->leftChild = new LCRS(tokens[i]);
//                     temp = temp->leftChild;
//                     state = getStateDFA(tokens[i]);
//                 } else {
//                     temp->rightSibling = new LCRS(tokens[i]);
//                     temp = temp->rightSibling;
//                 }
//                 break;

//             case VARIABLE_ASSIGNMENT:
//                 if (tokens[i - 1].type == SEMICOLON) {
//                     temp->leftChild = new LCRS(tokens[i]);
//                     temp = temp->leftChild;
//                     state = getStateDFA(tokens[i]);
//                 } else {
//                     temp->rightSibling = new LCRS(tokens[i]);
//                     temp = temp->rightSibling;
//                 }
//                 break;

//             case LOOP:
//                 if (tokens[i - 1].type == RIGHT_PARENTHESIS && leftParenCounter == 0) {
//                     temp->leftChild = new LCRS(tokens[i]);
//                     temp = temp->leftChild;
//                     state = getStateDFA(tokens[i]);
//                 } else {
//                     if (tokens[i].type == RIGHT_PARENTHESIS) 
//                         leftParenCounter--;

//                     if (tokens[i].type == LEFT_PARENTHESIS) 
//                         leftParenCounter++;
            
//                     temp->rightSibling = new LCRS(tokens[i]);
//                     temp = temp->rightSibling;
//                 }
//                 break;

//             case CONDITIONAL:
//                 if (tokens[i - 1].type == RIGHT_PARENTHESIS && leftParenCounter == 0) {
//                     temp->leftChild = new LCRS(tokens[i]);
//                     temp = temp->leftChild;
//                     state = getStateDFA(tokens[i]);
//                 } else {
//                     if (tokens[i].type == RIGHT_PARENTHESIS) 
//                         leftParenCounter--;

//                     if (tokens[i].type == LEFT_PARENTHESIS) 
//                         leftParenCounter++;
            
//                     temp->rightSibling = new LCRS(tokens[i]);
//                     temp = temp->rightSibling;
//                 }
//                 break;

//         }
//     }

//     concreteSyntaxTree = lcrs;
// }

// /**
//  * @brief Getter for CST 
//  */
// LCRS* RecursiveDescentParser::getConcreteSyntaxTree() {
//     return concreteSyntaxTree;
// }

// /**
//  * @brief Output operator overload
//  * @param os - The output stream operator
//  * @param obj - The RecursiveDescentParser object to output
//  * @returns The modified output stream
//  * @remark Outputs CST according to project spec
//  * 
//  *      ex: cout << RecursiveDescentParserObj;
//  */
// ostream& operator << (ostream& os, const RecursiveDescentParser& obj) {
//     os << obj.concreteSyntaxTree->BFS();
//     return os;
// }