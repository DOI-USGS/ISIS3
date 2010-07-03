/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/04/08 15:14:13 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "InfixToPostfix.h"
#include "iException.h"
#include <iostream>

using namespace std;

namespace Isis {
  //! Constructor
  InfixToPostfix::InfixToPostfix() {
    Initialize();
  }

  InfixToPostfix::~InfixToPostfix() {
    Uninitialize();
  }

  /**
   * This populates the known operators/functions list. If the operators list
   * is not empty, this function will do nothing.
   */
  void InfixToPostfix::Initialize() {
    p_operators.push_back(new InfixOperator(7, "^"));
    p_operators.push_back(new InfixOperator(5, "/"));
    p_operators.push_back(new InfixOperator(5, "*"));
    p_operators.push_back(new InfixOperator(3, "<<"));
    p_operators.push_back(new InfixOperator(3, ">>"));
    p_operators.push_back(new InfixOperator(2, "+"));
    p_operators.push_back(new InfixOperator(2, "-"));
    p_operators.push_back(new InfixOperator(1, ">"));
    p_operators.push_back(new InfixOperator(1, "<"));
    p_operators.push_back(new InfixOperator(1, ">="));
    p_operators.push_back(new InfixOperator(1, "<="));
    p_operators.push_back(new InfixOperator(1, "=="));
    p_operators.push_back(new InfixOperator(1, "!="));
    p_operators.push_back(new InfixOperator(-1, "("));

    // This makes multiple argument functions work
    p_operators.push_back(new InfixOperator(-1, ","));

    p_operators.push_back(new InfixFunction("--",     1));
    p_operators.push_back(new InfixFunction("sqrt",   1));
    p_operators.push_back(new InfixFunction("abs",    1));
    p_operators.push_back(new InfixFunction("sin",    1));
    p_operators.push_back(new InfixFunction("cos",    1));
    p_operators.push_back(new InfixFunction("tan",    1));
    p_operators.push_back(new InfixFunction("csc",    1));
    p_operators.push_back(new InfixFunction("sec",    1));
    p_operators.push_back(new InfixFunction("cot",    1));
    p_operators.push_back(new InfixFunction("asin",   1));
    p_operators.push_back(new InfixFunction("acos",   1));
    p_operators.push_back(new InfixFunction("atan",   1));
    p_operators.push_back(new InfixFunction("atan2",  2));
    p_operators.push_back(new InfixFunction("sinh",   1));
    p_operators.push_back(new InfixFunction("cosh",   1));
    p_operators.push_back(new InfixFunction("tanh",   1));
    p_operators.push_back(new InfixFunction("asinh",  1));
    p_operators.push_back(new InfixFunction("acosh",  1));
    p_operators.push_back(new InfixFunction("atanh",  1));
    p_operators.push_back(new InfixFunction("log",    1));
    p_operators.push_back(new InfixFunction("log10",  1));
    p_operators.push_back(new InfixFunction("ln",     1));
    p_operators.push_back(new InfixFunction("cubemin", 1));
    p_operators.push_back(new InfixFunction("cubemax", 1));
    p_operators.push_back(new InfixFunction("linemin", 1));
    p_operators.push_back(new InfixFunction("linemax", 1));
    p_operators.push_back(new InfixFunction("min",     2));
    p_operators.push_back(new InfixFunction("max",     2));
    p_operators.push_back(new InfixFunction("line",   0));
    p_operators.push_back(new InfixFunction("sample", 0));
    p_operators.push_back(new InfixFunction("band",   0));
    p_operators.push_back(new InfixFunction("pi",     0));
    p_operators.push_back(new InfixFunction("e",      0));
  }

  /**
   * This cleans the known operators/functions list.
   */
  void InfixToPostfix::Uninitialize() {
    for(int i = 0; i < p_operators.size(); i ++) {
      delete p_operators[i];
    }

    p_operators.clear();
  }

  /**
   * This function takes a space-delimited string and removes empty
   * delimiters. In order words, it compresses the spaces. This is used
   * to keep strings small, clean, and remove the necessity to check for
   * empty tokens constantly.
   *
   * That is, if your string is "a  b" the result will be "a b".
   *
   * @param equation A space-delimited string with excessive spaces
   *
   * @return iString A space-delimited string with data between every pair of spaces
   */
  iString InfixToPostfix::CleanSpaces(iString equation) {
    iString clean = "";
    while(!equation.empty()) {
      iString data = equation.Token(" ");
      if(data.empty()) {
        continue;
      }

      if(clean.empty()) {
        clean = data;
      }
      else {
        clean += " " + data;
      }
    }

    return clean;
  }

  /**
   * This method converts infix to postfix. It uses an enhanced verion of
   * the algorithm found on page 159 of "Data Structures & Algorithms in Java"
   * Second Edition by Robert Lafore. First, we prep the equation with
   * TokenizeEquation and then parse through it using the known-good algorithm.
   *
   * @param infix The infix equation
   *
   * @return iString The postfix equation
   */
  iString InfixToPostfix::Convert(const iString &infix) {
    // Prep our equation for the conversion
    iString equation = TokenizeEquation(infix);
    iString postfix = "";

    // The algorithm uses a stack
    std::stack<InfixOperator> theStack;

    // Use this to look for two operands in a row. If we find such,
    //   then we know the infix equation is bogus.
    int numConsecutiveOperands = 0;

    // Use this to look for two operators in a row. If we find such,
    //   then we know the infix equation is bogus.
    int numConsecutiveOperators = 0;

    // We'll use tokens to get through the entire equation, which is space-delimited
    //   because of TokenizeEquation. So continue processing until we're out of tokens
    //   and the string is empty.
    while(!equation.empty()) {

      // There will be no empty tokens, so don't worry about checking for it.
      //   TokenizeEquation cleans excess spaces in it's return value.
      iString data = equation.Token(" ");

      if(data.compare("(") == 0) {
        theStack.push(*FindOperator(data));
      }
      else if(data.compare(")") == 0) {
        CloseParenthesis(postfix, theStack);
      }
      else if(IsKnownSymbol(data)) {
        AddOperator(postfix, *FindOperator(data), theStack);

        if(IsFunction(data)) {
          // For a general check, zero single argument functions the
          //   same as an operand.
          if(((InfixFunction *)FindOperator(data))->ArgumentCount() == 0) {
            numConsecutiveOperators = 0;
            numConsecutiveOperands ++;
          }
          else {
            // We have a function with arguments, an operand or function call is expected next
            numConsecutiveOperators = 1;
            numConsecutiveOperands = 0;
          }
        }
        else {
          // We found an operator, not a function, so we expect a number next
          numConsecutiveOperators ++;
          numConsecutiveOperands = 0;
        }
      }
      else {
        try {
          // Make sure this is truly an operand and not an operator by casting it
          (double)data;
        }
        catch(iException e) {
          iException::Clear();
          throw iException::Message(iException::User, "The operator '" + data + "' is not recognized.", _FILEINFO_);
        }

        // This was clearly an operand at this point
        numConsecutiveOperators = 0;
        numConsecutiveOperands ++;

        postfix += ' ' + data + ' ';
      }

      // If we found consecutive operators or operands, tell the user
      if(numConsecutiveOperators > 1) {
        throw iException::Message(iException::User, "Missing an operand near the operator '" + data + "'.", _FILEINFO_);
      }
      else if(numConsecutiveOperands > 1) {
        throw iException::Message(iException::User, "Missing an operator before " + data + ".", _FILEINFO_);
      }
    }

    while(!theStack.empty()) {
      iString op = theStack.top().OutputString();

      // Any opening parentheses here are invalid at this point
      if(op == "(") {
        throw iException::Message(iException::User,
                                  "There are too many opening parentheses ('(') in the equation.",
                                  _FILEINFO_);
      }

      postfix += ' ' + op + ' ';
      theStack.pop();
    }

    // The , is set to an operator that causes multiple-argument functions to work, but
    //   it needs to be stripped out for postfix. This is the correct way to do it.
    postfix = postfix.Remove(",");

    // Clean spaces just to double check and return our postfix answer
    return CleanSpaces(postfix);
  }

  /**
   * This method will return true if it believes the argument represents
   *   a valid function or operator.
   *
   * @param representation The symbolic representation of the operator, such as 'sin'
   *
   * @return bool True if it looks valid, false if it's not known
   */
  bool InfixToPostfix::IsKnownSymbol(iString representation) {
    for(int i = 0; i < p_operators.size(); i++) {
      if(representation.compare(p_operators[i]->InputString()) == 0) {
        return true;
      }
    }

    return false;
  }

  /**
   * This method will return true if 'representation' is a known function.
   *
   * @param representation The symbolic representation of a function, such as 'abs'
   *
   * @return bool True if it's a known function, false otherwise
   */
  bool InfixToPostfix::IsFunction(iString representation) {
    if(IsKnownSymbol(representation)) {
      return FindOperator(representation)->IsFunction();
    }
    else {
      return false;
    }
  }

  /**
   * This is straight from the algorithm found on page 159 of "Data Structures & Algorithms in Java"
   * Second Edition by Robert Lafore.
   *
   * @param postfix The postix generated thus far
   * @param op The operator
   * @param theStack The operator stack
   */
  void InfixToPostfix::AddOperator(iString &postfix, const InfixOperator &op, std::stack<InfixOperator> &theStack) {
    while(!theStack.empty()) {
      InfixOperator top = theStack.top();
      theStack.pop();

      if(top.InputString().compare("(") == 0) {
        theStack.push(top);
        break;
      }

      if(top.Precedence() < op.Precedence()) {
        theStack.push(top);
        break;
      }
      else {
        postfix += ' ' + top.OutputString() + ' ';
      }
    }

    theStack.push(op);
  }

  /**
   * This is straight from the algorithm found on page 159 of "Data Structures & Algorithms in Java"
   * Second Edition by Robert Lafore.
   *
   * @param postfix The postix generated thus far
   * @param theStack The operator stack
   */
  void InfixToPostfix::CloseParenthesis(iString &postfix, std::stack<InfixOperator> &theStack) {
    bool openingFound = false;
    while(!theStack.empty()) {
      InfixOperator op = theStack.top();
      theStack.pop();

      if(op.InputString().compare("(") == 0) {
        openingFound = true;
        break;
      }
      else {
        postfix += ' ' + op.OutputString() + ' ';
      }
    }

    if(!openingFound) {
      throw iException::Message(iException::User,
                                "There are too many closing parentheses (')') in the equation.",
                                _FILEINFO_);
    }
  }

  /**
   * This method will return a pointer to the operator represented by
   *   'representation.' Because in this model a function is an operator,
   *   this will return a pointer to functions as well (in a base class pointer).
   *
   * @param representation The symbolic representation of the operator, such as '+'
   *
   * @return InfixOperator* A pointer to the operator object that contains known information about the operator
   */
  InfixOperator *InfixToPostfix::FindOperator(iString representation) {
    for(int i = 0; i < p_operators.size(); i++) {
      if(representation.compare(p_operators[i]->InputString()) == 0) {
        return p_operators[i];
      }
    }

    // Nothing found
    throw iException::Message(iException::User, "The operator '" + representation + "' is not recognized.", _FILEINFO_);
  }

  /**
   * This method will add spaces between all operators and numbers, making it
   * possible to get each element of the equation one by one. It will also parse
   * out the function calls, adding parenthesis where needed so the user doesn't
   * have to. The result is an equation ready for parsing (but NOT fully parenthesized,
   * just enough to make sure our algorithm makes no mistakes).
   *
   * @param equation An unformatted infix equation
   *
   * @return iString A tokenized equation with additional parentheses
   */
  iString InfixToPostfix::TokenizeEquation(const iString &equation) {
    iString output = "";

    // Insert whitespace, make everything lowercase, and change all braces to
    // parenthesis
    for(unsigned int i = 0; i < equation.size(); i++) {
      // Ensure there is whitespace in the equation
      if(!isalnum(equation[i]) && !isspace(equation[i]) && equation[i] != '.') {
        // Convert all braces to parens
        if(equation[i] == '[' || equation[i] == '{') {
          output += " ( ";
        }
        else if(equation[i] == ']' || equation[i] == '}') {
          output += " ) ";
        }
        // Test for multicharacter operators
        else if(i < equation.size() - 1 && equation[i] == '-' && equation[i+1] == '-') {
          output += " -- ";
          i++;
        }
        else if(i < equation.size() - 1 && equation[i] == '<' && equation[i+1] == '<') {
          output += " << ";
          i++;
        }
        else if(i < equation.size() - 1 && equation[i] == '>' && equation[i+1] == '>') {
          output += " >> ";
          i++;
        }
        else if(i < equation.size() - 1 && equation[i] == '>' && equation[i+1] == '=') {
          output += " >= ";
          i++;
        }
        else if(i < equation.size() - 1 && equation[i] == '<' && equation[i+1] == '=') {
          output += " <= ";
          i++;
        }
        else if(i < equation.size() - 1 && equation[i] == '=' && equation[i+1] == '=') {
          output += " == ";
          i++;
        }
        else if(i < equation.size() - 1 && equation[i] == '!' && equation[i+1] == '=') {
          output += " != ";
          i++;
        }
        // Take care of scientific notiation where the exponent is negative
        else if((i > 1) && equation[i] == '-' && tolower(equation[i-1]) == 'e' && isalnum(equation[i-2])) {
          output += equation[i];
        }
        // Look for negative operator disguised as '-'
        else if(equation[i] == '-') {
          bool isNegative = true;

          // If we run into a '(' or the beginning, then the '-' must mean '--' really.
          for(int index = i - 1; index >= 0; index --) {
            if(equation[index] == ' ') {
              continue;
            }

            if(equation[index] != '(' && equation[index] != '/' &&
                equation[index] != '*' && equation[index] != '+') {
              isNegative = false;
              break;
            }

            break;
          }

          if(isNegative) {
            output += " -- ";
          }
          else {
            output += " - ";
          }
        }
        else {
          output += ' ';
          output += equation[i];
          output += ' ';
        }
      }
      else {
        output += equation[i];
      }
    }

    iString cleanedEquation = CleanSpaces(FormatFunctionCalls(output.DownCase()));

    return cleanedEquation;
  }

  /**
   * This method looks through equation for function calls, parenthesizes them,
   * and calls itself again with each argument in order to parse any embedded functions.
   * This ensures order of operations holds for cases like sin(.5)^2. This method might add
   * too many parentheses, but that is harmless. This does not affect operators (excepting --) or
   * numbers. Only functions. The input should be space-delimited.
   *
   * @param equation The unparenthesized equation
   *
   * @return iString The parenthesized equation
   */
  iString InfixToPostfix::FormatFunctionCalls(iString equation) {
    // Clean our space-delimited equation
    equation = CleanSpaces(equation);
    iString output = "";

    // We'll use tokens to get through the entire equation, which is space-delimited.
    //   So continue processing until we're out of tokens and the string is empty.
    while(!equation.empty()) {
      iString element = equation.Token(" ");

      // Did we find a function? Figure out what it is!
      if(IsFunction(element)) {
        // Point to the function. We know if IsFunction returned true,
        //   the result is really a InfixFunction*
        InfixFunction *func = (InfixFunction *)FindOperator(element);

        // We want to wrap the entire thing in parentheses, and it's argument string.
        //   So sin(.5)^2 becomes (sin(.5))^2
        output += " ( " + func->InputString() + " (";

        // Deal with 0-argument functions
        if(func->ArgumentCount() == 0) {
          iString next = equation.Token(" ");

          // If they didn't add parentheses around the zero-argument
          //   function, we still know what they mean. Close the arguments
          //   and the wrapping parentheses.
          if(next != "(") {
            output += " ) ) ";

            // Step back, we did too much
            equation = next + " " + equation;
          }
          else {
            // We see a zero-arg function, and we grabbed an open parenthesis from it.
            //   Make sure the next thing is a close or we have a problem.
            if(equation.Token(" ") != ")") {
              throw iException::Message(iException::User,
                                        "The function " + func->InputString() + " should not have any arguments.",
                                        _FILEINFO_);
            }

            // Close the arguments and the wrapping parentheses. They wrote their call correct :)
            output += " ) ) ";
          }
        }
        else {
          // Deal with 1+ argument functions by parsing out the arguments

          // Make sure the user put parentheses around these, otherwise we're left in the dark.
          if(func->ArgumentCount() > 1 && equation.Token(" ") != "(") {
            throw iException::Message(iException::User,
                                      "Missing parenthesis after " + func->InputString(),
                                      _FILEINFO_);
          }

          // Single argument missing parenthesis?
          else if(func->ArgumentCount() == 1) {
            iString argument = equation.Token(" ");

            if(argument != "(") {
              // We might have a problem. They're calling a function without adding parentheses....
              //   unless it's a negate, because we insert those, tell them their mistake. It's not
              //   my job to figure out what they mean!
              if(func->InputString() != "--") {
                throw iException::Message(iException::User,
                                          "Missing parenthesis after " + func->InputString(),
                                          _FILEINFO_);
              }

              // It's a negate without parentheses, so they mean the next term?
              if(!IsFunction(argument)) {
                // If it isn't a function, it's safe to just append
                output += " " + FormatFunctionCalls(argument) + " ) ) ";
                continue;
              }
              else {
                // We are negating a function result. We must do a mini-parse to figure out
                //   the function and resursively call, then append the negation.
                iString functionName = argument;
                iString openParen = equation.Token(" ");

                // No open parens? Call ourself again with this supposed function
                if(openParen != "(") {
                  output += " " + FormatFunctionCalls(functionName) + " ) ) ";
                  equation = openParen + " " + equation;
                  continue;
                }
                else {
                  functionName += " (";
                }

                // Parse out our equation quickly, call ourself with it to properly handle it,
                //   and append the negation.
                int numParens = 0;
                while(numParens > -1) {
                  iString newElem = equation.Token(" ");

                  if(newElem == "") {
                    throw iException::Message(iException::User,
                                              "Missing closing parentheses after '" + argument + "'.",
                                              _FILEINFO_);
                  }

                  if(newElem == "(") numParens++;
                  else if(newElem == ")") numParens--;

                  functionName += " " + newElem;
                }

                output += " " + FormatFunctionCalls(functionName) + " ) ) ";
                continue;
              }
            }
          }

          /**
           * This code block is for multi-parameter functions. Functions with 1+ parameters
           * are parsed here, excepting the negation (with no parentheses) which is done just above.
           *
           * We figure out the arguments by looking for commas, outside of all parentheses, for each argument
           * up until the last one. We look for an extra closing parenthesis for the last argument. When we figure
           * out what an argument is, we recursively call FormatFunctionCalls to format any and all functionality
           * inside of the argument.
           */
          iString argument = "";
          int numParens = 0;
          int argNum = 0;
          while(argNum < func->ArgumentCount()) {
            iString elem = equation.Token(" ");

            // Ran out of data, the function call is not complete.
            if(elem == "") {
              throw iException::Message(iException::User,
                                        "The definition of '" + func->InputString() + "' is not complete.",
                                        _FILEINFO_);
            }

            if(elem == "(") {
              numParens ++;
              argument += " (";
            }
            else if(elem == ")") {
              numParens --;

              // Ignore last close, it's not part of the argument and will be added later
              if(numParens != -1) {
                argument += " )";
              }
            }
            else if(elem == "," && numParens == 0) {
              CheckArgument(func->InputString(), argNum, argument);
              argument = FormatFunctionCalls(argument);
              output += " ( " + argument + " ) , ";
              argNum++;
              argument = "";

              // Too many arguments? We don't expect a comma delimiter on the last argument.
              if(argNum == func->ArgumentCount()) {
                throw iException::Message(iException::User,
                                          "There were too many arguments supplied to the function '" + func->InputString() + "'.",
                                          _FILEINFO_);
              }
            }
            else {
              argument += " " + elem;
            }

            if(argNum == func->ArgumentCount() - 1 && numParens == -1) {
              CheckArgument(func->InputString(), argNum, argument);
              argument = FormatFunctionCalls(argument);
              // close function call & function wrap
              output += " " + argument + " ) ) ";
              argNum++;
              argument = "";
            }
            // Closed the function early?
            else if(numParens == -1) {
              throw iException::Message(iException::User,
                                        "There were not enough arguments supplied to the function '" + func->InputString() + "'.",
                                        _FILEINFO_);
            }
          }
        }
      }
      // Not a function, preserve & ignore
      else {
        output = output + " " + element;
      }
    }

    return output;
  }

  void InfixToPostfix::CheckArgument(iString funcName, int argNum, iString argument) {
    argument = argument.Remove(" ");
    argument = argument.Remove("(");
    argument = argument.Remove(")");

    if(argument == "") {
      throw iException::Message(iException::User,
                                "Argument " + (iString)(argNum + 1) + " in function " + funcName + " must not be empty.",
                                _FILEINFO_);
    }
  }
}
