#include "abstractsyntaxtree.hpp"
#include <queue>
#include <string>
#include <sstream>
#include <cassert>

/**
 * @param token - A token object
 * @returns The actual character(s) of the token
 */
inline auto toTokenCharacter(Token token) {
    return token.character;
}

/**
 * @brief Returns string version of token type
 * @param token - A Token object
 * @returns A string of the token type instead of 0, 1, 2, ...
 */
inline auto toTokenType(Token token) {
    switch (token.type) {
        case CHAR:
            return "CHAR";

        case VOID:
            return "VOID";

        case IDENTIFIER:
            return "IDENTIFIER";

        case INTEGER:
            return "INTEGER";

        case STRING:
            return "STRING";

        case LEFT_PARENTHESIS:
            return "LEFT_PARENTHESIS";

        case RIGHT_PARENTHESIS:
            return "RIGHT_PARENTHESIS";

        case LEFT_BRACKET:
            return "LEFT_BRACKET";

        case RIGHT_BRACKET:
            return "RIGHT_BRACKET";

        case LEFT_BRACE:
            return "LEFT_BRACE";

        case RIGHT_BRACE:
            return "RIGHT_BRACE";

        case DOUBLE_QUOTE:
            return "DOUBLE_QUOTE";

        case SINGLE_QUOTE:
            return "SINGLE_QUOTE";

        case SEMICOLON:
            return "SEMICOLON";

        case COMMA:
            return "COMMA";

        case ASSIGNMENT:
            return "ASSIGNMENT";

        case PLUS:
            return "PLUS";

        case MINUS:
            return "MINUS";

        case DIVIDE:
            return "DIVIDE";

        case ASTERISK:
            return "ASTERISK";

        case MODULO:
            return "MODULO";

        case CARAT:
            return "CARAT";

        case LT:
            return "LT";

        case GT:
            return "GT";

        case LT_EQUAL:
            return "LT_EQUAL";

        case GT_EQUAL:
            return "GT_EQUAL";
    
        case BOOLEAN_AND_OPERATOR:
            return "BOOLEAN_AND_OPERATOR";

        case BOOLEAN_OR_OPERATOR:
            return "BOOLEAN_OR_OPERATOR";

        case BOOLEAN_NOT_OPERATOR:
            return "BOOLEAN_NOT_OPERATOR";

        case BOOLEAN_EQUAL:
            return "BOOLEAN_EQUAL";

        case BOOLEAN_NOT_EQUAL:
            return "BOOLEAN_NOT_EQUAL";

        case BOOLEAN_TRUE:
            return "BOOLEAN_TRUE";

        case BOOLEAN_FALSE:
            return "BOOLEAN_FALSE";

    }
}

/**
 * @brief Constructor
 * @param tokens - A vector of tokens from a C-style program 
 */
AbstractSyntaxTree::AbstractSyntaxTree(const vector<Token>& tokens) {
    
}

/**
 * @brief Constructs AbstractSyntaxTree object
 * @param concreteSyntaxTree - The concrete syntax tree that was generated in
 *                                                      RecursiveDescentParser
 */
AbstractSyntaxTree::AbstractSyntaxTree(RecursiveDescentParser concreteSyntaxTree) {
    LCRS* cst = concreteSyntaxTree.getConcreteSyntaxTree();

    //cout << cst->token.character << "^^^^^\n";

    vector<vector<Token>> result;  // 2D vector to hold the tokens by levels
    queue<pair<LCRS*, int>> q;     // Queue to hold nodes along with their level
    q.push({cst, 0});             // Start with the root at level 0

    while (!q.empty()) {
        auto front = q.front();  // Get the front item (node and its level)
        q.pop();

        LCRS* currentNode = front.first;
        int level = front.second;

        // Ensure the vector is large enough to hold this level
        if (level >= result.size()) {
            result.resize(level + 1);  // Resize to accommodate new level
        }

        // Add the current node's token to the appropriate level
        result[level].push_back(currentNode->token);

        // Enqueue left child if exists, at the next level
        if (currentNode->leftChild) {
            q.push({currentNode->leftChild, level + 1});
        }

        // Enqueue right sibling if exists, at the same level
        if (currentNode->rightSibling) {
            q.push({currentNode->rightSibling, level});
        }
    }

    /*
    for (auto & x : result) {
        for (auto k : x) {
            cout << k.character << ' ';
        }

        cout << '\n';
    }
    */

    LCRS* ast = nullptr;//new LCRS(result[0]);
    LCRS* temp = ast;

    /*
    for (int i = 0; i < result.size(); i++) {
        AbstractStateDFA state = isDeclarationKeyword(result[i][0].character);
        Token token;

        switch (state) {
            case ABSTRACT_DECLARATION:
                token.character = "declaration";
                token.type = result[i][0].type;
                token.lineNumber = result[i][0].lineNumber;
                temp->leftChild = new LCRS(token);
                temp = temp->leftChild;
                break;

            case ABSTRACT_OTHER_STATE:
                for (int j = 0; j < result[i].size(); j++) {
                    temp->rightSibling = new LCRS(result[i][j]);
                    temp = temp->rightSibling;
                }

                temp = temp->leftChild;
                break;
        }

        for (int j = 0; j < result[i].size(); j++) {
            
        }
    }
    */

    vector<vector<Token>> abstract;

    for (int i = 0; i < result.size(); i++) {
        vector<Token> k;
        for (int j = 0; j < result[i].size(); j++) {
            Token token;
            if (isDeclarationKeyword(result[i][0].character)) {
                token.character = "Declaration";
                token.type = result[i][0].type;
                token.lineNumber = result[i][0].lineNumber;
                k.push_back(token);
                break;
            }

            if (result[i][0].character == "{") {
                result[i][0].character = "Begin Block";
                k.push_back(result[i][0]);
                break;
            }

            if (result[i][0].character == "}") {
                result[i][0].character = "End Block";
                k.push_back(result[i][0]);
                break;
            }

            if (result[i][1].character == "=") {
                token.character = "Assignment";
                token.type = result[i][0].type;
                token.lineNumber = result[i][0].lineNumber;
                k.push_back(token);

                vector<Token> postfix = infixToPostfix(result[i]);
                for (int r = 0; r < postfix.size(); r++) {
                    k.push_back(postfix[r]);
                }
                break;
            }

            token.character = result[i][j].character;
            token.type = result[i][j].type;
            token.lineNumber = result[i][j].lineNumber;
            k.push_back(token);
        }
        abstract.push_back(k);
    }

    for (auto i: abstract) {
        for (auto j : i) {
            cout << j.character << ' ';
        }
        cout << '\n';
    }
}

/**
 * @brief Output operator overload
 * @param os - The output stream operator
 * @param obj - The AbstractSyntaxTree object to output
 * @returns The modified output stream
 * @remark Outputs symbol table according to project spec
 *
 *      ex: cout << AbstractSyntaxTreeObj;
 */
ostream &operator<<(ostream &os, const AbstractSyntaxTree &obj) {
    os << obj.abstractSyntaxTree->BFS();
    return os;
}

/*
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

    
    for (int i = 0; i < linesAndWords.size(); i++) {
        for (int j = 0; j < linesAndWords[i].size(); j++) {
            cout << linesAndWords[i][j] << ' ';
        }
        cout << '\n';
    }
    */

/*
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

*/
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
}
    */

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