{% extends "img_base.tpl" %}

{% block instrument %}
SpacecraftName          =  Cassini-Huygens

{% set targetName =  TARGET_NAME.Value %}
{% if targetName == "DARK SKY"%}
{% set targetName = Sky %}
{% else if targetName == "S8_2004" %}
{% set targetName = Sky %}
{% else if targetName == "S12_2004" %}
{% set targetName = "Sky" %}
{% else if targetName == "S13_2004" %}
{% set targetName = "Sky" %}
{% else if targetName == "S14_2004" %}
{% set targetName = "Sky" %}
{% else if targetName == "S18_2004" %}
{% set targetName = "Sky" %}
{% else if  targetName == "UNK"%}
{% set targetName = "Unknown" %}
{% else %}
{% set targetName = lower(targetName) %}
{% set targetName = capitalize(targetName) %}
{% endif %}

InstrumentId            = {{ INSTRUMENT_ID.Value }}
TargetName              = {{ targetName }}
StartTime               = {% set startTime=START_TIME.Value %}
                          {{ RemoveStartTimeZ(startTime) }}
StopTime                = {% set stopTime=STOP_TIME.Value %}
                          {{ RemoveStartTimeZ(stopTime) }}
ExposureDuration        = {{ EXPOSURE_DURATION.Value }} <Milliseconds>

{% set antibloomingStateFlag = ANTIBLOOMING_STATE_FLAG.Value %}
{% if antibloomingStateFlag == "OFF" %}
{% set antibloomingStateFlag = "Off" %}
{% else if antibloomingStateFlag == "ON" %}
{% set antibloomingStateFlag = "On" %}
{% else if antibloomingStateFlag == "UNK" %}
{% set antibloomingStateFlag = "Unknown" %}
{% endif %}
AntibloomingStateFlag   = {{ antibloomingStateFlag }}

BiasStripMean           = {{ BIAS_STRIP_MEAN.Value }}

{% if exists("INST_CMPRS_RATIO.Value") %}
{% set compressionRatio = INST_CMPRS_RATIO.Value %}
{% else if exists("COMPRESSION_RATIO.Value") %}
{% set compressionRatio = COMPRESSION_RATIO.Value %}
{% endif %}
{% if compressionRatio == "N/A" %}
{% set compressionRatio = "NotCompressed" %}
{% endif %}
CompressionRatio        = {{ compressionRatio }}

{% if exists("INST_CMPRS_TYPE.Value") %}
  {% set compressionType = INST_CMPRS_TYPE.Value %}
{% else if exists("ENCODING_TYPE.Value") %}
  {% set compressionType = ENCODING_TYPE.Value %}
{% endif %}

{% if compressionType == "NOTCOMP" %}
{% set compressionType = "NotCompressed" %}
{% else if compressionType == "LOSSLESS" %}
{% set compressionType = "Lossless" %}
{% else if compressionType == "LOSSY" %}
{% set compressionType = "Lossy" %}
{% endif %}
CompressionType         = {{ compressionType }}

{% set dataConversionType = DATA_CONVERSION_TYPE.Value %}
{% if dataConversionType == "12BIT" %}
{% set dataConversionType = "12Bit" %}
{% else if dataConversionType == "TABLE" %}
{% set dataConversionType = "Table" %}
{% else if dataConversionType == "8LSB" %}
{% set dataConversionType = "8LSB" %}
{% endif %}
DataConversionType      = {{ dataConversionType }}

{% set delayedReadoutFlag = DELAYED_READOUT_FLAG.Value %}
{% if delayedReadoutFlag == "YES" %}
{% set delayedReadoutFlag = "Yes" %}
{% else if delayedReadoutFlag == "NO" %}
{% set delayedReadoutFlag = "No" %}
{% else if delayedReadoutFlag == "UNK" %}
{% set delayedReadoutFlag = "Unknown" %}
{% endif %}
DelayedReadoutFlag      = {{ delayedReadoutFlag }}

{% set flightSoftwareVersionId = FLIGHT_SOFTWARE_VERSION_ID.Value %}
{% if flightSoftwareVersionId == "UNK" %}
{% set flightSoftwareVersionId = "Unknown" %}
{% endif %}
FlightSoftwareVersionId = {{ flightSoftwareVersionId }}

{% set gainModeId = GAIN_MODE_ID.Value %}
{% if gainModeId == "12 ELECTRONS PER DN" %}
{% set gainModeId = "12" %}
{% else if gainModeId == "29 ELECTRONS PER DN" %}
{% set gainModeId = "29" %}
{% else if gainModeId == "95 ELECTRONS PER DN" %}
{% set gainModeId = "95" %}
{% else if gainModeId == "215 ELECTRONS PER DN" %}
{% set gainModeId = "215" %}
{% endif %}
GainModeId              = {{ gainModeId }} <ElectronsPerDN>

{% set gainState = GAIN_MODE_ID.Value %}
{% if gainState == "12 ELECTRONS PER DN" %}
{% set gainState = "3" %}
{% else if gainState == "29 ELECTRONS PER DN" %}
{% set gainState = "2" %}
{% else if gainState == "95 ELECTRONS PER DN" %}
{% set gainState = "1" %}
{% else if gainState == "215 ELECTRONS PER DN" %}
{% set gainState = "0" %}
{% endif %}
GainState               = {{ gainState }}
ImageTime               = {% set imageTime=IMAGE_TIME.Value %}
                          {{ RemoveStartTimeZ(imageTime) }}
InstrumentDataRate      = {{ INSTRUMENT_DATA_RATE.Value }} <KilobitsPerSecond>
OpticsTemperature       = ({{ OPTICS_TEMPERATURE.Value.0 }}, {{ OPTICS_TEMPERATURE.Value.1 }} <DegreesCelcius>)

{% set ReadoutCycleIndex = READOUT_CYCLE_INDEX.Value %}
{% if ReadoutCycleIndex == "UNK" %}
{% set ReadoutCycleIndex = "Unknown" %}
{% endif %}
ReadoutCycleIndex       = {{ ReadoutCycleIndex }}

{% set shutterModeId = SHUTTER_MODE_ID.Value %}
{% if shutterModeId == "UNK" %}
{% set shutterModeId = 'Unknown' %}
{% else if shutterModeId == "BOTSIM" %}
{% set shutterModeId= "BothSim" %}
{% else if shutterModeId == "NACONLY" %}
{% set shutterModeId = "NacOnly" %}
{% else if shutterModeId == "WACONLY" %}
{% set shutterModeId = "WacOnly" %}
{% endif %}
ShutterModeId           = {{ shutterModeId }}

{% set shutterStateId = SHUTTER_STATE_ID.Value %}
{% set shutterStateId = capitalize(shutterStateId) %}
ShutterStateId          = {{ shutterStateId }}

{% set summingMode = INSTRUMENT_MODE_ID.Value %}
{% if summingMode == "FULL" %}
{% set summingMode = "1" %}
{% else if summingMode == "SUM2" %}
{% set summingMode = "2" %}
{% else if summingMode == "SUM4" %}
{% set summingMode = "4" %}
{% endif %}
SummingMode             = {{ summingMode }}

{% set instrumentModeId = INSTRUMENT_MODE_ID.Value %}
{% if instrumentModeId == "FULL" %}
{% set instrumentModeId = "Full" %}
{% else if instrumentModeId == "SUM2" %}
{% set instrumentModeId = "Sum2" %}
{% else if instrumentModeId == "SUM4" %}
{% set instrumentModeId = "Sum4" %}
{% endif %}
InstrumentModeId        = {{ instrumentModeId }}

SpacecraftClockCount    = {{SPACECRAFT_CLOCK_CNT_PARTITION.Value}}/{{SPACECRAFT_CLOCK_START_COUNT.Value}}
{% endblock %}



{% block additional_groups %}

Group = Archive
  DataSetId     = {{ DATA_SET_ID.Value }}
  ImageNumber   = {{ IMAGE_NUMBER.Value }}
  ObservationId = {{ OBSERVATION_ID.Value }}
  ProductId     = {{ PRODUCT_ID.Value  }}
End_Group

Group = BandBin
  {% if exists("FILTER_NAME") %}
  {% set filterName=FILTER_NAME.Value %}
  FilterName   = {{ filterName.0 }}/{{ filterName.1 }}
  OriginalBand = 1
  {% set cassiniIssBandInfo = CassiniIssBandInfo(INSTRUMENT_ID.Value, filterName.0, filterName.1) %}
  Center       = {{ cassiniIssBandInfo.0 }}
  Width        = {{ cassiniIssBandInfo.1 }}
  {% endif %}
End_Group

Group = Kernels
  {% set instrument = INSTRUMENT_NAME.Value %}
  {% if instrument == "ISSNA" %}
  NaifFrameCode = "-82360"
  {% else %}
  NaifFrameCode = "-82361"
  {% endif %}
End_Group
{% endblock %}

{% block translation %}
CubeAtts                = "+SignedWord+-32752:32767"
DataPrefixBytes         = {{ IMAGE.LINE_PREFIX_BYTES.Value }}
StretchPairs            = {{ CassiniIssStretchPairs() }}
DataConversionType      = {{ dataConversionType }}
ValidMaximum            = {{ VALID_MAXIMUM.Value.1 }}
SummingMode             = {{ summingMode }}
CompressionType         = {{ compressionType }}
FlightSoftwareVersionId = {{ FLIGHT_SOFTWARE_VERSION_ID.Value }}
VicarLabelBytes         = {{ IMAGE_HEADER.BYTES.Value }}

Object = AncillaryProcess
  ProcessFunction = cassiniIssCreateLinePrefixTable
End_Object

Object = AncillaryProcess
  ProcessFunction = cassiniIssFixLabel
End_Object

Object = PostProcess
  ProcessFunction    = cassiniIssFixDnPostProcess
  StretchPairs       = {{ CassiniIssStretchPairs() }}
  DataConversionType = {{ dataConversionType }}
  ValidMaximum       = {{ VALID_MAXIMUM.Value.1 }}
End_Object
{% endblock %}
