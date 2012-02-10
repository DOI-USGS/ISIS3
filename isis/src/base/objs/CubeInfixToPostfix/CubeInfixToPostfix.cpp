/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/02/23 17:09:44 $
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

#include "CubeInfixToPostfix.h"
#include "iException.h"

using namespace std;

namespace Isis {
  /**
   * Constructs a CubeInfixToPostfix converter
   * 
   * @author janderson (2/2/2012)
   */
  CubeInfixToPostfix::CubeInfixToPostfix() {
    initialize();
  }

  /**
   * This method is used to create functions that are specific to 
   * cubes.  Moved the cubemin and cubemax functions out of the 
   * InfixToPostfix initialization method into this method 
   * 
   * @author janderson (2/2/2012)
   */
  void CubeInfixToPostfix::initialize() {
    // List of the form funct(fN) where N is the file number
    // For example pha(f1) returns the phase angle for f1
    p_operators.push_back(new InfixFunction("cubemin", 1));
    p_operators.push_back(new InfixFunction("cubemax", 1));
    p_operators.push_back(new InfixFunction("cubeavg", 1));
    p_operators.push_back(new InfixFunction("cubestd", 1));

    p_operators.push_back(new InfixFunction("pha", 1));
    p_operators.push_back(new InfixFunction("ema", 1));
    p_operators.push_back(new InfixFunction("ina", 1));

    p_operators.push_back(new InfixFunction("phal", 1));
    p_operators.push_back(new InfixFunction("emal", 1));
    p_operators.push_back(new InfixFunction("inal", 1));

    p_operators.push_back(new InfixFunction("phac", 1));
    p_operators.push_back(new InfixFunction("emac", 1));
    p_operators.push_back(new InfixFunction("inac", 1));

    p_operators.push_back(new InfixFunction("lat", 1));
    p_operators.push_back(new InfixFunction("lon", 1));
    p_operators.push_back(new InfixFunction("res", 1));
    p_operators.push_back(new InfixFunction("radius", 1));
  }

  /**
   * This method will return true if it believes the argument represents
   *   a valid function or operator.
   *
   * @param representation The symbolic representation of the operator, such as 'sin'
   *
   * @return bool True if it looks valid, false if it's not known
   */
  bool CubeInfixToPostfix::isKnownSymbol(iString representation) {
    for(int i = 0; i < p_operators.size(); i++) {
      if(representation.compare(p_operators[i]->inputString()) == 0) {
        return true;
      }
    }

    bool isFunction = (representation.size() > 1);
    if(representation[0] == 'f') {
      for(unsigned int i = 1; isFunction && i < representation.size(); i++) {
        isFunction &= (representation[i] >= '0' && representation[i] <= '9');
      }
    }
    else {
      isFunction = false;
    }

    return isFunction;
  }

  InfixOperator *CubeInfixToPostfix::findOperator(iString representation) {
    try {
      return InfixToPostfix::findOperator(representation);
    }
    catch(iException &e) {
      e.Clear();
    }

    bool isFunction = (representation.size() > 1);
    if(representation[0] == 'f') {
      for(unsigned int i = 1; i < representation.size(); i++) {
        isFunction &= (representation[i] >= '0' && representation[i] <= '9');
      }
    }
    else {
      isFunction = false;
    }

    if(isFunction) {
      p_operators.push_back(new InfixFunction(representation, 0));
      return p_operators[p_operators.size()-1];
    }

    throw iException::Message(iException::User, "The operator '" + representation + "' is not recognized.", _FILEINFO_);
  }


}; // end namespace Isis
