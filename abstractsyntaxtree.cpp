#include "abstractsyntaxtree.hpp"
#include <queue>
#include <string>
#include <sstream>
#include <cassert>

/**
 * @param token - A token object
 * @returns The actual character(s) of the token
 */
inline auto toTokenCharacter(Token token)
{
    return token.character;
}

/**
 * @brief Returns string version of token type
 * @param token - A Token object
 * @returns A string of the token type instead of 0, 1, 2, ...
 */
inline auto toTokenType(Token token)
{
    switch (token.type)
    {
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
AbstractSyntaxTree::AbstractSyntaxTree(const vector<Token> &tokens)
{
}

/**
 * @brief Constructs AbstractSyntaxTree object
 * @param concreteSyntaxTree - The concrete syntax tree that was generated in
 *                                                      RecursiveDescentParser
 */
AbstractSyntaxTree::AbstractSyntaxTree(RecursiveDescentParser concreteSyntaxTree, SymbolTable symbolTable)
{
    LCRS *cst = concreteSyntaxTree.getConcreteSyntaxTree();

    // cout << cst->token.character << "^^^^^\n";

    vector<vector<Token>> result; // 2D vector to hold the tokens by levels
    queue<pair<LCRS *, int>> q;   // Queue to hold nodes along with their level
    q.push({cst, 0});             // Start with the root at level 0

    while (!q.empty())
    {
        auto front = q.front(); // Get the front item (node and its level)
        q.pop();

        LCRS *currentNode = front.first;
        int level = front.second;

        // Ensure the vector is large enough to hold this level
        if (level >= result.size())
        {
            result.resize(level + 1); // Resize to accommodate new level
        }

        // Add the current node's token to the appropriate level
        result[level].push_back(currentNode->token);

        // Enqueue left child if exists, at the next level
        if (currentNode->leftChild)
        {
            q.push({currentNode->leftChild, level + 1});
        }

        // Enqueue right sibling if exists, at the same level
        if (currentNode->rightSibling)
        {
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

   //vector<Token>listOfProFuncs;

    
    for(int line = 0; line < result.size(); line++){
        findFunctionProcedureCall(result[line], symbolTable.table);
            //listOfProFuncs.push_back(result[line][1]);
            //cout << findFunctionProcedureCall(result[line], symbolTable.table) << endl;
            //cout << "Possible name: " << result[line][1].character << " on line " << line << endl;
        
    
        
    } 



    LCRS *ast = nullptr; // new LCRS(result[0]);
    LCRS *temp = ast;

    vector<vector<Token>> abstract;

    for (int i = 0; i < result.size(); i++)
    {
        vector<Token> k;
        bool isDeclaration = false;
        for (int j = 0; j < result[i].size(); j++)
        {
            Token token;
            //finds Procedure calls
            if (find(listOfProFuncs.begin(), listOfProFuncs.end(), result[i][0].character) != listOfProFuncs.end()){
                //cout << "Found " << result[i][0].character << endl;
                Token callToken;
                callToken.character = "Call";
                callToken.lineNumber = result[i][0].lineNumber;
                callToken.type = result[i][0].type;
                //result[i][0].character = "Call";
                //cout << "test1" << endl;
                vector<Token> postfix = infixToPostfix(result[i]);
                k.push_back(callToken);
                //cout << "test2" << endl;
                for (int index = 0; index < postfix.size(); index++){
                     k.push_back(postfix[index]);
                }
                //cout << "test3" << endl;
                int foundFunctionProcedureCall = findFunctionProcedureCall(k, symbolTable.table);
                // found function/procedure call in line
                if (foundFunctionProcedureCall != -1)
                {
                    /*
                    int closingParenIndex = 0;
                    for (int j = foundFunctionProcedureCall + 1; j < result[i].size() && result[i][j].character != ")"; j++) {
                        closingParenIndex++;
                        cout << result[i][j].character << " ###\n";
                    }
                    

                    cout << "_____ " << closingParenIndex << '\n';
                    */

                    int numberOfParams = findNumberOfParams(
                        result[i][foundFunctionProcedureCall].character, symbolTable.paramTable);
                    
                    //cout << "num params: " << numberOfParams << '\n';
                    
                    token.character = "(";
                    token.type = LEFT_PARENTHESIS;
                    k.insert(k.begin() + foundFunctionProcedureCall + 1, token);
                    token.character = ")";
                    token.type = RIGHT_PARENTHESIS;
                    k.insert(k.begin() + foundFunctionProcedureCall + 3 + numberOfParams, token);
                    
                }

                break;
            }

            if (isDeclarationKeyword(result[i][0].character))
            {
                int numDeclarations = 1;
                if (result[i][0].character == "int" ||
                    result[i][0].character == "char" ||
                    result[i][0].character == "bool")
                {
                    for (int j = 1; j < result[i].size(); j++)
                    {
                        if (result[i][j].character == ",")
                            numDeclarations++;
                    }
                }
                Token declarationToken;
                declarationToken.character = "Declaration";
                declarationToken.type = result[i][0].type;
                declarationToken.lineNumber = result[i][0].lineNumber;

                if (numDeclarations > 1)
                {
                    k.push_back(declarationToken);
                    abstract.push_back(k); // Push the declaration once

                    // Changed from numDeclarations to -1 to get rid of last 2 declarations
                    for (int j = 1; j < numDeclarations - 1; j++)
                    {
                        vector<Token> additionalDeclaration;
                        additionalDeclaration.push_back(declarationToken);
                        abstract.push_back(additionalDeclaration); // Push additional declarations
                    }
                }
                else
                {
                    k.push_back(declarationToken);
                    // abstract.push_back(k);
                }
                /*
                if (numDeclarations > 1)
                {
                    for (int j = 1; j < numDeclarations; j++)
                    {
                        k.push_back(token);
                        abstract.push_back(k);
                    }
                }
                else
                {
                    k.push_back(token);
                }*/

                break;
            }

            /*
            // Check if the current token is "if" and the next token is also "if"
            if (result[i][j].character == "If" && j + 1 < result[i].size() && result[i][j + 1].character == "if")
            {
                continue;
            }
            */
            


            // if statement
            if (result[i][0].character == "if")
            {
                // token.character = "IF";
                // token.type = result[i][0].type;
                // token.lineNumber = result[i][0].lineNumber;
                // k.push_back(token);

                vector<Token> postfix = infixToPostfix(result[i]);
                for (int r = 0; r < postfix.size(); r++)
                {
                    k.push_back(postfix[r]);
                }


                
                int foundFunctionProcedureCall = findFunctionProcedureCall(k, symbolTable.table);
                // found function/procedure call in line
                if (foundFunctionProcedureCall != -1)
                {
                    /*
                    int closingParenIndex = 0;
                    for (int j = foundFunctionProcedureCall + 1; j < result[i].size() && result[i][j].character != ")"; j++) {
                        closingParenIndex++;
                        cout << result[i][j].character << " ###\n";
                    }
                    

                    cout << "_____ " << closingParenIndex << '\n';
                    */

                    int numberOfParams = findNumberOfParams(
                        result[i][foundFunctionProcedureCall].character, symbolTable.paramTable);
                    
                    //cout << "num params: " << numberOfParams << '\n';
                    
                    token.character = "(";
                    token.type = LEFT_PARENTHESIS;
                    k.insert(k.begin() + foundFunctionProcedureCall + 1, token);
                    token.character = ")";
                    token.type = RIGHT_PARENTHESIS;
                    k.insert(k.begin() + foundFunctionProcedureCall + 3 + numberOfParams, token);
                    
                }

                break;
            }

            if (result[i][0].character == "for")
            {
                vector<Token> postfix;
                vector<Token> proxyVector;

                int count = 1;

                Token proxyToken;
                proxyToken.character = "For Expression " + to_string(count);
                proxyToken.type = result[i][0].type;
                proxyVector.push_back(proxyToken);

                Token proxyTokenTwo;
                proxyTokenTwo.character = "\n";
                proxyTokenTwo.type = result[i][0].type;
                
                result[i].push_back(proxyTokenTwo);                
                              
                for (int r = 2; r < result[i].size(); r++)
                {

                    proxyVector.push_back(result[i][r]);

                    if (result[i][r].character == ";" || result[i][r].character == "\n")
                    {
                        if(result[i][r].character == "\n"){
                            
                            proxyVector.pop_back();                          
                            proxyVector.pop_back();
                            
                        }
                        postfix = infixToPostfix(proxyVector);
                        
                        for (int o = 0; o < postfix.size(); o++)
                        {
                            
                            k.push_back(postfix[o]);
                            
                        }
                        
                        if(result[i][r].character != "\n"){
                        abstract.push_back(k);
                        
                    
                        k.clear();
                        proxyVector.clear();
                        count++;
                        proxyToken.character = "For Expression " + to_string(count);
                        proxyVector.push_back(proxyToken);
                        }
                    }
                }

                break;
            }

            // just placing this in to start while condition
            if (result[i][0].character == "while")
            {
                // token.character = "IF";
                // token.type = result[i][0].type;
                // token.lineNumber = result[i][0].lineNumber;
                // k.push_back(token);

                vector<Token> postfix = infixToPostfix(result[i]);
                for (int r = 0; r < postfix.size(); r++)
                {
                    k.push_back(postfix[r]);
                }

                break;
            }

            if (result[i][0].character == "{")
            {
                result[i][0].character = "Begin Block";
                k.push_back(result[i][0]);
                break;
            }

            if (result[i][0].character == "}")
            {
                result[i][0].character = "End Block";
                k.push_back(result[i][0]);
                break;
            }

            if (result[i][1].character == "=" || result[i][4].character == "=")
            {
                
                token.character = "Assignment";
                token.type = result[i][0].type;
                token.lineNumber = result[i][0].lineNumber;
                k.push_back(token);

                vector<Token> postfix = infixToPostfix(result[i]);
                for (int r = 0; r < postfix.size(); r++)
                {
                    k.push_back(postfix[r]);
                }

                int foundFunctionProcedureCall = findFunctionProcedureCall(k, symbolTable.table);
                // found function/procedure call in line
                if (foundFunctionProcedureCall != -1)
                {
                    int closingParenIndex = 0;
                    for (int j = foundFunctionProcedureCall + 2; j < result[i].size() && result[i][j].character != ")"; j++) {
                        closingParenIndex++;
                    }

                    /*
                    int numberOfParams = findNumberOfParams(
                        result[i][foundFunctionProcedureCall].character, symbolTable.paramTable);
                    */
                    // cout << "num params: " << numberOfParams << '\n';
                    token.character = "(";
                    token.type = LEFT_PARENTHESIS;
                    k.insert(k.begin() + foundFunctionProcedureCall + 1, token);
                    token.character = ")";
                    token.type = RIGHT_PARENTHESIS;
                    k.insert(k.begin() + foundFunctionProcedureCall + 2 + closingParenIndex, token);
                }

                break;
            }

            // print statement
            if (result[i][0].character == "printf")
            {
                vector<Token> postfix = infixToPostfix(result[i]);
                for (int r = 0; r < postfix.size(); r++)
                {
                    if (postfix[r].character != "\"")
                    { // Skip double quotes
                        k.push_back(postfix[r]);
                    }
                }
                break;
            }

            // new semicolon work
            if (result[i][result[i].size() - 1].character == ";")
            {
                /*
                token.character = "Semicolon";
                token.type = result[i][0].type;
                token.lineNumber = result[i][0].lineNumber;
                k.push_back(token);
                */

                vector<Token> postfix = infixToPostfix(result[i]);
                for (int r = 0; r < postfix.size(); r++)
                {
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
    
    for (auto i : abstract)
    {
        for (auto j : i)
        {
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
ostream &operator<<(ostream &os, const AbstractSyntaxTree &obj)
{
    os << obj.abstractSyntaxTree->BFS();
    return os;
}
