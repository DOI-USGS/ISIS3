#pragma once

#include <functional>
#include <map>
#include <QString>
#include <SpiceQL.h>
#include <typeinfo>
#include <typeindex>
#include <vector>

#include <nlohmann/json.hpp>

using json=nlohmann::json;

namespace Isis::RestfulSpice {
  extern std::map<std::string, std::string> spiceql_mission_map; 

  std::vector<std::vector<double>> getTargetStates(std::vector<double> ets, std::string target, std::string observer, std::string frame, std::string abscorr, std::string mission, std::string ckQuality, std::string spkQuality, bool useWeb);
  std::vector<std::vector<double>> getTargetOrientations(std::vector<double> ets, int toFrame, int refFrame, std::string mission, std::string ckQuality, bool useWeb);
  double strSclkToEt(int frameCode, std::string sclk, std::string mission, bool useWeb);
  double doubleSclkToEt(int frameCode, double sclk, std::string mission, bool useWeb);
  double utcToEt(std::string utc, bool useWeb);
  std::string etToUtc(double et, std::string format, double precision, bool useWeb);
  std::string etToStrSclk(int frameCode, double et, std::string mission, bool useWeb);
  int translateNameToCode(std::string frame, std::string mission, bool useWeb);
  std::string translateCodeToName(int code, std::string mission, bool useWeb);
  std::vector<int> getFrameInfo(int frame, std::string mission, bool useWeb) ;
  json getTargetFrameInfo(int targetId, std::string mission, bool useWeb) ;
  json findMissionKeywords(std::string key, std::string mission, bool useWeb);
  json findTargetKeywords(std::string key, std::string mission, bool useWeb);
  std::vector<std::vector<int>> frameTrace(double et, int initialFrame, std::string mission, std::string ckQuality, bool useWeb);
  std::vector<double> extractExactCkTimes(double observStart, double observEnd, int targetFrame, std::string mission, std::string ckQuality, bool useWeb);

  json spiceAPIQuery(std::string functionName, json args);
}