Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{ at(QUBE.CORE_ITEMS.Value, 0) }}
      Lines   = {{ at(QUBE.CORE_ITEMS.Value, 2) }}
      Bands   = {{ at(QUBE.CORE_ITEMS.Value, 1) }}
    End_Group

    Group = Pixels
      {%- set sampbits=QUBE.CORE_ITEM_BYTES.Value -%}
      {%- if sampbits == "1" -%}
      {%- set sampbits="8" -%}
      {%- else if sampbits == "2" -%}
      {%- set sampbits="16" -%}
      {%- else if sampbits == "4" -%}
      {%- set sampbits="32" -%}
      {%- endif -%}

      {%- set type=QUBE.CORE_ITEM_TYPE.Value -%}
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
      Base       = {% if exists("QUBE.CORE_BASE.Value") %}
                   {{ QUBE.CORE_BASE.Value }}
                   {% else %}
                   0.0
                   {% endif %}
      Multiplier = {% if exists("QUBE.CORE_MULTIPLIER.Value") %}
                   {{ QUBE.CORE_MULTIPLIER.Value }}
                   {% else %}
                   1.0
                   {% endif %}
    End_Group
  End_Object

  Group = Instrument
    {%- if exists("ROOT.MISSION_NAME.Value") -%}
    {%- set infoGroup=ROOT -%}
    {%- else -%}
    {%- set infoGroup=QUBE -%}
    {%- endif -%}
    SpacecraftName       = {{ infoGroup.MISSION_NAME.Value }}
    InstrumentId         = {{ infoGroup.INSTRUMENT_ID.Value }}
    TargetName           = {{ infoGroup.TARGET_NAME.Value }}
    SpacecraftClockStartCount = {{ infoGroup.SPACECRAFT_CLOCK_START_COUNT.Value }}
    SpacecraftClockStopCount = {{ infoGroup.SPACECRAFT_CLOCK_STOP_COUNT.Value }}
    StartTime            = {% set startTime=infoGroup.START_TIME.Value %}
                           {{ RemoveStartTimeZ(startTime) }}
    StopTime             = {% set stopTime=infoGroup.STOP_TIME.Value %}
                           {{ RemoveStartTimeZ(stopTime) }}
    NativeStartTime      = {{ infoGroup.NATIVE_START_TIME.Value }}
    NativeStopTime       = {{ infoGroup.NATIVE_STOP_TIME.Value }}
    InterlineDelayDuration = {{ infoGroup.INTERLINE_DELAY_DURATION.Value }}
    XOffset              = {{ infoGroup.X_OFFSET.Value }}
    ZOffset              = {{ infoGroup.Z_OFFSET.Value }}
    SwathWidth           = {{ infoGroup.SWATH_WIDTH.Value }}
    SwathLength          = {{ infoGroup.SWATH_LENGTH.Value }}
    Channel              = VIS
    ExposureDuration     = ({{ QUBE.EXPOSURE_DURATION.Value.0 }} <IR>, {{ QUBE.EXPOSURE_DURATION.Value.1 }} <VIS>)
    GainMode             = ({{ QUBE.GAIN_MODE_ID.Value.0 }}, {{ QUBE.GAIN_MODE_ID.Value.1 }})
  End_Group

  Group = Archive
    MissionPhaseName     = {{ infoGroup.MISSION_PHASE_NAME.Value }}
    SequenceId           = {{ infoGroup.SEQUENCE_ID.Value }}
    SequenceTitle        = {{ infoGroup.SEQUENCE_TITLE.Value }}
    ObservationId        = {{ infoGroup.OBSERVATION_ID.Value }}
    ProductId            = {{ infoGroup.PRODUCT_ID.Value }}
    InstrumentModeId     = {{ infoGroup.INSTRUMENT_MODE_ID.Value }}
    CompressorId         = {{ infoGroup.COMPRESSOR_ID.Value }}
    PowerStateFlag       = ({{ infoGroup.POWER_STATE_FLAG.Value.0 }}, {{ infoGroup.POWER_STATE_FLAG.Value.1 }})
    SpectralSummingFlag  = {{ infoGroup.SPECTRAL_SUMMING_FLAG.Value }}
    SpectralEditingFlag  = {{ infoGroup.SPECTRAL_EDITING_FLAG.Value }}
    StarTracking         = {{ infoGroup.STAR_TRACKING.Value }}
    SnapshotMode         = {{ infoGroup.SNAPSHOT_MODE.Value }}
    SamplingMode         = ({{ QUBE.SAMPLING_MODE_ID.Value.0 }}, {{ QUBE.SAMPLING_MODE_ID.Value.1 }})
  End_Group
End_Object

Object = Translation
  {% if exists("QUBE.CORE_NULL.Value") %}
  PdsNULL              = {{ QUBE.CORE_NULL.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_LOW_REPR_SATURATION.Value") %}
  PdsLRS               = {{ QUBE.CORE_LOW_REPR_SATURATION.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_LOW_INSTR_SATURATION.Value") %}
  PdsLIS               = {{ QUBE.CORE_LOW_INSTR_SATURATION.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_HIGH_REPR_SATURATION.Value") %}
  PdsHRS               = {{ QUBE.CORE_HIGH_REPR_SATURATION.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_HIGH_INSTR_SATURATION.Value") %}
  PdsHIS               = {{ QUBE.CORE_HIGH_INSTR_SATURATION.Value }}
  {% endif %}
  OriginalAxisOrder    = {{ QUBE.AXIS_NAME.Value.0 }}{{ QUBE.AXIS_NAME.Value.1 }}{{ QUBE.AXIS_NAME.Value.2 }}
  OriginalImageOffset  = {{ imageOffset }}
End_Object
End
