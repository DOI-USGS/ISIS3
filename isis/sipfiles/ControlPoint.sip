namespace Isis {

  class ControlPoint : public QObject {
    %TypeHeaderCode
      #include "ControlPoint.h"
    %End
    public:
      enum PointType {
        Fixed = 0,
        Constrained = 1,
        Free = 2
      };


      enum Status {
        Failure,
        Success,
        PointLocked
      };

      enum ConstraintStatus {
        LatitudeConstrained = 0,
        LongitudeConstrained = 1,
        RadiusConstrained = 2,
//      XConstrained = 3,
//      YConstrained = 4,
//      ZConstrained = 5;
      };

      // This stuff input to jigsaw
      // How did apriori source get computed??
      struct SurfacePointSource {
        enum Source {
          None,
          User,
          AverageOfMeasures,
          Reference,
          Basemap,
          BundleSolution
        };
      };

      struct RadiusSource {
        enum Source {
          None,
          User,
          AverageOfMeasures,
          Ellipsoid,
          DEM,
          BundleSolution
        };
      };

      ControlPoint() throw(Isis::IException);
      ControlPoint(const QString &id) throw(Isis::IException);

      // Isis::ControlNet *Parent() { return parentNetwork; }

      // void Load(PvlObject &p) throw(Isis::IException);

      // void Add(ControlMeasure *measure) throw(Isis::IException);
      // int Delete(ControlMeasure *measure) throw(Isis::IException);
      // int Delete(QString serialNumber) throw(Isis::IException);
      // int Delete(int index) throw(Isis::IException);
      // Status ResetApriori() throw(Isis::IException);

      // const Isis::ControlMeasure *GetMeasure(QString serialNumber) const throw(Isis::IException);
      // ControlMeasure *GetMeasure(QString serialNumber) throw(Isis::IException);

      // const ControlMeasure *GetMeasure(int index) const throw(Isis::IException);
      // ControlMeasure *GetMeasure(int index) throw(Isis::IException);

      // const ControlMeasure *GetRefMeasure() const throw(Isis::IException);
      // ControlMeasure *GetRefMeasure() throw(Isis::IException);

      // Status SetChooserName(QString name) throw(Isis::IException);
      // Status SetDateTime(QString newDateTime) throw(Isis::IException);
      // Status SetEditLock(bool editLock) throw(Isis::IException);
      // Status SetId(QString id) throw(Isis::IException);
      // Status SetRefMeasure(ControlMeasure *cm) throw(Isis::IException);
      // Status SetRefMeasure(int index) throw(Isis::IException);
      // Status SetRefMeasure(QString sn) throw(Isis::IException);
      // Status SetRejected(bool rejected) throw(Isis::IException);
      // Status SetIgnored(bool newIgnoreStatus) throw(Isis::IException);
      // Status SetAdjustedSurfacePoint(SurfacePoint newSurfacePoint) throw(Isis::IException);
      // Status SetType(PointType newType) throw(Isis::IException);
      //
      // Status SetAprioriRadiusSource(RadiusSource::Source source) throw(Isis::IException);
      // Status SetAprioriRadiusSourceFile(QString sourceFile) throw(Isis::IException);
      // Status SetAprioriSurfacePoint(SurfacePoint aprioriSP) throw(Isis::IException);
      // Status SetAprioriSurfacePointSource(SurfacePointSource::Source source) throw(Isis::IException);
      // Status SetAprioriSurfacePointSourceFile(QString sourceFile) throw(Isis::IException);

      // Status UpdateSphericalPointCoordinates(const Latitude &lat, const Longitude &lon,
      //             const Distance &radius) throw(Isis::IException);

      // Status ComputeApriori() throw(Isis::IException);
      // Status ComputeResiduals() throw(Isis::IException);
      // Status ComputeResiduals_Millimeters() throw(Isis::IException);
      //
      // SurfacePoint GetAdjustedSurfacePoint() const throw(Isis::IException);
      // SurfacePoint GetBestSurfacePoint() const throw(Isis::IException);

      QString GetChooserName() const throw(Isis::IException);
      QString GetDateTime() const throw(Isis::IException);
      bool IsEditLocked() const throw(Isis::IException);
      bool IsRejected() const throw(Isis::IException);
      QString GetId() const throw(Isis::IException);
      bool IsIgnored() const throw(Isis::IException);
      bool IsValid() const throw(Isis::IException);
      bool IsInvalid() const throw(Isis::IException);
      bool IsFixed() const throw(Isis::IException);

      bool HasAprioriCoordinates() throw(Isis::IException);
      bool IsConstrained() throw(Isis::IException);
      bool IsLatitudeConstrained() throw(Isis::IException);
      bool IsLongitudeConstrained() throw(Isis::IException);
      bool IsRadiusConstrained() throw(Isis::IException);
      int NumberOfConstrainedCoordinates() throw(Isis::IException);

      static QString PointTypeToString(PointType type) throw(Isis::IException);
      static PointType StringToPointType(QString pointTypeString) throw(Isis::IException);

      QString GetPointTypeString() const throw(Isis::IException);
      // PointType GetType() const throw(Isis::IException);

      static QString RadiusSourceToString(RadiusSource::Source source) throw(Isis::IException);
      static RadiusSource::Source StringToRadiusSource(QString str) throw(Isis::IException);
      QString GetRadiusSourceString() const throw(Isis::IException);
      static QString SurfacePointSourceToString(SurfacePointSource::Source source) throw(Isis::IException);
      // static SurfacePointSource::Source StringToSurfacePointSource(QString str) throw(Isis::IException);
      QString GetSurfacePointSourceString() const throw(Isis::IException);

      // SurfacePoint GetAprioriSurfacePoint() const throw(Isis::IException);
      // RadiusSource::Source GetAprioriRadiusSource() const throw(Isis::IException);
      // QString GetAprioriRadiusSourceFile() const throw(Isis::IException);
      // SurfacePointSource::Source GetAprioriSurfacePointSource() const throw(Isis::IException);
      // QString GetAprioriSurfacePointSourceFile() const throw(Isis::IException);

      int GetNumMeasures() const throw(Isis::IException);
      int GetNumValidMeasures() const throw(Isis::IException);
      int GetNumLockedMeasures() const throw(Isis::IException);
      bool HasSerialNumber(QString serialNumber) const throw(Isis::IException);
      // int IndexOf(ControlMeasure *, bool throws = true) const throw(Isis::IException);
      int IndexOf(QString sn, bool throws = true) const throw(Isis::IException);
      int IndexOfRefMeasure() const throw(Isis::IException);
      bool IsReferenceExplicit() const throw(Isis::IException);
      QString GetReferenceSN() const throw(Isis::IException);

      // Statistics GetStatistic(double(ControlMeasure::*statFunc)() const) const throw(Isis::IException);
      // Statistics GetStatistic(long dataType) const throw(Isis::IException);

      // QList< ControlMeasure * > getMeasures(bool excludeIgnored = false) const throw(Isis::IException);
      // QList< QString > getCubeSerialNumbers() const throw(Isis::IException);

      // const ControlMeasure *operator[](QString serialNumber) const throw(Isis::IException);
      // ControlMeasure *operator[](QString serialNumber) throw(Isis::IException);

      // const ControlMeasure *operator[](int index) const throw(Isis::IException);
      // ControlMeasure *operator[](int index) throw(Isis::IException);

      // bool operator!=(const ControlPoint &pPoint) const throw(Isis::IException);
      // bool operator==(const ControlPoint &pPoint) const throw(Isis::IException);
      // const ControlPoint &operator=(const ControlPoint &pPoint) throw(Isis::IException);

      // The next 7 methods are specifically to support BundleAdjust
      void ZeroNumberOfRejectedMeasures() throw(Isis::IException);
      void SetNumberOfRejectedMeasures(int numRejected) throw(Isis::IException);
      int GetNumberOfRejectedMeasures() const throw(Isis::IException);
      double GetSampleResidualRms() const throw(Isis::IException);
      double GetLineResidualRms() const throw(Isis::IException);
      double GetResidualRms() const throw(Isis::IException);
      void ClearJigsawRejected() throw(Isis::IException);

      // ControlPointFileEntryV0002 ToFileEntry() const throw(Isis::IException);

    private:
      // void SetExplicitReference(ControlMeasure *measure) throw(Isis::IException);
      void ValidateMeasure(QString serialNumber) const throw(Isis::IException);
      // void AddMeasure(ControlMeasure *measure) throw(Isis::IException);
      void PointModified() throw(Isis::IException);


    // private:
    //   ControlNet *parentNetwork;
    //
    //   //!< List of Control Measures
    //   QHash< QString, ControlMeasure * > * measures;
    //
    //   QStringList *cubeSerials;
    //
    //   ControlMeasure *referenceMeasure;
    //
    //   /**
    //    * This is the control point ID. This is supposed to be a unique
    //    *   identifier for control points. This often has a number in it, and
    //    *   looks like "T0052" where the next one is "T0053" and so on.
    //    */
    //   QString id;
    //
    //   /**
    //    * This is the user name of the person who last modified this control
    //    *   point. Modifications are things like updating the surface point, but
    //    *   not things like updating the last modified time. The calculations
    //    *   relating to this control point have to actually change for this to
    //    *   be updated. This is an empty string if we need to dynamically
    //    *   get the username of the caller when asked for (or written to file).
    //    */
    //   QString chooserName;
    //
    //   /**
    //    * This is the last modified date and time. This is updated automatically
    //    *   and works virtually in the same way as chooserName.
    //    */
    //   QString dateTime;
    //
    //   /**
    //    * What this control point is tying together.
    //    * @see PointType
    //    */
    //   PointType type;
    //
    //   /**
    //    * If we forced a build that we would normally have thrown an exception
    //    *   for then this is set to true. Otherwise, and most of the time, this
    //    *   is false.
    //    */
    //   bool invalid;
    //
    //   /**
    //    * This stores the edit lock state.
    //    * @see SetEditLock
    //    */
    //   bool editLock;
    //
    //   /**
    //    * This stores the jigsaw rejected state.
    //    * @see SetJigsawReject
    //    */
    //   bool jigsawRejected;
    //
    //   /**
    //    * This stores the constraint status of the a priori SurfacePoint
    //    *   @todo Eventually add x, y, and z
    //    */
    //   std::bitset<6> constraintStatus;
    //
    //   /**
    //    * This indicates if a program has explicitely set the reference in this
    //    *   point or the implicit reference is still the current reference. This
    //    *   is useful for programs that want to choose the reference for all
    //    *   points where this hasn't happened yet.
    //    */
    //   bool referenceExplicitlySet;
    //
    //   /**
    //    * True if we should preserve but ignore the entire control point and its
    //    *   measures.
    //    */
    //   bool ignore;
    //
    //   //! Where the apriori surface point originated from
    //   SurfacePointSource::Source aprioriSurfacePointSource;
    //
    //   //! FileName where the apriori surface point originated from
    //   QString aprioriSurfacePointSourceFile;
    //
    //   /**
    //    * Where the apriori surface point's radius originated from, most commonly
    //    *   used by jigsaw.
    //    */
    //   RadiusSource::Source aprioriRadiusSource;
    //
    //   /**
    //    * The name of the file that derives the apriori surface point's radius
    //    */
    //   QString aprioriRadiusSourceFile;
    //
    //   /**
    //    * The apriori surface point. This is the "known truth" or trustworthy
    //    *   point that should not be modified unless done very explicitely. This
    //    *   comes from places like hand picking where you really don't want the
    //    *   surface point to vary far from this point, but some variation is
    //    *   okay (1/10th of a pixel is fair for human accuracy for example). Very
    //    *   often this point does not exist.
    //    */
    //   SurfacePoint aprioriSurfacePoint;
    //
    //   /**
    //    * This is the calculated, or aposterori, surface point. This is what most
    //    *   programs should be working with and updating.
    //    */
    //   SurfacePoint adjustedSurfacePoint;
    //
    //   /**
    //    * This parameter is used and maintained by BundleAdjust for the jigsaw
    //    * application.  It is stored here because ControlPoint contains the index
    //    * of the measures.
    //    */
    //   int numberOfRejectedMeasures;
  };
};
