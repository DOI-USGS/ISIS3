
namespace Isis {
  class Statistics : public QObject {
    %TypeHeaderCode
      #include "Statistics.h"
    %End
    public:
      Statistics(const Isis::Statistics &other) throw(Isis::IException);

      void Reset() throw(Isis::IException);

      void AddData(const double *data, const unsigned int count) throw(Isis::IException);
      void AddData(const double data) throw(Isis::IException);

      void RemoveData(const double *data, const unsigned int count) throw(Isis::IException);
      void RemoveData(const double data) throw(Isis::IException);

      double ValidMinimum() const throw(Isis::IException);
      double ValidMaximum() const throw(Isis::IException);
      bool InRange(const double value) throw(Isis::IException);
      bool AboveRange(const double value) throw(Isis::IException);
      bool BelowRange(const double value) throw(Isis::IException);

      double Average() const throw(Isis::IException);
      double StandardDeviation() const throw(Isis::IException);
      double Variance() const throw(Isis::IException);
      double Sum() const throw(Isis::IException);
      double SumSquare() const throw(Isis::IException);
      double Rms() const throw(Isis::IException);

      double Minimum() const throw(Isis::IException);
      double Maximum() const throw(Isis::IException);
      double ChebyshevMinimum(const double percent = 99.5) const throw(Isis::IException);
      double ChebyshevMaximum(const double percent = 99.5) const throw(Isis::IException);
      double BestMinimum(const double percent = 99.5) const throw(Isis::IException);
      double BestMaximum(const double percent = 99.5) const throw(Isis::IException);
      double ZScore(const double value) const throw(Isis::IException);

      bool RemovedData() const throw(Isis::IException);
    };
};
