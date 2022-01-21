{% extends "img_base.tpl" %}

  {% block instrument %}
    MissionName                  = {{MISSION_NAME.Value}}
    SpacecraftName               = KAGUYA
    InstrumentName               = "{{INSTRUMENT_NAME.Value}}"
    InstrumentId                 = {{INSTRUMENT_ID.Value}}
    TargetName                   = {{TARGET_NAME.Value}}
    ObservationModeId            = {{OBSERVATION_MODE_ID.Value}}
    SensorDescription            = "{{SENSOR_DESCRIPTION.Value}}"
    SensorDescription2           = "{{SENSOR_DESCRIPTION2.Value}}"
  {% endblock %}

{% block additional_groups %}
  Group = Archive
    ProductId               = {{PRODUCT_ID.Value}}
    SoftwareName            = {{SOFTWARE_NAME.Value}}
    SoftwareVersion         = {{SOFTWARE_VERSION.Value}}
    ProcessVersionId        = {{PROCESS_VERSION_ID.Value}}
    ProductCreationTime     = {{PRODUCT_CREATION_TIME.Value}}
    ProgramStartTime        = {{PROGRAM_START_TIME.Value}}
    ProducerId              = {{PRODUCER_ID.Value}}
    ProductSetId            = {{PRODUCT_SET_ID.Value}}
    ProductVersionId        = {{PRODUCT_VERSION_ID.Value}}
    RegisteredProduct       = {{REGISTERED_PRODUCT.Value}}
    Level2AFileName         = ({{join(LEVEL2A_FILE_NAME.Value, ", ")}})
    SpiceMetakernelFileName = ({{join(SPICE_METAKERNEL_FILE_NAME.Value, ", ")}})
    DataSetId               = {{DATA_SET_ID.Value}}
    ImageValueType          = {{IMAGE.IMAGE_VALUE_TYPE.Value}}
    ImageUnit               = {{IMAGE.UNIT.Value}}
    MinForStatisticalEvaluation = {{IMAGE.MIN_FOR_STATISTICAL_EVALUATION.Value}}
    MaxForStatisticalEvaluation = {{IMAGE.MAX_FOR_STATISTICAL_EVALUATION.Value}}  
    SceneMaximumDn          = {{IMAGE.SCENE_MAXIMUM_DN.Value}}
    SceneMinimumDn          = {{IMAGE.SCENE_MINIMUM_DN.Value}}
    SceneAverageDn          = {{IMAGE.SCENE_AVERAGE_DN.Value}}
    SceneStdevDn            = {{IMAGE.SCENE_STDEV_DN.Value}}
    SceneModeDn             = {{IMAGE.SCENE_MODE_DN.Value}}
    ShadowedAreaMinimum     = {{IMAGE.SHADOWED_AREA_MINIMUM.Value}}
    ShadowedAreaMaximum     = {{IMAGE.SHADOWED_AREA_MAXIMUM.Value}}
    ShadowedAreaPercentage  = {{IMAGE.SHADOWED_AREA_PERCENTAGE.Value}}
    InvalidType             = ({{join(IMAGE.INVALID_TYPE.Value, ", ")}})
    InvalidValue            = ({{join(IMAGE.INVALID_VALUE.Value, ", ")}})
    InvalidPixels           = ({{join(IMAGE.INVALID_PIXELS.Value, ", ")}})
    OutOfImageBoundsValue   = {{IMAGE.OUT_OF_IMAGE_BOUNDS_VALUE.Value}}
    OutOfImageBoundsPixel   = {{IMAGE.OUT_OF_IMAGE_BOUNDS_PIXELS.Value}}
    StretchedFlag           = {{IMAGE.STRETCHED_FLAG.Value}}
    DarkFileName            = ({{ join(PROCESSING_PARAMETERS.DARK_FILE_NAME.Value, ", ") }})
    FlatFileName            = ({{ join(PROCESSING_PARAMETERS.FLAT_FILE_NAME.Value, ", ") }})
    EfficFileName           = ({{ join(PROCESSING_PARAMETERS.EFFIC_FILE_NAME.Value, ", ") }})
    NonlinFileName          = ({{ join(PROCESSING_PARAMETERS.NONLIN_FILE_NAME.Value, ", ") }})
    RadCnvCoef              = ({{ join(PROCESSING_PARAMETERS.RAD_CNV_COEF.Value, ", ") }})
    RefCnvCoef              = {{PROCESSING_PARAMETERS.REF_CNV_COEF.Value}}
    StandardGeometry        = ({{ join(PROCESSING_PARAMETERS.STANDARD_GEOMETRY.Value, ", ")}})
    PhotoCorrId             = {{PROCESSING_PARAMETERS.PHOTO_CORR_ID.Value}}
    PhotoCorrCoef           = ({{ join(PROCESSING_PARAMETERS.PHOTO_CORR_COEF.Value, ", ")}})
    ResamplingMethod        = {{PROCESSING_PARAMETERS.RESAMPLING_METHOD.Value}}
    TcoMosaicFileName       = ({{ join(PROCESSING_PARAMETERS.TCO_MOSAIC_FILE_NAME.Value, ", ")}})
    OverlapSelectionId      = "{{PROCESSING_PARAMETERS.OVERLAP_SELECTION_ID.Value}}"
    MatchingMosaic          = {{PROCESSING_PARAMETERS.MATCHING_MOSAIC.Value}}
    L2aSaturationThreshold  = {{PROCESSING_PARAMETERS.L2A_SATURATION_THRESHOLD.Value}}
    DarkValidMinimum        = {{PROCESSING_PARAMETERS.DARK_VALID_MINIMUM.Value}}
    RadianceSaturationThreshold = {{PROCESSING_PARAMETERS.RADIANCE_SATURATION_THRESHOLD.Value}}
    RefSaturationThreshold  = {{PROCESSING_PARAMETERS.REF_SATURATION_THRESHOLD.Value}}
    RefCnvCoef              = {{PROCESSING_PARAMETERS.REF_CNV_COEF.Value}}
    StandardGeometry        = ({{ join(PROCESSING_PARAMETERS.STANDARD_GEOMETRY.Value, ", ")}})
    DtmMosaicFileName       = ({{ join(PROCESSING_PARAMETERS.DTM_MOSAIC_FILE_NAME.Value, ", ")}})
    L2aDeadPixelThreshold   = {{PROCESSING_PARAMETERS.L2A_SATURATION_THRESHOLD.Value}}
  End_Group

  Group = BandBin
    FilterName = BroadBand
    Center     = 640nm
    Width      = 420nm
  End_Group

{% endblock %}
