{% extends "qube_base.tpl" %}

  {% block instrument %}
    SpacecraftName               = {{ INSTRUMENT_HOST_NAME.Value }}
    MissionPhaseName             = "{{ MISSION_PHASE_NAME.Value }}"
    InstrumentId                 = {{ INSTRUMENT_ID.Value }}
    SpacecraftClockStartCount    = {{ SPACECRAFT_CLOCK_START_COUNT.Value }}
    SpacecraftClockStopCount     = {{ SPACECRAFT_CLOCK_STOP_COUNT.Value }}
    StartTime                    = {{ START_TIME.Value }}
    StopTime                     = {{ STOP_TIME.Value }}
    {% set targetname=TARGET_NAME.Value %}
    TargetName                   = {% if targetname == "4 VESTA" %}
                                   VESTA
                                   {% else if targetname == "1 CERES" %}
                                   CERES
                                   {% else %}
                                   {{ targetname }}
                                   {% endif %}
    OriginalTargetName           = "{{ targetname }}"
    OrbitNumber                  = {{ ORBIT_NUMBER.Value }}
    DataQualityId                = {{ DATA_QUALITY_ID.Value }}
    ChannelId                    = {{ CHANNEL_ID.Value }}
    InstrumentModeId             = {{ INSTRUMENT_MODE_ID.Value }}
    ScanModeId                   = {{ SCAN_MODE_ID.Value }}
    {% if exists("DAWN_SCAN_PARAMETER") %}
    ScanParameter                = {{ DAWN_SCAN_PARAMETER.Value }}
    {% endif %}
    {% set frameArray=FRAME_PARAMETER.Value %}
    FrameParameter               = ({% for frame in frameArray %}
                                    {% if loop.is_last %}
                                    {{ frame }}
                                    {% else %}
                                    {{ frame }},
                                    {% endif %}
                                   {% endfor %})
    {% if exists("DAWN_VIR_IR_START_X_POSITION") %}
    VirIrStartXPosition         = {{ DAWN_VIR_IR_START_X_POSITION.Value }}
    {% endif %}
    {% if exists("DAWN_VIR_IR_START_Y_POSITION") %}
    VirIrStartYPosition         = {{ DAWN_VIR_IR_START_Y_POSITION.Value }}
    {% endif %}
    {% if exists("DAWN_VIR_VIS_START_X_POSITION") %}
    VirVisStartXPosition         = {{ DAWN_VIR_VIS_START_X_POSITION.Value }}
    {% endif %}
    {% if exists("DAWN_VIR_VIS_START_Y_POSITION") %}
    VirVisStartYPosition         = {{ DAWN_VIR_VIS_START_Y_POSITION.Value }}
    {% endif %}
    {% if exists("MAXIMUM_INSTRUMENT_TEMPERATURE") %}
    {% set maxtempArray=MAXIMUM_INSTRUMENT_TEMPERATURE.Value %}
    MaximumInstrumentTemperature = ({% for temp in maxtempArray %}
                                    {% if loop.is_last %}
                                    {{ temp }}
                                    {% else %}
                                    {{ temp }},
                                    {% endif %}
                                   {% endfor %})
    {% endif %}
  {% endblock %}

  {% block additional_groups %}
    Group = Archive 
    
      SoftwareVersionId         = "{{ SOFTWARE_VERSION_ID.Value }}"
      DataSetName               = "{{ DATA_SET_NAME.Value }}"
      DataSetId                 = {{ DATA_SET_ID.Value }}
      ProductId                 = {{ PRODUCT_ID.Value }}
      ProductType               = {{ PRODUCT_TYPE.Value }}
      ProducerFullName          = "{{ PRODUCER_FULL_NAME.Value }}"
      ProducerInstitutionName   = "{{ PRODUCER_INSTITUTION_NAME.Value }}"
      ProductCreationTime       = {{ PRODUCT_CREATION_TIME.Value }}
      ProductVersionId          = {{ PRODUCT_VERSION_ID.Value }}
      InstrumentName            = "{{ INSTRUMENT_NAME.Value }}"
      InstrumentType            = "{{ INSTRUMENT_TYPE.Value }}"
      ImageMidTime              = {{ IMAGE_MID_TIME.Value }}
      ProcessingLevelId         = {{ PROCESSING_LEVEL_ID.Value }}
      EncodingType              = {{ ENCODING_TYPE.Value }}
      {% if exists("PHOTOMETRIC_CORRECTION_TYPE") %}
      PhotometricCorrectionType = {{ PHOTOMETRIC_CORRECTION_TYPE.Value }}
      {% endif %}

    End_Group
  {% endblock %}

  {% block bandbin %}
    Unit         = {{ QUBE.BAND_BIN.BAND_BIN_UNIT.Value }}
    {% set centerArray=QUBE.BAND_BIN.BAND_BIN_CENTER.Value %}
    Center       = ({% for center in centerArray %}
                    {% if loop.is_last %}
                    {{ center }}
                    {% else %}
                    {{ center }},
                    {% endif %}
                   {% endfor %})
    {% set widthArray=QUBE.BAND_BIN.BAND_BIN_WIDTH.Value %}
    Width        = ({% for width in widthArray %}
                    {% if loop.is_last %}
                    {{ width }}
                    {% else %}
                    {{ width }},
                    {% endif %}
                    {% endfor %})
    {% set origArray=QUBE.BAND_BIN.BAND_BIN_ORIGINAL_BAND.Value %}
    OriginalBand = ({% for band in origArray %}
                    {% if loop.is_last %}
                    {{ band }}
                    {% else %}
                    {{ band }},
                    {% endif %}
                    {% endfor %})
  {% endblock %}

  {% block kernels %}
  {% set channelId=CHANNEL_ID.Value %}
    NaifFrameCode = {% if channelId == "VIS" %}-203211
                    {% else if channelId == "IR" %}-203213
                    {% endif %}
  {% endblock %}

  {% block translation %}
  {% endblock %}
