#ifndef ABSTRACT_SYNTAX_TREE_HPP
#define ABSTRACT_SYNTAX_TREE_HPP

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include "Tokenization.hpp" // Include the header where Token is defined
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
    AbstractSyntaxTree(RecursiveDescentParser concreteSyntaxTree);

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

    bool isDeclarationKeyword(const string &tokenCharacter)
    {
        return (tokenCharacter == "function" || tokenCharacter == "procedure" ||
                tokenCharacter == "int" || tokenCharacter == "char" || tokenCharacter == "bool" || tokenCharacter == "If");
    }

    int precedence(const Token &token)
    {
        if (token.character == "+" || token.character == "-")
            return 1;
        if (token.character == "*" || token.character == "/" || token.character == "%")
            return 2;
        if (token.character == "^")
            return 3;

        if (token.character == "<=")
            return 4;

        if (token.character == ">=")
            return 5;

        if (token.character == "&&")
            return 6;

        return 0;
    }

    bool isLeftAssociative(const Token &token)
    {
        return token.character != "^";
    }

    vector<Token> infixToPostfix(const vector<Token> &infix)
    {
        vector<Token> postfix;
        stack<Token> operators;

        for (const Token &token : infix)
        {
            if (token.type == INTEGER)
            {
                postfix.push_back(token);
            }
            else if (token.type == IDENTIFIER)
            {
                postfix.push_back(token);
            }
            else if (token.type == PLUS || token.type == MINUS || token.type == DIVIDE ||
                     token.type == ASTERISK || token.type == MODULO || token.type == ASSIGNMENT || token.type == GT_EQUAL || token.type == LT_EQUAL)
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
        }

        while (!operators.empty())
        {
            postfix.push_back(operators.top());
            operators.pop();
        }

        return postfix;
    }
};

#endif /* ABSTRACT_SYNTAX_TREE_HPP */
