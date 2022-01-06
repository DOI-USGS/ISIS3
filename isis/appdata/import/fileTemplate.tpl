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
{%- else if exists("QUBE.MISSION_NAME") -%}
{%- set SpacecraftName=QUBE.MISSION_NAME.Value -%}
{%- else if exists("MISSION_NAME") -%}
{%- set SpacecraftName=MISSION_NAME.Value -%}
{%- else if exists("INSTRUMENT_HOST_NAME") -%}
{%- set SpacecraftName=INSTRUMENT_HOST_NAME.Value -%}
{%- endif -%}

{%- if SpacecraftName == "TRACE GAS ORBITER" -%}
{%- set SpacecraftId="TGO" -%}
{%- else if SpacecraftName == "VIKING_ORBITER_1" or SpacecraftName == "VIKING_ORBITER_2" -%}
{%- set SpacecraftId="Viking" -%}
{%- set InstrumentId="VIS" -%}
{%- else if SpacecraftName == "MARS_RECONNAISSANCE_ORBITER" or SpacecraftName == "MARS_RECON_ORBITER" -%}
{%- set SpacecraftId="MRO" -%}
{%- else if SpacecraftName == "CASSINI-HUYGENS" -%}
{%- set SpacecraftId="Cassini" -%}
{%- if exists("QUBE") -%}
{%- set InstrumentId="VIMS" -%}
{%- else -%}
{%- set InstrumentId="ISS" -%}
{%- endif -%}
{%- else if SpacecraftName == "MARS_GLOBAL_SURVEYOR" -%}
{%- set SpacecraftId="MGS" -%}
{%- set InstrumentId="MOC" -%}
{%- else if SpacecraftName == "DAWN MISSION TO VESTA AND CERES" or SpacecraftName == "DAWN"-%}
{%- set SpacecraftId="Dawn" -%}
{%- else if SpacecraftName == "MARS EXPLORATION ROVER" -%}
{%- set SpacecraftId="Mer" -%}
{%- endif -%}

{%- if InstrumentId == "FC1" or InstrumentId == "FC2" -%}
{%- set InstrumentId="FC"-%}
{%- endif -%}

{%- if SpacecraftId -%}$ISISROOT/appdata/import/{{- SpacecraftId -}}{{- InstrumentId -}}.tpl{%- endif -%}
