{#- Pull out the instrument ID -#}
{%- if exists("MainLabel.IsisCube.Instrument.InstrumentId.Value") -%}
  {%- set InstrumentId=MainLabel.IsisCube.Instrument.InstrumentId.Value -%}
{%- endif -%}

{#- Pull out the spacecraft name -#}
{%- if exists("MainLabel.IsisCube.Instrument.SpacecraftName.Value") -%}
  {%- set SpacecraftName=MainLabel.IsisCube.Instrument.SpacecraftName.Value -%}
{%- else if exists("MainLabel.IsisCube.Mosaic.SpacecraftName.Value") -%}
  {%- set SpacecraftName=MainLabel.IsisCube.Mosaic.SpacecraftName.Value -%}
{%- endif -%}

{#- Set the spacecraft ID and instrument ID portioins of the template -#}
{#- Trace Gas Orbiter -#}
{%- if SpacecraftName == "TRACE GAS ORBITER" -%}
  {%- set SpacecraftId="TGO" -%}

{#- Lunar Reconnaissance Orbiter -#}
{%- else if SpacecraftName == "LUNAR RECONNAISSANCE ORBITER" -%}
  {%- set SpacecraftId="LRO" -%}
  {%- if exists("MainLabel.IsisCube.Instrument.FrameId.Value") -%}
    {%- if MainLabel.IsisCube.Instrument.FrameId.Value == "LEFT" -%}
      {%- set InstrumentId="CNacEdr" -%}
    {%- else if MainLabel.IsisCube.Instrument.FrameId.Value == "RIGHT" -%}
      {%- set InstrumentId="CNacEdr" -%}
    {%- endif -%}
  {%- endif -%}

{#- Default to using the spacecraft and instrument from the input label without modification  -#}
{%- else -%}
  {%- set SpacecraftId=SpacecraftName -%}
{%- endif -%}

{%- if SpacecraftId -%}$ISISROOT/appdata/export/{{- SpacecraftId -}}{{- InstrumentId -}}.tpl{% endif -%}