#ifndef PositionMemCache_h
#define PositionMemCache_h

#include "Position.h"
namespace Isis {
  class PositionMemCache:Position {
    public:
      // Replicates logic from Position SetEphemerisTimeMemCache
      virtual const std::vector<double> &SpicePosition::SetEphemerisTime(double et);

      // Only put memcach specific in here, let base do the generic
      virtual Table SpicePosition::Cache(const QString &tableName);

      // Used by base to update the labels
      virtual void SpicePosition::GetTableType(Table &table);
  };
};

#endif
