{% if exists("Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name") -%}
{% set InstrumentId=Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name -%}
{% endif -%}

{% if exists("Product_Observational.Observation_Area.Investigation_Area.name") -%}
{% set SpacecraftName=Product_Observational.Observation_Area.Investigation_Area.name -%}
{% else if exists("Product_Observational.Observation_Area.Investigation_Area.Instrument_Host_Name") -%}
{% set SpacecraftName=Product_Observational.Observation_Area.Investigation_Area.Instrument_Host_Name -%}
{% endif -%}

{% if SpacecraftName == "TRACE GAS ORBITER" -%}
{% set SpacecraftId="TGO" -%}
{% endif -%}

{% if SpacecraftId -%}$ISISROOT/appdata/import/{{- SpacecraftId -}}{{- InstrumentId -}}.tpl{% endif -%}
