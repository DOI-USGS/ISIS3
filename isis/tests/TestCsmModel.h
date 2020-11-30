#ifndef TestCsmModel_h
#define TestCsmModel_h

#include <string>

#include "csm/Plugin.h"
#include "csm/Model.h"
#include "csm/Version.h"

#include <nlohmann/json.hpp>

class TestCsmModel : public csm::Model {
 public:
   TestCsmModel();
   ~TestCsmModel();

   std::string getFamily() const;
   csm::Version getVersion() const;
   std::string getModelName() const;
   std::string getPedigree() const;
   std::string getImageIdentifier() const;
   void setImageIdentifier(const std::string& imageId,
                           csm::WarningList* warnings = NULL);
   std::string getSensorIdentifier() const;
   std::string getPlatformIdentifier() const; 
   std::string getCollectionIdentifier() const;
   std::string getTrajectoryIdentifier() const;
   std::string getSensorType() const;
   std::string getSensorMode() const;
   std::string getReferenceDateAndTime() const;
   std::string getModelState() const;
   void replaceModelState(const std::string& argState);
};
#endif
