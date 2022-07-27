{% extends "img_base.tpl" %}

{% block instrument %}
    SpacecraftName               = Messenger
    InstrumentName               = "{{INSTRUMENT_NAME.Value}}"
    InstrumentId                 = {{INSTRUMENT_ID.Value}}
    TargetName                   = "{{TARGET_NAME.Value}}"
    OriginalTargetName           = "{{TARGET_NAME.Value}}"
    StartTime                    = {{START_TIME.Value}}
    StopTime                     = {{STOP_TIME.Value}}
    SpacecraftClockCount         = {{SPACECRAFT_CLOCK_START_COUNT.Value}}
    MissionPhaseName             = "{{MISSION_PHASE_NAME.Value}}"
    ExposureDuration             = "{{MESS_EXPOSURE.Value}} <MS>"
    ExposureType                 = {{EXPOSURE_TYPE.Value}}
    DetectorTemperature          = {{DETECTOR_TEMPERATURE.Value}} <DEGC>
    FocalPlaneTemperature        = {{FOCAL_PLANE_TEMPERATURE.Value}} <DEGC>
    FilterTemperature            = {{FILTER_TEMPERATURE.Value}}
    OpticsTemperature            = {{OPTICS_TEMPERATURE.Value}} <DEGC>
    {% set ok_levels = ["7","6","5"] %}
    {% set bad_levels = ["3","2","1","0"] %}
    AttitudeQuality              = {% if MESS_ATT_FLAG.Value in ok_levels %}
                                    "Ok" 
                                    {% else if MESS_ATT_FLAG.Value == 4 %}
                                    "Illegal"
                                    {% else if MESS_ATT_FLAG.Value in bad_levels %}
                                    "Bad"
                                    {% else %}
                                    "UNKNOWN"
                                    {% endif %}
    FilterWheelPosition          = {{MESS_FW_POS.Value}}
    PivotPosition                = {{MESS_PIV_POS.Value}}
    FpuBinningMode               = {{MESS_FPU_BIN.Value}}
    PixelBinningMode             = {{MESS_PIXELBIN.Value}}
    SubFrameMode                 = {{MESS_SUBFRAME.Value}}
    JailBars                     = {{MESS_JAILBARS.Value}}
    DpuId                        = {{MESS_DPU_ID.Value}}
{% endblock %}


{% block additional_groups %}
  Group = Archive
    DataSetId                 = {{DATA_SET_ID.Value}}
    DataQualityID             = {{DATA_QUALITY_ID.Value}}
    ProducerId                = "{{PRODUCER_INSTITUTION_NAME.Value}}"
    EdrSourceProductId        = {{SOURCE_PRODUCT_ID.Value}}
    ProductId                 = {{PRODUCT_ID.Value}}
    SequenceName              = {{SEQUENCE_NAME.Value}}
    ObservationId             = {% if exists("OBSERVATION_ID") %} 
                                {{OBSERVATION_ID.value}}
                                {% else %}
                                "UNKNOWN"
                                {% endif %}
    ObservationType           = {% if exists("OBSERVATION_TYPE") %} 
                                {{OBSERVATION_TYPE.value}}
                                {% else %}
                                "UNKNOWN"
                                {% endif %}
    SiteId                    = {% if exists("SITE_ID") %} 
                                {{SITE_ID.value}}
                                {% else %}
                                "UNKNOWN"
                                {% endif %}
    MissionElapsedTime        = {{MESS_MET_EXP.Value}}
    EdrProductCreationTime    = {{PRODUCT_CREATION_TIME.Value}}
    ObservationStartTime      = {{START_TIME.Value}}
    SpacecraftClockStartCount = {{SPACECRAFT_CLOCK_START_COUNT.Value}}
    SpacecraftClockStopCount  = {{SPACECRAFT_CLOCK_STOP_COUNT.Value}}
    Exposure                  = {{MESS_EXPOSURE.Value}}
    CCDTemperature            = {{MESS_CCD_TEMP.Value}}
    OriginalFilterNumber      = {{FILTER_NUMBER.Value}}
    OrbitNumber               = {% if exists("ORBIT_NUMBER") %} 
                                {{ORBIT_NUMBER.value}}
                                {% else %}
                                "UNKNOWN"
                                {% endif %}
    {%set startTime = START_TIME.Value%}
    YearDoy                   = {{ YearDoy(startTime) }}
    SourceProductId           = {{SOURCE_PRODUCT_ID.Value}}
  End_Group

  Group = BandBin
    Name   = "748 BP 53"
    Number = 2
    Center = 747.7 <NM>
    Width  = 52.6 <NM>
  End_Group

{% endblock %}