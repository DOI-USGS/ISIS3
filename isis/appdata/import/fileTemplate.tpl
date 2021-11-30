{%- if exists("Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name") -%}
{%- set InstrumentId=Product_Observational.Observation_Area.Observing_System.Observing_System_Component.1.name -%}
{%- else if exists("INSTRUMENT_ID.Value") -%}
{%- set InstrumentId=INSTRUMENT_ID.Value -%}
{%- endif -%}

{%- if exists("Product_Observational.Observation_Area.Investigation_Area.name") -%}
{%- set SpacecraftName=Product_Observational.Observation_Area.Investigation_Area.name -%}
{%- else if exists("Product_Observational.Observation_Area.Investigation_Area.Instrument_Host_Name") -%}
{%- set SpacecraftName=Product_Observational.Observation_Area.Investigation_Area.Instrument_Host_Name -%}
{%- else if exists("SPACECRAFT_NAME") -%}
{%- set SpacecraftName=SPACECRAFT_NAME.Value -%}
{%- endif -%}

{%- if SpacecraftName == "TRACE GAS ORBITER" -%}
{%- set SpacecraftId="TGO" -%}
{%- else if SpacecraftName == "VIKING_ORBITER_1" or SpacecraftName == "VIKING_ORBITER_2" -%}
{%- set SpacecraftId="Viking" -%}
{%- set InstrumentId="VIS" -%}
{%- else if SpacecraftName == "MARS_RECONNAISSANCE_ORBITER" or SpacecraftName == "MARS_RECON_ORBITER" -%}
{%- set SpacecraftId="MRO" -%}
{%- endif -%}

{%- if SpacecraftId -%}$ISISROOT/appdata/import/{{- SpacecraftId -}}{{- InstrumentId -}}.tpl{%- endif -%}
