{% extends "img_base.tpl" %}

{% block instrument %}
SpacecraftName              = "{{ MISSION_NAME.Value }}"
InstrumentHostName          = "{{ INSTRUMENT_HOST_NAME.Value }}"
InstrumentHostId            = {{ INSTRUMENT_HOST_ID.Value }}
{% set frameId =  FRAME_ID.Value %}
{% if frameId == "LEFT" %}
InstrumentName              = "LUNAR RECONNAISSANCE ORBITER NARROW ANGLE CAMERA LEFT"
{% else %}
InstrumentName              = "LUNAR RECONNAISSANCE ORBITER NARROW ANGLE CAMERA RIGHT"
{% endif %}
{% if frameId == "LEFT" %}
InstrumentId                = "NACL"
{% else %}
InstrumentId                = "NACR"
{% endif %}
FrameId                     = {{ frameId }}
TargetName                  = {{ TARGET_NAME.Value }}
MissioinPhaseName           = "{{ MISSION_PHASE_NAME.Value }}"
PreRollTime                 = {{ LRO_PREROLL_TIME.Value }}
StartTime                   = {{ START_TIME.Value }}
StopTime                    = {{ STOP_TIME.Value }}
SpacecraftClockPrerollCount = {{ LRO_SPACECRAFT_CLOCK_PREROLL_COUNT.Value }}
SpacecraftClockStartCount   = {{ SPACECRAFT_CLOCK_START_COUNT.Value }}
SpacecraftClockStopCount    = {{ SPACECRAFT_CLOCK_STOP_COUNT.Value }}
LineExposureDuration        = {{ LINE_EXPOSURE_DURATION.Value }} <ms>
TemperatureSCS              = {{ LRO_TEMPERATURE_SCS.Value }} <degC>
TemperatureFPA              = {{ LRO_TEMPERATURE_FPA.Value }} <degC>
TemperatureFPGA             = {{ LRO_TEMPERATURE_FPGA.Value }} <degC>
TemperatureTelescope        = {{ LRO_TEMPERATURE_TELESCOPE.Value }} <degC>
SpatialSumming              = {{ CROSSTRACK_SUMMING.Value }}
TemperatureSCSRaw           = {{ LRO_TEMPERATURE_SCS_RAW.Value }}
TemperatureFPARaw           = {{ LRO_TEMPERATURE_FPA_RAW.Value }}
TemperatureFPGARaw          = {{ LRO_TEMPERATURE_FPGA_RAW.Value }}
TemperatureTelescopeRaw     = {{ LRO_TEMPERATURE_TELESCOPE_RAW.Value }}
{% endblock %}

{% block additional_groups %}
Group = Archive
  DataSetId               = {{ DATA_SET_ID.Value }}
  OriginalProductId       = {{ ORIGINAL_PRODUCT_ID.Value }}
  ProductId               = {{ PRODUCT_ID.Value }}
  ProducerId              = "{{ PRODUCER_ID.Value }}"
  ProducerInstitutionName = "{{ PRODUCER_INSTITUTION_NAME.Value }}"
  ProductVersionId        = {{ PRODUCT_VERSION_ID.Value }}
  UploadId                = {{ UPLOAD_ID.Value }}
  OrbitNumber             = {{ ORBIT_NUMBER.Value }}
  RationalDescription     = "{{ RATIONALE_DESC.Value }}"
  DataQualityId           = {{ DATA_QUALITY_ID.Value }}
  LineExposureCode        = {{ LRO_LINE_EXPOSURE_CODE.Value }}
  DACResetLevel           = {{ LRO_DAC_RESET_LEVEL.Value }}
  ChannelAOffset          = {{ LRO_CHANNEL_A_OFFSET.Value }}
  ChannelBOffset          = {{ LRO_CHANNEL_B_OFFSET.Value }}
  CompandCode             = {{ LRO_COMPAND_CODE.Value }}
  LineCode                = {{ LRO_LINE_CODE.Value }}
  CompressionFlag         = {{ LRO_COMPRESSION_FLAG.Value }}
  Mode                    = {{ LRO_MODE.Value }}"
End_Group

Group = BandBin
  FilterName   = BroadBand
  Center       = {{ CENTER_FILTER_WAVELENGTH.Value }} <ms>
  Width        = {{ BANDWIDTH.Value }} <ms>
End_Group

Group = Kernels
  {% if frameId == "LEFT" %}
  NaifFrameCode = -85600
  {% else %}
  NaifFrameCode = -85610
  {% endif %}
End_Group
{% endblock %}

{% block translation %}
CubeAtts             = "+Real"
{% set xtermSize = 0 %}
{%- if isArray(LRO_XTERM.Value) -%}
xterm                = (
## for term in LRO_XTERM.Value
  {% set xtermSize = xtermSize + 1%}
  {{ term }} {%- if not loop.is_last -%}, {%- endif -%}
## endfor
)
{%- else -%}
{% set xtermSize = 1 %}
xterm                = {{ LRO_XTERM.Value }}
{%- endif -%}

{% set mtermSize = 0 %}
{%- if isArray(LRO_MTERM.Value) -%}
mterm                = (
## for term in LRO_MTERM.Value
  {% set mtermSize = mtermSize + 1%}
  {{ term }} {%- if not loop.is_last -%}, {%- endif -%}
## endfor
)
{%- else -%}
{% set mtermSize = 1 %}
mterm                = {{ LRO_MTERM.Value }}
{%- endif -%}

{% set btermSize = 0 %}
{%- if isArray(LRO_BTERM.Value) -%}
bterm                = (
## for term in LRO_BTERM.Value
  {% set btermSize = btermSize + 1%}
  {{ term }} {%- if not loop.is_last -%}, {%- endif -%}
## endfor
)
{%- else -%}
{% set btermSize = 1 %}
bterm                = {{ LRO_BTERM.Value }}
{%- endif -%}
{% if mtermSize != xtermSize or btermSize != xtermSize%}
Failure              = "The decompanding terms do not have the same dimensions"
{% endif %}
{% endblock %}
