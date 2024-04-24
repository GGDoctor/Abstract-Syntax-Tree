#ifndef ABSTRACT_SYNTAX_TREE_HPP
#define ABSTRACT_SYNTAX_TREE_HPP

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include "Tokenization.hpp" // Include the header where Token is defined
#include "symboltable.hpp"
#include "RecursiveDescentParser.hpp"

using namespace std;

enum AbstractStateDFA
{
    ABSTRACT_OTHER_STATE,
    ABSTRACT_DECLARATION,
    ABSTRACT_BEGIN_BLOCK,
    ABSTRACT_END_BLOCK
};

/**
 * @class AbstractSyntaxTree
 */
class AbstractSyntaxTree
{
public:
    /**
     * @brief Constructs SymbolTable object
     * @param concreteSyntaxTree - The concrete syntax tree that was generated in
     *                                                      RecursiveDescentParser
     */
    AbstractSyntaxTree(RecursiveDescentParser concreteSyntaxTree, SymbolTable symbolTable);

    /**
     * @brief Constructor
     * @param tokens - A vector of tokens from a C-style program
     */
    AbstractSyntaxTree(const vector<Token> &tokens);

    /**
     * @brief Output operator overload
     * @param os - The output stream operator
     * @param obj - The AbstractSyntaxTree object to output
     * @returns The modified output stream
     * @remark Outputs ast according to project spec
     *
     *      ex: cout << AbstractSyntaxTreeObj;
     */
    friend ostream &operator<<(ostream &os, const AbstractSyntaxTree &obj);

private:
    LCRS *abstractSyntaxTree;

    vector<string>listOfProFuncs;


    bool isDeclarationKeyword(const string &tokenCharacter)
    {
        return (tokenCharacter == "function" || tokenCharacter == "procedure" ||
                tokenCharacter == "int" || tokenCharacter == "char" || tokenCharacter == "bool" );
    }

    int precedence(const Token &token)
    {
        if (token.character == "+" || token.character == "-")
            return 1;
        if (token.character == "*" || token.character == "/" || token.character == "%")
            return 2;
        if (token.character == "^")
            return 3;
        if (token.character == "==" || token.character == "!=" ||
            token.character == "<=" || token.character == ">=")
            return 4;
        if (token.character == "&&")
            return 5;
        if (token.character == "||")
            return 6;

        return 0;
    }

    bool isLeftAssociative(const Token &token)
    {
        return !(token.character == "^" || token.character == "&&");
    }

    vector<Token> infixToPostfix(const vector<Token> &infix)
    {
        vector<Token> postfix;
        stack<Token> operators;
        stack<char> bracketOrder;

        for (const Token &token : infix)
        {
            if (token.type == INTEGER || token.type == IDENTIFIER ||
                token.type == STRING || token.type == SINGLE_QUOTE ||
                token.type == LEFT_BRACKET || token.type == RIGHT_BRACKET || token.type == DOUBLE_QUOTE)
            {
                postfix.push_back(token);
            }
            else if (token.type == PLUS || token.type == MINUS || token.type == DIVIDE ||
                     token.type == ASTERISK || token.type == MODULO || token.type == ASSIGNMENT || token.type == GT_EQUAL || token.type == LT_EQUAL ||
                     token.type == BOOLEAN_AND_OPERATOR || token.type == BOOLEAN_EQUAL ||
                     token.type == BOOLEAN_OR_OPERATOR)
            {
                while (!operators.empty() &&
                       precedence(token) <= precedence(operators.top()) &&
                       isLeftAssociative(token))
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }
            else if (token.character == ">") // Handle '>' operator
            {
                while (!operators.empty() && operators.top().character != "(" &&
                       precedence(token) <= precedence(operators.top()) &&
                       isLeftAssociative(token))
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }
            else if (token.character == "<") // Handle '<' operator
            {
                while (!operators.empty() && operators.top().character != "(" &&
                       precedence(token) <= precedence(operators.top()) &&
                       isLeftAssociative(token))
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }

            else if (token.character == "!") // Handle '>' operator
            {
                while (!operators.empty() && operators.top().character != "(" &&
                       precedence(token) <= precedence(operators.top()) &&
                       isLeftAssociative(token))
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }

            else if (token.character == "(")
            {
                operators.push(token);
            }
            else if (token.character == ")")
            {
                while (!operators.empty() && operators.top().character != "(")
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.pop(); // Pop the '('
            }

            else if (token.character == "[")
            {
                while (!operators.empty() && operators.top().character != "[")
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.pop(); // Pop the '['
            }
             else if (token.character == "]")
            {
                while (!operators.empty() && operators.top().character != "]")
                {
                    postfix.push_back(operators.top());
                    operators.pop();
                }
                operators.pop(); // Pop the ']'
            }
            
        }


        while (!operators.empty())
        {
            postfix.push_back(operators.top());
            operators.pop();
        }

        return postfix;
    }

    // finds if a line contains a function/procedure call from symbol table
    // returns -1 if no function/procedure call found in line, otherwise
    // returns the index of function/procedure call was
    int findFunctionProcedureCall(const vector<Token> &line, const list<TableEntry> &symbolTable)
    {
        for (int i = 0; i < line.size(); i++)
        {
            for (const auto &symbol : symbolTable)
            {
                if (symbol.identifierName == line[i].character &&
                    (symbol.identifierType == "function" || symbol.identifierType == "procedure"))
                {
                     //creates a list of function and procedure names
                     if(!count(listOfProFuncs.begin(), listOfProFuncs.end(), line[i].character)){
                        listOfProFuncs.push_back(line[i].character);
                     }
                     //cout << "Found: " << line[i].character << " at index " << i << endl;
                     
                    return i; // return index of function/procedure call
                }
            }
        }
        // for(int i = 0; i < listOfProFuncs.size(); i++){
        //     cout << listOfProFuncs[i] << endl;
        // }
        return -1;
    }

    int findNumberOfParams(const string &functionName, const list<ParamListEntry> &paramTable)
    {
        int numberOfParams = 0;
        for (const auto &param : paramTable)
        {
            if (functionName == param.paramListName)
            {
                numberOfParams++;
            }
        }
        return numberOfParams;
    }
};

#endif /* ABSTRACT_SYNTAX_TREE_HPP */