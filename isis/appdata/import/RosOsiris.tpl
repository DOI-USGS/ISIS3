{% extends "img_base.tpl" %}


{% block instrument %}
    SpacecraftName             = {{INSTRUMENT_HOST_NAME.Value}}
    InstrumentId               = {{INSTRUMENT_ID.Value}}
    SpacecraftClockStartCount  = {{SPACECRAFT_CLOCK_START_COUNT.Value}}
    SpacecraftClockStopCount   = {{SPACECRAFT_CLOCK_STOP_COUNT.Value}}
    StartTime                  = {{START_TIME.Value}}
    StopTime                   = {{STOP_TIME.Value}}
    ExposureDuration           = {{SR_ACQUIRE_OPTIONS.EXPOSURE_DURATION.Value}} <s>
    {% set pix_width_arr = SR_COMPRESSION.PIXEL_AVERAGING_WIDTH.Value%}
    PixelAveragingWidth        = {% if isArray(pix_width_arr) %}
                               ({{ join(pix_width_arr, ", ") }})
                                {% else %}
                                {{SR_COMPRESSION.PIXEL_AVERAGING_WIDTH.Value}}
                                {% endif %}
                          

    {% set pix_height_arr = SR_COMPRESSION.PIXEL_AVERAGING_HEIGHT.Value%}
    PixelAveragingHeight       = {% if isArray(pix_height_arr) %} 
                                ({{ join(pix_height_arr, ", ") }})
                                {% else %}
                                {{SR_COMPRESSION.PIXEL_AVERAGING_HEIGHT.Value}}
                                {% endif %}

    {% set tar_name = TARGET_NAME.Value %}
    {% if tar_name == "67P/CHURYUMOV-GERASIMENKO 1 (1969 R1)"%}
    TargetName = "67P/CHURYUMOV-GERASIMENKO (1969 R1)"
    {% else %}
    TargetName                 = {{tar_name}}
    {% endif%}
    OriginalTargetName         = {{TARGET_NAME.Value}}
    FirstLine                  = {{IMAGE.FIRST_LINE.Value}}
    FirstLineSample            = {{IMAGE.FIRST_LINE_SAMPLE.Value}}
    ProcessingLevelID          = {{PROCESSING_LEVEL_ID.Value}}
    ProcessingLevelDescription = "{{PROCESSING_LEVEL_DESC.Value}}"
{% endblock %}

{% block archive %}
    FileName                = {{FILE_NAME.Value}}
    SoftwareName            = {{SOFTWARE_NAME.Value}}
    SoftwareVersionId       = {{SOFTWARE_VERSION_ID.Value}}
    DataSetName             = "{{DATA_SET_NAME.Value}}"
    DataSetId               = {{DATA_SET_ID.Value}}
    ProductId               = {{PRODUCT_ID.Value}}
    ProductType             = {{PRODUCT_TYPE.Value}}
    ProducerFullName        = "{{PRODUCER_FULL_NAME.Value}}"
    ProducerInstitutionName = "{{PRODUCER_INSTITUTION_NAME.Value}}"
    ProductCreationTime     = {{PRODUCT_CREATION_TIME.Value}}
    ProductVersionId        = {{PRODUCT_VERSION_ID.Value}}
{% endblock %}

{% block additional_groups %}
  Group = BandBin
    {% set filterName=splitOnChar(SR_MECHANISM_STATUS.FILTER_NAME.Value, "_") %}
    {% set filterOneName = filterName.0 %}
    {% set filterTwoName = filterName.1 %}

    {% if filterName.0 == "FFP-Vis" %}
      {% set filterOneCenter = 600.0 %}
      {% set filterOneWidth = 600.0 %}
    {% else if filterName.0 == "FFP-UV" %}
      {% set filterOneCenter = 600.0 %}
      {% set filterOneWidth = 600.0 %}
    {% else if filterName.0 == "FFP-IR" %}
      {% set filterOneCenter = 600.0 %}
      {% set filterOneWidth = 600.0 %}
    {% else if filterName.0 == "Netural" %}
      {% set filterOneCenter = 640.0 %}
      {% set filterOneWidth = 520.0 %}
    {% else if filterName.0 == "Vis-Hydra" %}
      {% set filterOneCenter = 600.0 %}
      {% set filterOneWidth = 600.0 %}
    {% else if filterName.0 == "Hydra" %}
      {% set filterOneCenter = 701.0 %}
      {% set filterOneWidth = 22.1 %}
    {% else if filterName.0 == "Ortho" %}
      {% set filterOneCenter = 805.3 %}
      {% set filterOneWidth = 40.5 %}
    {% else if filterName.0 == "Near-IR" %}
      {% set filterOneCenter = 882.1 %}
      {% set filterOneWidth = 65.9 %}
    {% else if filterName.0 == "Fe203" %}
      {% set filterOneCenter = 931.9 %}
      {% set filterOneWidth = 34.9 %}
    {% else if filterName.0 == "IR" %}
      {% set filterOneCenter = 989.3 %}
      {% set filterOneWidth = 38.2 %}
    {% else if filterName.0 == "NFP-Vis" %}
      {% set filterOneCenter = 600.0 %}
      {% set filterOneWidth = 600.0 %}
    {% else if filterName.0 == "Far-UV" %}
      {% set filterOneCenter = 269.3 %}
      {% set filterOneWidth = 53.6 %}
    {% else if filterName.0 == "Near-UV" %}
      {% set filterOneCenter = 360.0 %}
      {% set filterOneWidth = 51.1 %}
    {% else if filterName.0 == "Orange" %}
      {% set filterOneCenter = 649.2 %}
      {% set filterOneWidth = 84.5 %}
    {% else if filterName.0 == "Blue" %}
      {% set filterOneCenter = 480.7 %}
      {% set filterOneWidth = 74.9 %}
    {% else if filterName.0 == "Red" %}
      {% set filterOneCenter = 743.7 %}
      {% set filterOneWidth = 64.1 %}
    {% else if filterName.0 == "Green" %}
      {% set filterOneCenter = 537.7 %}
      {% set filterOneWidth = 62.4 %}
    {% else if filterName.0 == "Empty" %}
      {% set filterOneCenter = 0.0 %}
      {% set filterOneWidth = 0.0 %}
    {% else if filterName.0 == "UV245" %}
      {% set filterOneCenter = 246.2 %}
      {% set filterOneWidth = 14.1 %}
    {% else if filterName.0 == "CS" %}
      {% set filterOneCenter = 259.0%}
      {% set filterOneWidth = 5.6%}
    {% else if filterName.0 == "UV295" %}
      {% set filterOneCenter = 295.9 %}
      {% set filterOneWidth = 10.9 %}
    {% else if filterName.0 == "OH-WAC" %}
      {% set filterOneCenter = 309.7 %}
      {% set filterOneWidth = 4.1 %}
    {% else if filterName.0 == "UV325" %}
      {% set filterOneCenter = 325.8 %}
      {% set filterOneWidth = 10.7 %}
    {% else if filterName.0 == "NH" %}
      {% set filterOneCenter = 335.9 %}
      {% set filterOneWidth = 4.1 %}
    {% else if filterName.0 == "UV375" %}
      {% set filterOneCenter = 375.6 %}
      {% set filterOneWidth = 9.8 %}
    {% else if filterName.0 == "CN" %}
      {% set filterOneCenter = 388.4 %}
      {% set filterOneWidth = 5.2 %}
    {% else if filterName.0 == "NH2" %}
      {% set filterOneCenter = 572.1 %}
      {% set filterOneWidth = 11.5 %}
    {% else if filterName.0 == "Na" %}
      {% set filterOneCenter = 590.7 %}
      {% set filterOneWidth = 4.7 %}
    {% else if filterName.0 == "VIS610" %}
      {% set filterOneCenter = 612.6 %}
      {% set filterOneWidth = 9.8 %}
    {% else if filterName.0 == "OI" %}
      {% set filterOneCenter = 631.6 %}
      {% set filterOneWidth = 4.0 %}
    {% endif %}

    {% if filterName.1 == "FFP-Vis" %}
      {% set filterTwoCenter = 600.0 %}
      {% set filterTwoWidth = 600.0 %}
    {% else if filterName.1 == "FFP-UV" %}
      {% set filterTwoCenter = 600.0 %}
      {% set filterTwoWidth = 600.0 %}
    {% else if filterName.1 == "FFP-IR" %}
      {% set filterTwoCenter = 600.0 %}
      {% set filterTwoWidth = 600.0 %}
    {% else if filterName.1 == "Netural" %}
      {% set filterTwoCenter = 640.0 %}
      {% set filterTwoWidth = 520.0 %}
    {% else if filterName.1 == "Vis-Hydra" %}
      {% set filterTwoCenter = 600.0 %}
      {% set filterTwoWidth = 600.0 %}
    {% else if filterName.1 == "Hydra" %}
      {% set filterTwoCenter = 701.0 %}
      {% set filterTwoWidth = 22.1 %}
    {% else if filterName.1 == "Ortho" %}
      {% set filterTwoCenter = 805.3 %}
      {% set filterTwoWidth = 40.5 %}
    {% else if filterName.1 == "Near-IR" %}
      {% set filterTwoCenter = 882.1 %}
      {% set filterTwoWidth = 65.9 %}
    {% else if filterName.1 == "Fe203" %}
      {% set filterTwoCenter = 931.9 %}
      {% set filterTwoWidth = 34.9 %}
    {% else if filterName.1 == "IR" %}
      {% set filterTwoCenter = 989.3 %}
      {% set filterTwoWidth = 38.2 %}
    {% else if filterName.1 == "NFP-Vis" %}
      {% set filterTwoCenter = 600.0 %}
      {% set filterTwoWidth = 600.0 %}
    {% else if filterName.1 == "Far-UV" %}
      {% set filterTwoCenter = 269.3 %}
      {% set filterTwoWidth = 53.6 %}
    {% else if filterName.1 == "Near-UV" %}
      {% set filterTwoCenter = 360.0 %}
      {% set filterTwoWidth = 51.1 %}
    {% else if filterName.1 == "Orange" %}
      {% set filterTwoCenter = 649.2 %}
      {% set filterTwoWidth = 84.5 %}
    {% else if filterName.1 == "Blue" %}
      {% set filterTwoCenter = 480.7 %}
      {% set filterTwoWidth = 74.9 %}
    {% else if filterName.1 == "Red" %}
      {% if InstrumentId == "OSIWAC" %}
       {% set filterTwoCenter = 629.8 %}
        {% set filterTwoWidth = 156.8 %}
      {% else if InstrumentId == "OSINAC"%}
        {% set filterTwoCenter = 743.7 %}
        {% set filterTwoWidth = 64.1 %}
      {% endif %}
    {% else if filterName.1 == "Green" %}
      {% if InstrumentId == "OSIWAC" %}
        {% set filterTwoCenter = 537.2 %}
        {% set filterTwoWidth = 63.2 %}
      {% else if InstrumentId == "OSINAC"%}
        {% set filterTwoCenter = 537.7 %}
        {% set filterTwoWidth = 62.4 %}
      {% endif %}
    {%else if filterName.1 == "Empty" %}
        {% set filterTwoCenter = 0.0 %}
        {% set filterTwoWidth = 0.0 %}
    

    {% else if filterName.1 == "UV245" %}
      {% set filterTwoCenter = 246.2 %}
      {% set filterTwoWidth = 14.1 %}
    {% else if filterName.1 == "CS" %}
      {% set filterTwoCenter = 259.0%}
      {% set filterTwoWidth = 5.6%}
    {% else if filterName.1 == "UV295" %}
      {% set filterTwoCenter = 295.9 %}
      {% set filterTwoWidth = 10.9 %}
    {% else if filterName.1 == "OH-WAC" %}
      {% set filterTwoCenter = 309.7 %}
      {% set filterTwoWidth = 4.1 %}
    {% else if filterName.1 == "UV325" %}
      {% set filterTwoCenter = 325.8 %}
      {% set filterTwoWidth = 10.7 %}
    {% else if filterName.1 == "NH" %}
      {% set filterTwoCenter = 335.9 %}
      {% set filterTwoWidth = 4.1 %}
    {% else if filterName.1 == "UV375" %}
      {% set filterTwoCenter = 375.6 %}
      {% set filterTwoWidth = 9.8 %}
    {% else if filterName.1 == "CN" %}
      {% set filterTwoCenter = 388.4 %}
      {% set filterTwoWidth = 5.2 %}
    {% else if filterName.1 == "NH2" %}
      {% set filterTwoCenter = 572.1 %}
      {% set filterTwoWidth = 11.5 %}
    {% else if filterName.1 == "Na" %}
      {% set filterTwoCenter = 590.7 %}
      {% set filterTwoWidth = 4.7 %}
    {% else if filterName.1 == "VIS610" %}
      {% set filterTwoCenter = 612.6 %}
      {% set filterTwoWidth = 9.8 %}
    {% else if filterName.1 == "OI" %}
      {% set filterTwoCenter = 631.6 %}
      {% set filterTwoWidth = 4.0 %}


    {% endif %}

    FilterNumber       = {{SR_MECHANISM_STATUS.FILTER_NUMBER.Value}}
    CombinedFilterName = {{SR_MECHANISM_STATUS.FILTER_NAME.Value}}
    FilterId           = {{SR_MECHANISM_STATUS.FILTER_NUMBER.Value}}
    FilterOneName      = {{filterOneName}}
    FilterOneCenter    = {{filterOneCenter}} <nanometers>
    FilterOneWidth     = {{filterOneWidth}} <nanometers>
    FilterTwoName      = {{filterTwoName}}
    FilterTwoCenter    = {{filterTwoCenter}} <nanometers>
    FilterTwoWidth     = {{filterTwoWidth}} <nanometers>
  End_Group

  Group = Kernels
    NaifFrameCode = -226111
  End_Group
{% endblock %}
