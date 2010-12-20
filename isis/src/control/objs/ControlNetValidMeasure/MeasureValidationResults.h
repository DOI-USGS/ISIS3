#ifndef _MeasureValidationResults_h_
#define _MeasureValidationResults_h_

class QString;
template< class T > class QVector;

/**
 * @file
 * $Revision: $
 * $Date: $
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

namespace Isis {
  
  class MeasureValidationResults {

    public:
      enum Option {
        EmissionAngle,
        IncidenceAngle,
        DNValue,
        Resolution,
        PixelsFromEdge,
        MetersFromEdge
      };

      MeasureValidationResults();
      ~MeasureValidationResults();

      bool isValid();
      bool getValidStatus(Option opt);

      QString toString();
      QString toString(QString serialNumber, QString pointID);
      QString toString(QString sample, QString line, QString serialNumber,
          QString pointID);

      void addFailure(Option opt, double tolerance);
      void addFailure(Option opt, double computed, double min, double max);

      QString getFailurePrefix(Option opt);

    private:
      QVector<Option> * failures;
      QString * errorMsg;
  };
};

#endif

