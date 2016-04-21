/**
 * @file                                                                  
 * $Revision: 6084 $ 
 * $Date: 2015-03-04 18:17:45 -0700 (Wed, 04 Mar 2015) $ 
 * $Id: InlineInfixToPostfix.h 6084 2015-03-05 01:17:45Z kbecker@GS.DOI.NET $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "InlineInfixToPostfix.h"

// Qt library
#include <QString>
#include <QVector>

// other ISIS
#include "IException.h"
#include "IString.h"

namespace Isis {
  /**
   * Constructs an InlineInfixToPostfix object. The operators list is
   * filled with string representations of known symbols, recognized
   * by both this class and the parent class.
   */
  InlineInfixToPostfix::InlineInfixToPostfix() : InfixToPostfix() { 
    initialize();
  }


  /**
   * Destroys the InlineInfixToPostfix object. 
   */
  InlineInfixToPostfix::~InlineInfixToPostfix() { 
  }


  /**
   * This method attempts to verify that the given argument is recognized as a 
   * valid function, operator, scalar, or variable. 
   *
   * @param representation The symbolic representation of the operator 
   *  
   * @return bool Indicates whether the representaion is recognized.
   */
  bool InlineInfixToPostfix::isKnownSymbol(QString representation) {

    if ( representation.isEmpty() ) return (false);

    // not an empty string
    if ( InfixToPostfix::isKnownSymbol(representation) ) { 
      return (true); 
    }

    // not known symbol from parent class
    if ( isScalar(representation) ) { 
      return (false); 
    }

    // not scalar, the only thing left is a variable...
    return isVariable(representation);
  }


  /**
   * This method will first search the recognized list of operators and functions 
   * for the given token. If found, it will return a pointer to the represented 
   * operator or function. If not found, the method checks whether the given token
   * is a variable.  If it is a variable, a new operator is constructed from the 
   * given token and a pointer to the represented variable is returned. If the 
   * given token is neither an operator, a function nor a variable, then an 
   * exception is thrown. 
   *  
   *
   * @param token The symbolic representation of the operator, function, or 
   *              variable
   *
   * @return InfixOperator* A pointer to the operator object that contains known information about 
   *                        the operator
   * 
   * @throw IException::User "The token is not recognized as an operator, function or variable."
   */
  InfixOperator *InlineInfixToPostfix::findOperator(QString token) {
    try {
      return InfixToPostfix::findOperator(token);
    }
    catch(IException &e) {
      if ( isVariable(token) ) { 
        // If the token is not a scalar, the isVariable() method will assume it's a variable.
        // Then the token is added it to the variables list and an InfixFunction from this token
        // is added to the operators.
        // Now that is has been added to the operators list, we can call findOperator() again and
        // should find this representation in the list.
        return (findOperator(token)); 
      }
      else {
        QString msg = "The token '" + token 
                      + "' is not recognized as an operator, function or variable.";
        throw IException(e, IException::User, msg, _FILEINFO_);
      }
    }
  
  }


  /**
   * Adds several infix operators and functions to the operator list that are
   * not already recognized by the parent class, InfixToPostfix.
   */
  void InlineInfixToPostfix::initialize() {
    p_operators.push_back(new InfixOperator(1, "%")); 
    p_operators.push_back(new InfixFunction("mod", 2));
    p_operators.push_back(new InfixFunction("fmod", 2));
  
    p_operators.push_back(new InfixOperator(1, "&"));
    p_operators.push_back(new InfixOperator(1, "and"));
  
    p_operators.push_back(new InfixOperator(1, "|"));
    p_operators.push_back(new InfixOperator(1, "or"));
    return; 
  }


  /**
   * Determines whether the given string exists as a recognized variable. 
   * NOTE:  This list is constructed using the isVariable() method. 
   *  
   * @param token A string token to be compared to the recognized 
   *              variable list.
   *  
   * @return bool Indicates whether the given string is recognized as a 
   *         variable.
   */
  bool InlineInfixToPostfix::exists(const QString &token) {
    return (m_variables.contains(token, Qt::CaseInsensitive));
  }


  /**
   * Determines whether the given token represents a scalar value (i.e. whether 
   * it can be converted to a double). 
   *  
   * @param token String to be tested.
   * @return bool Indicates whether the given string can be converted to a 
   *              double.
   */
  bool InlineInfixToPostfix::isScalar(const QString &token) {

    // NOTE: The following checks are commented out due to redundancy
    //       This is a private method only called from isKnownSymbol()
    //       and all of these conditions have already been checked.
    // if (token.isEmpty()) return (false); 
    // if (InfixToPostfix::isKnownSymbol(token)) return (false);

    try {
      Isis::toDouble(token);
      return (true);
    }
    catch (IException &) {
      return (false);
    }
  }


  /**
   * Determines whether the given token is a variable and, if so, appends it to 
   * the list of variables. A token is considered a variable if it is not a 
   * known operator symbol from the parent InfixToPostfix class, not a scalar, 
   * and is not empty. Variables are implemented as functions, so new variables 
   * are also added to the operators list. 
   *  
   * @param token A string containing the token to be tested.
   * @return bool Indicates whether the string was added to the lists of recognized 
   *              variables and operators.
   */
  bool InlineInfixToPostfix::isVariable(const QString &token) {

    // NOTE: The following checks are commented out due to redundancy
    //       This is a private method only called from isKnownSymbol()
    //       and findOperator().
    //       All of these conditions have already been checked for
    //       isKnownSymbol().
    // if (isScalar(token)) return (false);
    // if (InfixToPostfix::isKnownSymbol(token)) return (false);

    if (token.isEmpty()) return (false); 

    m_variables.push_back(token);
    p_operators.push_back(new InfixFunction(token,  0));
    return (true);
  }

} // Namespace Isis

