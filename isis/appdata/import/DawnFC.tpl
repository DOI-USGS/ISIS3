{% extends "img_base.tpl" %}


{% block instrument %}
SpacecraftName            = {{ MISSION_ID.Value }}
InstrumentId              = {{ INSTRUMENT_ID.Value }}
SpacecraftClockStartCount = {{ SPACECRAFT_CLOCK_START_COUNT.Value }}
SpacecraftClockStopCount  = {{ SPACECRAFT_CLOCK_STOP_COUNT.Value }}
StartTime                 = {{ START_TIME.Value }}
StopTime                  = {{ STOP_TIME.Value }}
ExposureDuration          = {{ EXPOSURE_DURATION.Value }} <millisecond>
PixelAveragingWidth       = {% if exists("PIXEL_AVERAGING_WIDTH") %}
                            {{ PIXEL_AVERAGING_WIDTH.Value }}
                            {% else %}
                            1
                            {% endif %}
PixelAveragingHeight      = {% if exists("PIXEL_AVERAGING_HEIGHT") %}
                            {{ PIXEL_AVERAGING_HEIGHT.Value }}
                            {% else %}
                            1
                            {% endif %}
TargetName                = {% if exists("TARGET_NAME") %}
                            {% set target=TARGET_NAME.Value %}
                            {% if target=="4 VESTA" %}
                            VESTA
                            {% else %}
                            {{ target }}
                            {% endif %}
                            {% else %}
                            N/A
                            {% endif %}
OriginalTargetName        = {% if exists("TARGET_NAME") %}
                            {{ TARGET_NAME.Value }}
                            {% else %}
                            N/A
                            {% endif %}
OrbitNumber               = {{ ORBIT_NUMBER.Value }}
FirstLine                 = {{ IMAGE.FIRST_LINE.Value }}
FirstLineSample           = {{ IMAGE.FIRST_LINE_SAMPLE.Value }}
{% endblock %}

{% block archive %}
    FileName                = {{ FILE_NAME.Value }}
    SoftwareName            = {{ SOFTWARE_NAME.Value }}
    SoftwareVersionId       = {{ SOFTWARE_VERSION_ID.Value }}
    DataSetName             = "{{ DATA_SET_NAME.Value }}"
    DataSetId               = {{ DataSetId }}
    ProductId               = {{ ProductId }}
    ProductType             = {{ PRODUCT_TYPE.Value }}
    StandardDataProductId   = {{ STANDARD_DATA_PRODUCT_ID.Value }}
    ObservationId           = {{ OBSERVATION_ID.Value }}
    ProducerFullName        = "{{ PRODUCER_FULL_NAME.Value }}"
    ProducerInstitutionName = "{{ PRODUCER_INSTITUTION_NAME.Value }}"
    ProductCreationTime     = {{ PRODUCT_CREATION_TIME.Value }}
    ProductVersionId        = {{ PRODUCT_VERSION_ID.Value }}
    ReleaseId               = {{ RELEASE_ID.Value }}
{% endblock %}

{% block additional_groups %}
Group = BandBin
{% set filterNumber=FILTER_NUMBER.Value %}
{% if filterNumber == "1" %}
  {% set center="700" %}
  {% set width="700" %}
  {% set filterName="Clear_F1"%}
{% else if filterNumber == "2" %}
  {% set center="555" %}
  {% set width="43" %}
  {% set filterName="Green_F2" %}
{% else if filterNumber == "3" %}
  {% set center="749" %}
  {% set width="44" %}
  {% set filterName="Red_F3" %}
{% else if filterNumber == "4" %}
  {% set center="917" %}
  {% set width="45" %}
  {% set filterName="NIR_F4" %}
{% else if filterNumber == "5" %}
  {% set center="965" %}
  {% set width="85" %}
  {% set filterName="NIR_F5" %}
{% else if filterNumber == "6" %}
  {% set center="829" %}
  {% set width="33" %}
  {% set filterName="NIR_F6" %}
{% else if filterNumber == "7" %}
  {% set center="653" %}
  {% set width="42" %}
  {% set filterName="Red_F7" %}
{% else if filterNumber == "8" %}
  {% set center="438" %}
  {% set width="40" %}
  {% set filterName="Blue_F8" %}
{% endif %}
  FilterNumber = {{ filterNumber }}
  Center       = {{ center }}
  Width        = {{ width }}
  FilterName   = {{ filterName }}
End_Group

Group = Kernels
  {% set instID = INSTRUMENT_ID.Value %}
  NaifFrameCode = {% if instID == "FC1" %}
                  {{ (-203110) - int(filterNumber) }}
                  {% else if instID == "FC2" %}
                  {{ (-203120) - int(filterNumber) }}
                  {% endif %}
End_Group
{% endblock %}

{% block translation %}
CoreAxisNames               = SAMPLELINEBAND
{% endblock %}
