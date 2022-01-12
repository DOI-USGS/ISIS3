{% extends "img_base.tpl" %}


{% block instrument %}
    SpacecraftName             = {{INSTRUMENT_HOST_NAME.Value}}
    InstrumentId               = {{INSTRUMENT_ID.Value}}
    SpacecraftClockStartCount  = {{SPACECRAFT_CLOCK_START_COUNT.Value}}
    SpacecraftClockStopCount   = {{SPACECRAFT_CLOCK_STOP_COUNT.Value}}
    StartTime                  = {{START_TIME.Value}}
    StopTime                   = {{STOP_TIME.Value}}
    ExposureDuration           = {{SR_ACQUIRE_OPTIONS.EXPOSURE_DURATION.Value}}
    {% set pix_width_arr = SR_COMPRESSION.PIXEL_AVERAGING_WIDTH.Value%}
    PixelAveragingWidth        = ({% for pix in pix_width_arr %}
                                    {% if loop.is_last %}
                                    {{ pix }}
                                    {% else %}
                                    {{ pix }},
                                    {% endif %}
                                   {% endfor %})  

    {% set pix_height_arr = SR_COMPRESSION.PIXEL_AVERAGING_HEIGHT.Value%}
    PixelAveragingHeight       = ({% for pix in pix_height_arr %}
                                    {% if loop.is_last %}
                                    {{ pix }}
                                    {% else %}
                                    {{ pix }},
                                    {% endif %}
                                   {% endfor %})
    {% set tar_name = SR_COMPRESSION.PIXEL_AVERAGING_WIDTH.Value%}
    TargetName                 = ({% for name in tar_name %}
                                    {% if loop.is_last %}
                                    {{ name }}
                                    {% else %}
                                    {{ name }},
                                    {% endif %}
                                   {% endfor %})
    
    OriginalTargetName         = {{TARGET_NAME.Value}}
    FirstLine                  = {{IMAGE.FIRST_LINE.Value}}
    FirstLineSample            = {{IMAGE.FIRST_LINE_SAMPLE.Value}}
    ProcessingLevelID          = {{PROCESSING_LEVEL_ID.Value}}
    ProcessingLevelDescription = {{"PROCESSING_LEVEL_DESC.Value"}}
{% endblock %}

{% block archive %}
    FileName                = {{FILE_NAME.Value}}
    SoftwareName            = {{SOFTWARE_NAME.Value}}
    SoftwareVersionId       = {{SOFTWARE_VERSION_ID.Value}}
    DataSetName             = {{"DATA_SET_NAME.Value"}}
    DataSetId               = {{DATA_SET_ID.Value}}
    ProductId               = {{PRODUCT_ID.Value}}
    ProductType             = {{PRODUCT_TYPE.Value}}
    ProducerFullName        = {{"PRODUCER_FULL_NAME.Value"}}
    ProducerInstitutionName = {{"PRODUCER_INSTITUTION_NAME.Value"}}
    ProductCreationTime     = {{PRODUCT_CREATION_TIME.Value}}
    ProductVersionId        = {{PRODUCT_VERSION_ID.Value}}
{% endblock %}

{% block additional_groups %}
  Group = BandBin
    {% set filterNumber=SR_MECHANISM_STATUS.FILTER_NUMBER.Value %}
    {% if filterNumber == "22" %}
      {% set combinedFilterName = "FFP-Vis_Orange" %}
      {% set filterId = "22" %}
      {% set filterOneName = "FFP-Vis_Orange" %}
      {% set filterOneCenter = "600.0 <nanometers>" %}
      {% set filterOneWidth = "600.0 <nanometers>" %}
      {% set filterTwoName = "Orange" %}
      {% set filterTwoCenter = "649.2 <nanometers>" %}
      {% set filterTwoWidth = "84.5 <nanometers>" %}
    {% endif %}
    FilterNumber       = {{filterNumber}}
    CombinedFilterName = {{combinedFilterName}}
    FilterId           = {{filterId}}
    FilterOneName      = {{filterOneName}}
    FilterOneCenter    = {{filterOneCenter}}
    FilterOneWidth     = {{filterOneWidth}}
    FilterTwoName      = {{filterTwoName}}
    FilterTwoCenter    = {{filterTwoCenter}}
    FilterTwoWidth     = {{filterTwoWidth}}
  End_Group

  Group = Kernels
    NaifFrameCode = (-226111)
  End_Group
{% endblock %}
