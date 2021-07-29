{% if exists("MainLabel.IsisCube.Instrument.InstrumentId.Value") -%}
{% set InstrumentId=MainLabel.IsisCube.Instrument.InstrumentId.Value -%}
{% endif -%}

{% if exists("MainLabel.IsisCube.Instrument.SpacecraftName.Value") -%}
{% set SpacecraftName=MainLabel.IsisCube.Instrument.SpacecraftName.Value -%}
{% else if exists("MainLabel.IsisCube.Mosaic.SpacecraftName.Value") -%}
{% set SpacecraftName=MainLabel.IsisCube.Mosaic.SpacecraftName.Value -%}
{% endif -%}

{% if SpacecraftName == "TRACE GAS ORBITER" -%}
{% set SpacecraftId="TGO" -%}
{% else -%}
{% set SpacecraftId=SpacecraftName -%}
{% endif -%}

{% if SpacecraftId -%}$ISISROOT/appdata/export/{{- SpacecraftId -}}{{- InstrumentId -}}.tpl{% endif -%}