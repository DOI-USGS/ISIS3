
#include <iostream>
#include <string>
#include "RestfulSpice.h"
#include "restincurl.h"

using json=nlohmann::json;

namespace Isis::RestfulSpice{

  std::vector<std::vector<double>> getTargetStates(std::vector<double> ets, std::string target, std::string observer, std::string frame, std::string abcorr, std::string mission, std::string ckQuality, std::string spkQuality, bool useWeb){
    if (useWeb){
      // @TODO validity checks
      json args = json::object({
          {"ets", ets},
          {"target", target},
          {"observer", observer},
          {"frame", frame},
          {"abcorr", abcorr},
          {"mission", mission},
          {"ckQuality", ckQuality},
          {"spkQuality", spkQuality}
          });
      // @TODO check that json exists / contains what we're looking for
      json out = spiceAPIQuery("getTargetStates", args);
      return out["body"]["return"].get<std::vector<std::vector<double>>>();
    }else{
      return SpiceQL::getTargetStates(ets, target, observer, frame, abcorr, mission, ckQuality, spkQuality, false);
    }
  }

  std::vector<std::vector<double>> getTargetOrientations(std::vector<double> ets, int toFrame, int refFrame, std::string mission, std::string ckQuality, bool useWeb) {
    if (useWeb){
      json args = json::object({
        {"ets", ets},
        {"toFrame", toFrame},
        {"refFrame", refFrame},
        {"mission", mission},
        {"ckQuality", ckQuality}
      });
      json out = spiceAPIQuery("getTargetOrientations", args);
      return out["body"]["return"].get<std::vector<std::vector<double>>>();
    }else{
      return SpiceQL::getTargetOrientations(ets, toFrame, refFrame, mission, ckQuality, false);
    }
  }

  double strSclkToEt(int frameCode, std::string sclk, std::string mission, bool useWeb) {
    if (useWeb){
      json args = json::object({
        {"frameCode", frameCode},
        {"mission", mission}
      });
      json out = spiceAPIQuery("strSclkToEt", args);
      return out["body"]["return"].get<double>();
    }else{
      return SpiceQL::strSclkToEt(frameCode, sclk, mission, false);
    }
  }

  double doubleSclkToEt(int frameCode, double sclk, std::string mission, bool useWeb){
    if (useWeb){
      json args = json::object({
        {"frameCode", frameCode},
        {"sclk", sclk},
        {"mission", mission}
      });
      json out = spiceAPIQuery("doubleSclkToEt", args);
      return out["body"]["return"].get<double>();
   }else{
      return SpiceQL::doubleSclkToEt(frameCode, sclk, mission, false);
    }
  }

  double utcToEt(std::string utc, bool useWeb){
    if (useWeb){
      json args = json::object({
        {"utc", utc}
      });
      json out = spiceAPIQuery("utcToEt", args);
      return out["body"]["return"].get<double>();

    }else{
      return SpiceQL::utcToEt(utc, false);
    }
  }

  int translateNameToCode(std::string frame, std::string mission, bool useWeb){
    if (useWeb){
      json args = json::object({
        {"frame", frame},
        {"mission", mission}
      });
      json out = spiceAPIQuery("translateNameToCode", args);
      return out["body"]["return"].get<int>();
    }else{
      return SpiceQL::translateNameToCode(frame, mission, false);
    }
  }

  std::string translateCodeToName(int code, std::string mission, bool useWeb){
    if (useWeb){
      json args = json::object({
        {"code", code},
        {"mission", mission}
      });
      json out = spiceAPIQuery("translateCodeToame", args);
      return out["body"]["return"].get<std::string>();
    }else{
      return SpiceQL::translateCodeToName(code, mission, false);
    }
  }

  std::vector<int> getFrameInfo(int frame, std::string mission, bool useWeb) {
    if (useWeb){
      json args = json::object({
        {"frame", frame},
        {"mission", mission}
      });
      json out = spiceAPIQuery("getFrameInfo", args);
      return out["body"]["return"].get<std::vector<int>>();

    }else{
      return SpiceQL::getFrameInfo(frame, mission, false);
    }
  }

  json getTargetFrameInfo(int targetId, std::string mission, bool useWeb) {
    if (useWeb){
      json args = json::object({
        {"targetId", targetId},
        {"mission", mission}
      });
      json out = spiceAPIQuery("getTargetFrameInfo", args);
      return out["body"]["return"];
    }else{
      return SpiceQL::getTargetFrameInfo(targetId, mission, false);
    }
  }

  json findMissionKeywords(std::string key, std::string mission, bool useWeb){
    if (useWeb){
      json args = json::object({
        {"key", key},
        {"mission", mission}
      });
      json out = spiceAPIQuery("findMissionKeywords", args);
      return out["body"]["return"];
    }else{
      return SpiceQL::findMissionKeywords(key, mission, false);
    }
  }

  json findTargetKeywords(std::string key, std::string mission, bool useWeb){
    if (useWeb){
      json args = json::object({
        {"key", key},
        {"mission", mission}
      });
      json out = spiceAPIQuery("findTargetKeywords", args);
      return out["body"]["return"];
    }else{
      return SpiceQL::findTargetKeywords(key, mission, false);
    }
  }

  std::vector<std::vector<int>> frameTrace(double et, int initialFrame, std::string mission, std::string ckQuality, bool useWeb) {
    if (useWeb){
      json args = json::object({
        {"et", et},
        {"initialFrame", initialFrame},
        {"mission", mission},
        {"ckQuality", ckQuality}
      });
      json out = spiceAPIQuery("frameTrace", args);
      return out["body"]["return"].get<std::vector<std::vector<int>>>();
    }else{
      return SpiceQL::frameTrace(et, initialFrame, mission, ckQuality, false);
    }
  }

  std::vector<double> extractExactCkTimes(double observStart, double observEnd, int targetFrame, std::string mission, std::string ckQuality, bool useWeb) {
    if (useWeb){
      json args = json::object({
        {"observStart", observStart},
        {"observEnd", observEnd},
        {"targetFrame",targetFrame},
        {"mission", mission},
        {"ckQuality", ckQuality}
      });
      json out = spiceAPIQuery("extractExactCkTimes", args);
      return out["body"]["return"].get<std::vector<double>>();
    }else{
      return SpiceQL::extractExactCkTimes(observStart, observEnd, targetFrame, mission, ckQuality, false);
    }
  }

  json spiceAPIQuery(std::string functionName, json args){
    restincurl::Client client;
    std::string queryString = "http://10.12.56.68/" + functionName +"/?";

    for (auto x : args.items())
    {
        queryString+= x.key();
        queryString+= "=";
        try{
          queryString+= x.value();
        }catch(...){
          // @TODO get rid of catch alls
          std::string tmp = x.value().dump();
          try{
            // @TODO mystical nonsense, find a better way to do this. This is checking if the value is numeric or a list.
            (int)x.value();
          }catch(...){
          // if list, get rid of brackets for api call
            tmp.replace(tmp.begin(), tmp.begin()+1, "");
            tmp.replace(tmp.end()-1, tmp.end(), "");
          }
          queryString+= tmp;
        }
        queryString+= "&";
    }
    json j;
    client.Build()->Get(queryString).Option(CURLOPT_FOLLOWLOCATION, 1L).AcceptJson().WithCompletion([&](const restincurl::Result& result) {
      j = json::parse(result.body);
    }).ExecuteSynchronous();
    client.CloseWhenFinished();
    client.WaitForFinish();
    return j;
  }

}
