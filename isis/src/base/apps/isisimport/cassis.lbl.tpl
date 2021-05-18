Object = IsisCube
  Object = Core
    StartByte   = 65537
    Format      = Tile
    TileSamples = 512
    TileLines   = 280

    Group = Dimensions
      Samples = {{Product_Observational.File_Area_Observational.Array_2D_Image.Axis_Array.0.elements}}
      Lines   = {{Product_Observational.File_Area_Observational.Array_2D_Image.Axis_Array.1.elements}}
      Bands   = {{1}}
    End_Group

    Group = Pixels
      Type       = Real
      ByteOrder  = Lsb
      Base       = 0.0
      Multiplier = 1.0
    End_Group
  End_Object

  Group = Instrument
    SpacecraftName            = "TRACE GAS ORBITER"
    InstrumentId              = CaSSIS
    TargetName                = Mars
    StartTime                 = 2016-11-22T15:45:50.984
    SpacecraftClockStartCount = 2f014e8bf23d21d1
    ExposureDuration          = 2.880e-003 <seconds>
    Filter                    = PAN
    Expanded                  = 1
    SummingMode               = 0
  End_Group

  Group = Archive
    DataSetId                    = TBD
    ProductVersionId             = UNK
    ProducerId                   = UBE
    ProducerName                 = "Nicolas Thomas"
    ProductCreationTime          = 2017-10-03T09:38:00
    FileName                     = CAS-MCO-2016-11-22T15.45.50.984-PAN-00000--
                                   B1
    ScalingFactor                = 1.00
    Offset                       = 0.00
    PredictMaximumExposureTime   = 423.6358 <ms>
    CassisOffNadirAngle          = 9.899 <deg>
    PredictedRepetitionFrequency = 97605.7 <ms>
    GroundTrackVelocity          = 0.5735 <km/s>
    ForwardRotationAngle         = 158.600 <deg>
    SpiceMisalignment            = 1.235 <deg>
    FocalLength                  = 0.8770 <m>
    FNumber                      = 6.50
    ExposureTimeCommand          = 300
    FrameletNumber               = 0
    NumberOfFramelets            = 30
    ImageFrequency               = 4000000 <ms>
    NumberOfWindows              = 6
    UniqueIdentifier             = 100731832
    UID                          = 100731832
    ExposureTimestamp            = 2f014e8bf23d21d1
    ExposureTimePEHK             = 2.880e-003 <ms>
    PixelsPossiblySaturated      = 0.00
    IFOV                         = 1.140e-005
    IFOVUnit                     = rad/px
    FiltersAvailable             = "BLU RED NIR PAN"
    FocalLengthUnit              = M
    TelescopeType                = "Three-mirror anastigmat with powered fold
                                    mirror"
    DetectorDescription          = "2D Array"
    PixelHeight                  = 10.0
    PixelHeightUnit              = MICRON
    PixelWidth                   = 10.0
    PixelWidthUnit               = MICRON
    DetectorType                 = "SI CMOS HYBRID (OSPREY 2K)"
    ReadNoise                    = 61.0
    ReadNoiseUnit                = ELECTRON
    MissionPhase                 = MCO
    SubInstrumentIdentifier      = 61.0
    WindowCount                  = 0
    Window1Binning               = 0
    Window1StartSample           = 0
    Window1EndSample             = 2047
    Window1StartLine             = 354
    Window1EndLine               = 633
    Window2Binning               = 0
    Window2StartSample           = 0
    Window2EndSample             = 2047
    Window2StartLine             = 712
    Window2EndLine               = 966
    Window3Binning               = 0
    Window3StartSample           = 0
    Window3EndSample             = 2047
    Window3StartLine             = 1048
    Window3EndLine               = 1302
    Window4Binning               = 0
    Window4StartSample           = 0
    Window4EndSample             = 2047
    Window4StartLine             = 1409
    Window4EndLine               = 1662
    Window5Binning               = 0
    Window5StartSample           = 640
    Window5EndSample             = 767
    Window5StartLine             = 200
    Window5EndLine               = 208
    Window6Binning               = 0
    Window6StartSample           = 1280
    Window6EndSample             = 1407
    Window6StartLine             = 1850
    Window6EndLine               = 1858
    YearDoy                      = 2016327
    ObservationId                = CRUS_049185_238_0
  End_Group

  Group = BandBin
    FilterName = PAN
    Center     = 675 <nm>
    Width      = 250 <nm>
    NaifIkCode = -143421
  End_Group

  Group = Kernels
    NaifFrameCode = -143400
  End_Group

  Group = AlphaCube
    AlphaSamples        = 2048
    AlphaLines          = 2048
    AlphaStartingSample = 0.5
    AlphaStartingLine   = 354.5
    AlphaEndingSample   = 2048.5
    AlphaEndingLine     = 634.5
    BetaSamples         = 2048
    BetaLines           = 280
  End_Group
End_Object

Object = Label
  Bytes = 65536
End_Object

Object = History
  Name      = IsisCube
  StartByte = 2359297
  Bytes     = 572
End_Object

Object = OriginalXmlLabel
  Name      = IsisCube
  StartByte = 2359869
  Bytes     = 13408
  ByteOrder = Lsb
End_Object
End
