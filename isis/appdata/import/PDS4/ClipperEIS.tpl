{% set Product_Observational="Product_Observational" %}
{% set IdArea="Product_Observational.Identification_Area" %}
{% set FileArea="Product_Observational.File_Area_Observational" %}
{% set ImageFileArea="Product_Observational.File_Area_Observational.0" %}
{% if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image") %}
{% set imageArray="Product_Observational.File_Area_Observational.0.Array_2D_Image" %}
{% else %}
{% set imageArray="Product_Observational.File_Area_Observational.0.Array_3D_Image" %}
{% set exposure="Product_Observational.Observation_Area.Discipline_Area.img:Exposure" %}
{% endif %}

{% if Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "WAC FC" or Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "WAC PB" %}
{% set sensor="WAC" %}
{% else if Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "NAC FC" or Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "NAC PB" %}
{% set sensor="NAC" %}
{% else %}
{% set sensor="UNK" %}
{% endif %}

Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{ Product_Observational.File_Area_Observational.0.Array_2D_Image.Axis_Array.0.elements }}
      Lines   = {{ Product_Observational.File_Area_Observational.0.Array_2D_Image.Axis_Array.1.elements }}
      Bands   = {% if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.Axis_Array.2.elements") %}
                {{ Product_Observational.File_Area_Observational.0.Array_2D_Image.Axis_Array.2.elements }}
                {% else %}
                1
                {% endif %}
    End_Group



    Group = Pixels
      {% if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type") %}
      {% set type=Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type %}
      Type       = {% if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "IEEE754LSBDouble" %} Double
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "IEEE754LSBSingle" %} Real
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "IEEE754MSBDouble" %} Double
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "IEEE754MSBSingle" %} Real
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedByte" %} SignedByte
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedLSB2" %} SignedWord
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedLSB4" %} SignedInteger
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedMSB2" %} SignedWord
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedMSB4" %} SignedInteger
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedByte" %} UnsignedByte
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedLSB2" %} UnsignedWord
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedLSB4" %} UnsignedInteger
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedMSB2" %} UnsignedWord
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedMSB4" %} UnsignedInteger
                   {% else %} Real
                   {% endif %}
      ByteOrder  = {% if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "IEEE754LSBDouble" %} LSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "IEEE754LSBSingle" %} LSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "IEEE754MSBDouble" %} MSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "IEEE754MSBSingle" %} MSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedByte" %} LSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedLSB2" %} LSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedLSB4" %} LSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedMSB2" %} MSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "SignedMSB4" %} MSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedByte" %} LSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedLSB2" %} LSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedLSB4" %} LSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedMSB2" %} MSB
                   {% else if Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.data_type == "UnsignedMSB4" %} MSB
                   {% else %} Lsb
                   {% endif %}
      {% else %}
      Type       = Real
      ByteOrder  = Lsb
      {% endif %}

      Base       = {% if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.value_offset") %}
                   {{ Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.value_offset }}
                   {% else if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.offset._text") %}
                   {{ Product_Observational.File_Area_Observational.0.Array_2D_Image.offset._text }}
                   {% else %}
                   0
                   {% endif %}
      Multiplier = {% if exists("Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.scaling_factor") %}
                   {{ Product_Observational.File_Area_Observational.0.Array_2D_Image.Element_Array.scaling_factor._text }}
                   {% else %}
                   1
                   {% endif %}
    End_Group
  End_Object

  Group = Instrument
    SpacecraftName            = "{{ Product_Observational.Observation_Area.Investigation_Area.name }}"
    InstrumentId              = "{{ Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name }} {{ Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name }}"
    TargetName                = {{ at(splitOnChar(Product_Observational.Observation_Area.Target_Identification.name, " "), 1) }}
    StartTime                 = {{ RemoveStartTimeZ(Product_Observational.Observation_Area.Time_Coordinates.start_date_time) }}
    ExposureDuration          = {{ Product_Observational.Observation_Area.Discipline_Area.img_Exposure.img_exposure_duration._text }}<seconds>
  End_Group

  Group = BandBin
    {# Hard code to clear filter #}
    FilterName = Clear
    {% if Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "WAC FC" or Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "WAC PB" %}
    Center     = 712.5 <nm>
    Width      = 675 <nm>
    {% else %}
    Center     = 702.5 <nm>
    Width      = 695 <nm>
    {% endif %}
  End_Group

  Group = Kernels
    NaifFrameCode = {% if Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "WAC FC" or Product_Observational.Observation_Area.Observing_System.Observing_System_Component.2.name == "WAC PB" %}
                    -159104
                    {% else %}
                    -159103
                    {% endif %}

  End_Group
End_Object

Object = Translation
End_Object
End
