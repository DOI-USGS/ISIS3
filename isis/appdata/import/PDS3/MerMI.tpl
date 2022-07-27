{% extends "img_base.tpl" %}

{% block instrument %}
    {% if exists("ROVER_MOTION_COUNTER") %}
    RoverMotionCounter          = ({{ join(ROVER_MOTION_COUNTER.Value, ",") }})
    {% else %}
    RoverMotionCounter          = NULL
    {% endif %}
    {% if exists("ROVER_MOTION_COUNTER_NAME") %}
    RoverMotionCounterName      = ({{ join(ROVER_MOTION_COUNTER_NAME.Value, ",") }})
    {% else %}
    RoverMotionCounterName          = NULL
    {% endif %}
    {% set spacecraftName=INSTRUMENT_HOST_NAME.Value %}
    SpacecraftName              = {% if spacecraftName == "MARS EXPLORATION ROVER 1" %}
                                  MARS_EXPLORATION_ROVER_1
                                  {% else if spacecraftName == "MARS EXPLORATION ROVER 2" %}
                                  MARS_EXPLORATION_ROVER_2
                                  {% else if spacecraftName == "SIMULATED MARS EXPLORATION ROVER 1" %}
                                  SIMULATED_MARS_EXPLORATION_ROVER_1
                                  {% else if spacecraftName == "SIMULATED MARS EXPLORATION ROVER 2" %}
                                  SIMULATED_MARS_EXPLORATION_ROVER_2
                                  {% else %}
                                  {{ spacecraftName }}
                                  {% endif %}
    InstrumentID                = {{ INSTRUMENT_ID.Value }}
    InstrumentName              = "{{ INSTRUMENT_NAME.Value }}"
    InstrumentSerialNumber      = {% if exists("INSTRUMENT_SERIAL_NUMBER") %}
                                  {{ INSTRUMENT_SERIAL_NUMBER.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    LocalTrueSolarTime          = {% if exists("LOCAL_TRUE_SOLAR_TIME") %}
                                  {{ LOCAL_TRUE_SOLAR_TIME.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    PlanetDayNumber             = {% if exists("PLANET_DAY_NUMBER") %}
                                  {{ PLANET_DAY_NUMBER.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    SolarLongitude              = {% if exists("SOLAR_LONGITUDE") %}
                                  {{ SOLAR_LONGITUDE.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    SpacecraftClockCntPartition = {% if exists("SPACECRAFT_CLOCK_CNT_PARTITION") %}
                                  {{ SPACECRAFT_CLOCK_CNT_PARTITION.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    SpacecraftClockStartCount   = {% if exists("SPACECRAFT_CLOCK_START_COUNT") %}
                                  {{ SPACECRAFT_CLOCK_START_COUNT.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    SpacecraftClockStopCount    = {% if exists("SPACECRAFT_CLOCK_STOP_COUNT") %}
                                  {{ SPACECRAFT_CLOCK_STOP_COUNT.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    StartTime                   = {% if exists("START_TIME") %}
                                  {% set startTime=START_TIME.Value %}
                                  {{ RemoveStartTimeZ(startTime) }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    StopTime                    = {% if exists("STOP_TIME") %}
                                  {% set stopTime=STOP_TIME.Value %}
                                  {{ RemoveStartTimeZ(stopTime) }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    ExposureDuration            = {% if exists("INSTRUMENT_STATE_PARMS.EXPOSURE_DURATION") %}
                                  {{ INSTRUMENT_STATE_PARMS.EXPOSURE_DURATION.Value }}<ms>
                                  {% else %}
                                  NULL
                                  {% endif %}
    ExposureDurationCount       = {% if exists("INSTRUMENT_STATE_PARMS.EXPOSURE_DURATION_COUNT") %}
                                  {{ INSTRUMENT_STATE_PARMS.EXPOSURE_DURATION_COUNT.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    FilterName                  = {% if exists("INSTRUMENT_STATE_PARMS.FILTER_NAME") %}
                                  {{ INSTRUMENT_STATE_PARMS.FILTER_NAME.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    FilterNumber                = {% if exists("INSTRUMENT_STATE_PARMS.FILTER_NUMBER") %}
                                  {{ INSTRUMENT_STATE_PARMS.FILTER_NUMBER.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    FlatFieldCorrectionFlag     = {% if exists("INSTRUMENT_STATE_PARMS.FLAT_FIELD_CORRECTION_FLAG") %}
                                  {{ INSTRUMENT_STATE_PARMS.FLAT_FIELD_CORRECTION_FLAG.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    InstrumentModeID            = {% if exists("INSTRUMENT_STATE_PARMS.INSTRUMENT_MODE_ID") %}
                                  {{ INSTRUMENT_STATE_PARMS.INSTRUMENT_MODE_ID.Value }}
                                  {% else %}
                                  NULL
                                  {% endif %}
    {% if exists("INSTRUMENT_STATE_PARMS.INSTRUMENT_TEMPERATURE") %}
    InstrumentTemperature      = ({{ join(INSTRUMENT_STATE_PARMS.INSTRUMENT_TEMPERATURE.Value, ",") }})<degC>
    {% else %}
    InstrumentTemperature          = NULL
    {% endif %}

    {% if exists("INSTRUMENT_STATE_PARMS.INSTRUMENT_TEMPERATURE_NAME") %}
    InstrumentTemperatureName = ("{{ join(INSTRUMENT_STATE_PARMS.INSTRUMENT_TEMPERATURE_NAME.Value, "\", \"")}}")
    {% else %}
    InstrumentTemperatureName = NULL
    {% endif %}

    OffsetModeID                  = {% if exists("INSTRUMENT_STATE_PARMS.OFFSET_MODE_ID") %}
                                    {{ INSTRUMENT_STATE_PARMS.OFFSET_MODE_ID.Value }}
                                    {% else %}
                                    NULL
                                    {% endif %}
    ShutterEffectCorrectionFlag   = {% if exists("INSTRUMENT_STATE_PARMS.SHUTTER_EFFECT_CORRECTION_FLAG") %}
                                    {{ INSTRUMENT_STATE_PARMS.SHUTTER_EFFECT_CORRECTION_FLAG.Value }}
                                    {% else %}
                                    NULL
                                    {% endif %}
    TemperatureMiCCD            = {{ INSTRUMENT_STATE_PARMS.INSTRUMENT_TEMPERATURE.Value.6}}
    TemperatureMiElectronics    = {{ INSTRUMENT_STATE_PARMS.INSTRUMENT_TEMPERATURE.Value.7 }}
{% endblock %}

{% block additional_groups %}
  Group = Archive
    DataSetID   = {% if exists("DATA_SET_ID") %}
                  {{ DATA_SET_ID.Value }}
                  {% else %}
                  NULL
                  {% endif %}
    DataSetName = {% if exists("DATA_SET_NAME") %}
                  "{{ DATA_SET_NAME.Value }}"
                  {% else %}
                  NULL
                  {% endif %}
    ProductID   = {% if exists("PRODUCT_ID") %}
                  {{ PRODUCT_ID.Value }}
                  {% else %}
                  NULL
                  {% endif %}
  End_Group

  Group = MerImageRequestParms
    PixelAveragingHeight =  {% if exists("IMAGE_REQUEST_PARMS.PIXEL_AVERAGING_HEIGHT") %}
                            {{ IMAGE_REQUEST_PARMS.PIXEL_AVERAGING_HEIGHT.Value }}
                            {% else %}
                            NULL
                            {% endif %}
    PixelAveragingWidth  =  {% if exists("IMAGE_REQUEST_PARMS.PIXEL_AVERAGING_WIDTH") %}
                            {{ IMAGE_REQUEST_PARMS.PIXEL_AVERAGING_WIDTH.Value }}
                            {% else %}
                            NULL
                            {% endif %}
  End_Group

  Group = MerSubframeRequestParms
    FirstLine        = {% if exists("SUBFRAME_REQUEST_PARMS.FIRST_LINE") %}
                       {{ SUBFRAME_REQUEST_PARMS.FIRST_LINE.Value }}
                       {% else %}
                       NULL
                       {% endif %}
    FirstLineSamples = {% if exists("SUBFRAME_REQUEST_PARMS.FIRST_LINE_SAMPLE") %}
                       {{ SUBFRAME_REQUEST_PARMS.FIRST_LINE_SAMPLE.Value }}
                       {% else %}
                       NULL
                       {% endif %}
  End_Group
{% endblock %}
