#ifndef ShadowCamUtilities_h
#define ShadowCamUtilities_h
/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <QString>

#include "PvlContainer.h"
#include "PvlGroup.h"

namespace Isis {
namespace ShadowCam {

  /**
   * @brief Sets 8 bit max and min to special pixels.
   *
   * @param in Input line buffer
   *
   * @return float Converted value
   */
  float Set8bitMaxMintoSpecialPixelsHIS4LIS4(std::uint16_t in);

  /**
   * @brief Sets special pixels to 0 or 255.
   *
   * @param in Input line buffer
   *
   * @return uint16_t Converted value
   */
  std::uint16_t Set_LIS_HIS_SpecialPixelsTo_0_255(std::uint16_t in);

  /**
   * @brief Checks if pixel is a special pixel.
   * @details This should be checked after decompanding since in decompanding step, 
   *          0 and 255 are set only to HIS and LIS. You may miss LRS or HRS after 
   *          decompanding.
   *
   * @param in Input line buffer
   *
   * @return bool True if pixel is special pixel, false if not.
   */
  bool IsSpecialPixelSHC(double in);

  /**
   * @brief Return data index based on channel, channel width, and pixel column
   *
   * @param channel The channel of the ccd
   * @param channel_width The width in pixels of shc channel
   * @param pixel The sample value 
   *
   * @return size_t Data index in cube
   */
  std::size_t GetDataIndex(int channel, int channel_width, int pixel);

  /**
   * @brief Gets the highest version of a filename and expands any ISIS variables in it, if present.
   *
   * @param filename The input filename
   *
   * @throws IException::User If the file does not exist after version resolution and variable expansion
   * 
   * @return std::string The version-resolved and expanded filename as an std::string
   */
  std::string GetVersionedFilename(const QString &filename);

  /**
   * @brief Get the value from a keyword in the KPLO ShadowCam object labels.
   *
   * @param pvlContainer The PVL container in which to search for the keyword
   * @param pvlKeyword The PVL keyword from which to retrieve the value 
   *
   * @throws IException::User If the keyword is not found in the PVL container
   * 
   * @return QString The value for that keyword
   * @internal
   *    @history 2024-07-12 Victor Silva
   */
  QString GetFromLabels(const PvlContainer &pvlContainer, const QString &pvlKeyword);

  /**
   * @brief Get the TDI factor from TDI direction.
   *
   * @param instrument The PVL group containing the TDIDirection keyword
   * 
   * @throws IException::User If the TDIDirection keyword is not found in the instrument PVL group
   * @throws IException::User If the TDIDirection value is not "A" or "B"
   * 
   * @return int The TDI factor
   * 
   * @internal
   *    @history 2026-03-18 Cordell Michaud - Updated to throw an error if TDIDirection value is not "A" or "B", rather than defaulting to 0.
   */
  int GetTdiFactor(const PvlGroup &instrument);

  /**
   * @brief Converts a string to lowercase.
   *
   * @param str The string to convert
   * 
   * @return string The string converted to lowercase
   * 
   * @internal
   *    @history 2026-03-18 Cordell Michaud - Modified to use QString for conversion and return std::string.
   */
  std::string ToLower(const std::string &str);

  /**
   * @brief Check if a line contains any keyword
   *
   * @param line The line in which to check for keywords
   * @param keywords The vector of keywords to search for
   * 
   * @return bool True if any keyword is found, false otherwise
   */
  bool ContainsKeyword(const std::string &line, const std::vector<std::string> &keywords);

  /**
   * @brief Check of image is a calibrated image
   *
   * @param file A FileName object to read
   * 
   * @return bool True if the file is a calibrated file, False otherwise
   */
   bool IsCalibrated(const FileName &file);
}
}

#endif