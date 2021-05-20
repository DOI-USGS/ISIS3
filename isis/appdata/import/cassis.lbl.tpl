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
    SpacecraftName            = {%if exists("Product_Observational.Observation_Area.Investigation_Area.name")%} "{{Product_Observational.Observation_Area.Investigation_Area.name}}"{%else%}"{{Product_Observational.Observation_Area.Investigation_Area.Instrument_Host_Name}}"{%endif%}
    InstrumentId              = {{Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name}}
    TargetName                = {%if exists("Product_Observational.Observation_Area.Target_Identification.name")%}{{Product_Observational.Observation_Area.Target_Identification.name}}{%else%}{{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.TARGET}}{%endif%}
    StartTime                 = {%if exists("Product_Observational.Observation_Area.Time_Coordinates.start_date_time")%}{{Product_Observational.Observation_Area.Time_Coordinates.start_date_time}}{%else%}{{Product_Observational.CaSSIS_Header.DERIVED_HEADER_DATA.OnboardImageAcquisitionTime._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}SpacecraftClockStartCount = {{Product_Observational.CaSSIS_Header.FSW_HEADER.attrib_ExposureTimestamp}}{%endif%}
    ExposureDuration          = {%if exists("Product_Observational.Observation_Area.Discipline_Area.img_Imaging")%}{{Product_Observational.Observation_Area.Discipline_Area.img_Imaging.img_Imaging_Instrument_Parameters.img_exposure_duration._text}}<seconds>{%else%}{{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Exposure_Time}}<seconds>{%endif%}
    Filter                    = {%if exists("Product_Observational.Observation_Area.Discipline_Area.img_Imaging")%}{{Product_Observational.Observation_Area.Discipline_Area.img_Imaging.img_Image_Product_Information.img_Filter.img_filter_name}}{%else%}{{Product_Observational.CaSSIS_Header.DERIVED_HEADER_DATA.Filter._text}}{%endif%}
    Expanded                  = {%if exists("Product_Observational.CaSSIS_Header")%}{{Product_Observational.CaSSIS_Header.DERIVED_HEADER_DATA.Expanded._text}}{%else%}1{%endif%}
    SummingMode               = 0
  End_Group

  Group = Archive
    {%if exists("Product_Observational.Identification_Area.Alias_List")%}ObservationId             = {{Product_Observational.Identification_Area.Alias_List.Alias.alternate_id}}{%endif%}
    DataSetId                    = {{Product_Observational.Identification_Area.logical_identifier}}
    {%if exists("Product_Observational.Identification_Area.version_id")%}ProductVersionId             = {{Product_Observational.Identification_Area.version_id}}{%endif%}
    {%if exists("Product_Observational.Identification_Area.Producer_data")%}ProducerId                   = {{Product_Observational.Identification_Area.Producer_data.Producer_id}}{%endif%}
    {%if exists("Product_Observational.Identification_Area.Producer_data")%}ProducerName                 = "{{Product_Observational.Identification_Area.Producer_data.Producer_full_name}}"{%endif%}
    {%if exists("Product_Observational.File_Area_Observational.File.creation_date_time")%}ProductCreationTime          = {{Product_Observational.File_Area_Observational.File.creation_date_time}}{%endif%}
    FileName                     = {{Product_Observational.File_Area_Observational.File.file_name}}
    ScalingFactor                = {{Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.scaling_factor}}
    {%if exists("Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.offset")%}Offset                       = {{Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.offset}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}PredictMaximumExposureTime   = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.PREDICTED_MAXIMUM_EXPOSURE_TIME._text}} <ms>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}CassisOffNadirAngle          = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.CASSIS_OFF_NADIR_ANGLE._text}} <deg>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}PredictedRepetitionFrequency = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.PREDICTED_REQUIRED_REPETITION_FREQUENCY._text}} <ms>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}GroundTrackVelocity          = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.TGO_GROUND_TRACK_VELOCITY._text}} <km/s>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}ForwardRotationAngle         = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.FORWARD_ROTATION_ANGLE_REQUIRED._text}} <deg>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}SpiceMisalignment            = {{Product_Observational.CaSSIS_Header.GEOMETRIC_DATA.SPICE_KERNEL_MISALIGNMENT_PREDICT._text}} <deg>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}FocalLength                  = {{Product_Observational.CaSSIS_Header.CaSSIS_General.TELESCOPE_FOCAL_LENGTH._text}} <m>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}FNumber                      = {{Product_Observational.CaSSIS_Header.CaSSIS_General.TELESCOPE_F_NUMBER}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}ExposureTimeCommand          = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.T_exp._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}FrameletNumber               = {{Product_Observational.CaSSIS_Header.FSW_HEADER.attrib_SequenceCounter}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}NumberOfFramelets            = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.Num_exp._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}ImageFrequency               = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.Step_exp._text}} <ms>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}NumberOfWindows              = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.Num_win._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header.hasSymbol")%}UniqueIdentifier             = {{Product_Observational.CaSSIS_Header.IMAGE_COMMAND.Unique_Identifier._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}UID                          = {{Product_Observational.CaSSIS_Header.FSW_HEADER.attrib_UID}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}ExposureTimestamp            = {{Product_Observational.CaSSIS_Header.FSW_HEADER.attrib_ExposureTimestamp}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}ExposureTimePEHK             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Exposure_Time}} <ms>{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}PixelsPossiblySaturated      = {{Product_Observational.CaSSIS_Header.DERIVED_HEADER_DATA.PixelsPossiblySaturated._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}IFOV                         = {{Product_Observational.CaSSIS_Header.CaSSIS_General.INSTRUMENT_IFOV._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}IFOVUnit                     = {{Product_Observational.CaSSIS_Header.CaSSIS_General.INSTRUMENT_IFOV.attrib_Unit}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}FiltersAvailable             = "{{Product_Observational.CaSSIS_Header.CaSSIS_General.FILTERS_AVAILABLE}}"{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}FocalLength                  = {{Product_Observational.CaSSIS_Header.CaSSIS_General.TELESCOPE_FOCAL_LENGTH._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}FocalLengthUnit              = {{Product_Observational.CaSSIS_Header.CaSSIS_General.TELESCOPE_FOCAL_LENGTH.attrib_Unit}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}TelescopeFNumber              = {{Product_Observational.CaSSIS_Header.CaSSIS_General.TELESCOPE_F_NUMBER}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}TelescopeType                = "{{Product_Observational.CaSSIS_Header.CaSSIS_General.TELESCOPE_TYPE}}"{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}DetectorDescription          = "{{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_DESC}}"{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}PixelHeight                  = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_PIXEL_HEIGHT._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}PixelHeightUnit              = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_PIXEL_HEIGHT.attrib_Unit}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}PixelWidth                   = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_PIXEL_WIDTH._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}PixelWidthUnit               = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_PIXEL_WIDTH.attrib_Unit}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}DetectorType                 = "{{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_TYPE}}"{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}ReadNoise                    = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_READ_NOISE._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}ReadNoiseUnit                = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_READ_NOISE.attrib_Unit}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}MissionPhase                 = {{Product_Observational.CaSSIS_Header.DERIVED_HEADER_DATA.MissionPhase._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}SubInstrumentIdentifier      = {{Product_Observational.CaSSIS_Header.CaSSIS_General.DETECTOR_READ_NOISE._text}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}WindowCount                  = {{Product_Observational.CaSSIS_Header.FSW_HEADER.attrib_WindowCounter}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window1Binning               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_1}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window1StartSample           = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window1_Start_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window1EndSample             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window1_End_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window1StartLine             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window1_Start_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window1EndLine               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window1_End_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window2Binning               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_2}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window2StartSample           = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window2_Start_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window2EndSample             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window2_End_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window2StartLine             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window2_Start_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window2EndLine               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window2_End_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window3Binning               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_3}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window3StartSample           = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window3_Start_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window3EndSample             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window3_End_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window3StartLine             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window3_Start_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window3EndLine               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window3_End_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window4Binning               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_4}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window4StartSample           = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window4_Start_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window4EndSample             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window4_End_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window4StartLine             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window4_Start_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window4EndLine               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window4_End_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window5Binning               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_5}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window5StartSample           = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window5_Start_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window5EndSample             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window5_End_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window5StartLine             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window5_Start_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window5EndLine               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window5_End_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window6Binning               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_6}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window6StartSample           = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window6_Start_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window6EndSample             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window6_End_Col}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window6StartLine             = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window6_Start_Row}}{%endif%}
    {%if exists("Product_Observational.CaSSIS_Header")%}Window6EndLine               = {{Product_Observational.CaSSIS_Header.PEHK_HEADER.attrib_Window6_End_Row}}{%endif%}
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
