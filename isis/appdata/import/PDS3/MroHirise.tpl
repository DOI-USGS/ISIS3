{% extends "img_base.tpl" %}
  {% block instrument %}
    SpacecraftName                  = "{{INSTRUMENT_HOST_NAME.Value}}"
    InstrumentId                    = {{INSTRUMENT_ID.Value}}
    TargetName                      = {{TARGET_NAME.Value}}
    StartTime                       = {{TIME_PARAMETERS.START_TIME.Value}}
    StopTime                        = {{TIME_PARAMETERS.STOP_TIME.Value}}
    ObservationStartCount           = {{TIME_PARAMETERS.MRO_OBSERVATION_START_COUNT.Value}}
    SpacecraftClockStopCount        = {{TIME_PARAMETERS.SPACECRAFT_CLOCK_STOP_COUNT.Value}}
    ReadoutStartCount               = {{TIME_PARAMETERS.MRO_READOUT_START_COUNT.Value}}
    CalibrationStartTime            = {{TIME_PARAMETERS.MRO_CALIBRATION_START_TIME.Value}}
    CalibrationStartCount           = {{TIME_PARAMETERS.MRO_CALIBRATION_START_COUNT.Value}}
    MissionPhaseName                = "{{MISSION_PHASE_NAME.Value}}"
    LineExposureDuration            = {{INSTRUMENT_SETTING_PARAMETERS.MRO_LINE_EXPOSURE_DURATION.Value}}
    ScanExposureDuration            = {{INSTRUMENT_SETTING_PARAMETERS.MRO_SCAN_EXPOSURE_DURATION.Value}}
    DeltaLineTimerCount             = {{INSTRUMENT_SETTING_PARAMETERS.MRO_DELTA_LINE_TIMER_COUNT.Value}}
    Summing                         = {{INSTRUMENT_SETTING_PARAMETERS.MRO_BINNING.Value}}
    Tdi                             = {{INSTRUMENT_SETTING_PARAMETERS.MRO_TDI.Value}}
    FocusPositionCount              = {{INSTRUMENT_SETTING_PARAMETERS.MRO_FOCUS_POSITION_COUNT.Value}}
    PoweredCpmmFlag                 = ({{ join(INSTRUMENT_SETTING_PARAMETERS.MRO_POWERED_CPMM_FLAG.Value, ", ") }})
    CpmmNumber                      = {{INSTRUMENT_SETTING_PARAMETERS.MRO_CPMM_NUMBER.Value}}
    CcdId                           = {{INSTRUMENT_SETTING_PARAMETERS.MRO_CPMM_NUMBER.Value}}
    ChannelNumber                   = {{INSTRUMENT_SETTING_PARAMETERS.MRO_CHANNEL_NUMBER.Value}}
    LookupTableType                 = ({{ join(INSTRUMENT_SETTING_PARAMETERS.MRO_LOOKUP_CONVERSION_TABLE.Value, ", ") }})
    LookupTableNumber               = {{INSTRUMENT_SETTING_PARAMETERS.MRO_LOOKUP_TABLE_NUMBER.Value}}
    LookupTableMinimum              = {{INSTRUMENT_SETTING_PARAMETERS.MRO_LOOKUP_TABLE_MINIMUM.Value}}
    LookupTableMaximum              = {{INSTRUMENT_SETTING_PARAMETERS.MRO_LOOKUP_TABLE_MAXIMUM.Value}}
    LookupTableMedian               = {{INSTRUMENT_SETTING_PARAMETERS.MRO_LOOKUP_TABLE_MEDIAN.Value}}
    LookupTableKValue               = {{INSTRUMENT_SETTING_PARAMETERS.MRO_LOOKUP_TABLE_K_VALUE.Value}}
    StimulationLampFlag             = ({{ join(INSTRUMENT_SETTING_PARAMETERS.MRO_STIMULATION_LAMP_FLAG.Value, ", ")}})
    HeaterControlFlag               = ({{ join(INSTRUMENT_SETTING_PARAMETERS.MRO_HEATER_CONTROL_FLAG.Value, ", ")}})
    OptBnchFlexureTemperature       = {{TEMPERATURE_PARAMETERS.MRO_OPT_BNCH_FLEXURE_TEMPERATURE.Value}}
    OptBnchMirrorTemperature        = {{TEMPERATURE_PARAMETERS.MRO_OPT_BNCH_MIRROR_TEMPERATURE.Value}}
    OptBnchFoldFlatTemperature      = {{TEMPERATURE_PARAMETERS.MRO_OPT_BNCH_FOLD_FLAT_TEMPERATURE.Value}}
    OptBnchFpaTemperature           = {{TEMPERATURE_PARAMETERS.MRO_OPT_BNCH_FPA_TEMPERATURE.Value}}
    OptBnchFpeTemperature           = {{TEMPERATURE_PARAMETERS.MRO_OPT_BNCH_FPE_TEMPERATURE.Value}}
    OptBnchLivingRmTemperature      = {{TEMPERATURE_PARAMETERS.MRO_OPT_BNCH_LIVING_RM_TEMPERATURE.Value}}
    OptBnchBoxBeamTemperature       = {{TEMPERATURE_PARAMETERS.MRO_OPT_BNCH_BOX_BEAM_TEMPERATURE.Value}}
    OptBnchCoverTemperature         = {{TEMPERATURE_PARAMETERS.MRO_OPT_BNCH_COVER_TEMPERATURE.Value}}
    FieldStopTemperature            = {{TEMPERATURE_PARAMETERS.MRO_FIELD_STOP_TEMPERATURE.Value}}
    FpaPositiveYTemperature         = {{TEMPERATURE_PARAMETERS.MRO_FPA_POSITIVE_Y_TEMPERATURE.Value}}
    FpaNegativeYTemperature         = {{TEMPERATURE_PARAMETERS.MRO_FPA_NEGATIVE_Y_TEMPERATURE.Value}}
    FpeTemperature                  = {{TEMPERATURE_PARAMETERS.MRO_FPE_TEMPERATURE.Value}}
    PrimaryMirrorMntTemperature     = {{TEMPERATURE_PARAMETERS.MRO_PRIMARY_MIRROR_MNT_TEMPERATURE.Value}}
    PrimaryMirrorTemperature        = {{TEMPERATURE_PARAMETERS.MRO_PRIMARY_MIRROR_MNT_TEMPERATURE.Value}}
    PrimaryMirrorBafTemperature     = {{TEMPERATURE_PARAMETERS.MRO_PRIMARY_MIRROR_BAF_TEMPERATURE.Value}}
    MsTrussLeg0ATemperature         = {{TEMPERATURE_PARAMETERS.MRO_MS_TRUSS_LEG_0_A_TEMPERATURE.Value}}
    MsTrussLeg0BTemperature         = {{TEMPERATURE_PARAMETERS.MRO_MS_TRUSS_LEG_0_B_TEMPERATURE.Value}}
    MsTrussLeg120ATemperature       = {{TEMPERATURE_PARAMETERS.MRO_MS_TRUSS_LEG_120_A_TEMPERATURE.Value}}
    MsTrussLeg120BTemperature       = {{TEMPERATURE_PARAMETERS.MRO_MS_TRUSS_LEG_120_B_TEMPERATURE.Value}}
    MsTrussLeg240ATemperature       = {{TEMPERATURE_PARAMETERS.MRO_MS_TRUSS_LEG_240_A_TEMPERATURE.Value}}
    MsTrussLeg240BTemperature       = {{TEMPERATURE_PARAMETERS.MRO_MS_TRUSS_LEG_240_B_TEMPERATURE.Value}}
    BarrelBaffleTemperature         = {{TEMPERATURE_PARAMETERS.MRO_BARREL_BAFFLE_TEMPERATURE.Value}}
    SunShadeTemperature             = {{TEMPERATURE_PARAMETERS.MRO_SUN_SHADE_TEMPERATURE.Value}}
    SpiderLeg30Temperature          = {{TEMPERATURE_PARAMETERS.MRO_SPIDER_LEG_30_TEMPERATURE.Value}}
    SpiderLeg150Temperature         = {{TEMPERATURE_PARAMETERS.MRO_SPIDER_LEG_150_TEMPERATURE.Value}}
    SpiderLeg270Temperature         = {{TEMPERATURE_PARAMETERS.MRO_SPIDER_LEG_270_TEMPERATURE.Value}}
    SecMirrorMtrRngTemperature      = {{TEMPERATURE_PARAMETERS.MRO_SEC_MIRROR_MTR_RNG_TEMPERATURE.Value}}
    SecMirrorTemperature            = {{TEMPERATURE_PARAMETERS.MRO_SEC_MIRROR_TEMPERATURE.Value}}
    SecMirrorBaffleTemperature      = {{TEMPERATURE_PARAMETERS.MRO_SEC_MIRROR_BAFFLE_TEMPERATURE.Value}}
    IeaTemperature                  = {{TEMPERATURE_PARAMETERS.MRO_IEA_TEMPERATURE.Value}}
    FocusMotorTemperature           = {{TEMPERATURE_PARAMETERS.MRO_FOCUS_MOTOR_TEMPERATURE.Value}}
    IePwsBoardTemperature           = {{TEMPERATURE_PARAMETERS.MRO_IE_PWS_BOARD_TEMPERATURE.Value}}
    CpmmPwsBoardTemperature         = {{TEMPERATURE_PARAMETERS.MRO_CPMM_PWS_BOARD_TEMPERATURE.Value}}
    MechTlmBoardTemperature         = {{TEMPERATURE_PARAMETERS.MRO_MECH_TLM_BOARD_TEMPERATURE.Value}}
    InstContBoardTemperature        = {{TEMPERATURE_PARAMETERS.MRO_INST_CONT_BOARD_TEMPERATURE.Value}}
    DllLockedFlag                   = ({{ join(INSTRUMENT_SETTING_PARAMETERS.MRO_DLL_LOCKED_FLAG.Value, ", ")}})
    DllResetCount                   = {{INSTRUMENT_SETTING_PARAMETERS.MRO_DLL_RESET_COUNT.Value}}
    DllLockedOnceFlag               = ({{ join(INSTRUMENT_SETTING_PARAMETERS.MRO_DLL_LOCKED_ONCE_FLAG.Value, ", ")}})
    DllFrequenceCorrectCount        = {{INSTRUMENT_SETTING_PARAMETERS.MRO_DLL_FREQUENCY_CORRECT_COUNT.Value}}
    ADCTimingSetting                = ({{ join(INSTRUMENT_SETTING_PARAMETERS.MRO_ADC_TIMING_SETTINGS.Value, ", ")}})
  {% endblock %}

{% block additional_groups %}
  Group = Archive
    DataSetId                       = {{DATA_SET_ID.Value}}
    ProduccerId                     = {{PRODUCER_ID.Value}}
    ObservationId                   = {{OBSERVATION_ID.Value}}
    ProductId                       = {{PRODUCER_ID.Value}}
    ProductVersionId                = {{PRODUCT_VERSION_ID.Value}}
    EdrProductCreationTime          = {{TIME_PARAMETERS.PRODUCT_CREATION_TIME.Value}}
    RationaleDescription            = "{{RATIONALE_DESC.Value}}"
    OrbitNumber                     = {{ORBIT_NUMBER.Value}}
    SoftwareName                    = "{{SOFTWARE_NAME.Value}}"
    ObservationStartTime            = {{TIME_PARAMETERS.MRO_OBSERVATION_START_TIME.Value}}
    ReadoutStartTime                = {{TIME_PARAMETERS.MRO_READOUT_START_TIME.Value}}
    TrimLines                       = {{INSTRUMENT_SETTING_PARAMETERS.MRO_TRIM_LINES.Value}}
    FelicsCompressionFlag           = {{INSTRUMENT_SETTING_PARAMETERS.MRO_FELICS_COMPRESSION_FLAG.Value}}
    IdFlightSoftwareName            = {{FLIGHT_SOFTWARE_VERSION_ID.Value}}
  End_Group

  Group = BandBin
    Name                      = {{INSTRUMENT_SETTING_PARAMETERS.FILTER_NAME.Value}}
    Center                    = {{INSTRUMENT_SETTING_PARAMETERS.CENTER_FILTER_WAVELENGTH.Value}}
    Width                     = {{INSTRUMENT_SETTING_PARAMETERS.BANDWIDTH.Value}}
  End_Group

  Group = Kernels
    NaifIkCode                = "-74699"
  End_Group

{% endblock %}
