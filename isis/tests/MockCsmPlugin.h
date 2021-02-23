#ifndef MockCsmPlugin_h
#define MockCsmPlugin_h

#include <string>

#include <QMap>

#include "csm/Plugin.h"
#include "csm/Model.h"
#include "csm/Version.h"

#include <nlohmann/json.hpp>

/**
 * Test Community Sensor Model (CSM) plugin class to use for loading specific camera models.
 */
class MockCsmPlugin : public csm::Plugin {
 public:
  // Static variables that describe the plugin
  static const std::string PLUGIN_NAME;
  static const std::string MANUFACTURER_NAME;
  static const std::string RELEASE_DATE;
  static const int N_SENSOR_MODELS;

  MockCsmPlugin();
  ~MockCsmPlugin();

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

  void registerModel(std::string stateString, csm::Model* model);

  private:
    static const MockCsmPlugin m_registeredPlugin; //! static instance of plugin
    static QMap<std::string, csm::Model*> m_registeredModels;
};

#endif
