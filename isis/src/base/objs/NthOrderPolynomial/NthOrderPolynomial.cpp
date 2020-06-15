/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:08 $
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

#include <math.h>
#include <QString>

#include "FileName.h"
#include "Constants.h"
#include "IException.h"
#include "NthOrderPolynomial.h"

using namespace std;
namespace Isis {

  /**
   * Create an NthOrderPolynomial
   * 
   * @param degree The order/degree of the polynomial
   * 
   */
  NthOrderPolynomial::NthOrderPolynomial(int degree) : 
    Isis::BasisFunction("NthOrderPolynomial", 2, degree) {
    p_degree = degree;
  }
  

  /**
   * This is the the overriding virtual function that provides the expansion into
   * the nth order polynomial equation. 
   *  
   * See BasisFunction for more information.
   *
   * @param vars A vector of double values to use for the expansion.
   */
  void NthOrderPolynomial::Expand(const std::vector<double> &vars) {

    if((int) vars.size() != Variables()) {
      QString mess = "Number of variables given (" + QString::number(vars.size())
          + ") does not match expected (" + Variables() + ")!";
      throw IException(IException::Programmer, mess, _FILEINFO_);
    }
    
    double t1 = vars[0];
    double t2 = vars[1];
    p_terms.clear();
    for (int i = p_degree; i >= 1; i--) {
      p_terms.push_back(pow(t1, i) - pow(t2, i));
    }
    return;
  }
} // end namespace isis

