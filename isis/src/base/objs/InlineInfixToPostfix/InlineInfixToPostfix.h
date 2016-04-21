#ifndef InlineInfixToPostfix_h
#define InlineInfixToPostfix_h
/**
 * @file                                                                  
 * $Revision: 6129 $
 * $Date: 2015-04-02 10:42:32 -0700 (Thu, 02 Apr 2015) $
 * $Id: InlineInfixToPostfix.h 6129 2015-04-02 17:42:32Z jwbacker@GS.DOI.NET $
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

// parent class, has InfixOperator type
#include "InfixToPostfix.h"

#include <QString>
#include <QStringList>

namespace Isis {
  /**
   * @brief A parser for converting equation strings to postfix.
   *
   * This class converts infix equations to postfix for parsing.
   *
   * @ingroup Math
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-24 Jeffrey Covington and Jeannie Backer - Improved documentation.
   *   @history 2015-03-24 Jeannie Backer - Changed m_variables from a QVector
   *                           to QStringList
   *   @history 2016-02-21 Kristin Berry - Added unitTest.
   */
  class InlineInfixToPostfix : public InfixToPostfix {
 
    public:
      InlineInfixToPostfix();
      virtual ~InlineInfixToPostfix();
 
    protected:
      virtual bool isKnownSymbol(QString representation);
      virtual InfixOperator *findOperator(QString element);
 
    private:
      void initialize();
      bool exists(const QString &str);
      bool isScalar(const QString &scalar);
      bool isVariable(const QString &str);
 
      QStringList m_variables; //!< The list of variables (represented as strings).
 
  };

} // Namespace Isis

#endif
