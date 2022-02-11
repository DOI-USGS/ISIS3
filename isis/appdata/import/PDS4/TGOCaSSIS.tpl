{% set PO=Product_Observational %}
{% set CassHeader="Product_Observational.CaSSIS_Header" %}
{% set IdArea="Product_Observational.Identification_Area" %}
{% set FileArea="Product_Observational.File_Area_Observational" %}
{% set threeDArray="Product_Observational.File_Area_Observational.Array_3D_Image" %}
{% set cart="Product_Observational.Observation_Area.Discipline_Area.cart_Cartography" %}
{% set date="Product_Observational.Observation_Area.Discipline_Area.cart_Cartography" %}


Object = IsisCube
  Object = Core
    {% if exists(threeDArray) %}
    Group = Dimensions
      Samples = {{ PO.File_Area_Observational.Array_3D_Image.Axis_Array.0.elements }}
      Lines   = {{ PO.File_Area_Observational.Array_3D_Image.Axis_Array.1.elements }}
      Bands   = {% if exists(FileArea + ".Array_3D_Image.Axis_Array.2.elements") %}
                {{ PO.File_Area_Observational.Array_3D_Image.Axis_Array.2.elements }}
                {% else %}
                1
                {% endif %}
    End_Group

    Group = Pixels
      {% if exists("Product_Observational.File_Area_Observational.Array_3D_Image.Element_Array.idl_data_type") %}
      {% set type=PO.File_Area_Observational.Array_3D_Image.Element_Array.idl_data_type %}
      Type       = {% if type == "1" %} UnsignedByte
                   {% else if type == "2" %} SignedWord
                   {% else if type == "3" %} SignedInteger
                   {% else if type == "4" or type == "5" %} Real
                   {% else if type == "12" %} UnsignedWord
                   {% else if type == "13" %} UnsignedInteger
                   {% else if type == "14" %} SignedInteger
                   {% else if type == "14" %} UnsignedInteger
                   {% endif %}
      {% else %}
      Type       = Real
      {% endif %}
      ByteOrder  = Lsb

      Base       = {% if exists("Product_Observational.File_Area_Observational.Array_3D_Image.Element_Array.offset") %}
                   {{ PO.File_Area_Observational.Array_3D_Image.Element_Array.offset }}
                   {% else %}
                   {{ PO.File_Area_Observational.Array_3D_Image.Element_Array.value_offset }}
                   {% endif %}
      Multiplier = {{ PO.File_Area_Observational.Array_3D_Image.Element_Array.scaling_factor }}
    End_Group

    {% else %}
    Group = Dimensions
      Samples = {{ PO.File_Area_Observational.Array_2D_Image.Axis_Array.0.elements }}
      Lines   = {{ PO.File_Area_Observational.Array_2D_Image.Axis_Array.1.elements }}
      Bands   = {% if exists(FileArea + ".Array_2D_Image.Axis_Array.2.elements") %}
                {{ PO.File_Area_Observational.Array_2D_Image.Axis_Array.2.elements }}
                {% else %}
                1
                {% endif %}
    End_Group

    Group = Pixels
      {% if exists("Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.idl_data_type") %}
      {% set type=PO.File_Area_Observational.Array_2D_Image.Element_Array.idl_data_type %}
      Type       = {% if type == "1" %} UnsignedByte
                   {% else if type == "2" %} SignedWord
                   {% else if type == "3" %} SignedInteger
                   {% else if type == "4" or type == "5" %} Real
                   {% else if type == "12" %} UnsignedWord
                   {% else if type == "13" %} UnsignedInteger
                   {% else if type == "14" %} SignedInteger
                   {% else if type == "14" %} UnsignedInteger
                   {% endif %}
      {% else %}
      Type       = Real
      {% endif %}
      ByteOrder  = Lsb
      Base       = {% if exists("Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.offset") %}
                   {{ PO.File_Area_Observational.Array_2D_Image.Element_Array.offset }}
                   {% else %}
                   {{ PO.File_Area_Observational.Array_2D_Image.Element_Array.value_offset }}
                   {% endif %}
      Multiplier = {{ PO.File_Area_Observational.Array_2D_Image.Element_Array.scaling_factor }}
    End_Group
    {% endif %}
  End_Object

  Group = Instrument
    SpacecraftName            = {% if exists("Product_Observational.Observation_Area.Investigation_Area.name") %}
                                "{{ PO.Observation_Area.Investigation_Area.name }}"
                                {% else %}
                                "{{ PO.Observation_Area.Investigation_Area.Instrument_Host_Name }}"
                                {% endif %}
    InstrumentId              = {{ PO.Observation_Area.Observing_System.Observing_System_Component.1.name }}
    TargetName                = {% if exists("Product_Observational.Observation_Area.Target_Identification.name") %}
                                {% set target=PO.Observation_Area.Target_Identification.name %}
                                {{ target }}
                                {% else %}
                                {% set target=PO.CaSSIS_Header.GEOMETRIC_DATA.TARGET %}
                                {{ target }}
                                {% endif %}
    StartTime                 = {% if exists("Product_Observational.Observation_Area.Time_Coordinates.start_date_time") %}
                                {% set startTime=PO.Observation_Area.Time_Coordinates.start_date_time %}
                                {{ RemoveStartTimeZ(startTime) }}
                                {% else %}
                                {% set startTime=PO.CaSSIS_Header.DERIVED_HEADER_DATA.OnboardImageAcquisitionTime._text %}
                                {{ RemoveStartTimeZ(startTime) }}
                                {% endif %}
                                {% if exists(CassHeader) %}
    SpacecraftClockStartCount = {{ PO.CaSSIS_Header.FSW_HEADER.attrib_ExposureTimestamp }}
                                {% endif %}
    ExposureDuration          = {% if exists("Product_Observational.Observation_Area.Discipline_Area.img_Imaging") %}
                                {{ PO.Observation_Area.Discipline_Area.img_Imaging.img_Imaging_Instrument_Parameters.img_exposure_duration._text }}<seconds>
                                {% else %}
                                {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Exposure_Time }}<seconds>
                                {% endif %}
    Filter                    = {% if exists("Product_Observational.Observation_Area.Discipline_Area.img_Imaging") %}
                                {{ PO.Observation_Area.Discipline_Area.img_Imaging.img_Image_Product_Information.img_Filter.img_filter_name }}
                                {% else %} {{ PO.CaSSIS_Header.DERIVED_HEADER_DATA.Filter._text }}
                                {% endif %}
    Expanded                  = {% if exists(CassHeader) %}
                                {% set expanded=int(PO.CaSSIS_Header.DERIVED_HEADER_DATA.Expanded._text) %}
                                {{ PO.CaSSIS_Header.DERIVED_HEADER_DATA.Expanded._text }}
                                {% else %}
                                1
                                {% set expanded=1 %}
                                {% endif %}
    SummingMode               = {% if expanded == 1 %}
                                0
                                {% else %}
                                Window{{ PO.CaSSIS_Header.FSW_HEADER.attrib_WindowCounter }}Binning
                                {% endif %}
  End_Group

  Group = Archive
    {%  if exists(IdArea + ".Alias_List") %}
    ObservationId             = {{ PO.Identification_Area.Alias_List.Alias.alternate_id }}
    {% endif %}
    DataSetId                    = {{ PO.Identification_Area.logical_identifier }}
    {%  if exists(IdArea + ".version_id") %}
    ProductVersionId             = {{ PO.Identification_Area.version_id }}
    {% endif %}
    {%  if exists(IdArea + ".Producer_data") %}
    ProducerId                   = {{ PO.Identification_Area.Producer_data.Producer_id }}
    {% endif %}
    {%  if exists(IdArea + ".Producer_data") %}
    ProducerName                 = "{{ PO.Identification_Area.Producer_data.Producer_full_name }}"
    {% endif %}
    {%  if exists(IdArea + ".Product_Id") %}
    ProductId                    = "{{ PO.Identification_Area.Product_Id }}"
    {% endif %}
    {%  if exists(FileArea + ".File.creation_date_time") %}
    ProductCreationTime          = {{ PO.File_Area_Observational.File.creation_date_time }}
    {% endif %}
    FileName                     = {{ PO.File_Area_Observational.File.file_name }}
    ScalingFactor                = {{ PO.File_Area_Observational.Array_2D_Image.Element_Array.scaling_factor }}
    {%  if exists(FileArea + ".Array_2D_Image.Element_Array.offset") %}
    Offset                       = {{ PO.File_Area_Observational.Array_2D_Image.Element_Array.offset }}
    {% endif %}
    {%  if exists(CassHeader) %}
    PredictMaximumExposureTime   = {{ PO.CaSSIS_Header.GEOMETRIC_DATA.PREDICTED_MAXIMUM_EXPOSURE_TIME._text }} <ms>
    {% endif %}
    {%  if exists(CassHeader) %}
    CassisOffNadirAngle          = {{ PO.CaSSIS_Header.GEOMETRIC_DATA.CASSIS_OFF_NADIR_ANGLE._text }} <deg>
    {% endif %}
    {%  if exists(CassHeader) %}
    PredictedRepetitionFrequency = {{ PO.CaSSIS_Header.GEOMETRIC_DATA.PREDICTED_REQUIRED_REPETITION_FREQUENCY._text }} <ms>
    {% endif %}
    {%  if exists(CassHeader) %}
    GroundTrackVelocity          = {{ PO.CaSSIS_Header.GEOMETRIC_DATA.TGO_GROUND_TRACK_VELOCITY._text }} <km/s>
    {% endif %}
    {%  if exists(CassHeader) %}
    ForwardRotationAngle         = {{ PO.CaSSIS_Header.GEOMETRIC_DATA.FORWARD_ROTATION_ANGLE_REQUIRED._text }} <deg>
    {% endif %}
    {%  if exists(CassHeader) %}
    SpiceMisalignment            = {{ PO.CaSSIS_Header.GEOMETRIC_DATA.SPICE_KERNEL_MISALIGNMENT_PREDICT._text }} <deg>
    {% endif %}
    {%  if exists(CassHeader) %}
    FocalLength                  = {{ PO.CaSSIS_Header.CaSSIS_General.TELESCOPE_FOCAL_LENGTH._text }} <m>
    {% endif %}
    {%  if exists(CassHeader) %}
    FNumber                      = {{ PO.CaSSIS_Header.CaSSIS_General.TELESCOPE_F_NUMBER }}
    {% endif %}
    {%  if exists(CassHeader) %}
    ExposureTimeCommand          = {{ PO.CaSSIS_Header.IMAGE_COMMAND.T_exp._text }}
    {% endif %}
    {%  if exists(CassHeader) %}
    FrameletNumber               = {{ PO.CaSSIS_Header.FSW_HEADER.attrib_SequenceCounter }}
    {% endif %}
    {%  if exists(CassHeader) %}
    NumberOfFramelets            = {{ PO.CaSSIS_Header.IMAGE_COMMAND.Num_exp._text }}
    {% endif %}
    {%  if exists(CassHeader) %}
    ImageFrequency               = {{ PO.CaSSIS_Header.IMAGE_COMMAND.Step_exp._text }} <ms>
    {% endif %}
    {%  if exists(CassHeader) %}
    NumberOfWindows              = {{ PO.CaSSIS_Header.IMAGE_COMMAND.Num_win._text }}
    {% endif %}
    {%  if exists(CassHeader + ".IMAGE_COMMAND") %}
    UniqueIdentifier             = {% set uniqueId=PO.CaSSIS_Header.IMAGE_COMMAND.Unique_Identifier._text %}
                                   {{ uniqueId }}
    {% endif %}
    {%  if exists(CassHeader) %}
    UID                          = {{ PO.CaSSIS_Header.FSW_HEADER.attrib_UID }}
    {% endif %}
    {%  if exists(CassHeader) %}
    ExposureTimestamp            = {{ PO.CaSSIS_Header.FSW_HEADER.attrib_ExposureTimestamp }}
    {% endif %}
    {%  if exists(CassHeader) %}
    ExposureTimePEHK             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Exposure_Time }} <ms>
    {% endif %}
    {%  if exists(CassHeader + ".DERIVED_HEADER_DATA.PixelsPossiblySaturated") %}
    PixelsPossiblySaturated      = {{ PO.CaSSIS_Header.DERIVED_HEADER_DATA.PixelsPossiblySaturated._text }}
    {% endif %}
    {%  if exists(CassHeader) %}
    IFOV                         = {{ PO.CaSSIS_Header.CaSSIS_General.INSTRUMENT_IFOV._text }}
    {% endif %}
    {%  if exists(CassHeader) %}
    IFOVUnit                     = {{ PO.CaSSIS_Header.CaSSIS_General.INSTRUMENT_IFOV.attrib_Unit }}
    {% endif %}
    {%  if exists(CassHeader) %}
    FiltersAvailable             = "{{ PO.CaSSIS_Header.CaSSIS_General.FILTERS_AVAILABLE }}"
    {% endif %}
    {%  if exists(CassHeader) %}
    FocalLengthUnit              = {{ PO.CaSSIS_Header.CaSSIS_General.TELESCOPE_FOCAL_LENGTH.attrib_Unit }}
    {% endif %}
    {%  if exists(CassHeader) %}
    TelescopeType                = "{{ PO.CaSSIS_Header.CaSSIS_General.TELESCOPE_TYPE }}"
    {% endif %}
    {%  if exists(CassHeader) %}
    DetectorDescription          = "{{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_DESC }}"{% endif %}
    {%  if exists(CassHeader) %}
    PixelHeight                  = {{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_PIXEL_HEIGHT._text }}{% endif %}
    {%  if exists(CassHeader) %}
    PixelHeightUnit              = {{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_PIXEL_HEIGHT.attrib_Unit }}{% endif %}
    {%  if exists(CassHeader) %}
    PixelWidth                   = {{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_PIXEL_WIDTH._text }}{% endif %}
    {%  if exists(CassHeader) %}
    PixelWidthUnit               = {{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_PIXEL_WIDTH.attrib_Unit }}{% endif %}
    {%  if exists(CassHeader) %}
    DetectorType                 = "{{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_TYPE }}"
    {% endif %}
    {%  if exists(CassHeader) %}
    ReadNoise                    = {{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_READ_NOISE._text }}
    {% endif %}
    {%  if exists(CassHeader) %}
    ReadNoiseUnit                = {{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_READ_NOISE.attrib_Unit }}
    {% endif %}
    {%  if exists(CassHeader) %}
    MissionPhase                 = {{ PO.CaSSIS_Header.DERIVED_HEADER_DATA.MissionPhase._text }}
    {% endif %}
    {%  if exists(CassHeader) %}
    SubInstrumentIdentifier      = {{ PO.CaSSIS_Header.CaSSIS_General.DETECTOR_READ_NOISE._text }}
    {% endif %}
    {%  if exists(CassHeader) %}
    WindowCount                  = {{ PO.CaSSIS_Header.FSW_HEADER.attrib_WindowCounter }}
    {% endif %}
    {%  if exists(CassHeader) %}
    Window1Binning               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_1 }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window1StartSample           = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window1_Start_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window1EndSample             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window1_End_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window1StartLine             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window1_Start_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window1EndLine               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window1_End_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window2Binning               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_2 }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window2StartSample           = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window2_Start_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window2EndSample             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window2_End_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window2StartLine             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window2_Start_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window2EndLine               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window2_End_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window3Binning               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_3 }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window3StartSample           = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window3_Start_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window3EndSample             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window3_End_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window3StartLine             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window3_Start_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window3EndLine               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window3_End_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window4Binning               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_4 }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window4StartSample           = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window4_Start_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window4EndSample             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window4_End_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window4StartLine             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window4_Start_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window4EndLine               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window4_End_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window5Binning               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_5 }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window5StartSample           = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window5_Start_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window5EndSample             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window5_End_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window5StartLine             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window5_Start_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window5EndLine               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window5_End_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window6Binning               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Binning_window_6 }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window6StartSample           = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window6_Start_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window6EndSample             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window6_End_Col }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window6StartLine             = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window6_Start_Row }}
    {% endif %}
    {% if exists(CassHeader) %}
    Window6EndLine               = {{ PO.CaSSIS_Header.PEHK_HEADER.attrib_Window6_End_Row }}
    {% endif %}
    YearDoy                      = {{ YearDoy(startTime) }}
    {%  if exists(CassHeader + ".IMAGE_COMMAND") %}
    ObservationId                = {{ UniqueIdtoObservId(uniqueId, target) }}
    {% endif %}
  End_Group

  Group = BandBin
    {% if exists("Product_Observational.Observation_Area.Discipline_Area.img_Imaging") %}
    {% set filterName=Product_Observational.Observation_Area.Discipline_Area.img_Imaging.img_Image_Product_Information.img_Filter.img_filter_name %}
    {% else %}
    {% set filterName=Product_Observational.CaSSIS_Header.DERIVED_HEADER_DATA.Filter._text %}
    {% endif %}
    FilterName = {{ filterName }}
    Center     = {% if filterName == "RED" %} 835.4 <nm>
                 {% else if filterName == "PAN" %}  677.4 <nm>
                 {% else if filterName == "NIR" %}  940.2 <nm>
                 {% else if filterName == "BLU" %}  497.4 <nm>
                 {% endif %}
    Width      = {% if filterName == "RED" %} 98.0 <nm>
                 {% else if filterName == "PAN" %}  231.5 <nm>
                 {% else if filterName == "NIR" %}  120.6 <nm>
                 {% else if filterName == "BLU" %}  134.3 <nm>
                 {% endif %}
    NaifIkCode = {% if filterName == "RED" %}  -143422
                 {% else if filterName == "PAN" %}  -143421
                 {% else if filterName == "NIR" %}  -143423
                 {% else if filterName == "BLU" %}  -143424
                 {% else %} -143400{% endif %}
  End_Group

  Group = Kernels
    NaifFrameCode = -143400
  End_Group

  {% if exists(cart) %}
  Group = Mapping
    ProjectionName     = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Planar.cart_Map_Projection.cart_map_projection_name }}
    CenterLongitude    = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Planar.cart_Map_Projection.cart_Equirectangular.cart_longitude_of_central_meridian._text }}
    CenterLatitude     = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Planar.cart_Map_Projection.cart_Equirectangular.cart_latitude_of_projection_origin._text }}
    Scale              = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Planar.cart_Planar_Coordinate_Information.cart_Coordinate_Representation.cart_pixel_scale_x._text }}
    PixelResolution    = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Planar.cart_Planar_Coordinate_Information.cart_Coordinate_Representation.cart_pixel_resolution_x._text }}
    MaximumLatitude    = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Domain.cart_Bounding_Coordinates.cart_north_bounding_coordinate._text }}
    MinimumLatitude    = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Domain.cart_Bounding_Coordinates.cart_south_bounding_coordinate._text }}
    MaximumLongitude   = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Domain.cart_Bounding_Coordinates.cart_west_bounding_coordinate._text }}
    MinimumLongitude   = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Domain.cart_Bounding_Coordinates.cart_east_bounding_coordinate._text }}
    UpperLeftCornerX   = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Planar.cart_Geo_Transformation.cart_upperleft_corner_x._text }}
    UpperLeftCornerY   = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Planar.cart_Geo_Transformation.cart_upperleft_corner_y._text }}
    EquatorialRadius   = {% if exists(cart + ".cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Geodetic_Model.cart_semi_major_radius") %}
                         {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Geodetic_Model.cart_semi_major_radius._text }}
                         {% endif %}
    PolarRadius        = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Geodetic_Model.cart_polar_radius._text }}
    LatitudeType       = {{ PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Geodetic_Model.cart_latitude_type }}
    LongitudeDirection = {% set LongDir=PO.Observation_Area.Discipline_Area.cart_Cartography.cart_Spatial_Reference_Information.cart_Horizontal_Coordinate_System_Definition.cart_Geodetic_Model.cart_longitude_direction %}
                         {% if LongDir == "Positive East" %}
                         PositiveEast
                         {% else if LongDir == "Positive West" %}
                         PositiveWest
                         {% endif %}
    TargetName         = {{ PO.Observation_Area.Target_Identification.name }}
    LongitudeDomain    = 360
  End_Group
  {% endif %}

  {% if exists(CassHeader) %}
    {% set windowNumber=int(PO.CaSSIS_Header.FSW_HEADER.attrib_WindowCounter) + 1  %}
    {% if windowNumber == 1 %}
        {% set frameletStartSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window1_Start_Col) + 1 %}
        {% set frameletEndSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window1_End_Col) + 1 %}
        {% set frameletStartLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window1_Start_Row) + 1 %}
        {% set frameletEndLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window1_End_Row) + 1 %}
    {% endif %}
    {% if windowNumber == 2 %}
        {% set frameletStartSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window2_Start_Col) + 1 %}
        {% set frameletEndSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window2_End_Col) + 1 %}
        {% set frameletStartLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window2_Start_Row) + 1 %}
        {% set frameletEndLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window2_End_Row) + 1 %}
    {% endif %}
    {% if windowNumber == 3 %}
        {% set frameletStartSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window3_Start_Col) + 1 %}
        {% set frameletEndSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window3_End_Col) + 1 %}
        {% set frameletStartLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window3_Start_Row) + 1 %}
        {% set frameletEndLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window3_End_Row) + 1 %}
    {% endif %}
    {% if windowNumber == 4 %}
        {% set frameletStartSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window4_Start_Col) + 1 %}
        {% set frameletEndSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window4_End_Col) + 1 %}
        {% set frameletStartLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window4_Start_Row) + 1 %}
        {% set frameletEndLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window4_End_Row) + 1 %}
    {% endif %}
    {% if windowNumber == 5 %}
        {% set frameletStartSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window5_Start_Col) + 1 %}
        {% set frameletEndSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window5_End_Col) + 1 %}
        {% set frameletStartLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window5_Start_Row) + 1 %}
        {% set frameletEndLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window5_End_Row) + 1 %}
    {% endif %}
    {% if windowNumber == 6 %}
        {% set frameletStartSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window6_Start_Col) + 1 %}
        {% set frameletEndSample = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window6_End_Col) + 1 %}
        {% set frameletStartLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window6_Start_Row) + 1 %}
        {% set frameletEndLine = int(PO.CaSSIS_Header.PEHK_HEADER.attrib_Window6_End_Row) + 1 %}
    {% endif %}

  Group = AlphaCube
    AlphaSamples        = 2048
    AlphaLines          = 2048
    AlphaStartingSample = {{ frameletStartSample - 0.5 }}
    AlphaStartingLine   = {{ frameletStartLine - 0.5 }}
    AlphaEndingSample   = {{ frameletEndSample + 0.5 }}
    AlphaEndingLine     = {{ frameletEndLine + 0.5 }}
    BetaSamples         = {{ frameletEndSample - frameletStartSample + 1 }}
    BetaLines           = {{ frameletEndLine - frameletStartLine + 1 }}
  End_Group
  {% endif %}
End_Object
End
