{% extends "img_base.tpl" %}

  {% block instrument %}
    MissionName                  = {{ MISSION_NAME.Value }}
    SpacecraftName               = KAGUYA
    InstrumentName               = "{{ INSTRUMENT_NAME.Value }}"
    InstrumentId                 = {{ INSTRUMENT_ID.Value }}
    TargetName                   = {{ TARGET_NAME.Value }}
    ObservationModeId            = {{ OBSERVATION_MODE_ID.Value }}
    SensorDescription            = "{{ SENSOR_DESCRIPTION.Value }}"
    SensorDescription2           = "{{ SENSOR_DESCRIPTION2.Value }}"
    MissionPhaseName             = {{ MISSION_PHASE_NAME.Value }}
    RevolutionNumber             = {{ REVOLUTION_NUMBER.Value }}
    StripSequenceNumber          = {{ STRIP_SEQUENCE_NUMBER.Value }}
    SceneSequenceNumber          = {{ SCENE_SEQUENCE_NUMBER.Value }}
    UpperLeftDaytimeFlag         = {{ UPPER_LEFT_DAYTIME_FLAG.Value }}
    UpperRightDaytimeFlag        = {{ UPPER_RIGHT_DAYTIME_FLAG.Value }}
    LowerLeftDaytimeFlag         = {{ LOWER_LEFT_DAYTIME_FLAG.Value }}
    LowerRightDaytimeFlag        = {{ LOWER_RIGHT_DAYTIME_FLAG.Value }}
    {% set detstatus=DETECTOR_STATUS.Value %}
    DetectorStatus               = ({% for det in detstatus %}
                                      {% if loop.is_last %}
                                        {{ det }}
                                      {% else %}
                                        {{ det }},
                                      {% endif %}
                                   {% endfor %})
    ExposureModeId               = {{ EXPOSURE_MODE_ID.Value }}
    LineExposureDuration         = {{ LINE_EXPOSURE_DURATION.Value }} <msec>
    SpacecraftClockStartCount    = {{ SPACECRAFT_CLOCK_START_COUNT.Value }} <sec>
    SpacecraftClockStopCount     = {{ SPACECRAFT_CLOCK_STOP_COUNT.Value }} <sec>
    CorrectedScClockStartCount   = {{ CORRECTED_SC_CLOCK_START_COUNT.Value }} <sec>
    CorrectedScClockStopCount    = {{ CORRECTED_SC_CLOCK_STOP_COUNT.Value }} <sec>
    StartTimeRaw                 = {{ START_TIME.Value }}
    StopTimeRaw                  = {{ STOP_TIME.Value }}
    StartTime                    = {{ CORRECTED_START_TIME.Value }}
    StopTime                     = {{ CORRECTED_STOP_TIME.Value }}
    LineSamplingInterval         = {{ LINE_SAMPLING_INTERVAL.Value }} <msec>
    CorrectedSamplingInterval    = {{ CORRECTED_SAMPLING_INTERVAL.Value }} <msec>
    UpperLeftLatitude            = {{ UPPER_LEFT_LATITUDE.Value }} <deg>
    UpperLeftLongitude           = {{ UPPER_LEFT_LONGITUDE.Value }} <deg>
    UpperRightLatitude           = {{ UPPER_RIGHT_LATITUDE.Value }} <deg>
    UpperRightLongitude          = {{ UPPER_RIGHT_LONGITUDE.Value }} <deg>
    LowerLeftLatitude            = {{ LOWER_LEFT_LATITUDE.Value }} <deg>
    LowerLeftLongitude           = {{ LOWER_LEFT_LONGITUDE.Value }} <deg>
    LowerRightLatitude           = {{ LOWER_RIGHT_LATITUDE.Value }} <deg>
    LowerRightLongitude          = {{ LOWER_RIGHT_LONGITUDE.Value }} <deg>
    LocationFlag                 = {{ LOCATION_FLAG.Value }}
    RollCant                     = {{ ROLL_CANT.Value }}
    SceneCenterLatitude          = {{ SCENE_CENTER_LATITUDE.Value }} <deg>
    SceneCenterLongitude         = {{ SCENE_CENTER_LONGITUDE.Value }} <deg>
    IncidenceAngle               = {{ INCIDENCE_ANGLE.Value }} <deg>
    EmissionAngle                = {{ EMISSION_ANGLE.Value }} <deg>
    PhaseAngle                   = {{ PHASE_ANGLE.Value }} <deg>
    SolarAzimuthAngle            = {{ SOLAR_AZIMUTH_ANGLE.Value }} <deg>
    FocalPlaneTemperature        = {{ FOCAL_PLANE_TEMPERATURE.Value }} <degC>
    TelescopeTemperature         = {{ TELESCOPE_TEMPERATURE.Value }} <degC>
    SatelliteMovingDirection     = {{ SATELLITE_MOVING_DIRECTION.Value }}
    FirstSampledLinePosition     = {{ FIRST_SAMPLED_LINE_POSITION.Value }}
    FirstDetectorElementPosition = {{ FIRST_DETECTOR_ELEMENT_POSITION.Value }}
    AAxisRadius                  = {{ A_AXIS_RADIUS.Value }} <km>
    BAxisRadius                  = {{ B_AXIS_RADIUS.Value }} <km>
    CAxisRadius                  = {{ C_AXIS_RADIUS.Value }} <km>
    {% set pixpos=DEFECT_PIXEL_POSITION.Value %}
    DefectPixelPosition          = ({% for val in pixpos %}
                                      {% if loop.is_last %}
                                        {{ val }}
                                      {% else %}
                                        {{ val }},
                                      {% endif %}
                                   {% endfor %})
    SpacecraftAltitude           = {{ SPACECRAFT_ALTITUDE.Value }} <km>
    SpacecraftGroundSpeed        = {{ SPACECRAFT_GROUND_SPEED.Value }} <km/sec>
  {% endblock %}

{% block additional_groups %}
  Group = Archive
    ProductId               = {{ PRODUCT_ID.Value }}
    FileName                = {{ FILE_NAME.Value }}
    SoftwareName            = {{ SOFTWARE_NAME.Value }}
    SoftwareVersion         = {{ SOFTWARE_VERSION.Value }}
    ProcessVersionId        = {{ PROCESS_VERSION_ID.Value }}
    ProductCreationTime     = {{ PRODUCT_CREATION_TIME.Value }}
    ProgramStartTime        = {{ PROGRAM_START_TIME.Value }}
    ProducerId              = {{ PRODUCER_ID.Value }}
    ProductSetId            = {{ PRODUCT_SET_ID.Value }}
    ProductVersionId        = {{ PRODUCT_VERSION_ID.Value }}
    RegisteredProduct       = {{ REGISTERED_PRODUCT.Value }}
    Level2AFileName         = {{ LEVEL2A_FILE_NAME.Value }}
    SpiceMetakernelFileName = {{ SPICE_METAKERNEL_FILE_NAME.Value }}
    DataSetId               = {{ DATA_SET_ID.Value }}
  End_Group

  Group = BandBin
   {% set filtNames=FILTER_NAME.Value %}
    FilterName = ({% for filter in filtNames %}
                    {% if loop.is_last %}
                      {{ filter }}
                    {% else %}
                      {{ filter }},
                    {% endif %}
                 {% endfor %})
    {% set centerArray=CENTER_FILTER_WAVELENGTH.Value %}
    Center     = ({% for center in centerArray %}
                    {% if loop.is_last %}
                      {{ center }}
                    {% else %}
                      {{ center }},
                    {% endif %}
                 {% endfor %}) <nm>
    {% set widthArray=BANDWIDTH.Value %}
    Width      = ({% for width in widthArray %}
                    {% if loop.is_last %}
                      {{ width }}
                    {% else %}
                      {{ width }},
                    {% endif %}
                 {% endfor %}) <nm>
    BaseBand   = {{ BASE_BAND.Value }}
  End_Group

  Group = Kernels
  {% set baseBand=BASE_BAND.Value %}
  {% if CharAt(baseBand, 1) == "V" %}
    NaifFrameCode = -13133{{ CharAt(baseBand, 2) }}
    NaifCkCode    = -131330
  {% else if CharAt(baseBand, 1) == "N" %}
    NaifFrameCode = -13134{{ CharAt(baseBand, 2) }}
    NaifCkCode    = -131340
  {% endif %}
  End_Group
{% endblock %}
