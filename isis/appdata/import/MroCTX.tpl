{% if exists("SPATIAL_SUMMING") %}
  {% set sumMode=SPATIAL_SUMMING.Value %}
{% else %}
  {% set sumMode=SAMPLING_FACTOR.Value %}
{% endif %}

{% if exists("EDIT_MODE_ID") %}
  {% set editMode=EDIT_MODE_ID.Value %}
{% else %}
  {% set editMode=SAMPLE_FIRST_PIXEL.Value %}
{% endif %}

{%- if exists("EDIT_MODE_ID") -%}
  {%- set startSamp=EDIT_MODE_ID.Value -%}
{%- else -%}
  {%- set startSamp=SAMPLE_FIRST_PIXEL.Value -%}
{%- endif -%}
{%- if sumMode == "1" -%}
  {%- set startPix=0 -%}
  {%- if editMode == "0" -%}
    {%- set endPix=37 -%}
    {%- set suf=18 -%}
  {%- else -%}
    {%- set endPix=15 -%}
    {%- set suf=0 -%}
  {%- endif -%}
{%- else if sumMode == "2" -%}
  {%- if editMode == "0" -%}
    {%- set startPix=7 -%}
    {%- set endPix=18 -%}
    {%- set suf=9 -%}
  {%- else -%}
    {%- set startPix=0 -%}
    {%- set endPix=7 -%}
    {%- set suf=0 -%}
  {%- endif -%}
{%- endif -%}

Object = IsisCube
  Object = Core
  Group = Dimensions
    {%- set samples = IMAGE.LINE_SAMPLES.Value -%}
    Samples = {{ samples - endPix - suf - 1 }}
    Lines   = {{ IMAGE.LINES.Value }}
    Bands   = {% if exists("IMAGE.BANDS.Value") %}
              {{ IMAGE.BANDS.Value }}
              {% else %}
              1
              {% endif %}
  End_Group

  Group = Pixels
    {%- set type=IMAGE.SAMPLE_TYPE.Value -%}
    {%- set sampbits=IMAGE.SAMPLE_BITS.Value -%}
    {%- if type == "LSB_INTEGER" -%} {%- set pixType="Integer" -%}
    {%- else if type == "MSB_INTEGER" -%} {%- set pixType="Integer" -%}
    {%- else if type == "PC_INTEGER" -%} {%- set pixType="Integer" -%}
    {%- else if type == "MAC_INTEGER" -%} {%- set pixType="Integer" -%}
    {%- else if type == "SUN_INTEGER" -%} {%- set pixType="Integer" -%}
    {%- else if type == "VAX_INTEGER" -%} {%- set pixType="Integer" -%}
    {%- else if type == "UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
    {%- else if type == "UNSIGNED INTEGER" -%} {%- set pixType="Natural" -%}
    {%- else if type == "LSB_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
    {%- else if type == "MSB_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
    {%- else if type == "PC_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
    {%- else if type == "MAC_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
    {%- else if type == "SUN_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
    {%- else if type == "VAX_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
    {%- else if type == "FLOAT" -%} {%- set pixType="Real" -%}
    {%- else if type == "REAL" -%} {%- set pixType="Real" -%}
    {%- else if type == "PC_REAL" -%} {%- set pixType="Real" -%}
    {%- else if type == "IEEE_REAL" -%} {%- set pixType="Real" -%}
    {%- else if type == "MAC_REAL" -%} {%- set pixType="Real" -%}
    {%- else if type == "SUN_REAL" -%} {%- set pixType="Real" -%}
    {%- else if type == "VAX_REAL" -%} {%- set pixType="Real" -%}
    {%- else -%} {%- set pixType="LSB_INTEGER" -%}
    {%- endif -%}
    Type       = {% if pixType == "Real" and sampbits == "64" %} Double
                 {% else if pixType == "Real" and sampbits == "32" %} Real
                 {% else if pixType == "Integer" and sampbits == "8" %} UnsignedByte
                 {% else if pixType == "Integer" and sampbits == "16" %} SignedWord
                 {% else if pixType == "Integer" and sampbits == "32" %} SignedInteger
                 {% else if pixType == "Natural" and sampbits == "8" %} UnsignedByte
                 {% else if pixType == "Natural" and sampbits == "16" %} UnsignedWord
                 {% else if pixType == "Natural" and sampbits == "32" %} UnsignedInteger
                 {% endif %}
    ByteOrder  = {% if type == "LSB_INTEGER" %} LSB
                 {% else if type == "PC_INTEGER" %} LSB
                 {% else if type == "VAX_INTEGER" %} LSB
                 {% else if type == "LSB_UNSIGNED_INTEGER" %} LSB
                 {% else if type == "PC_UNSIGNED_INTEGER" %} LSB
                 {% else if type == "VAX_UNSIGNED_INTEGER" %} LSB
                 {% else if type == "PC_REAL" %} LSB
                 {% else if type == "VAX_REAL" %} LSB
                 {% else if type == "MSB_INTEGER" %} MSB
                 {% else if type == "MAC_INTEGER" %} MSB
                 {% else if type == "SUN_INTEGER" %} MSB
                 {% else if type == "UNSIGNED_INTEGER" %} MSB
                 {% else if type == "UNSIGNED INTEGER" %} MSB
                 {% else if type == "MSB_UNSIGNED_INTEGER" %} MSB
                 {% else if type == "MAC_UNSIGNED_INTEGER" %} MSB
                 {% else if type == "SUN_UNSIGNED_INTEGER" %} MSB
                 {% else if type == "FLOAT" %} MSB
                 {% else if type == "REAL" %} MSB
                 {% else if type == "IEEE_REAL" %} MSB
                 {% else if type == "MAC_REAL" %} MSB
                 {% else if type == "SUN_REAL" %} MSB
                 {% else %} LSB_INTEGER
                 {% endif %}
    Base       = {% if exists("IMAGE.OFFSET.Value") %}
                 {{ IMAGE.OFFSET.Value }}
                 {% else %}
                 0.0
                 {% endif %}
    Multiplier = {% if exists("IMAGE.SCALING_FACTOR.Value") %}
                 {{ IMAGE.SCALING_FACTOR.Value }}
                 {% else %}
                 1.0
                 {% endif %}
  End_Group
  End_Object

  Group = Instrument
    SpacecraftName        = {% if exists("SPACECRAFT_NAME.Value") %}
                            Mars_Reconnaissance_Orbiter
                            {% else %}
                            UNKNOWN
                            {% endif %}
    InstrumentId          = {% if exists("INSTRUMENT_ID.Value") %}
                            {{ INSTRUMENT_ID.Value }}
                            {% else %}
                            UNKNOWN
                            {% endif %}
    TargetName            = {% if exists("TARGET_NAME.Value")%}
                            {% set target=TARGET_NAME.Value %}
                            {% if target == "MARS"%} Mars
                            {% else if target == "PHOBOS" %} Phobos
                            {% else if target == "DEIMOS" %} Deimos
                            {% else if target == "MOON" %} Moon
                            {% else %} Sky
                            {% endif %}
                            {% else %} UNKNOWN
                            {% endif %}
    MissionPhaseName      = {% if exists("MISSION_PHASE_NAME.Value") %}
                            {% set missionPhase = MISSION_PHASE_NAME.Value %}
                            {% if missionPhase == "MAPPING"%} Mapping
                            {% else %} {{ missionPhase }}
                            {% endif %}
                            {% else %} UNKNOWN
                            {% endif %}
    StartTime             = {% if exists("START_TIME.Value") %}
                            {{ START_TIME.Value }}
                            {% else %} -9999
                            {% endif %}
    SpacecraftClockCount  = {% if exists("SPACECRAFT_CLOCK_START_COUNT.Value") %}
                            {{ SPACECRAFT_CLOCK_START_COUNT.Value }}
                            {% else %} -9999
                            {% endif %}
    OffsetModeId          = {% if exists("OFFSET_MODE_ID.Value") %}
                            {{ OFFSET_MODE_ID.Value }}
                            {% else %} UNKNOWN
                            {% endif %}
    LineExposureDuration  = {% if exists("LINE_EXPOSURE_DURATION.Value") %}
                            {{ LINE_EXPOSURE_DURATION.Value }} <MSEC>
                            {% else %} UNKNOWN
                            {% endif %}
    FocalPlaneTemperature = {% if exists("FOCAL_PLANE_TEMPERATURE.Value") %}
                            {{ FOCAL_PLANE_TEMPERATURE.Value }} <K>
                            {% else %} UNKNOWN
                            {% endif %}
    SampleBitModeId       = {% if exists("SAMPLE_BIT_MODE_ID.Value") %}
                            {{ SAMPLE_BIT_MODE_ID.Value }}
                            {% else %} UNKNOWN
                            {% endif %}
    SpatialSumming        = {% if exists("SPATIAL_SUMMING") %}
                            {{ SPATIAL_SUMMING.Value }}
                            {% else %}
                            {{ SAMPLING_FACTOR.Value }}
                            {% endif %}
    SampleFirstPixel      = {% if exists("EDIT_MODE_ID") %}
                            {{ EDIT_MODE_ID.Value }}
                            {% else %}
                            {{ SAMPLE_FIRST_PIXEL.Value }}
                            {% endif %}
  End_Group

  Group = Archive
    DataSetId           = {% if exists("DATA_SET_ID.Value") %}
                          {{ DATA_SET_ID.Value }}
                          {% else %} UNKNOWN
                          {% endif %}
    ProductId           = {% if exists("PRODUCT_ID.Value") %}
                          {{ PRODUCT_ID.Value }}
                          {% else %} UNKNOWN
                          {% endif %}
    ProducerId          = {% if exists("PRODUCER_ID.Value") %}
                          {{ PRODUCER_ID.Value }}
                          {% else %} UNKNOWN
                          {% endif %}
    ProductCreationTime = {% if exists("PRODUCT_CREATION_TIME.Value") %}
                          {{ PRODUCT_CREATION_TIME.Value }}
                          {% else %} UNKNOWN
                          {% endif %}
    OrbitNumber         = {% if exists("ORBIT_NUMBER.Value") %}
                          {{ ORBIT_NUMBER.Value }}
                          {% else %} UNKNOWN
                          {% endif %}
  End_Group

  Group = BandBin
    FilterName = BroadBand
    Center     = 0.65 <micrometers>
    Width      = 0.15 <micrometers>
  End_Group

  Group = Kernels
    NaifFrameCode = -74021
  End_Group
End_Object

Object = Translation
  {% if exists("ptrIMAGE.Value") %}
  DataFilePointer             = {{ ptrIMAGE.Value }}
  {% endif %}
  {% if exists("RECORD_BYTES") %}
  DataFileRecordBytes         = {{ RECORD_BYTES.Value }}
  {% endif %}
  DataPrefixBytes             = {{ endPix + 1 }}
  DataSuffixBytes             = {{ suf }}
End_Object
End
