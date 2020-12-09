#ifndef TestCsmPlugin_h
#define TestCsmPlugin_h

#include <string>

#include "csm/Plugin.h"
#include "csm/Model.h"
#include "csm/Version.h"

#include <nlohmann/json.hpp>

/**
 * Test Community Sensor Model (CSM) plugin class.
 * 
 * @author 2020-12-08 Kristin Berry
 */
class TestCsmPlugin : public csm::Plugin {
 public:
  // Static variables that describe the plugin
  static const std::string PLUGIN_NAME;
  static const std::string MANUFACTURER_NAME;
  static const std::string RELEASE_DATE;
  static const int N_SENSOR_MODELS;

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
    static const TestCsmPlugin m_registeredPlugin; //! static instance of plugin
};

#endif 
