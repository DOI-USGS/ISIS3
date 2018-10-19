namespace Isis {
  class ControlNet : public QObject {
    %TypeHeaderCode
    #include "ControlNet.h"
    %End
    public:
      ControlNet() throw(Isis::IException);
      ControlNet(const Isis::ControlNet &other) throw(Isis::IException);

      void clear() throw(Isis::IException);

      void Write(const QString &filename, bool pvl = false) throw(Isis::IException);

      double GetMaximumResidual() throw(Isis::IException);
      QString GetNetworkId() const throw(Isis::IException);
      int GetNumEditLockMeasures() throw(Isis::IException);
      int GetNumEditLockPoints() throw(Isis::IException);
      int GetNumIgnoredMeasures() throw(Isis::IException);
      int GetNumberOfValidMeasuresInImage(const QString &serialNumber) throw(Isis::IException);
      int GetNumberOfJigsawRejectedMeasuresInImage(const QString &serialNumber) throw(Isis::IException);
      void ClearJigsawRejected() throw(Isis::IException);
      void IncrementNumberOfRejectedMeasuresInImage(const QString &serialNumber) throw(Isis::IException);
      void DecrementNumberOfRejectedMeasuresInImage(const QString &serialNumber) throw(Isis::IException);
      int GetNumMeasures() const throw(Isis::IException);
      int GetNumPoints() const throw(Isis::IException);
      int GetNumValidMeasures() throw(Isis::IException);
      int GetNumValidPoints() throw(Isis::IException);
      QString GetTarget() const throw(Isis::IException);
      QString GetUserName() const throw(Isis::IException);
    };
};
