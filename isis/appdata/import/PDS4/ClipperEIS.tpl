{% if Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "WAC FC" or Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "WAC PB" %}
{% set sensor="WAC" %}
{% else if Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "NAC FC" or Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "NAC PB" %}
{% set sensor="NAC" %}
{% else %}
{% set sensor="UNK" %}
{% endif %}

{% set ImageArray = Product_Observational.File_Area_Observational.0.Array_2D_Image %}

Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{ ImageArray.Axis_Array.0.elements }}
      Lines   = {{ ImageArray.Axis_Array.1.elements }}
      Bands   = 1
    End_Group



    Group = Pixels
      {% set pixelType = ImageArray.Element_Array.data_type %}
      {% if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type") %}
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

      Base       = {% if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.value_offset") %}
                   {{ ImageArray.Element_Array.value_offset }}
                   {% else if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.offset._text") %}
                   {{ ImageArray.offset._text }}
                   {% else %}
                   0
                   {% endif %}
      Multiplier = {% if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.scaling_factor") %}
                   {{ ImageArray.Element_Array.scaling_factor._text }}
                   {% else %}
                   1
                   {% endif %}
    End_Group
  End_Object

  Group = Instrument
    Sensor = {{sensor}}
    SpacecraftName            = "{{ Product_Observational.Observation_Area.Investigation_Area.name }}"
    InstrumentId              = "{{ Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name }} {{ Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name }}"
    TargetName                = {{ at(splitOnChar(Product_Observational.Observation_Area.Target_Identification.name, " "), 1) }}
    StartTime                 = {{ RemoveStartTimeZ(Product_Observational.Observation_Area.Time_Coordinates.start_date_time) }}
    ExposureDuration          = {{ Product_Observational.Observation_Area.Discipline_Area.img_Exposure.img_exposure_duration._text }}<seconds>
  End_Group

  Group = BandBin
    {# Hard code to clear filter #}
    FilterName = Clear
    {% if sensor == "WAC" %}
    Center     = 712.5 <nm>
    Width      = 675 <nm>
    {% else %}
    Center     = 702.5 <nm>
    Width      = 695 <nm>
    {% endif %}
  End_Group

  Group = Kernels
    NaifFrameCode = {% if sensor == "WAC" %}
                    -159104
                    {% else %}
                    -159103
                    {% endif %}

  End_Group
End_Object

Object = Translation
End_Object
End
