#ifndef TestCsmPlugin_h
#define TestCsmPlugin_h

#include <string>

#include "csm/Plugin.h"
#include "csm/Model.h"
#include "csm/Version.h"

#include <nlohmann/json.hpp>

class TestCsmPlugin : public csm::Plugin {
 public:
  TestCsmPlugin();
  ~TestCsmPlugin();

  std::string getPluginName() const;

  std::string getManufacturer() const;

  std::string getReleaseDate() const;

  csm::Version getCsmVersion() const;

  size_t getNumModels() const;

  std::string getModelName(size_t modelIndex) const;

  std::string getModelFamily(size_t modelIndex) const;

  csm::Version getModelVersion(const std::string& modelName) const;

  bool canModelBeConstructedFromState(const std::string& modelName,
      const std::string& modelState,
      csm::WarningList* warnings = NULL) const;

  bool canModelBeConstructedFromISD(
      const csm::Isd& imageSupportData,
      const std::string& modelName,
      csm::WarningList* warnings = NULL) const;

  bool canISDBeConvertedToModelState(
      const csm::Isd& imageSupportData,
      const std::string& modelName,
      csm::WarningList* warnings = NULL) const;

  std::string convertISDToModelState(
      const csm::Isd& imageSupportData,
      const std::string& modelName,
      csm::WarningList* warnings = NULL) const;

  csm::Model* constructModelFromState(
      const std::string& modelState,
      csm::WarningList* warnings = NULL) const;

  csm::Model* constructModelFromISD(
      const csm::Isd& imageSupportData,
      const std::string& modelName,
      csm::WarningList* warnings = NULL) const;

  std::string getModelNameFromModelState(
      const std::string& modelState,
      csm::WarningList* warnings = NULL) const;

  private:
    static const TestCsmPlugin m_registeredPlugin;

};

#endif 
