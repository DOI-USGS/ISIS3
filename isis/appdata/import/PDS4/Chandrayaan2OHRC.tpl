{% set file_name = Product_Observational.File_Area_Observational.File.file_name %}
{% set sensor = CharAt(file_name, 10) %}

{% set ImageArray = Product_Observational.File_Area_Observational.Array_2D_Image %}

Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{ ImageArray.Axis_Array.1.elements }}
      Lines   = {{ ImageArray.Axis_Array.0.elements }}
      Bands   = 1
    End_Group

    Group = Pixels
      {% set pixelType = ImageArray.Element_Array.data_type %}
      {% if exists("Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.data_type") %}
      Type       = {% if pixelType == "IEEE754LSBDouble" %} Double
                   {% else if pixelType == "IEEE754LSBSingle" %} Real
                   {% else if pixelType == "IEEE754MSBDouble" %} Double
                   {% else if pixelType == "IEEE754MSBSingle" %} Real
                   {% else if pixelType == "SignedByte" %} SignedByte
                   {% else if pixelType == "SignedLSB2" %} SignedWord
                   {% else if pixelType == "SignedLSB4" %} SignedInteger
                   {% else if pixelType == "SignedMSB2" %} SignedWord
                   {% else if pixelType == "SignedMSB4" %} SignedInteger
                   {% else if pixelType == "UnsignedByte" %} UnsignedByte
                   {% else if pixelType == "UnsignedLSB2" %} UnsignedWord
                   {% else if pixelType == "UnsignedLSB4" %} UnsignedInteger
                   {% else if pixelType == "UnsignedMSB2" %} UnsignedWord
                   {% else if pixelType == "UnsignedMSB4" %} UnsignedInteger
                   {% else %} Real
                   {% endif %}
      ByteOrder  = {% if pixelType == "IEEE754LSBDouble" %} LSB
                   {% else if pixelType == "IEEE754LSBSingle" %} LSB
                   {% else if pixelType == "IEEE754MSBDouble" %} MSB
                   {% else if pixelType == "IEEE754MSBSingle" %} MSB
                   {% else if pixelType == "SignedByte" %} LSB
                   {% else if pixelType == "SignedLSB2" %} LSB
                   {% else if pixelType == "SignedLSB4" %} LSB
                   {% else if pixelType == "SignedMSB2" %} MSB
                   {% else if pixelType == "SignedMSB4" %} MSB
                   {% else if pixelType == "UnsignedByte" %} LSB
                   {% else if pixelType == "UnsignedLSB2" %} LSB
                   {% else if pixelType == "UnsignedLSB4" %} LSB
                   {% else if pixelType == "UnsignedMSB2" %} MSB
                   {% else if pixelType == "UnsignedMSB4" %} MSB
                   {% else %} Lsb
                   {% endif %}
      {% else %}
      Type       = Real
      ByteOrder  = Lsb
      {% endif %}

      Base       = {% if exists("Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.value_offset") %}
                   {{ ImageArray.Element_Array.value_offset }}
                   {% else if exists("Product_Observational.File_Area_Observational.Array_2D_Image.offset._text") %}
                   {{ ImageArray.offset._text }}
                   {% else %}
                   0
                   {% endif %}
      Multiplier = {% if exists("Product_Observational.File_Area_Observational.Array_2D_Image.Element_Array.scaling_factor") %}
                   {{ ImageArray.Element_Array.scaling_factor._text }}
                   {% else %}
                   1
                   {% endif %}
    End_Group
  End_Object

  Group = Instrument
    SpacecraftName            = {{ capitalize(Product_Observational.Observation_Area.Investigation_Area.name) }}
    {% set inst_name = Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name %}
    {% if inst_name == "orbiter high resolution camera" %}
    InstrumentId              = OHRC
    {% endif %}
    TargetName                = {{ Product_Observational.Observation_Area.Target_Identification.name }}
    StartTime                 = {{ RemoveStartTimeZ(Product_Observational.Observation_Area.Time_Coordinates.start_date_time) }}
    StopTime                  = {{ RemoveStartTimeZ(Product_Observational.Observation_Area.Time_Coordinates.stop_date_time) }}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_line_exposure_duration") %}
    LineExposureDuration      = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_line_exposure_duration._text }} <ms>
    {% else %}
    LineExposureDuration      = 3.236 <ms>
    {% endif %}
  End_Group

  {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters") %}
  Group = Archive
    JobId                   = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_job_id }}
    OrbitNumber             = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_imaging_orbit_number }}
    DetectorPixelWidth      = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_detector_pixel_width._text }} <micrometers>
    FocalLength             = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_focal_length._text }} <mm>
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_reference_data_used") %}
    ReferenceData           = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_reference_data_used }}
    {% else %}
    ReferenceData = NA
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_orbit_limb_direction") %}
    OrbitLimbDirection      = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_orbit_limb_direction }}
    {% else %}
    OrbitLimbDirection      = Descending
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_spacecraft_yaw_direction") %}
    SpacecraftYawDirection  = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_spacecraft_yaw_direction }}
    {% else %}
    SpacecraftYawDirection  = True
    {% endif %}
    SpacecraftAltitude      = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_spacecraft_altitude._text }} <km>
    PixelResolution         = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_pixel_resolution._text }} <meters/pixel>
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_roll._text") %}
    Roll                    = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_roll._text }} <degrees>
    {% else %}
    Roll                    = -1 <degrees>
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_pitch._text") %}
    Pitch                   = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_pitch._text }} <degrees>
    {% else %}
    Pitch                   = -1 <degrees>
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_yaw._text") %}
    Yaw                     = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_yaw._text }} <degrees>
    {% else %}
    Yaw                     = -1 <degrees>
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_sun_azimuth._text") %}
    SunAzimuth              = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_sun_azimuth._text }} <degrees>
    {% else %}
    SunAzimuth              = -1 <degrees>
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_sun_elevation._text") %}
    SunElevation            = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_sun_elevation._text }} <degrees>
    {% else %}
    SunElevation            = -1 <degrees>
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_solar_incidence._text") %}
    SolarIncidence          = {{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_solar_incidence._text }} <degrees>
    {% else %}
    SolarIncidence          = -1 <degrees>
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_projection") %}
    Projection              = "{{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_projection }}"
    {% else %}
    Projection              = "NA"
    {% endif %}
    {% if exists("Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_area") %}
    Area                    = "{{ Product_Observational.Observation_Area.Mission_Area.isda_Product_Parameters.isda_area }}"
    {% else %}
    Area                    = "NA"
    {% endif %}
  End_Group
  {% endif %}

  Group = BandBin
    Center = 675
    Width = 175
  End_Group

  Group = Kernels
    NaifFrameCode = -152270
  End_Group
End_Object

Object = Translation
End_Object
End
