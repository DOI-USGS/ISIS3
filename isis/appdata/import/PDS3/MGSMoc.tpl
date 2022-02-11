Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{ IMAGE.LINE_SAMPLES.Value }}
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
    {% if exists("SPACECRAFT_NAME") %}
    SpacecraftName       =  "MARS GLOBAL SURVEYOR"
    {% endif %}
    {% if exists("INSTRUMENT_ID") %}
    InstrumentId         = {{ INSTRUMENT_ID.Value }}
    {% endif %}
    {% if exists("TARGET_NAME") %}
    TargetName           = Mars
    {% endif %}
    {% if exists("START_TIME") %}
    {% set startTime=START_TIME.Value %}
    {% set YearDOY=YearDoy(startTime) %}
    StartTime            = {{ START_TIME.Value }}
    {% endif %}
    {% if exists("STOP_TIME") %}
    StopTime            = {{ STOP_TIME.Value }}
    {% endif %}
    {% if exists("CROSSTRACK_SUMMING") %}
    CrosstrackSumming    = {{ CROSSTRACK_SUMMING.Value }}
    {% endif %}
    {% if exists("DOWNTRACK_SUMMING") %}
    DowntrackSumming     = {{ DOWNTRACK_SUMMING.Value }}
    {% endif %}
    {% if exists("FOCAL_PLANE_TEMPERATURE") %}
    FocalPlaneTemperature= {{ FOCAL_PLANE_TEMPERATURE.Value }}
    {% endif %}
    {% if exists("GAIN_MODE_ID") %}
    GainModeId           = {{ GAIN_MODE_ID.Value }}
    {% endif %}
    {% if exists("LINE_EXPOSURE_DURATION") %}
    LineExposureDuration = {{ LINE_EXPOSURE_DURATION.Value }} <milliseconds>
    {% endif %}
    {% if exists("MISSION_PHASE_NAME") %}
    MissionPhaseName     = {{ MISSION_PHASE_NAME.Value }}
    {% endif %}
    {% if exists("OFFSET_MODE_ID") %}
    OffsetModeId         = {{ OFFSET_MODE_ID.Value }}
    {% endif %}
    {% if exists("SPACECRAFT_CLOCK_START_COUNT") %}
    SpacecraftClockCount = {{ SPACECRAFT_CLOCK_START_COUNT.Value }}
    {% set clockCount=SPACECRAFT_CLOCK_START_COUNT.Value %}
    {% endif %}
    {% if exists("RATIONALE_DESC") %}
    RationaleDesc        = "{{ RATIONALE_DESC.Value }}"
    {% endif %}
    {% if exists("ORBIT_NUMBER") %}
    OrbitNumber          = {{ ORBIT_NUMBER.Value }}
    {% endif %}
    {% if exists("EDIT_MODE_ID") %}
    FirstLineSample      = {{ int(EDIT_MODE_ID.Value) + 1}}
    {% endif %}
  End_Group

  Group = Archive
    {% if exists("DATA_SET_ID") %}
    DataSetId           = {{ DATA_SET_ID.Value }}
    {% endif %}
    {% if exists("PRODUCT_ID") %}
    ProductId           = {{ PRODUCT_ID.Value }}
    {% set productId=PRODUCT_ID.Value %}
    {% endif %}
    {% if exists("PRODUCER_ID") %}
    ProducerId          = {{ PRODUCER_ID.Value }}
    {% endif %}
    {% if exists("PRODUCT_CREATION_TIME") %}
    ProductCreationTime = {{ PRODUCT_CREATION_TIME.Value }}
    {% endif %}
    {% if exists("SOFTWARE_NAME") %}
    SoftwareName        = "{{ SOFTWARE_NAME.Value }}"
    {% endif %}
    {% if exists("UPLOAD_ID") %}
    UploadId            = {{ UPLOAD_ID.Value }}
    {% endif %}
    {% if exists("DATA_QUALITY_DESC") %}
    DataQualityDesc     = {{ DATA_QUALITY_DESC.Value }}
    {% endif %}
    ImageNumber         = {{ SetImageNumber(YearDOY, productId) }}
    ImageKeyId          = {{ SetImageKeyId(clockCount, productId) }}
  End_Group

  Group = BandBin
    {% set filter=FILTER_NAME.Value %}
    FilterName    = {% if filter == "BLUE" or filter == "RED" %} {{ filter }}
                    {% else %} BROAD_BAND
                    {% endif %}
    OriginalBand  = 1
    Center        = {% if filter == "BLUE" %} 0.4346 <micrometers>
                    {% else if filter == "RED" %} 0.6134 <micrometers>
                    {% else %} 0.700 <micrometers>
                    {% endif %}
    Width         = {% if filter == "BLUE" %} 0.050 <micrometers>
                    {% else if filter == "RED" %} 0.050 <micrometers>
                    {% else %} 0.400 <micrometers>
                    {% endif %}
  End_Group

  Group = Kernels
  {% if filter == "BLUE" %}
  {% set frameCode=-94033 %}
  {% else if filter == "RED" %}
  {% set frameCode=-94032 %}
  {% else %}
  {% set frameCode=-94031 %}
  {% endif %}
    NaifFrameCode = {{ frameCode }}
  End_Group
End_Object

Object = Translation
  {% if exists("ptrIMAGE.Value") %}
  DataFilePointer             = {{ ptrIMAGE.Value }}
  {% endif %}
  {% if exists("RECORD_BYTES") %}
  DataFileRecordBytes         = {{ RECORD_BYTES.Value }}
  {% endif %}
  compressed = {% if exists("IMAGE.ENCODING_TYPE") %} true
               {% else %} false
               {% endif %}
  projected  = {% if exists("IMAGE_MAP_PROJECTION") %} true
               {% else %} false
               {% endif %}
End_Object
End
