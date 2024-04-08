#include "symboltable.hpp"
#include <string>
#include <sstream>
#include <cassert>

/**
 * @brief Constructs SymbolTable object
 * @param concreteSyntaxTree - The concrete syntax tree that was generated in
 *                                                      RecursiveDescentParser
 */
SymbolTable::SymbolTable(RecursiveDescentParser concreteSyntaxTree) {
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
        TableEntry tableEntry;
        StateDFA state = linesAndWords[i][0] == "function" ? FUNCTION : 
                         linesAndWords[i][0] == "procedure"  ? PROCEDURE : 
                        (linesAndWords[i][0] == "int" || 
                         linesAndWords[i][0] == "char" ||
                         linesAndWords[i][0] == "bool") ? 
                         VARIABLE : OTHER_STATE;

        switch (state) {
            case FUNCTION:
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
            case PROCEDURE:
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
            case VARIABLE:
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
            case OTHER_STATE:
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
ostream &operator<<(ostream &os, const SymbolTable &obj) {
    ParamListEntry previous;

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

void SymbolTable::parseParams(const vector<string> &params, int scope, 
                                            const string &paramListName) {
    if (params[0] == "void")
        return;
    ParamListEntry paramListEntry;
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
