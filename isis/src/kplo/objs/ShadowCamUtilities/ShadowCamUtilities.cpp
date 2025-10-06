#include "ShadowCamUtilities.h"

using namespace std;

namespace Isis {

  /**
   * @brief function to return data index based on channel, channel width, and pixel column
   *
   * @param channel the channel of the ccd
   * @param channel_width the width in pixels of shc channel
   * @param pixel  the sample value 
   *
   * @return size_t position in cube
   */
  size_t GetDataIndex(int channel, int channel_width, int pixel) {
    return (channel * channel_width + pixel);
  }

   /**
   * @brief function to set 8 bit max and min to special pixels
   *
   * @param in line buffer in
   *
   * @return float converted value
   */
  float Set8bitMaxMintoSpecialPixelsHIS4LIS4(uint16_t in){
    if(in == 0) 
      return LOW_INSTR_SAT4;
    else if (in == 255)
      return HIGH_INSTR_SAT4;
    else{
      return static_cast<float>(in);
    }
  }

    /**
   * @brief function to set check if pixel is special pixel.
   *        This should be checked after decompanding since in decompanding step, 
   *        0 and 255 are set only to HIS and LIS. You may miss LRS, HRS after 
   *        decompanding
   *
   * @param in line buffer in
   *
   * @return bool
   */
  bool IsSpecialPixelSHC(double pixel) {
    return IsNullPixel(pixel) || IsHighPixel(pixel) || IsHrsPixel(pixel) || IsLowPixel(pixel) || IsLrsPixel(pixel);
  }

  /**
   * @brief function to set special pixels to 0 or 255
   *
   * @param in line buffer in
   *
   * @return double Converted value
   */
  uint16_t Set_LIS_HIS_SpecialPixelsTo_0_255(uint16_t in){
    if(IsNullPixel(in) || IsLowPixel(in) )
      return 0;
    else if (IsHighPixel(in))
      return 255;
    else{
      QString msg = "WARNING: Pixel value is not (LOW_INSTR_SAT8, HIGH_INSTR_SAT8, NULL). Contact programmer to update code to handle this.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  /**
   * @brief function to checks if file versioned
   *        and if it is, will return the expanded
   *        highest version available. 
   *
   * @param q_filename QString filename
   *
   * @return returned as string
   */
  string GetVersionedFilename(QString q_filename){

    FileName fileName(q_filename);

    if(fileName.isVersioned())
      fileName = fileName.highestVersion();
    
    if(!fileName.fileExists()) {
      QString msg = fileName.expanded() + " does not exist.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
      
    return fileName.expanded().toStdString();
  }

  /**
   * This is the function that is called to get 
   * the value from a keyword in the 
   * KPLO ShadowCam object labels.
   *
   * @param PvlContainer pvlContainer - contains key:value pairs in labels
   * @param QString pvlKeyword - the keyboard string we want the value from
   *
   * @return QString value for that keyword
   * @internal
   *    @history 2024-07-12 Victor Silva
   */
  QString GetFromLabels(PvlContainer pvlContainer, QString pvlKeyword){
    if(!pvlContainer.hasKeyword(pvlKeyword)){
      QString msg = "Error: keyword: ";
              msg += pvlKeyword;
              msg += " was not found in labels.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    return ((QString)pvlContainer[pvlKeyword]);
  }

  /**
   * @brief function to get tdi factor from tdi direction 
   *
   * @param PvleGroup instrument
   * @return returned as integer
   */
  int GetTdiFactor(PvlGroup instrument){
    // Default value for tdi direction "A" is zero for tdi factor
    int tdi_factor = 0;
    if (!instrument.hasKeyword("TDIDirection"))
      throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
    
    if (QString::compare(instrument["TDIDirection"], "B", Qt::CaseInsensitive) == 0)
      tdi_factor = 1;
    
    return tdi_factor;
  }

  /**
   * @brief function to convert strings to lowercase
   *
   * @param string str
   * @return returned as string
   */
  std::string ToLower(const std::string& str){
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {return std::tolower(c);});
    return result;
  }

  /**
   * @brief function to check if a line contains any keyword
   *
   * @param string line
   * @param string vector keywords
   * @return returned as boolean
   */
  bool ContainsKeyword(const std::string& line, const vector<std::string>& keywords) {
    for (const auto& keyword : keywords) {
      if (line.find(keyword) != std::string::npos) {
        return true;
      }
    }
    return false;
  }
}
