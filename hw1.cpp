#include "tokens.hpp"
#include <iostream>
#include <unordered_map>
#include <string.h>
#include <sstream>

using std::cout;
using std::endl;
using std::string;


const char *getTokenName(int value)
{
    switch (value)
    {
#define TOKEN_NAME_CASE(x) \
    case x:                \
        return #x;
        TOKEN_NAME_CASE(INT)
        TOKEN_NAME_CASE(BYTE)
        TOKEN_NAME_CASE(B)
        TOKEN_NAME_CASE(BOOL)
        TOKEN_NAME_CASE(AND)
        TOKEN_NAME_CASE(OR)
        TOKEN_NAME_CASE(NOT)
        TOKEN_NAME_CASE(TRUE)
        TOKEN_NAME_CASE(FALSE)
        TOKEN_NAME_CASE(RETURN)
        TOKEN_NAME_CASE(IF)
        TOKEN_NAME_CASE(ELSE)
        TOKEN_NAME_CASE(WHILE)
        TOKEN_NAME_CASE(BREAK)
        TOKEN_NAME_CASE(CONTINUE)
        TOKEN_NAME_CASE(SC)
        TOKEN_NAME_CASE(LPAREN)
        TOKEN_NAME_CASE(RPAREN)
        TOKEN_NAME_CASE(LBRACE)
        TOKEN_NAME_CASE(RBRACE)
        TOKEN_NAME_CASE(ASSIGN)
        TOKEN_NAME_CASE(RELOP)
        TOKEN_NAME_CASE(BINOP)
        TOKEN_NAME_CASE(COMMENT)
        TOKEN_NAME_CASE(ID)
        TOKEN_NAME_CASE(NUM)
        TOKEN_NAME_CASE(STRING)
#undef TOKEN_NAME_CASE
    default:
        return "UNKNOWN_TOKEN";
    }
}

bool isValidPrintableAscii(char char1, char char2)
{
    if ( (char1 == '0' && char2 == '9') || (char1 == '0' && char2 == 'a') || (char1 == '0' && char2 == 'A') || (char1 == '0' && char2 == 'd') || (char1 == '0' && char2 == 'D'))
    {
        return true;
    }
    else if ( (char1 == '7' && char2 == 'f') || (char1 == '7' && char2 == 'F'))
    {
        return false;
    }
    else if (char1 >= '2' && char1 <= '7')
    {
        if (char2 >= '0' && char2 <= '9')
            return true;
        if (char2 >= 'A' && char2 <= 'F')
            return true;
        if (char2 >= 'a' && char2 <= 'f')
            return true;
        return false;
    }
    return false;
}

bool isValidPrintableCharacter(int c)
{
    return (c >= 0x20 && c <= 0x7E) || (c==0x09) || (c==0x0a) || (c==0x0d);
}

char hexPairToChar(char hex1, char hex2)
{
    std::string hexStr(1, hex1);
    hexStr += hex2;

    int decimalValue;
    std::istringstream iss(hexStr);
    iss >> std::hex >> decimalValue;

    char resultChar = static_cast<char>(decimalValue);

    return resultChar;
}

void handlehex(size_t *index_ptr, string &text, std::stringstream &stream, bool continuePrinting)
{
    size_t index = (*index_ptr) + 2; // skip "\x"
    if (index < text.size())
    {
        char first_ch = text[index];
        if ((index + 1) < text.size())
        {
            char second_ch = text[index + 1];
            bool isprintable = isValidPrintableAscii(first_ch, second_ch);
            if (isprintable)
            {
                if (continuePrinting)
                {
                    stream << hexPairToChar(first_ch, second_ch);
                }
                (*index_ptr) += 3;
                return;
            }
            else
            {
                // edge case - second charater is the String terminator ie. "
                if (second_ch == '\"')
                    cout << "Error undefined escape sequence x" << first_ch << endl;
                else
                    cout << "Error undefined escape sequence x" << first_ch << second_ch << endl; // Invalid Hex sequence (not printable) ---> Illegal
                exit(0);
            }
        }
        else
        {
            cout << "Error undefined escape sequence x" << first_ch << endl; // hex value with one character --> ILLEGAL
            exit(0);
        }
    }
    else
    {
        cout << "Error undefined escape sequence x" << endl; // no character were found after "/x" ---> should never get here
        exit(0);
    }
}

void handlestring(int token)
{
    std::stringstream output;
    string str = string(yytext);
    output << yylineno << " " << "STRING" << " ";
    bool isclosed = ( str[str.size()] == '\"' );
    bool continuePrinting = true;
    for (size_t i = 0; i < yyleng; i++)
    {
        char curr_ch = str[i];
        char next_ch = str[i + 1];
        if (curr_ch == '\"')
        {
            if (i == 0)
            {
                continue; // beginning of string -- should be ignored
            }
            else
            {
                isclosed = true; // we reached the end of the string!
                continue;
            }
        }
        
        if (curr_ch == '\\')
        {
            switch (next_ch)
            {
            case 'n':
            {
                if (continuePrinting)
                {
                    output << '\n';   
                }
                i += 1;
                break;
            }
            case 'r':
            {
                if (continuePrinting)
                {
                    output << '\r';
                }
                i += 1;
                break;
            }
            case 't':
            {
                if (continuePrinting)
                {
                    output << '\t';
                }
                i += 1;
                break;
            }
            case '\\':
            {
                if (continuePrinting)
                {
                    output << '\\';
                }
                i += 1;
                break;
            }
            case '\"':
            {
                if (continuePrinting)
                {
                    output << '\"';
                }
                i += 1;
                break;
            }
            case '0':
            {
                i += 1;
                continuePrinting = false;
                break;
            }
            case 'x':
            {
                handlehex(&(i), str, output, continuePrinting); // we already know: yytext[i] == "\", yytext[i+1] == "x". we want to know what's next!
                break;
            }
            default:
            {
                int asciiValue = static_cast<int>(next_ch);
                cout << "Error undefined escape sequence " << static_cast<char>(asciiValue) << endl; // print the illegal escape sequence
                exit(0);
                break;
            }
            }
        }
        else
        {
            if ( continuePrinting )
            {
                if ( isValidPrintableCharacter( static_cast< unsigned char>(curr_ch) ) )
                {
                    output << curr_ch;
                }
                else
                {
                    cout << "Error " << curr_ch << "\n"; // illegal non-printable character
                    exit(0);
                }
            }
        }
    }
    if (isclosed)
    {
        cout << output.str() << endl;
    }
    else
    {
        cout << "Error unclosed string\n";
        exit(0);
    }
}

int main()
{   
    int token;
    while ( (token = yylex()) != 0 )
    {
        if ( getTokenName(token) == "STRING" )
        {
            handlestring(token); // handles unclosed strings aswell
        }
        else if ( getTokenName(token) == "COMMENT" )
        {
            std::cout << yylineno << " " << getTokenName(token) << " " << yytext << std::endl;
        }
        else
        {
            std::cout << yylineno << " " << getTokenName(token) << " " << yytext << std::endl;
        }
    }
    if ( strlen(yytext) > 1)
    {
        cout << "Error " << '\0' << endl;
        exit(0);
    }
    if(strlen(yytext) == 1 && yytext[0]!=0x1A)
        cout << "Error " << yytext << endl; // receiver an ILLEGAL character (ERROR)
    exit(0); 
}