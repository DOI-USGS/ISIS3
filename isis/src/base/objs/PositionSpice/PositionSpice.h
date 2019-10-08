#ifndef PositionSpice_h
#define PositionSpice_h

#include "Position.h"

namespace Isis {
  class PositionSpice: public Position {
    public:
      PositionSpice(int targetCode, int observerCode);

      // Replicates logic from Position SetEphemerisTimeMemCache
      virtual std::vector<std::vector<double>> SetEphemerisTime(double et);

      // Only put memcach specific in here, let base do the generic
      // virtual Table Cache(const QString &tableName);

      // Used by base to update the labels
      // virtual void GetTableType(Table &table);
  };
};

#endif
