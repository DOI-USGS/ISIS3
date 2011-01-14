#ifndef ControlNet_h
#define ControlNet_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/07/15 17:33:52 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "PBControlNetIO.pb.h"

#include <QHash>
#include <QString>

#include "iString.h"


namespace Isis {
  class Camera;
  class ControlPoint;
  class iString;
  class Progress;
  class SerialNumberList;

  /**
   * @brief a control network
   *
   * This class is used to store a network of ControlPoints
   *
   * @ingroup ControlNetwork
   *
   * @author 2005-07-29 Jeff Anderson
   *
   * @see ControlPoint ControlMeasure
   *
   * @internal
   *   @history 2005-07-29 Jeff Anderson Original version
   *   @history 2006-01-11 Jacob Danton Updated unitTest
   *   @history 2006-06-22 Brendan George Updated to conform to changes in
   *            SerialNumberList class
   *   @history 2008-04-04 Christopher Austin Added Exists function
   *   @history 2008-04-18 Debbie A. Cook Added Progress reports to loading and
   *            SetImages and calculates the total number of measurements in
   *            the control net
   *   @history 2008-06-18 Christopher Austin Fixed documentation errors
   *   @history 2009-01-06 Jeannie Walldren Fixed typo in
   *            SetImages() exception output. Added
   *            documentation.
   *   @history 2009-02-05 Christopher Austin when the created date or the
   *            modified date are not set, they default to the time in which
   *            Write() is called.
   *   @history 2009-04-07 Tracie Sucharski Added NumValidMeasures and
   *            NumIgnoredMeasures methods.
   *   @history 2009-06-03 Christopher Austin Added p_invalid functionality
   *            along with forceBuild, as well as other small fixes including
   *            documentation.
   *   @history 2009-07-13 Stacy Alley The std::vector of
   *            ControlPoints called 'p_points' was replaced with
   *            a QVector of QString 'p_pointIds' in conjunction
   *            with a QHash of <QString, ControlPoint> called
   *            'p_pointsHash'. This was done to speed up the Add
   *            method which was essentially slowing down the
   *            reading or creation of Control Networks.
   *   @history 2010-01-12 Tracie Sucharski Added support for binary networds,
   *            added new parameters, renamed ComputeErrors to ComputeResiduals,
   *            renamed MaximumError to MaximumResidual, renamed AverageError to
   *            AverageResidual.
   *   @history 2010-05-06 Tracie Sucharski Use defaults of 0. instead of
   *            Isis::Null, because 0. is the default in the protocol buffers.
   *   @history 2010-08-05 Steven Lambright New label format much closer to a
   *            cube so that we can expand upon it easily later. Also added 
   *            support for more than just the protocol buffer in the file, at
   *            the current cost of reading the protocol buffer's binary data
   *            into memory. This might need to be changed later.
   *   @history 2010-08-06 Tracie Sucharski Updated for changes made after
   *            additional working sessions for Control network
   *            design.
   *   @history 2009-09-01 Eric Hyer Added two includes: QVector and QString
   *   @history 2010-09-09 Sharmila Prasad Added API to sort Control Net by
   *            Point ID Changed PointID's vector to StringList
   *   @history 2009-09-25 Travis Addair Changed methods
   *            which return the number of control measures in the
   *            network to compute those values at the time the
   *            method is called, not when the control network is
   *            first initialized.
   *   @history 2010-10-05 Tracie Sucharski Renamed the Write method to
   *            WritePvl.  Create new method, Write which takes another
   *            parameter indicating whether to write pvl format or binary
   *            format, The default will write binary.
   *   @history 2010-10-05 Eric Hyer ControlMeasure and ControlPoint now
   *            return QStrings for some methods.  Fixed breakages caused by
   *            this.
   *   @history 2010-10-06 Sharmila Prasad Added method to get CreatedDate
   *   @history 2010-11-21 Tracie Sucharski - Added new keyword, jigsawRejected
   *            to the read and write methods.
   *   @history 2011-01-13 Mackenzie Boyd Added copy constructor and assignment
   *            operator.
   *
   */
  class ControlNet {
    public:
      ControlNet();
      ControlNet(const ControlNet& other);
      ControlNet(const iString &ptfile,
                 Progress *progress = 0,
                 bool forceBuild = false);

      ~ControlNet();

      void ReadControl(const iString &ptfile,
                       Progress *progress = 0,
                       bool forceBuild = false);
      void ReadPBControl(const iString &ptfile);
      void Write(const iString &ptfile, bool pvl = false);
      void WritePB(const iString &ptfile);
      void WritePvl(const iString &ptfile);

      void Add(ControlPoint point, bool forceBuild = false);
      void Delete (int index);
      void Delete (const iString &id);
      void UpdatePoint(const ControlPoint &point);

      void ComputeResiduals();
      void ComputeApriori();
      void SortControlNet();

      ControlPoint GetPoint(int index) const;

      double AverageResidual();
      Isis::Camera *Camera(int index);
      iString CreatedDate() const;
      iString Description() const;
      bool Exists( ControlPoint &point );
      ControlPoint *Find(const ControlPoint &point);
      ControlPoint *Find(const std::string &id) { return Find(iString(id)); }
      ControlPoint *Find(const iString &id);
      ControlPoint *FindClosest(const iString &serialNumber,
                                double sample, double line);
      bool Invalid() const;
      double MaximumResidual();
      iString NetworkId() const;
      int NumEditLockMeasures();
      int NumEditLockPoints ();
      int NumIgnoredMeasures();
      int NumMeasures();
      int NumPoints() const;
      int NumValidMeasures();
      int NumValidPoints ();
      int Size() const;
      iString Target() const;
      iString UserName() const;

      void SetCreatedDate(const iString &date);
      void SetDescription(const iString &newDescription);
      void SetImages (const iString &imageListFile);
      void SetImages (SerialNumberList &list, Progress *progress=0);
      void SetModifiedDate(const iString &date);
      void SetNetworkId(const iString &id);
      void SetTarget(const iString &target);
      void SetUserName(const iString &name);

      const ControlNet& operator=(ControlNet other);
      ControlPoint operator[](int index) const;
      ControlPoint operator[](iString id) const;

    // Data
    private:
      QStringList p_pointIds;  //!< String List of ControlPoint Ids
      QHash <QString, ControlPoint> p_pointsHash; //!< Hash table of Control Points
      iString p_targetName;            //!< Name of the target
      iString p_networkId;             //!< The Network Id
      iString p_created;               //!< Creation Date
      iString p_modified;              //!< Date Last Modified
      iString p_description;           //!< Textual Description of network
      iString p_userName;              //!< The user who created the network
      int p_numMeasures;          //!< Total number of measures in the network
      int p_numIgnoredMeasures;            //!< Number of ignored measures
      std::map<iString, Isis::Camera *> p_cameraMap; //!< A map from serialnumber to camera
      std::vector<Isis::Camera *> p_cameraList; //!< Vector of image number to camera

      bool p_invalid;  //!< If the Control Network is currently invalid
  };
};

#endif

