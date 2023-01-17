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
    {% block instrument %}
    {% endblock %}
  End_Group


  {% block additional_groups %}
  {% endblock %}

End_Object

Object = Translation
  {% if exists("ptrIMAGE.Units") %}
  DataFilePointer             = {{ ptrIMAGE.Value }} <{{ ptrIMAGE.Units }}>
  {% else if exists("ptrIMAGE.Value") %}
    {% if isArray(ptrIMAGE.Value) %}
    DataFilePointer             = ({{ ptrIMAGE.Value.0 }}, {{ ptrIMAGE.Value.1 }})
    {% else %}
    DataFilePointer             = {{ ptrIMAGE.Value }}
    {% endif %}
  {% endif %}
  {% if exists("RECORD_BYTES") %}
  DataFileRecordBytes         = {{ RECORD_BYTES.Value }}
  {% endif %}
  {% if exists("IMAGE.CORE_NULL.Value") %}
  CoreNull             = {{ IMAGE.CORE_NULL.Value }}
  {% endif %}
  {% if exists("IMAGE.CORE_LOW_REPR_SATURATION.Value") %}
  CoreLRS              = {{ IMAGE.CORE_LOW_REPR_SATURATION.Value }}
  {% endif %}
  {% if exists("IMAGE.CORE_LOW_INSTR_SATURATION.Value") %}
  CoreLIS              = {{ IMAGE.CORE_LOW_INSTR_SATURATION.Value }}
  {% endif %}
  {% if exists("IMAGE.CORE_HIGH_REPR_SATURATION.Value") %}
  CoreHRS              = {{ IMAGE.CORE_HIGH_REPR_SATURATION.Value }}
  {% endif %}
  {% if exists("IMAGE.CORE_HIGH_INSTR_SATURATION.Value") %}
  CoreHIS              = {{ IMAGE.CORE_HIGH_INSTR_SATURATION.Value }}
  {% endif %}
  {% if exists("IMAGE.SUFFIX_BYTES.Value") %}
  SuffixBytes          = {{ IMAGE.SUFFIX_BYTES.Value }}
  {% endif %}
  {% if exists("IMAGE.SUFFIX_ITEMS.Value") %}
  SuffixItems          = ({{ IMAGE.SUFFIX_ITEMS.Value.0 }}, {{ IMAGE.SUFFIX_ITEMS.Value.1 }}, {{ IMAGE.SUFFIX_ITEMS.Value.2 }})
  {% endif %}
  {% block translation %}
  CubeAtts             = ""
  {% endblock %}
End_Object
End
