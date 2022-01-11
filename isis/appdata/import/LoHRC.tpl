{% if exists("QUBE.ISIS_INSTRUMENT.SPACECRAFT_NAME.Value") %}
    {% set spacecraftName=QUBE.ISIS_INSTRUMENT.SPACECRAFT_NAME.Value %}
    {% set instrumentID=QUBE.ISIS_INSTRUMENT.INSTRUMENT_ID.Value %}
{% else %}
    {% set spacecraftName=SPACECRAFT_NAME.Value %}
    {% set instrumentID=INSTRUMENT_ID.Value %}
{% endif %}

{% if exists("ptrIMAGE") %}
    {% extends "img_base.tpl" %}
{% else if exists("ptrQUBE") %}
    {% extends "qube_base.tpl" %}
{% endif %}

{% block instrument %}
{{ super() }}
    TargetName               = Moon

    SpacecraftName           = {% if spacecraftName == "LUNAR_ORBITER_3" %}
                              "Lunar Orbiter 3"
                              {% else if spacecraftName == "LUNAR_ORBITER_4" %}
                              "Lunar Orbiter 4"
                              {% else if spacecraftName == "LUNAR_ORBITER_5" %}
                              "Lunar Orbiter 5"
                              {% endif %}

    StartTime                = {% if exists("QUBE.ISIS_INSTRUMENT") %}
                            {{ QUBE.ISIS_INSTRUMENT.START_TIME.Value }}
                            {% else %}
                            {{START_TIME.Value}}
                            {% endif %}

    InstrumentId             = {% if instrumentID == "24_INCH_FOCAL_LENGTH_CAMERA" or instrumentID == "24INCH_FLC" %}
                               "High Resolution Camera"
                               {% else if instrumentID == "80_MM_FOCAL_LENGTH_CAMERA" or instrumentID == "80MM_FLC" %}
                               "Medium Resolution Camera"
                               {% endif %}

    FrameNumber              = {% if exists("QUBE.ISIS_INSTRUMENT") %}
                                {{QUBE.ISIS_INSTRUMENT.FRAME_NUMBER.Value}}
                                {% else %}
                                {{FRAME_NUMBER.Value}}
                                {% endif %}

    

    {% if exists("QUBE.ISIS_INSTRUMENT.FIDUCIAL_ID.Value")%}
        FiducialCoordinateMicron = {{RemoveUnits(QUBE.ISIS_INSTRUMENT.FIDUCIAL_COORD_MICRON.Value)}} <um>
        {% set fiducialArray=QUBE.ISIS_INSTRUMENT.FIDUCIAL_ID.Value %}
        FiducialID               =  ({% for fid in fiducialArray %}
                                        {% if loop.is_last %}
                                            {{ fid }}
                                        {% else %}
                                            {{ fid }},
                                        {% endif %}
                                    {% endfor %})

        {% set fidSamArray=QUBE.ISIS_INSTRUMENT.FIDUCIAL_SAMPLES.Value %}
        FiducialSamples          = ({% for val in fidSamArray %}
                                        {% if loop.is_last %}
                                            {{ val }}
                                        {% else %}
                                            {{ val }},
                                        {% endif %}
                                    {% endfor %}) <pixels>

        {% set fidLineArray=QUBE.ISIS_INSTRUMENT.FIDUCIAL_LINES.Value %}
        FiducialLines            = ({% for line in fidLineArray %}
                                        {% if loop.is_last %}
                                            {{ line }}
                                        {% else %}
                                            {{ line }},
                                        {% endif %}
                                    {% endfor %}) <pixels>
                                    
        {% set fidXCoords=QUBE.ISIS_INSTRUMENT.FIDUCIAL_X_COORDINATES.Value %}
        FiducialXCoordinates     = ({% for xCoord in fidXCoords %}
                                        {% if loop.is_last %}
                                            {{ xCoord }}
                                        {% else %}
                                            {{ xCoord }},
                                        {% endif %}
                                    {% endfor %}) <mm>

        {% set fidYCoords=QUBE.ISIS_INSTRUMENT.FIDUCIAL_Y_COORDINATES.Value %}
        FiducialYCoordinates     = ({% for yCoord in fidYCoords %}
                                        {% if loop.is_last %}
                                            {{ yCoord }}
                                        {% else %}
                                            {{ yCoord }},
                                        {% endif %}
                                    {% endfor %}) <mm>
    
    {% else if exists("LO_FIDUCIAL_ID.Value") %}

        FiducialCoordinateMicron = {{RemoveUnits(LO_FIDUCIAL_COORDINATE_MICRON.Value)}} <um>
        {% set fiducialArray=LO_FIDUCIAL_ID.Value %}
        FiducialID               =  ({% for fid in fiducialArray %}
                                        {% if loop.is_last %}
                                            {{ fid }}
                                        {% else %}
                                            {{ fid }},
                                        {% endif %}
                                    {% endfor %})

        {% set fidSamArray=LO_FIDUCIAL_SAMPLES.Value %}
        FiducialSamples          = ({% for val in fidSamArray %}
                                        {% if loop.is_last %}
                                            {{ val }}
                                        {% else %}
                                            {{ val }},
                                        {% endif %}
                                    {% endfor %}) <pixels>

        {% set fidLineArray=LO_FIDUCIAL_LINES.Value %}
        FiducialLines            = ({% for line in fidLineArray %}
                                        {% if loop.is_last %}
                                            {{ line }}
                                        {% else %}
                                            {{ line }},
                                        {% endif %}
                                    {% endfor %}) <pixels>
                                    
        {% set fidXCoords=LO_FIDUCIAL_X_COORDINATES.Value %}
        FiducialXCoordinates     = ({% for xCoord in fidXCoords %}
                                        {% if loop.is_last %}
                                            {{ xCoord }}
                                        {% else %}
                                            {{ xCoord }},
                                        {% endif %}
                                    {% endfor %}) <mm>

        {% set fidYCoords=LO_FIDUCIAL_Y_COORDINATES.Value %}
        FiducialYCoordinates     = ({% for yCoord in fidYCoords %}
                                        {% if loop.is_last %}
                                            {{ yCoord }}
                                        {% else %}
                                            {{ yCoord }},
                                        {% endif %}
                                    {% endfor %}) <mm>

    {% else if exists("LO_BORESIGHT_LINE")%}
        BoresightLine = {{LO_BORESIGHT_LINE}}
        BoresightSample = {{LO_BORESIGHT_SAMPLE}}

    {% else if exists("QUBE.ISIS_INSTRUMENT.BORESIGHT_LINE") %}
        BoresightLine = {{QUBE.ISIS_INSTRUMENT.BORESIGHT_LINE}}
        BoresightSample = {{QUBE.ISIS_INSTRUMENT.BORESIGHT_SAMPLE}}
        FiducialCoordinateMicron = {{RemoveUnits(QUBE.ISIS_INSTRUMENT.FIDUCIAL_COORD_MICRON.Value)}} <um>

    {% endif %}

    {% if exists("QUBE.IMAGE_NUMBER") %}
        {% set ImageNumber = QUBE.IMAGE_NUMBER.Value %}
    {% else if exists("IMAGE_NUMBER") %}
        {% set ImageNumber = IMAGE_NUMBER.Value %}
    {% endif %}

    SubFrame                 = {{SetSubFrame(ImageNumber.0)}}

{% endblock %}

{% block bandbin %}
    {% if exists("QUBE.ISIS_INSTRUMENT.FIDUCIAL_ID") %}
        FilterName   = {{QUBE.BAND_BIN.BAND_BIN_UNIT.Value}}
        Center       = {{QUBE.BAND_BIN.BAND_BIN_CENTER.Value}}
        OriginalBand = {{QUBE.BAND_BIN.BAND_BIN_ORIGINAL_BAND.Value}}

    {% else if exists("QUBE.ISIS_INSTRUMENT.BORESIGHT_SAMPLE") %}
        FilterName   = {{QUBE.BAND_BIN.BAND_BIN_UNIT.Value}}
        Center       = {{QUBE.BAND_BIN.BAND_BIN_CENTER.Value}}
        OriginalBand = {{QUBE.BAND_BIN.BAND_BIN_ORIGINAL_BAND.Value}}
    {% endif %}
    
{% endblock %}

{% block additional_groups %}
    Group = kernels
    {% if spacecraftName ==  "LUNAR_ORBITER_3" %}
        {% set frameCode = "-533" %}

    {% else if spacecraftName ==  "LUNAR_ORBITER_4" %}
        {% set frameCode = "-534" %}

    {% else if spacecraftName ==  "LUNAR_ORBITER_5" %}
        {% set frameCode = "-535" %}

    {% endif %}

    {% if instrumentID == "24_INCH_FOCAL_LENGTH_CAMERA" or instrumentID == "24INCH_FLC" %}
        {% set frameCodeSuffix = "001" %}

    {% else if instrumentID == "80_MM_FOCAL_LENGTH_CAMERA" or instrumentID == "80MM_FLC" %}
        {% set frameCodeSuffix = "002" %}

    {% endif %}

    NaifFrameCode = {{frameCode}}{{frameCodeSuffix}}
    End_Group
    
{% endblock %}

