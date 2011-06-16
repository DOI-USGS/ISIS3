/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2009/10/31 00:19:38 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <sstream>
#include <fstream>

#include "SessionLog.h"
#include "Process.h"
#include "Filename.h"
#include "Message.h"
#include "iException.h"
#include "iString.h"
#include "Preference.h"
#include "Application.h"
#include "History.h"
#include "OriginalLabel.h"
#include "LineManager.h"

using namespace std;
namespace Isis {

  //! Constructs a Process Object
  Process::Process() {
    p_progress = new Isis::Progress();
    p_progress->SetText("Working");
    p_propagateLabels = true;
    p_propagateTables = true;
    p_propagatePolygons = true;
    p_propagateHistory = true;
    p_propagateOriginalLabel = true;
  }

  //! Destroys the Process Object. It will close all opened cubes
  Process::~Process() {
    EndProcess();
    delete p_progress;
  }

  /**
   * Opens an input cube specified by the programmer and verifies requirements
   * are met.
   *
   * @param fname Programmer specified work file. For example, "myfile.cub".
   *
   * @param att  The cube attributes to use when opening the input cube.
   *
   * @param requirements  Same as requirements on SetInputCube. See that method
   *                      for more details. Defaults to 0
   *
   * @return Cube*
   *
   * @throws Isis::iException::Message
   */
  Isis::Cube *Process::SetInputCube(const std::string &fname,
                                    const Isis::CubeAttributeInput &att,
                                    int requirements) {
    Isis::Cube *cube = new Isis::Cube;
    if(att.Bands().size() != 0) {
      vector<string> lame = att.Bands();
      cube->setVirtualBands(lame);
    }

    try {
      if(requirements & Isis::ReadWrite) {
        cube->open(fname, "rw");
      }
      else {
        cube->open(fname);
      }
    }
    catch(Isis::iException &e) {
      e.Report();
      delete cube;
      throw;
    }
    
    // Test for same size or one in all dimensions
    if(requirements & Isis::AllMatchOrOne) {
      if(InputCubes.size() > 0) {
        if(cube->getLineCount() != 1) {
          if(cube->getLineCount() != InputCubes[0]->getLineCount()) {
            string message = "The number of lines in the secondary input cubes must match";
            message += " the primary input cube or be exactly one";
            throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
          }
        }

        if(cube->getSampleCount() != 1) {
          if(cube->getSampleCount() != InputCubes[0]->getSampleCount()) {
            string message = "The number of samples in the secondary input cubes must match";
            message += " the primary input cube or be exactly one";
            throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
          }
        }
        if(cube->getBandCount() != 1) {
          if(cube->getBandCount() != InputCubes[0]->getBandCount()) {
            string message = "The number of bands in the secondary input cubes must match";
            message += " the primary input cube or be exactly one";
            throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
          }
        }

        // Do not do a spatial match if this flag was set
        requirements = requirements & !Isis::SpatialMatch;
      }
    }

    // Test for size match if requested
    if(requirements & Isis::SizeMatch) {
      if(InputCubes.size() > 0) {
        if(cube->getLineCount() != InputCubes[0]->getLineCount()) {
          string message = "The number of lines in the input cubes must match";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
        if(cube->getSampleCount() != InputCubes[0]->getSampleCount()) {
          string message = "The number of samples in the input cubes must match";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
        if(cube->getBandCount() != InputCubes[0]->getBandCount()) {
          string message = "The number of bands in the input cubes must match";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }

    // Test for spatial match if requested
    if(requirements & Isis::SpatialMatch) {
      if(InputCubes.size() > 0) {
        if(cube->getLineCount() != InputCubes[0]->getLineCount()) {
          string message = "The number of lines in the input cubes must match";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
        if(cube->getSampleCount() != InputCubes[0]->getSampleCount()) {
          string message = "The number of samples in the input cubes must match";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }

    // Test for one band
    if(requirements & Isis::OneBand) {
      if(cube->getBandCount() != 1) {
        string message = "Input cube [" + fname + "] must have one band";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }

    // Test for same bands or one band
    if(requirements & Isis::BandMatchOrOne) {
      if(cube->getBandCount() != 1) {
        if(InputCubes.size() > 0) {
          if(cube->getBandCount() != InputCubes[0]->getBandCount()) {
            string message = "The number of bands in the secondary input cubes must match";
            message += " the primary input cube or be exactly one";
            throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
          }
        }
      }
    }

    // Everything is good so save the cube on the stack
    InputCubes.push_back(cube);
    return cube;
  }

  /**
   * Set the InputCube vector to an opened Cube. This is used if there already exists 
   * a valid opened cube 
   * 
   * @author Sharmila Prasad (5/7/2011)
   * 
   * @param inCube - Pointer to input Cube
   */
  void Process::SetInputCube(Isis::Cube *inCube)
  {
    if(inCube != NULL && inCube->isOpen()) {
      InputCubes.push_back(inCube);
    }
    else {
      string message = "Input cube does not exist";
      throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
    }
  }
  
  /**
   * Opens an input cube specified by the user and verifies requirements are met.
   *
   * @return Cube*
   *
   * @param parameter User parameter to obtain file to open. Typically, the value
   *                  is "FROM". For example, the user can specify on the command
   *                  line FROM=myfile.cub and this method will attempt to open
   *                  the cube "myfile.cub" if the parameter was set to "FROM".
   *
   *
   * @param requirements Use to specify requirements for the input file. The
   *                     following are requirments are checked against
   *                     1) ic_base::SizeMatch checks to make sure the input cube
   *                     has the same bands, samples, and lines as the first
   *                     input cube open using this method,
   *                     2) ic_base::SpatialMatch checks to make sure the input
   *                     cube has the same samples and lines as the first input
   *                     cube, 3) ic_base::Georeferenced checks to make sure
   *                     latitude/longitudes can be obtained from the input cube,
   *                     4) ic_base::FullyGeoreferenced checks to make sure
   *                     latitude/longitude, phase/incidence/emission, and other
   *                     geometric parameters can be obtained from the input
   *                     cube, 5) ic_base::OneBand checks to make sure the input
   *                     cube has exactly one band. Note, that these requirements
   *                     can be logically or'ed. For example,
   *                     ic_base::SpatialMatch | ic_base::georeferenced.
   *                     Defaults to 0
   *
   * @throws Isis::iException::Message
   */
  Isis::Cube *Process::SetInputCube(const std::string &parameter,
                                    const int requirements) {
    string fname = Application::GetUserInterface().GetFilename(parameter);
    Isis::CubeAttributeInput &att = Application::GetUserInterface().GetInputAttribute(parameter);
    return SetInputCube(fname, att, requirements);
  }

  /**
   * Allocates a user-specified output cube whose size matches the first input
   * cube.
   *
   * @return Cube*
   *
   * @param parameter User specified output file. For example, "TO" is a popular
   *                  user parameter. If the user specified TO=output.cub, then
   *                  this routine would allocate the file output.cub with size
   *                  specified by the first opened input cube. The output pixel
   *                  type will be propagated from the first loaded input cube or
   *                  will use the value in the application XML file for
   *                  pixelType.
   *
   * @throws Isis::iException::Message
   */
  Isis::Cube *Process::SetOutputCube(const std::string &parameter) {
    // Make sure we have an input cube to get a default size from
    if(InputCubes.size() == 0) {
      string message = "No input images have been selected ... therefore";
      message += "the output image size can not be determined";
      throw Isis::iException::Message(Isis::iException::Programmer, message, _FILEINFO_);
    }

    int nl = InputCubes[0]->getLineCount();
    int ns = InputCubes[0]->getSampleCount();
    int nb = InputCubes[0]->getBandCount();
    return SetOutputCube(parameter, ns, nl, nb);
  }

  /**
   * Allocates a user specified output cube whose size is specified by the
   * programmer.
   *
   * @return Cube*
   *
   * @param parameter User specified output file. For example, "TO" is a popular
   *                  user parameter. If the user specified TO=output.cub, then
   *                  this routine would allocate the file output.cub with size
   *                  specified by the first opened input cube. The output pixel
   *                  type will be propagated from the first loaded input cube or
   *                  will use the value in the application XML file for
   *                  pixelType.
   *
   * @param ns Number of samples to allocate
   *
   * @param nl Number of lines to allocate
   *
   * @param nb Number of bands to allocate
   *
   * @throws Isis::iException::Message
   */
  Isis::Cube *Process::SetOutputCube(const std::string &parameter, const int ns,
                                     const int nl, const int nb) {
    // Make sure we have good dimensions
    if((ns <= 0) || (nl <= 0) || (nb <= 0)) {
      ostringstream message;
      message << "Invalid cube size specifications [ns=" << ns << ",nl=" << nl
              << ",nb=" << nb << "]";
      throw Isis::iException::Message(Isis::iException::Programmer, message.str(), _FILEINFO_);
    }

    string fname = Application::GetUserInterface().GetFilename(parameter);
    Isis::CubeAttributeOutput &atts = Application::GetUserInterface().GetOutputAttribute(parameter);
    return SetOutputCube(fname, atts, ns, nl, nb);
  }

  /**
   * Allocates a output cube whose name and size is specified by the programmer.
   *
   * @return Cube*
   *
   * @param fname Name of the output cube to allocate
   *
   * @param att The cube attributes to use when creating the output cube.
   *
   * @param ns Number of samples to allocate
   *
   * @param nl Number of lines to allocate
   *
   * @param nb Number of bands to allocate
   *
   * @throws Isis::iException::Message
   */
  Isis::Cube *Process::SetOutputCube(const std::string &fname,
                                     const Isis::CubeAttributeOutput &att,
                                     const int ns, const int nl,
                                     const int nb) {
    // Make sure we have good dimensions
    if((ns <= 0) || (nl <= 0) || (nb <= 0)) {
      ostringstream message;
      message << "Invalid cube size specifications [ns=" << ns << ",nl=" << nl
              << ",nb=" << nb << "]";
      throw Isis::iException::Message(Isis::iException::Programmer, message.str(), _FILEINFO_);
    }

    // Setup the cube
    Isis::Cube *cube = new Isis::Cube;
    try {
      cube->setDimensions(ns, nl, nb);
      cube->setByteOrder(att.ByteOrder());
      cube->setFormat(att.FileFormat());
      cube->setLabelsAttached(att.AttachedLabel());

      if(att.PropagatePixelType()) {
        if(InputCubes.size() > 0) {
          cube->setPixelType(InputCubes[0]->getPixelType());
        }
        else {
          string msg = "You told me to propagate PixelType from input to output";
          msg += " cube but there are no input cubes loaded";
          throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
        }
      }
      else {
        cube->setPixelType(att.PixelType());
      }

      if(att.PropagateMinimumMaximum()) {
        if(cube->getPixelType() == Isis::Real) {
          cube->setBaseMultiplier(0.0, 1.0);
        }
        else if(InputCubes.size() == 0) {
          string msg = "You told me to propagate base/multiplier from input to output";
          msg += " cube but there are no input cubes loaded";
          throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
        }
        else if(cube->getPixelType() >= InputCubes[0]->getPixelType()) {
          double base = InputCubes[0]->getBase();
          double mult = InputCubes[0]->getMultiplier();
          cube->setBaseMultiplier(base, mult);
        }
        else if((cube->getPixelType() != Isis::Real) &&
                (cube->getPixelType() != Isis::UnsignedByte) &&
                (cube->getPixelType() != Isis::SignedWord)) {
          string msg = "Looks like your refactoring to add different pixel types";
          msg += " you'll need to make changes here";
          throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
        }
        else {
          string msg = "You've chosen to reduce your output PixelType for [" +
                       fname + "] you must specify the output pixel range too";
          throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
        }
      }
      else {
        // Not propagating so either the user entered or the programmer did
        cube->setMinMax(att.Minimum(), att.Maximum());
      }

      if(InputCubes.size() > 0) {
        int needLabBytes = InputCubes[0]->getLabelSize(true) + (1024 * 6);
        if(needLabBytes > cube->getLabelSize()) {
          cube->setLabelSize(needLabBytes);
        }
      }

      // Allocate the cube
      cube->create(fname);

      // Transfer labels from the first input cube
      if((p_propagateLabels) && (InputCubes.size() > 0)) {
        Isis::PvlObject &incube =
            InputCubes[0]->getLabel()->FindObject("IsisCube");
        Isis::PvlObject &outcube = cube->getLabel()->FindObject("IsisCube");
        for(int i = 0; i < incube.Groups(); i++) {
          outcube.AddGroup(incube.Group(i));
        }
      }

      // Transfer tables from the first input cube
      if((p_propagateTables) && (InputCubes.size() > 0)) {
        Isis::Pvl &inlab = *InputCubes[0]->getLabel();
        for(int i = 0; i < inlab.Objects(); i++) {
          if(inlab.Object(i).IsNamed("Table")) {
            Isis::Blob t((string)inlab.Object(i)["Name"], inlab.Object(i).Name());
            InputCubes[0]->read(t);
            cube->write(t);
          }
        }
      }

      // Transfer blobs from the first input cube
      if((p_propagatePolygons) && (InputCubes.size() > 0)) {
        Isis::Pvl &inlab = *InputCubes[0]->getLabel();
        for(int i = 0; i < inlab.Objects(); i++) {
          if(inlab.Object(i).IsNamed("Polygon")) {
            Isis::Blob t((string)inlab.Object(i)["Name"], inlab.Object(i).Name());
            InputCubes[0]->read(t);
            cube->write(t);
          }
        }
      }

      // Transfer tables from the first input cube
      if((p_propagateOriginalLabel) && (InputCubes.size() > 0)) {
        Isis::Pvl &inlab = *InputCubes[0]->getLabel();
        for(int i = 0; i < inlab.Objects(); i++) {
          if(inlab.Object(i).IsNamed("OriginalLabel")) {
            Isis::OriginalLabel ol;
            InputCubes[0]->read(ol);
            cube->write(ol);
          }
        }
      }

      // Transfer history from the first input cube
      WriteHistory(*cube);
    }
    catch(Isis::iException &e) {
      delete cube;
      throw;
    }

    // Everything is fine so save the cube on the stack
    OutputCubes.push_back(cube);
    return cube;
  }

  /**
   * End the processing sequence and cleans up by closing cubes, freeing memory,
   * etc.
   */
  void Process::EndProcess() {
    ClearInputCubes();
    ClearOutputCubes();
  }

  /**
   * Close output cubes from the list and clear the list
   * 
   * @author Sharmila Prasad (5/7/2011)
   */
  void Process::ClearOutputCubes(){
    // Close the output cubes
    for(int i = 0; i < (int)OutputCubes.size(); i++) {
      OutputCubes[i]->close();
      delete OutputCubes[i];
    }
    OutputCubes.clear();
  }
  /**
   * This method allows the programmer to turn on/off the propagation of labels
   * from the 1st input cube to any of the output cubes.  By default, propagation
   * occurs automatically in the Process class when a call to either of the
   * SetOutputCube methods is invoked. If the program * requires no propagation
   * then utilize this method. This method can be invoked between successive
   * calls of SetOutputCube so that some cube will have labels propagated while
   * others will not.
   *
   * @param prop Flag indicating if labels are be propagated to output cubes.
   */
  void Process::PropagateLabels(const bool prop) {
    p_propagateLabels = prop;
  }

  /**
   * This method allows the programmer to propagate labels from a specific
   * secondary cube.
   *
   * @param cube iString containing the name of the cube containing the labels
   *             to propagate.
   */
  void Process::PropagateLabels(const std::string &cube) {
    // Open the Pvl file
    Isis::Pvl pvl(cube);

    // Loop for each output cube
    for(int i = 0; i < (int)OutputCubes.size(); i++) {
      Isis::PvlObject &incube = pvl.FindObject("IsisCube");
      Isis::PvlObject &outcube =
          OutputCubes[i]->getLabel()->FindObject("IsisCube");
      for(int g = 0; g < incube.Groups(); g++) {
        outcube.AddGroup(incube.Group(g));
      }
    }
  }

  /**
   * This method allows the programmer to propagate input tables to the output
   * cube (default is true)
   *
   * @param prop Flag indicating if input tables are to be propagated to output
   *             cubes.
   */
  void Process::PropagateTables(const bool prop) {
    p_propagateTables = prop;
  }

  /**
   * This method allows the programmer to propagate input blobs to the output
   * cube (default is true)
   *
   * @param prop Flag indicating if input blobs are to be propagated to output
   *             cubes.
   */
  void Process::PropagatePolygons(const bool prop) {
    p_propagatePolygons = prop;
  }

  /**
   * This method allows the programmer to propagate history to the output cube
   * (default is true)
   *
   * @param prop Flag indicating if history is to be propagated to output cubes.
   */
  void Process::PropagateHistory(const bool prop) {
    p_propagateHistory = prop;
  }

  /**
  * This method allows the programmer to propagate original labels
  * to the output cube (default is true)
  *
  * @param prop Flag indicating if original labels is to be propagated
  * to output cubes.
  */
  void Process::PropagateOriginalLabel(const bool prop) {
    p_propagateOriginalLabel = prop;
  }

  /**
   * This method reads the mission specific data directory from the user
   * preference file, makes sure that mission is available in the Isis
   * installation, and the attaches the provided input file to the directory in
   * order to create a full file specification.
   *
   * @param mission Name of the mission data directory
   *
   * @param file Name of the file to attach to the end of the directory
   *
   * @param highestVersion If set to true the method will return the highest
   *                       version number of the given file. Therefore, file must
   *                       contain question marks such as "file???.dat". See the
   *                       Filename class for more information on versioned
   *                       files. Defaults to false.
   */
  string Process::MissionData(const std::string &mission, const std::string &file,
                              bool highestVersion) {
    Isis::PvlGroup &dataDir = Isis::Preference::Preferences().FindGroup("DataDirectory");
    string dir = dataDir[mission];

    // See if the data directory is installed
    Isis::Filename installed(dir);
    if(!installed.Exists()) {
      string message = "Data directory for mission [" + mission + "] " +
                       "is not installed at your site";
      throw Isis::iException::Message(Isis::iException::Io, message, _FILEINFO_);
    }

    Isis::Filename expanded(dir + "/" + file);
    if(highestVersion) expanded.HighestVersion();
    return expanded.Expanded();
  }

  /**
   * This method clears the input cube list.  It is most commonly used in
   * applications which need to generate an output cube without an input cube,
   * however, propogation of input characteristics are known. For example,
   * creating the initial cube for a mosaic.
   */
  void Process::ClearInputCubes() {
    // Close the input cubes
    for(int i = 0; i < (int)InputCubes.size(); i++) {
      InputCubes[i]->close();
      delete InputCubes[i];
    }
    InputCubes.clear();
  }

  /**
   * Writes out the History blob to the cube
   */
  void Process::WriteHistory(Cube &cube) {
    if(p_propagateHistory) {
      bool addedHist = false;
      if(InputCubes.size() > 0) {
        Isis::Pvl & inlab = *InputCubes[0]->getLabel();
        for(int i = 0; i < inlab.Objects(); i++) {
          if(inlab.Object(i).IsNamed("History") && Isis::iApp != NULL) {
            Isis::History h((string)inlab.Object(i)["Name"]);
            InputCubes[0]->read(h);
            h.AddEntry();
            cube.write(h);
            addedHist = true;
          }
        }
      }

      if(!addedHist && Isis::iApp != NULL) {
        Isis::History h("IsisCube");
        h.AddEntry();
        cube.write(h);
      }
    }
  }

  /**
   * Calculates and stores off statistics on every band of every
   * cube added to this process via the SetInputCube method.
   *
   * The newly calculated statistics are stored in two ways: as a
   * vector where each entry is a single Statistics object for
   * every band of a particular input cube, and as a vector where
   * each entry is a vector of Statistics objects, for each band
   * separately, of a particular input cube.
   */
  void Process::CalculateStatistics() {
    for(unsigned cubeNum = 0; cubeNum < InputCubes.size(); cubeNum++) {
      Cube *cube = InputCubes[cubeNum];

      // Construct a line buffer manager and a statistics object
      Isis::LineManager line(*cube);
      Isis::Statistics *cubeStats = new Isis::Statistics();

      int bandStart = 1;
      int bandStop = cube->getBandCount();
      int maxSteps = cube->getLineCount() * cube->getBandCount();

      iString cubeNumStr((int)cubeNum + 1);
      iString totalCubes((int)InputCubes.size());
      string msg = "Calculating statistics for cube " + cubeNumStr + " of " + totalCubes;

      Isis::Progress progress;
      progress.SetText(msg);
      progress.SetMaximumSteps(maxSteps);
      progress.CheckStatus();

      // Loop and get the statistics for a good minimum/maximum
      vector<Statistics *> allBandStats;
      for(int useBand = bandStart; useBand <= bandStop; useBand++) {
        Isis::Statistics *bandStats = new Isis::Statistics();

        for(int i = 1; i <= cube->getLineCount(); i++) {
          line.SetLine(i, useBand);
          cube->read(line);
          bandStats->AddData(line.DoubleBuffer(), line.size());
          cubeStats->AddData(line.DoubleBuffer(), line.size());
          progress.CheckStatus();
        }

        allBandStats.push_back(bandStats);
      }

      p_bandStats.push_back(allBandStats);
      p_cubeStats.push_back(cubeStats);
    }
  }

} // end namespace isis

