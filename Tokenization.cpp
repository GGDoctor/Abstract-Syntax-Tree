/**
 * @file Tokenization.cpp
 * @brief Implementation file for the Tokenization class
 * @authors Jacob Franco, Zach Gassner, Haley Joerger, Adam Lyday
 */

#include "Tokenization.hpp"

/**
 * @brief Constructor
 * @param input - A string representing a C-style program 
 * @remark Constructs Tokenization object by tokenizing the input program and
 *          storing tokens in private variable
 */
Tokenization::Tokenization(const string& input) {
    string currentToken; //Read char by char
    bool inComment = false;
    bool inString = false;
    int lineNumber = 1;

    for (size_t i = 0; i < input.size(); ++i) {

        if (inString) {
            Token stringToken;
            stringToken.type = STRING;
            stringToken.lineNumber = lineNumber;

            while (input[i] != tokens.back().character[0]) {
                stringToken.character += input[i++];
            }

            tokens.push_back(stringToken);    
        }

        char nextChar = i + 1 < input.size() ? input[i + 1] : 0; 
        Token inputToken;

        switch(input[i]) {
            case ' ':
                break;

            case '\n':
                lineNumber++;
                break;

            case '(':
                inputToken.type = LEFT_PARENTHESIS;
                inputToken.character = "(";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;
                
            case ')':
                inputToken.type = RIGHT_PARENTHESIS;
                inputToken.character = ")";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '[':
                inputToken.type = LEFT_BRACKET;
                inputToken.character = "[";
                inputToken.lineNumber = lineNumber;

                // Check if the next character is a '-' to indicate a negative array size
                if (input[i + 1] == '-') {
                    std::cerr << "Syntax error on line " << lineNumber << ": array declaration size must be a positive integer.\n";
                    exit(EXIT_FAILURE); // Or handle the error as needed
                }

                tokens.push_back(inputToken);
                break;

            case ']':
                inputToken.type = RIGHT_BRACKET;
                inputToken.character = "]";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '{':
                inputToken.type = LEFT_BRACE;
                inputToken.character = "{";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '}':
                inputToken.type = RIGHT_BRACE;
                inputToken.character = "}";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '"':
                inputToken.type = DOUBLE_QUOTE;
                inputToken.character = "\"";
                inputToken.lineNumber = lineNumber;

                if(input[i+1]=='\n' || input[i-1] == '\\'){
                    std::cerr << "Syntax error on line " << lineNumber << ": unterminated string quote." << endl;
                    exit(EXIT_FAILURE);
                }

                tokens.push_back(inputToken);
                inString = !inString;
                break;

            case '\'':
                inputToken.type = SINGLE_QUOTE;
                inputToken.character = "\'";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                inString = !inString;
                break;

            case ';':
                inputToken.type = SEMICOLON;
                inputToken.character = ";";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;
            
            case ',':
                inputToken.type = COMMA;
                inputToken.character = ",";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '=':
                if (nextChar == '=') {
                    inputToken.type = BOOLEAN_EQUAL;
                    inputToken.character = "==";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                    i++;
                } else {
                    inputToken.type = ASSIGNMENT;
                    inputToken.character = "=";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                }
                
                break;

            case '+':
                inputToken.type = PLUS;
                inputToken.character = "+";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '-':
                if (isdigit(nextChar)) {
                    i++;
                    inputToken.type = INTEGER;
                    inputToken.character = '-';
                    inputToken.lineNumber = lineNumber;
                    
                    while (isdigit(input[i])) {
                        inputToken.character += input[i++];
                    }

                    i--;
                    tokens.push_back(inputToken);
                } else {
                    inputToken.type = MINUS;
                    inputToken.character = "-";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                }
            
                break;

            case '/':
                inputToken.type = DIVIDE;
                inputToken.character = "/";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '*':
                inputToken.type = ASTERISK;
                inputToken.character = "*";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '%':
                inputToken.type = MODULO;
                inputToken.character = "%";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;

            case '^':
                inputToken.type = CARAT;
                inputToken.character = "^";
                inputToken.lineNumber = lineNumber;
                tokens.push_back(inputToken);
                break;
            
            case '<':
                if (nextChar == '=') {
                    inputToken.type = LT_EQUAL;
                    inputToken.character = "<=";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                    i++;
                } else {
                    inputToken.type = LT;
                    inputToken.character = "<";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                }
                
                break;
            
            case '>':
                if (nextChar == '=') {
                    inputToken.type = GT_EQUAL;
                    inputToken.character = ">=";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                    i++;
                } else {
                    inputToken.type = GT;
                    inputToken.character = ">";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                }

                break;

            case '&':
                if (nextChar == '&') {
                    inputToken.type = BOOLEAN_AND_OPERATOR;
                    inputToken.character = "&&";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                    i++;
                }
                
                break;

            case '|':
                if (nextChar == '|') {
                    inputToken.type = BOOLEAN_OR_OPERATOR;
                    inputToken.character = "||";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                    i++;
                }
                
                break;
            
            case '!':
                if (nextChar == '=') {
                    inputToken.type = BOOLEAN_NOT_EQUAL;
                    inputToken.character = "!=";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                    i++;
                } else {
                    inputToken.type = BOOLEAN_NOT_OPERATOR;
                    inputToken.character = "!";
                    inputToken.lineNumber = lineNumber;
                    tokens.push_back(inputToken);
                }

                break;

        default:
            if (isdigit(input[i])) {
                while (!isspace(input[i]) && !(find( listOfSymbols.begin(), 
                listOfSymbols.end(), input[i]) != listOfSymbols.end())) {
                    if (!isdigit(input[i])) {
                        cout << "Syntax error on line " << lineNumber << ": invalid integer\n";
                        exit(0);
                    }

                    inputToken.character += input[i++];
                }
                inputToken.type = INTEGER;             
            } else {
                while (!isspace(input[i]) && !(find( listOfSymbols.begin(), 
                listOfSymbols.end(), input[i]) != listOfSymbols.end())) {
                    inputToken.character += input[i++];
                }

                if (inputToken.character == "int" || 
                    inputToken.character == "char" ||
                    inputToken.character == "bool" ||
                    inputToken.character == "string") {
                    
                    int index = i;
                    string nextTokenCharacter = "";

                    // ignore spaces
                    while (index < input.size() && isspace(input[index])) {
                        index++;
                    }

                    // getting next token characters
                    while (index < input.size() && !isspace(input[index])
                            && input[index] != ';' && input[index] != ')') {
                        nextTokenCharacter += input[index++];
                    }

                    if (nextTokenCharacter == "int" ||
                        nextTokenCharacter == "char" ||
                        nextTokenCharacter == "bool" ||
                        nextTokenCharacter == "string" ||
                        nextTokenCharacter == "void") {
                        
                        cerr << "Syntax error on line " << lineNumber
                             << ": reserved word \"" << nextTokenCharacter
                             << "\" cannot be used for the name of a "
                             << "variable.\n";
                        exit(0);
                    }

                    if (nextTokenCharacter == "printf") {
                        cerr << "Syntax error on line " << lineNumber
                             << ": reserved word \"printf\" cannot be used "
                             << "for the name of a function.\n";
                        exit(0);
                    }
                }
            
                inputToken.type = IDENTIFIER;
            }

            i--;
            inputToken.lineNumber = lineNumber;
            tokens.push_back(inputToken);


            break;
        }
    }
}
    

/**
 * @brief Output operator overload
 * @param os - The output stream operator
 * @param obj - The Tokenization object to output
 * @returns The modified output stream
 * @remark Outputs tokens according to Project 2 spec
 * 
 *      ex: cout << TokenizationObj;
 */
ostream& operator << (ostream& os, const Tokenization& obj) {
    os << "Token list: " << '\n' << '\n';

    for (const auto& token: obj.tokens) {
        string tokenType = "Token type: ";
        string tokenString = "Token: ";

        switch (token.type) {
            case VOID:
                tokenType += "VOID";
                break;

            case CHAR: 
                tokenType += "CHAR";
                break;

            case IDENTIFIER:
                tokenType += "IDENTIFIER";
                break;

            case INTEGER:
                tokenType += "INTEGER";
                break;

            case STRING:
                tokenType += "STRING";
                break;

            case LEFT_PARENTHESIS:
                tokenType += "LEFT_PARENTHESIS";
                break;

            case RIGHT_PARENTHESIS:
                tokenType += "RIGHT_PARENTHESIS";
                break;

            case LEFT_BRACKET:
                tokenType += "LEFT_BRACKET";
                break;

            case RIGHT_BRACKET:
                tokenType += "RIGHT_BRACKET";
                break;

            case LEFT_BRACE:
                tokenType += "LEFT_BRACE";
                break;

            case RIGHT_BRACE:
                tokenType += "RIGHT_BRACE";
                break;

            case DOUBLE_QUOTE:
                tokenType += "DOUBLE_QUOTE";
                break;

            case SINGLE_QUOTE:
                tokenType += "SINGLE_QUOTE";
                break;

            case SEMICOLON:
                tokenType += "SEMICOLON";
                break;

            case COMMA:
                tokenType += "COMMA";
                break;

            case ASSIGNMENT:
                tokenType += "ASSIGNMENT";
                break;

            case PLUS:
                tokenType += "PLUS";
                break;

            case MINUS:
                tokenType += "MINUS";
                break;

            case DIVIDE:
                tokenType += "DIVIDE";
                break;

            case ASTERISK:
                tokenType += "ASTERISK";
                break;

            case MODULO:
                tokenType += "MODULO";
                break;

            case CARAT:
                tokenType += "CARAT";
                break;

            case LT:
                tokenType += "LT";
                break;

            case GT:
                tokenType += "GT";
                break;

            case LT_EQUAL:
                tokenType += "LT_EQUAL";
                break;

            case GT_EQUAL:
                tokenType += "GT_EQUAL";
                break;
        
            case BOOLEAN_AND_OPERATOR:
                tokenType += "BOOLEAN_AND_OPERATOR";
                break;

            case BOOLEAN_OR_OPERATOR:
                tokenType += "BOOLEAN_OR_OPERATOR";
                break;

            case BOOLEAN_NOT_OPERATOR:
                tokenType += "BOOLEAN_NOT_OPERATOR";
                break;

            case BOOLEAN_EQUAL:
                tokenType += "BOOLEAN_EQUAL";
                break;

            case BOOLEAN_NOT_EQUAL:
                tokenType += "BOOLEAN_NOT_EQUAL";
                break;

            case BOOLEAN_TRUE:
                tokenType += "BOOLEAN_TRUE";
                break;

            case BOOLEAN_FALSE:
                tokenType += "BOOLEAN_FALSE";
                break;

        }

        tokenString += token.character;
        os << tokenType << '\n' << tokenString << '\n' << '\n';
    }

    return os;
}

/**
 * @brief Getter for tokens private variable
 * @returns The tokens vector
 */
vector<Token> Tokenization::getTokens() {
    return tokens;
}