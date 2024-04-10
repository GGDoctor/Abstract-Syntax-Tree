/**
 * @file RecursiveDescentParser.cpp
 * @brief Implementation file for the RecursiveDescentParser class
 * @authors Jacob Franco, Zach Gassner, Haley Joerger, Adam Lyday
 */

#include "RecursiveDescentParser.hpp"
#include <queue>

/**
 * @param token - A token object
 * @returns The actual character(s) of the token
 */
auto toTokenCharacter(Token token) {
    return token.character;
}

/**
 * @brief Returns string version of token type
 * @param token - A Token object
 * @returns A string of the token type instead of 0, 1, 2, ...
 */
auto toTokenType(Token token) {
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
 * @brief Breadth-first search function
 * @returns A string with the resulting BFS
 */
string LCRS::BFS() const {
    /**
     * @remark pair a LCRS node and its level in the tree 
     */
    queue<std::pair<const LCRS*, int>> q;
    q.push({this, 0});
    string result = "";
    int currentLevel = -1;

    while (!q.empty()) {
        const LCRS* current = q.front().first;
        int level = q.front().second;
        q.pop();

        if (level != currentLevel) {
            if (currentLevel != -1) 
                result += '\n';
            
            currentLevel = level;
        }

        result += toTokenCharacter(current->token) + ' ';

        if (current->leftChild)
            q.push({current->leftChild, level + 1});
        if (current->rightSibling)
            q.push({current->rightSibling, level});

    }

    return result += '\n';
}

/**
 * @brief Returns the DFA state 
 * @param token - The token to get the DFA state for
 */
State getStateDFA(Token token) {
    if (token.type == IDENTIFIER) {
        if (token.character == "function" || token.character == "procedure") 
            return FUNCTION_DECLARATION;
        
        if (token.character == "int" || token.character == "string" ||
            token.character == "char" || token.character == "bool")
            return VARIABLE_DECLARATION;

        if (token.character == "if")
            return CONDITIONAL;

        if (token.character == "for" || token.character == "while")
            return LOOP;

        if (token.character == "=")
            return VARIABLE_ASSIGNMENT;
    }
        
    return OTHER;
}

/**
 * @brief Constructor
 * @param tokens - A vector of tokens from a C-style program 
 */
RecursiveDescentParser::RecursiveDescentParser(const vector<Token>& tokens) {
    /**
     * used to keep track of left parenthesis in the cases of math/bool expression 
     */
    int leftParenCounter = 0;

    LCRS* lcrs = tokens.size() > 0 ? new LCRS(tokens[0]) : nullptr;
    LCRS* temp = lcrs;
    State state = getStateDFA(tokens[0]);

    for (int i = 1; i < tokens.size(); i++) {
        switch (state) {
            case OTHER:
                if (tokens[i - 1].type == SEMICOLON || 
                    tokens[i - 1].type == RIGHT_BRACE ||
                    tokens[i - 1].type == LEFT_BRACE ||
                    tokens[i - 1].character == "else") {
                    temp->leftChild = new LCRS(tokens[i]);
                    temp = temp->leftChild;
                    state = getStateDFA(tokens[i]);
                } else {
                    temp->rightSibling = new LCRS(tokens[i]);
                    temp = temp->rightSibling;
                }
                break;

            case FUNCTION_DECLARATION:
                if (tokens[i - 1].type == RIGHT_PARENTHESIS) {
                    temp->leftChild = new LCRS(tokens[i]);
                    temp = temp->leftChild;
                    state = getStateDFA(tokens[i]);
                } else {
                    temp->rightSibling = new LCRS(tokens[i]);
                    temp = temp->rightSibling;
                }
                break;

            case VARIABLE_DECLARATION:
                if (tokens[i - 1].type == SEMICOLON) {
                    temp->leftChild = new LCRS(tokens[i]);
                    temp = temp->leftChild;
                    state = getStateDFA(tokens[i]);
                } else {
                    temp->rightSibling = new LCRS(tokens[i]);
                    temp = temp->rightSibling;
                }
                break;

            case VARIABLE_ASSIGNMENT:
                if (tokens[i - 1].type == SEMICOLON) {
                    temp->leftChild = new LCRS(tokens[i]);
                    temp = temp->leftChild;
                    state = getStateDFA(tokens[i]);
                } else {
                    temp->rightSibling = new LCRS(tokens[i]);
                    temp = temp->rightSibling;
                }
                break;

            case LOOP:
                if (tokens[i - 1].type == RIGHT_PARENTHESIS && leftParenCounter == 0) {
                    temp->leftChild = new LCRS(tokens[i]);
                    temp = temp->leftChild;
                    state = getStateDFA(tokens[i]);
                } else {
                    if (tokens[i].type == RIGHT_PARENTHESIS) 
                        leftParenCounter--;

                    if (tokens[i].type == LEFT_PARENTHESIS) 
                        leftParenCounter++;
            
                    temp->rightSibling = new LCRS(tokens[i]);
                    temp = temp->rightSibling;
                }
                break;

            case CONDITIONAL:
                if (tokens[i - 1].type == RIGHT_PARENTHESIS && leftParenCounter == 0) {
                    temp->leftChild = new LCRS(tokens[i]);
                    temp = temp->leftChild;
                    state = getStateDFA(tokens[i]);
                } else {
                    if (tokens[i].type == RIGHT_PARENTHESIS) 
                        leftParenCounter--;

                    if (tokens[i].type == LEFT_PARENTHESIS) 
                        leftParenCounter++;
            
                    temp->rightSibling = new LCRS(tokens[i]);
                    temp = temp->rightSibling;
                }
                break;

        }
    }

    concreteSyntaxTree = lcrs;
}

/**
 * @brief Getter for CST 
 */
LCRS* RecursiveDescentParser::getConcreteSyntaxTree() {
    return concreteSyntaxTree;
}

/**
 * @brief Output operator overload
 * @param os - The output stream operator
 * @param obj - The RecursiveDescentParser object to output
 * @returns The modified output stream
 * @remark Outputs CST according to project spec
 * 
 *      ex: cout << RecursiveDescentParserObj;
 */
ostream& operator << (ostream& os, const RecursiveDescentParser& obj) {
    os << obj.concreteSyntaxTree->BFS();
    return os;
}