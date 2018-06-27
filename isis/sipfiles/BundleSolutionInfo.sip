
%Include type_conversions.sip

namespace Isis{

class BundleSolutionInfo : public QObject {
  %TypeHeaderCode
    #include "BundleSolutionInfo.h"
  %End

 public:
   // do constructor since at the moment there doesn't seem to be a reason to contruct it directly in the python side
   void setOutputStatistics(Isis::BundleResults statisticsResults) throw(Isis::IException);
   void setRunTime(QString runTime) throw(Isis::IException);

   QString id() const throw(Isis::IException);
   QSharedPointer<Isis::BundleSettings> bundleSettings() throw(Isis::IException);
   Isis::BundleResults bundleResults() throw(Isis::IException);
   QString runTime() const throw(Isis::IException);

   bool outputText() throw(Isis::IException);
   bool outputPointsCSV() throw(Isis::IException);
   bool outputResiduals() throw(Isis::IException);

  }; // end namespace Isis
};
