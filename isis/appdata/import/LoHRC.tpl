{% extends "qube_base.tpl" %}

{% block instrument %}
    TargetName               = Moon
    {% set spacecraftName=infoGroup.SPACECRAFT_NAME.Value.0 %}
    SpacecraftName           = {% if spacecraftName == "LUNAR_ORBITER_3" %}
                              "Lunar Orbiter 3"
                              {% else if spacecraftName == "LUNAR_ORBITER_4" %}
                              "Lunar Orbiter 4"
                              {% else if spacecraftName == "LUNAR_ORBITER_5" %}
                              "Lunar Orbiter 5"
                              {% endif %}
    StartTime                = {{ infoGroup.START_TIME.Value }}
    {% set instrumentID=infoGroup.INSTRUMENT_ID.Value %}
    InstrumentId             = {% if instrumentID == "24_INCH_FOCAL_LENGTH_CAMERA" or instrumentID == "24INCH_FLC" %}
                               "High Resolution Camera"
                               {% else if instrumentID in FLC %}
                               "High Resolution Camera"
                               {% endif %}
    FrameNumber              = 3133
    FiducialCoordinateMicron = 50 <um>
    FiducialID               = (1b, 2a, 23b, 47b, 48b, 72b, 73a, 93b, 94b,
                                34b, 96b, 117b, 118b, 143b, 144b, 167b, 168b,
                                189b, 190b, 99b, 193b, 194b, 213b, 214b, 239b,
                                263b, 264b, 286b, 236b, 283b)
    FiducialSamples          = (32162.0, 32192.0, 29745.0, 27114.0, 27116.0,
                                24484.0, 24295.0, 22070.0, 22072.0, 28653.0,
                                21854.0, 19438.0, 19441.0, 16593.0, 16593.0,
                                13961.0, 13960.0, 11551.0, 11548.0, 21412.0,
                                11114.0, 11109.0, 8918.0, 8914.0, 6070.0,
                                3440.0, 3436.0, 1027.0, 6505.0,
                                1248.0) <pixels>
    FiducialLines            = (8510.0, 597.0, 8505.0, 8506.0, 589.0, 587.0,
                                8504.0, 8502.0, 583.0, 590.0, 587.0, 8496.0,
                                581.0, 8498.0, 584.0, 8496.0, 581.0, 8500.0,
                                579.0, 8495.0, 8493.0, 580.0, 8496.0, 584.0,
                                8493.0, 8497.0, 580.0, 584.0, 581.0,
                                8496.0) <pixels>
    FiducialXCoordinates     = (-108.168, -108.339, -91.403, -73.101, -73.046,
                                -54.733, -53.474, -38.026, -37.964, -83.703,
                                -36.446, -19.716, -19.684, 0.08, 0.122, 18.376,
                                18.427, 35.174, 35.198, -33.441, 38.211, 38.23,
                                53.474, 53.498, 73.296, 91.568, 91.579,
                                108.352, 70.25, 106.844) <mm>
    FiducialYCoordinates     = (27.476, -27.488, 27.483, 27.498, -27.497,
                                -27.488, 27.5, 27.496, -27.49, -27.494,
                                -27.488, 27.495, -27.494, 27.505, -27.493,
                                27.496, -27.49, 27.492, -27.486, 27.499,
                                27.488, -27.485, 27.49, -27.487, 27.491,
                                27.474, -27.49, -27.481, -27.488, 27.479) <mm>
    SubFrame                 = 1
{% endblock %}

{% block bandbin %}
    FilterName   = none
    Center       = 1.0
    OriginalBand = 1
{% endblock %}

{% block kernels %}
    NaifFrameCode = -533001
  End_Group
{% endblock %}
