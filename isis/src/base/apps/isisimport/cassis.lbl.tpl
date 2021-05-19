Object = IsisCube
  Object = Core
    StartByte   = 65537
    Format      = Tile
    TileSamples = 512
    TileLines   = 280

    Group = Dimensions
      Samples = {{Product_Observational.File_Area_Observational.Array_2D_Image.Axis_Array.0.elements}}
      Lines   = {{Product_Observational.File_Area_Observational.Array_2D_Image.Axis_Array.1.elements}}
      Bands   = {%if exists("Product_Observational.File_Area_Observational.Array_2D_Image.Axis_Array.2.elements")%}{{Product_Observational.File_Area_Observational.Array_2D_Image.Axis_Array.2.elements}}{%else%}1{%endif%}
    End_Group

    Group = Pixels
      Type       = Real
      ByteOrder  = Lsb
      Base       = 0.0
      Multiplier = 1.0
    End_Group
  End_Object

  Group = Instrument
    SpacecraftName            = {%if exists("Product_Observational.Identification_Area.name")%}"{{Product_Observational.Identification_Area.name}}"{%else%}"{{Product_Observational.Observation_Area.Investigation_Area.Instrument_Host_Name}}"{%endif%}
    InstrumentId              = {{Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name}}
    TargetName                = {%if exists("Product_Observational.Target_Identification.name")%}{{Product_Observational.Observation_Area.Target_Identification.name}}{%else%}{{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.TARGET}}{%endif%}
    StartTime                 = {%if exists("Product_Observational.Observation_Area.Time_Coordinates.start_date_time")%}{{Product_Observational.Observation_Area.Time_Coordinates.start_date_time}}{%else%}2016-11-22T15:45:50.984{%endif%}
    SpacecraftClockStartCount = 2f014e8bf23d21d1
    ExposureDuration          = 2.880e-003 <seconds>
    Filter                    = RED
    Expanded                  = 1
    SummingMode               = 0
  End_Group

  Group = Archive
    {%if exists("Product_Observational.Identification_Area.Alias_List")%}ObservationId             = {{Product_Observational.Identification_Area.Alias_List.Alias.alternate_id}}{%endif%}
    DataSetId                    = {{Product_Observational.Identification_Area.logical_identifier}}
    {%if exists("Product_Observational.Identification_Area.version_id")%}ProductVersionId             = {{Product_Observational.Identification_Area.version_id}}"{%endif%}
    {%if exists("Product_Observational.Identification_Area.Producer_data")%}ProducerId                 = {{Product_Observational.Identification_Area.Producer_data.Producer_id}}{%endif%}
    {%if exists("Product_Observational.Identification_Area.Producer_data")%}ProducerName                 = "{{Product_Observational.Identification_Area.Producer_data.Producer_full_name}}"{%endif%}
    {%if exists("Product_Observational.File_Area_Observational.File.creation_date_time")%}ProductCreationTime          = {{Product_Observational.File_Area_Observational.File.creation_date_time}}{%endif%}
    FileName                     = {{Product_Observational.File_Area_Observational.File.file_name}}
    ScalingFactor                = {{Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.scaling_factor}}
    {%if exists("Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.offset")%}Offset                       = {{Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.offset}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}PredictMaximumExposureTime   = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.PREDICTED_MAXIMUM_EXPOSURE_TIME}} <ms> {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}CassisOffNadirAngle          = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.CASSIS_OFF_NADIR_ANGLE}} <deg> {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}PredictedRepetitionFrequency = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.PREDICTED_REQUIRED_REPETITION_FREQUENCY}} <ms> {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}GroundTrackVelocity          = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.TGO_GROUND_TRACK_VELOCITY}} <km/s> {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}ForwardRotationAngle         = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.FORWARD_ROTATION_ANGLE_REQUIRED}} <deg> {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}SpiceMisalignment            = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.SPICE_KERNEL_MISALIGNMENT_PREDICT}} <deg> {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}FocalLength                  = {{Product_Observational.CaSSIS_Header.CaSSIS_General.TELESCOPE_FOCAL_LENGTH}} <m> {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}FNumber                      = {{Product_Observational.CaSSIS_Header.CaSSIS_General.TELESCOPE_F_NUMBER}} {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}ExposureTimeCommand          = {{Product_Observational.CaSSIS_Header.CaSSIS_General.IMAGE_COMMAND.T_exp.0}} {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}FrameletNumber               = {{Product_Observational.CaSSIS_Header.FSW_HEADER}} {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}NumberOfFramelets            = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.Num_exp}} {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}ImageFrequency               = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.Step_exp}} <ms> {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}NumberOfWindows              = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.Num_win}} {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}UniqueIdentifier             = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.Unique_Identifier}} {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}UID                          = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.FSW_HEADER.@UID}} {%endif%}
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
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}ReadNoise                = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_READ_NOISE}} {%endif%}
    ReadNoiseUnit                = ELECTRON
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}MissionPhase                 = {{Product_Observational.CaSSIS_Header.DERIVED_HEADER_DATA.MissionPhase}} {%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}SubInstrumentIdentifier      = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_READ_NOISE}} {%endif%}
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
