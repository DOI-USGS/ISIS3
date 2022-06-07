{% set CoreOrganization=QUBE.AXIS_NAME.Value %}
Object = IsisCube
  Object = Core
    Group = Dimensions
    {% for org in CoreOrganization %}
      {% if org == "SAMPLE" %}
      Samples =  {{ at(QUBE.CORE_ITEMS.Value, loop.index1 - 1) }}
      {% else if org == "LINE" %}
      Lines   = {{ at(QUBE.CORE_ITEMS.Value, loop.index1 - 1) }}
      {% else if org == "BAND" %}
      Bands   =  {{ at(QUBE.CORE_ITEMS.Value, loop.index1 - 1) }}
      {% endif %}
   {% endfor %}
    End_Group

    Group = Pixels
      {%- set sampbits=QUBE.CORE_ITEM_BYTES.Value -%}
      {%- if sampbits == "1" -%}
      {%- set sampbits="8" -%}
      {%- else if sampbits == "2" -%}
      {%- set sampbits="16" -%}
      {%- else if sampbits == "4" -%}
      {%- set sampbits="32" -%}
      {%- endif -%}

      {%- set type=QUBE.CORE_ITEM_TYPE.Value -%}
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
      Base       = {% if exists("QUBE.CORE_BASE.Value") %}
                   {{ QUBE.CORE_BASE.Value }}
                   {% else %}
                   0.0
                   {% endif %}
      Multiplier = {% if exists("QUBE.CORE_MULTIPLIER.Value") %}
                   {{ QUBE.CORE_MULTIPLIER.Value }}
                   {% else %}
                   1.0
                   {% endif %}
    End_Group
  End_Object

  Group = Instrument
    {% block instrument %}
    {%- if exists("ROOT.MISSION_NAME.Value") -%}
    {%- set infoGroup=ROOT -%}
    {%- else if exists("ROOT.ISIS_INSTRUMENT.SPACECRAFT_NAME") -%}
    {%- set infoGroup=ROOT -%}
    {%- else -%}
    {%- set infoGroup=QUBE -%}
    {%- endif -%}
    {% endblock %}
  End_Group

  {% block additional_groups %}
  {% endblock %}

  Group = BandBin
  {% block bandbin %}
  {% endblock %}
  End_Group

  Group = Kernels
  {% block kernels %}
  {% endblock %}
  End_Group
End_Object


Object = Translation
  {% if exists("ptrQUBE.Units") %}
  DataFilePointer             = {{ ptrQUBE.Value }} <{{ ptrQUBE.Units }}>
  {% else if exists("ptrQUBE.Value") %}
    {% if isArray(ptrQUBE.Value) %}
    DataFilePointer             = ({{ ptrQUBE.Value.0 }}, {{ ptrQUBE.Value.1 }})
    {% else %}
    DataFilePointer             = {{ ptrQUBE.Value }}
    {% endif %}
  {% endif %}
  {% if exists("RECORD_BYTES") %}
  DataFileRecordBytes  = {{ RECORD_BYTES.Value }}
  {% endif %}
  {% if exists("QUBE.AXIS_NAME.Value") %}
  CoreAxisNames        = {{ QUBE.AXIS_NAME.Value.0 }}{{ QUBE.AXIS_NAME.Value.1 }}{{ QUBE.AXIS_NAME.Value.2 }}
  {% endif %}
  {% if exists("BAND_BIN.BAND_BIN_BASE.Value") %}
  BandBinBase          = {{ BAND_BIN.BAND_BIN_BASE.Value }}
  {% endif %}
  {% if exists("BAND_BIN.BAND_BIN_MULTIPLIER.Value") %}
  BandBinMultiplier    = {{ BAND_BIN.BAND_BIN_MULTIPLIER.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_NULL.Value") %}
  CoreNull             =  {{ QUBE.CORE_NULL.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_LOW_REPR_SATURATION.Value") %}
  CoreLRS              = {{ QUBE.CORE_LOW_REPR_SATURATION.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_LOW_INSTR_SATURATION.Value") %}
  CoreLIS              = {{ QUBE.CORE_LOW_INSTR_SATURATION.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_HIGH_REPR_SATURATION.Value") %}
  CoreHRS              = {{ QUBE.CORE_HIGH_REPR_SATURATION.Value }}
  {% endif %}
  {% if exists("QUBE.CORE_HIGH_INSTR_SATURATION.Value") %}
  CoreHIS              = {{ QUBE.CORE_HIGH_INSTR_SATURATION.Value }}
  {% endif %}
  {% if exists("QUBE.SUFFIX_BYTES.Value") %}
  SuffixBytes          = {{ QUBE.SUFFIX_BYTES.Value }}
  {% endif %}
  {% if exists("QUBE.SUFFIX_ITEMS.Value") %}
  SuffixItems          = ({{ QUBE.SUFFIX_ITEMS.Value.0 }}, {{ QUBE.SUFFIX_ITEMS.Value.1 }}, {{ QUBE.SUFFIX_ITEMS.Value.2 }})
  {% endif %}
  {% block translation %}
  {% endblock %}
End_Object
End
