Object = IsisCube
  Object = Core
    Group = Dimensions
      Samples = {{ IMAGE.LINE_SAMPLES.Value }}
      Lines   = {{ IMAGE.LINES.Value }}
      Bands   = {% if exists("IMAGE.BANDS.Value") %}
                {{ IMAGE.BANDS.Value }}
                {% else %}
                1
                {% endif %}
    End_Group

    Group = Pixels
      {%- set type=IMAGE.SAMPLE_TYPE.Value -%}
      {%- set sampbits=IMAGE.SAMPLE_BITS.Value -%}
      {%- if type == "LSB_INTEGER" -%} {%- set pixType="Integer" -%}
      {%- else if type == "MSB_INTEGER" -%} {%- set pixType="Integer" -%}
      {%- else if type == "PC_INTEGER" -%} {%- set pixType="Integer" -%}
      {%- else if type == "MAC_INTEGER" -%} {%- set pixType="Integer" -%}
      {%- else if type == "SUN_INTEGER" -%} {%- set pixType="Integer" -%}
      {%- else if type == "VAX_INTEGER" -%} {%- set pixType="Integer" -%}
      {%- else if type == "UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
      {%- else if type == "UNSIGNED INTEGER" -%} {%- set pixType="Natural" -%}
      {%- else if type == "LSB_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
      {%- else if type == "MSB_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
      {%- else if type == "PC_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
      {%- else if type == "MAC_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
      {%- else if type == "SUN_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
      {%- else if type == "VAX_UNSIGNED_INTEGER" -%} {%- set pixType="Natural" -%}
      {%- else if type == "FLOAT" -%} {%- set pixType="Real" -%}
      {%- else if type == "REAL" -%} {%- set pixType="Real" -%}
      {%- else if type == "PC_REAL" -%} {%- set pixType="Real" -%}
      {%- else if type == "IEEE_REAL" -%} {%- set pixType="Real" -%}
      {%- else if type == "MAC_REAL" -%} {%- set pixType="Real" -%}
      {%- else if type == "SUN_REAL" -%} {%- set pixType="Real" -%}
      {%- else if type == "VAX_REAL" -%} {%- set pixType="Real" -%}
      {%- else -%} {%- set pixType="LSB_INTEGER" -%}
      {%- endif -%}
      Type       = {% if pixType == "Real" and sampbits == "64" %} Double
                   {% else if pixType == "Real" and sampbits == "32" %} Real
                   {% else if pixType == "Integer" and sampbits == "8" %} UnsignedByte
                   {% else if pixType == "Integer" and sampbits == "16" %} SignedWord
                   {% else if pixType == "Integer" and sampbits == "32" %} SignedInteger
                   {% else if pixType == "Natural" and sampbits == "8" %} UnsignedByte
                   {% else if pixType == "Natural" and sampbits == "16" %} UnsignedWord
                   {% else if pixType == "Natural" and sampbits == "32" %} UnsignedInteger
                   {% endif %}
      ByteOrder  = {% if type == "LSB_INTEGER" %} LSB
                   {% else if type == "PC_INTEGER" %} LSB
                   {% else if type == "VAX_INTEGER" %} LSB
                   {% else if type == "LSB_UNSIGNED_INTEGER" %} LSB
                   {% else if type == "PC_UNSIGNED_INTEGER" %} LSB
                   {% else if type == "VAX_UNSIGNED_INTEGER" %} LSB
                   {% else if type == "PC_REAL" %} LSB
                   {% else if type == "VAX_REAL" %} LSB
                   {% else if type == "MSB_INTEGER" %} MSB
                   {% else if type == "MAC_INTEGER" %} MSB
                   {% else if type == "SUN_INTEGER" %} MSB
                   {% else if type == "UNSIGNED_INTEGER" %} MSB
                   {% else if type == "UNSIGNED INTEGER" %} MSB
                   {% else if type == "MSB_UNSIGNED_INTEGER" %} MSB
                   {% else if type == "MAC_UNSIGNED_INTEGER" %} MSB
                   {% else if type == "SUN_UNSIGNED_INTEGER" %} MSB
                   {% else if type == "FLOAT" %} MSB
                   {% else if type == "REAL" %} MSB
                   {% else if type == "IEEE_REAL" %} MSB
                   {% else if type == "MAC_REAL" %} MSB
                   {% else if type == "SUN_REAL" %} MSB
                   {% else %} LSB_INTEGER
                   {% endif %}
      Base       = {% if exists("IMAGE.OFFSET.Value") %}
                   {{ IMAGE.OFFSET.Value }}
                   {% else %}
                   0.0
                   {% endif %}
      Multiplier = {% if exists("IMAGE.SCALING_FACTOR.Value") %}
                   {{ IMAGE.SCALING_FACTOR.Value }}
                   {% else %}
                   1.0
                   {% endif %}
    End_Group
  End_Object

  Group = Instrument
    SpacecraftName       = {{ SPACECRAFT_NAME.Value }}
    InstrumentId         = {{ INSTRUMENT_NAME.Value }}
    TargetName           = {{ TARGET_NAME.Value }}
    StartTime            = {% set startTime=IMAGE_TIME.Value %}
                           {{ RemoveStartTimeZ(startTime) }}
    ExposureDuration     = {{ EXPOSURE_DURATION.Value }} <seconds>
    SpacecraftClockCount = {{ IMAGE_NUMBER.Value }}
    FloodModeId          = {{ FLOOD_MODE_ID.Value }}
    GainModeId           = {{ GAIN_MODE_ID.Value }}
    OffsetModeId         = {{ OFFSET_MODE_ID.Value }}
  End_Group

  Group = Archive
    DataSetId       = {{ DATA_SET_ID.Value }}
    ProductId       = {{ IMAGE_ID.Value }}
    MissonPhaseName = {{ MISSION_PHASE_NAME.Value }}
    ImageNumber     = {{ IMAGE_NUMBER.Value }}
    OrbitNumber     = {{ ORBIT_NUMBER.Value }}
  End_Group

  Group = BandBin
  {% set filterName=FILTER_NAME.Value %}
    FilterName = {{ FILTER_NAME.Value }}
    FilterId   = {% if filterName == "BLUE" %} 1
                 {% else if filterName == "MINUS_BLUE" %} 2
                 {% else if filterName == "VIOLET" %} 3
                 {% else if filterName == "CLEAR" %} 4
                 {% else if filterName == "GREEN" %} 5
                 {% else if filterName == "RED" %} 6
                 {% endif %}
    Center     = {% if filterName == "BLUE" %} 0.470000 <micrometers>
                 {% else if filterName == "MINUS_BLUE" %} 0.550000 <micrometers>
                 {% else if filterName == "VIOLET" %} 0.440000 <micrometers>
                 {% else if filterName == "CLEAR" %} 0.520000 <micrometers>
                 {% else if filterName == "GREEN" %} 0.530000 <micrometers>
                 {% else if filterName == "RED" %} 0.590000 <micrometers>
                 {% endif %}
    Width      = {% if filterName == "BLUE" %} 0.180000 <micrometers>
                 {% else if filterName == "MINUS_BLUE" %} 0.220000 <micrometers>
                 {% else if filterName == "VIOLET" %} 0.120000 <micrometers>
                 {% else if filterName == "CLEAR" %} 0.350000 <micrometers>
                 {% else if filterName == "GREEN" %} 0.100000 <micrometers>
                 {% else if filterName == "RED" %} 0.150000 <micrometers>
                 {% endif %}
  End_Group

  Group = Kernels
    {% set spacecraft = SPACECRAFT_NAME.Value %}
    {% set instrument = INSTRUMENT_NAME.Value %}
    {% if spacecraft == "VIKING_ORBITER_1" %}
    NaifFrameCode = {% if instrument == "VISUAL_IMAGING_SUBSYSTEM_CAMERA_A" %} -27001
                    {% else %} -27002
                    {% endif %}
                    {% set spn = 1 %}
    {% else %}
    NaifFrameCode = {% if instrument == "VISUAL_IMAGING_SUBSYSTEM_CAMERA_A" %} -30001
                    {% else %} -30002
                    {% endif %}
                    {% set spn = 2 %}
    {% endif %}
  End_Group

  Group = Reseaus
  {% if spn == 1 %}
    {% if INSTRUMENT_NAME.Value == "VISUAL_IMAGING_SUBSYSTEM_CAMERA_A" %}
    Line     = (4, 8, 11, 13, 15, 17, 18, 20, 21, 22, 23, 132, 134, 137, 139,
                141, 143, 145, 147, 148, 150, 151, 152, 260, 264, 266, 268,
                270, 272, 274, 276, 277, 279, 281, 389, 391, 394, 396, 398,
                400, 401, 403, 405, 407, 408, 409, 518, 521, 523, 526, 528,
                529, 531, 532, 534, 536, 538, 647, 648, 651, 653, 655, 657,
                658, 660, 662, 663, 665, 666, 776, 779, 781, 783, 785, 786,
                788, 789, 790, 792, 794, 906, 907, 909, 911, 913, 914, 915,
                917, 918, 920, 922, 923, 1035, 1037, 1039, 1040, 1042, 1043,
                1044, 1046, 1047, 1048, 1051)
    Sample   = (18, 137, 255, 372, 489, 605, 721, 837, 954, 1070, 1187, 20,
                79, 197, 314, 431, 547, 663, 779, 895, 1011, 1127, 1186, 20,
                139, 256, 373, 490, 605, 721, 837, 953, 1069, 1185, 21, 81,
                199, 316, 432, 548, 663, 779, 895, 1010, 1126, 1184, 22, 141,
                257, 374, 489, 605, 721, 836, 952, 1068, 1184, 22, 82, 199,
                316, 431, 547, 663, 778, 893, 1009, 1125, 1183, 22, 141, 257,
                373, 489, 604, 719, 835, 950, 1066, 1182, 22, 81, 198, 314,
                430, 545, 660, 776, 891, 1007, 1123, 1181, 20, 138, 255, 371,
                486, 602, 717, 832, 948, 1064, 1180)
    Type     = (1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6)
    Valid    = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    Template = $viking1/reseaus/vo1.visa.template.cub
    {% else %}
    Line     = (5, 6, 8, 9, 10, 11, 12, 13, 14, 14, 15, 133, 134, 135, 137,
                138, 139, 140, 141, 141, 142, 143, 144, 263, 264, 266, 267,
                268, 269, 269, 270, 271, 272, 273, 393, 393, 395, 396, 397,
                398, 399, 399, 400, 401, 402, 403, 523, 524, 525, 526, 527,
                527, 528, 529, 530, 530, 532, 652, 652, 654, 655, 656, 657,
                657, 658, 659, 660, 661, 662, 781, 783, 784, 785, 786, 787,
                788, 788, 789, 790, 791, 911, 912, 913, 914, 915, 916, 917,
                918, 918, 919, 920, 921, 1040, 1041, 1043, 1044, 1045, 1045,
                1046, 1047, 1047, 1048, 1050)
    Sample   = (24, 142, 259, 375, 491, 607, 723, 839, 954, 1070, 1185, 24,
                84, 201, 317, 433, 549, 665, 780, 896, 1011, 1127, 1183, 25,
                142, 259, 375, 492, 607, 722, 838, 953, 1068, 1183, 25, 84,
                201, 317, 433, 549, 665, 779, 895, 1010, 1125, 1182, 25, 143,
                259, 375, 491, 607, 722, 837, 952, 1067, 1182, 25, 84, 201,
                317, 433, 548, 664, 779, 894, 1009, 1124, 1181, 25, 142, 258,
                374, 490, 605, 720, 835, 951, 1066, 1180, 24, 83, 200, 316,
                431, 547, 662, 776, 892, 1007, 1122, 1179, 23, 140, 257, 373,
                488, 603, 718, 833, 948, 1063, 1179)
    Type     = (1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6)
    Valid    = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    Template = $viking1/reseaus/vo1.visb.template.cub
    {% endif %}
  {% else %}
    {% if INSTRUMENT_NAME.Value == "VISUAL_IMAGING_SUBSYSTEM_CAMERA_A" %}
    Line     = (2, 6, 9, 12, 14, 15, 17, 18, 20,  21, 22, 131,  132, 135,
                138, 140, 142, 143, 145, 146, 147, 149, 150, 260, 263, 265,
                267, 269, 271, 272, 274, 275, 277, 278, 389, 390,  393, 395,
                397,  398, 400,  401,  403, 404, 406, 407, 518,  520, 523,
                524, 526, 528, 529,  531,  532,  534, 536, 647, 648, 650, 652,
                654, 656, 657, 658, 660, 661, 663, 664, 776, 778, 780, 782,
                783, 785, 786, 788, 789,  791 793, 905, 906, 908, 910, 911,
                913, 914, 915,  917, 918, 920, 921, 1034, 1035, 1037, 1039,
                1040, 1041, 1043, 1044, 1045, 1046,1049)
    Sample   = (25, 142, 259, 375, 491, 606, 722, 838, 953, 1069, 1184, 25,
                84, 200, 316, 432, 548, 663, 778, 894, 1009, 1124, 1181, 25,
                142, 258, 374, 489, 604, 720, 835, 950, 1065, 1180, 26, 84,
                200, 316, 431, 546, 662, 777, 892, 1007, 1122, 1179, 26, 142,
                258, 373, 489, 604, 719, 837, 949, 1064, 1179, 26, 84, 200,
                316, 431, 546, 661, 776, 891, 1006, 1121, 1177, 25, 142, 257,
                373, 488, 603, 718, 833, 948, 1063, 1177, 24, 83, 199, 315,
                430, 545, 660, 775, 890, 1005, 1120, 1177, 23, 140, 256, 371,
                487, 602, 717, 832, 947, 1062, 1176)
    Type     = (1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6)
    Valid    = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    Template = $viking2/reseaus/vo2.visa.template.cub
    {% else %}
    Line     = (5, 7, 9, 10, 11, 12, 14, 15, 17, 19, 20, 135, 136, 137, 138,
                139, 140, 142, 143, 144, 146, 148, 149, 265, 266, 267, 268,
                269, 270, 271, 272, 274, 276, 278, 395, 395, 397, 398, 398,
                399, 400, 401, 402, 404, 406, 407, 525, 526, 526, 527, 528,
                528, 529, 530, 531, 533, 535, 654, 655, 655, 656, 657, 657,
                658, 659, 660, 661, 663, 664, 784, 784, 785, 785, 786, 786,
                787, 788, 789, 790, 793, 914, 914, 914, 914, 915, 915, 916,
                916, 917, 918, 920, 922, 1043, 1043, 1043, 1043, 1044, 1044,
                1045, 1045, 1046, 1048, 1051)
    Sample   = (23, 141, 257, 373, 489, 605, 720, 836, 952, 1068, 1185, 23,
                82, 199, 315, 431, 546, 662, 778, 894, 1010, 1126, 1184, 24,
                141, 257, 373, 489, 605, 720, 836, 952, 1068, 1184, 24, 82,
                199, 315, 432, 547, 663, 779, 895, 1011, 1127, 1184, 24, 141,
                258, 374, 490, 605, 721, 837, 953, 1070, 1185, 24, 83, 200,
                316, 432, 548, 664, 780, 896, 1012, 1128, 1185, 24, 142, 258,
                374, 490, 606, 722, 838, 954, 1070, 1186, 23, 82, 199, 316,
                432, 548, 664, 780, 896, 1013, 1129, 1186, 22, 140, 257, 374,
                489, 606, 722, 838, 954, 1071, 1187)
    Type     = (1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5,
                5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5,
                5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5,
                5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6)
    Valid    = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    Template = $viking2/reseaus/vo2.visb.template.cub
    {% endif %}
  {% endif %}
    Status   = Nominal
  End_Group
End_Object
End
