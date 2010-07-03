#include "Calculator.h"
#include "InfixToPostfix.h"
#include "Preference.h"
#include <float.h>

using namespace std;
using namespace Isis;

int main(int argc, char *argv[])
{
  Isis::Preference::Preferences(true);

  cout << "-------------------------------------------------------" << endl;
  cout << "Test InfixToPostfix" << endl;

  const int NUM_EQUATIONS = 25;
  iString equations[NUM_EQUATIONS] = {
    "-4",
    "1*2",
    "((1)+(1))",
    "1*2/2-2",
    "sin(5)",
    "sin 5",
    "--sin(-(54+65))",
    "--sin(-54+--65)",
    "2/3^6",
    "atan2(5,--4)",
    "atan2(--5)",
    "atan2(1,2,3)",
    "atan2(1,)",
    "atan2(1,2",
    "999-548-126^2",
    "sin(0)^2",
    "somefunc(5)",
    "3#3",
    "(3)(2)", // Can't detect the problem here yet because f3 and f2 are functions/operators
    "(3)(2)",
    "atan2(1+2/3^(--6), 5^ (tan ( 42 ^ (1 / --264) / 4 ) - 65 ) != 0)",
    "1++2", // This doesn't work
    "1+-2", // This does work, however, because -2 is a negation and not a subtract.
    "(1+3*(4)",
    "(1+3*(4)))"
  };

  InfixToPostfix converter;
  for(int equation = 0; equation < NUM_EQUATIONS; equation ++) {
    cout << endl << endl << equation+1 << ": Convert '" << equations[equation] << "' to postfix" << endl;

    try {
      iString tokenized = converter.TokenizeEquation(equations[equation]);
      cout << "   Tokenized equation: '" << tokenized << "'" << endl;
      iString postfix = converter.Convert(equations[equation]);
      cout << "   Postfix: '" << postfix << "'" << endl;
    }
    catch(iException e) {
      e.Report(false);
      iException::Clear();
    }
  }
}
