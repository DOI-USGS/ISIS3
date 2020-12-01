#ifndef AlternativeTestCsmModel_h
#define AlternativeTestCsmModel_h

#include <string>

#include "csm/Plugin.h"
#include "csm/Model.h"
#include "csm/Version.h"

class AlternativeTestCsmModel : public csm::Model {
 public:
   AlternativeTestCsmModel();
   ~AlternativeTestCsmModel();

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
   std::string constructStateFromIsd(const csm::Isd stringIsd);

  private:
    std::string m_modelState; 
};
#endif
