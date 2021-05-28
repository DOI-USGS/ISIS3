<?xml version="1.0" encoding="utf-8"?>
<?xml-model href="http://pds.nasa.gov/pds4/geom/v1/PDS4_GEOM_1B00_1610.sch" schematypens="http://purl.oclc.org/dsdl/schematron"?>
<?xml-model href="http://pds.nasa.gov/pds4/cart/v1/PDS4_CART_1900.sch" schematypens="http://purl.oclc.org/dsdl/schematron"?>
<?xml-model href="http://pds.nasa.gov/pds4/img/v1/PDS4_IMG_1A10_1510.sch" schematypens="http://purl.oclc.org/dsdl/schematron"?>
<?xml-model href="http://pds.nasa.gov/pds4/disp/v1/PDS4_DISP_1B00.sch" schematypens="http://purl.oclc.org/dsdl/schematron"?>
<?xml-model href="http://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1B00.sch" schematypens="http://purl.oclc.org/dsdl/schematron"?>
<Product_Observational xmlns:psa="http://psa.esa.int/psa/v1" xmlns:cas="http://psa.esa.int/psa/em16/cas/v1" xmlns:img="http://pds.nasa.gov/pds4/img/v1" xmlns="http://pds.nasa.gov/pds4/pds/v1" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://pds.nasa.gov/pds4/pds/v1 http://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1B00.xsd http://pds.nasa.gov/pds4/disp/v1 http://pds.nasa.gov/pds4/disp/v1/PDS4_DISP_1B00.xsd http://pds.nasa.gov/pds4/img/v1 http://pds.nasa.gov/pds4/img/v1/PDS4_IMG_1A10_1510.xsd http://pds.nasa.gov/pds4/cart/v1 http://pds.nasa.gov/pds4/cart/v1/PDS4_CART_1900.xsd http://psa.esa.int/psa/v1 http://psa.esa.int/psa/v1/PDS4_PSA_1000.xsd http://psa.esa.int/psa/em16/cas/v1 http://psa.esa.int/psa/em16/cas/v1/PDS4_PSA_EM16_CAS_1000.xsd http://pds.nasa.gov/pds4/geom/v1 http://pds.nasa.gov/pds4/geom/v1/PDS4_GEOM_1B00_1610.xsd" xmlns:disp="http://pds.nasa.gov/pds4/disp/v1" xmlns:cart="http://pds.nasa.gov/pds4/cart/v1" xmlns:geom="http://pds.nasa.gov/pds4/geom/v1">
{% if exists("MainLabel.IsisCube.Mosaic.ObservationId.Value") -%}
  {% set productId = MainLabel.IsisCube.Mosaic.ObservationId.Value -%}
  {% set sProductId = "MainLabel.IsisCube.Mosaic.ObservationId.Value" -%}
{% else if exists("MainLabel.IsisCube.Archive.ObservationId.Value") -%}
  {% set productId = MainLabel.IsisCube.Archive.ObservationId.Value -%}
  {% set sProductId = "MainLabel.IsisCube.Archive.ObservationId.Value" -%}
{% else -%}
  {% set sProductId = "None" -%}
{% endif -%}
{# Cassis Header is removed when converting back to PDS4 with tgocassisrdrgen
<CaSSIS_Header>
 <IDENTIFICATION_DATA/>
 <GEOMETRIC_DATA>
  <PREDICTED_MAXIMUM_EXPOSURE_TIME unit="{{MainLabel.IsisCube.Archive.PredictMaximumExposureTime.Units}}">{{MainLabel.IsisCube.Archive.PredictMaximumExposureTime.Value}}</PREDICTED_MAXIMUM_EXPOSURE_TIME>
  <CASSIS_OFF_NADIR_ANGLE unit="{{MainLabel.IsisCube.Archive.CassisOffNadirAngle.Units}}">{{MainLabel.IsisCube.Archive.CassisOffNadirAngle.Value}}</CASSIS_OFF_NADIR_ANGLE>
  <PREDICTED_REQUIRED_REPETITION_FREQUENCY unit="{{MainLabel.IsisCube.Archive.PredictedRepetitionFrequency.Units}}">{{MainLabel.IsisCube.Archive.PredictedRepetitionFrequency.Value}}</PREDICTED_REQUIRED_REPETITION_FREQUENCY>
  <TGO_GROUND_TRACK_VELOCITY unit="{{MainLabel.IsisCube.Archive.GroundTrackVelocity.Units}}">{{MainLabel.IsisCube.Archive.GroundTrackVelocity.Value}}</TGO_GROUND_TRACK_VELOCITY>
  <FORWARD_ROTATION_ANGLE_REQUIRED unit="{{MainLabel.IsisCube.Archive.ForwardRotationAngle.Units}}">{{MainLabel.IsisCube.Archive.ForwardRotationAngle.Value}}</FORWARD_ROTATION_ANGLE_REQUIRED>
  <SPICE_KERNEL_MISALIGNMENT_PREDICT unit="{{MainLabel.IsisCube.Archive.SpiceMisalignment.Units}}">{{MainLabel.IsisCube.Archive.SpiceMisalignment.Value}}</SPICE_KERNEL_MISALIGNMENT_PREDICT>
 </GEOMETRIC_DATA>
 <CaSSIS_General>
  <TELESCOPE_FOCAL_LENGTH unit="{{MainLabel.IsisCube.Archive.FocalLength.Units}}">{{MainLabel.IsisCube.Archive.FocalLength.Value}}</TELESCOPE_FOCAL_LENGTH>
  <TELESCOPE_F_NUMBER>{{MainLabel.IsisCube.Archive.FNumber.Value}}</TELESCOPE_F_NUMBER>
 </CaSSIS_General>
 <IMAGE_COMMAND/>
 <FSW_HEADER/>
 <PEHK_HEADER/>
 <DERIVED_HEADER_DATA>
   {% if exists("MainLabel.IsisCube.Archive.PixelsPossiblySaturated") -%}
  <PixelsPossiblySaturated>{{MainLabel.IsisCube.Archive.PixelsPossiblySaturated.Value}}</PixelsPossiblySaturated>
  {% endif -%}
  <Filter>{{MainLabel.IsisCube.BandBin.FilterName.Value}}</Filter>
 </DERIVED_HEADER_DATA>
</CaSSIS_Header> -#}
<Identification_Area>
  <logical_identifier>urn:esa:psa:em16_tgo_cas:{% if exists("MainLabel.IsisCube.Instrument") -%}
                                                  {% set targetGroup = MainLabel.IsisCube.Instrument -%}
                                                  {% set sTargetGroup = "MainLabel.IsisCube.Instrument" -%}
                                                  {% if exists("MainLabel.IsisCube.Mapping") -%}
                                                    data_projected:
                                                  {% else -%}
                                                    data_raw:
                                                  {% endif -%}
                                                {% else if exists("MainLabel.IsisCube.Mosaic") -%}
                                                  {% set targetGroup = MainLabel.IsisCube.Mosaic -%}
                                                  {% set sTargetGroup = "MainLabel.IsisCube.Mosaic" -%}
                                                  data_derived:
                                                {% else -%}
                                                  {% set sTargetGroup = "None" -%}
                                                  TBD:
                                                {% endif -%}
                                                {% if exists(sProductId) -%}{{lower(productId)-}}{% endif -%}
  </logical_identifier>
  <version_id>1.0</version_id>
  <title>PDS4 product exported from ISIS3 cube.</title>
  <information_model_version>1.8.0.0</information_model_version>
  <product_class>Product_Observational</product_class>
  <Alias_List>
   <Alias>
    <alternate_id>{% if exists(sProductId) -%}{{productId}}{% endif -%}</alternate_id>
    <comment>CaSSIS Internal Identifier</comment>
   </Alias>
  </Alias_List>
  <Modification_History>
  {# TODO - Loop through isis label History group? -#}
   <Modification_Detail>
    <modification_date>{{currentTime}}</modification_date>
    <version_id>1.0</version_id>
    <description>Created PDS4 output product from ISIS cube with the isisexport application from ISIS. </description>
   </Modification_Detail>
  </Modification_History>
 </Identification_Area>
 <Observation_Area>
  <Time_Coordinates>
   <start_date_time>{{targetGroup.StartTime.Value}}Z</start_date_time>
   {% if exists(sTargetGroup + ".StopTime") %}
   <stop_date_time>{{targetGroup.StopTime.Value}}Z</stop_date_time>
   {% else %}
   <stop_date_time>{{targetGroup.StartTime.Value}}Z</stop_date_time>
   {% endif %}
   {% if exists(sTargetGroup + ".LocalTime") %}
   <local_true_solar_time>{{targetGroup.LocalTime.Value}}</local_true_solar_time>
   {% endif %}
   {% if exists(sTargetGroup + ".SolarLongitude") %}
   <solar_longitude unit="deg">{{targetGroup.SolarLongitude.Value}}</solar_longitude>
   {% endif %}
  </Time_Coordinates>
  <Investigation_Area>
   <name>{{targetGroup.SpacecraftName.Value}}</name>
   <type>Mission</type>
   <Internal_Reference>
    <lid_reference>urn:esa:psa:context:investigation:mission.tgo</lid_reference>
    <reference_type>data_to_investigation</reference_type>
    <comment>This is the PDS4 logical identifier for the ExoMars Trace Gas Orbiter mission.</comment>
   </Internal_Reference>
  </Investigation_Area>
  <Observing_System>
   <name>{{targetGroup.SpacecraftName.Value}} {{targetGroup.InstrumentId.Value}}</name>
   <Observing_System_Component>
    <name>{{targetGroup.SpacecraftName.Value}}</name>
    <type>Spacecraft</type>
   </Observing_System_Component>
   <Observing_System_Component>
    <name>{{targetGroup.InstrumentId.Value}}</name>
    <type>Instrument</type>
   </Observing_System_Component>
  </Observing_System>
  <Target_Identification>
   <name>{{targetGroup.TargetName.Value}}</name>
   <type>Planet</type>
  </Target_Identification>
  {# TODO - seems like none of these values are consistently included in the Archive group. May require coordination with isisimport
  {% if exists("MainLabel.IsisCube.Archive") -%}
  <Mission_Area>
   <psa:Mission_Information>
    <psa:mission_phase_name>{{MainLabel.IsisCube.Archive.MissionPhase.Value}}</psa:mission_phase_name>
   </psa:Mission_Information>
   <psa:Sub-Instrument>
    <psa:identifier>SCI</psa:identifier>
    <psa:name>CASSIS Science</psa:name>
    <psa:type>Imager</psa:type>
   </psa:Sub-Instrument>
   <psa:Processing_Context>
    <psa:processing_software_title>Geometry_Pipeline</psa:processing_software_title>
    <psa:processing_software_version>1.0.1</psa:processing_software_version>
   </psa:Processing_Context>
   <psa:Observation_Context>
    <psa:instrument_pointing_mode>No pointing</psa:instrument_pointing_mode>
    <psa:instrument_pointing_description>No pointing</psa:instrument_pointing_description>
    <psa:observation_identifier>{{MainLabel.IsisCube.Archive.UID.Value}}</psa:observation_identifier>
   </psa:Observation_Context>
   <cas:CASSIS_Data>
    <cas:instrument_information>
     <cas:instrument_ifov>{{MainLabel.IsisCube.Archive.IFOV.Value}}</cas:instrument_ifov>
     <cas:instrument_ifov_unit>{{MainLabel.IsisCube.Archive.IFOVUnit.Value}}</cas:instrument_ifov_unit>
     <cas:filters_available>{{MainLabel.IsisCube.Archive.FiltersAvailable.Value}}</cas:filters_available>
    </cas:instrument_information>
    <cas:telescope_information>
     <cas:focal_length>{{MainLabel.IsisCube.Archive.FocalLength.Value}}</cas:focal_length>
     <cas:focal_length_unit>{{MainLabel.IsisCube.Archive.FocalLengthUnit.Value}}</cas:focal_length_unit>
     <cas:f_number>{{MainLabel.IsisCube.Archive.FNumber.Value}}</cas:f_number>
     <cas:telescope_type>{{MainLabel.IsisCube.Archive.TelescopeType.Value}}</cas:telescope_type>
    </cas:telescope_information>
    <cas:detector_information>
     <cas:description>{{MainLabel.IsisCube.Archive.TelescopeType.Value}}</cas:description>
     <cas:pixel_height>{{MainLabel.IsisCube.Archive.PixelHeight.Value}}</cas:pixel_height>
     <cas:pixel_height_unit>{{MainLabel.IsisCube.Archive.PixelHeightUnit.Value}}</cas:pixel_height_unit>
     <cas:pixel_width>{{MainLabel.IsisCube.Archive.PixelWidth.Value}}</cas:pixel_width>
     <cas:pixel_width_unit>{{MainLabel.IsisCube.Archive.PixelWidthUnit.Value}}</cas:pixel_width_unit>
     <cas:detector_type>{{MainLabel.IsisCube.Archive.DetectorType.Value}}</cas:detector_type>
     <cas:read_noise>{{MainLabel.IsisCube.Archive.ReadNoise.Value}}</cas:read_noise>
     <cas:read_noise_unit>{{MainLabel.IsisCube.Archive.ReadNoiseUnit.Value}}</cas:read_noise_unit>
    </cas:detector_information>
    <cas:geometry_information/>
   </cas:CASSIS_Data>
  </Mission_Area>
  {% endif -%}
  #}
  <Discipline_Area>
  <disp:Display_Settings>
  <Local_Internal_Reference>
    <local_identifier_reference>Array_3D_Image</local_identifier_reference>
    <local_reference_type>display_settings_to_array</local_reference_type>
  </Local_Internal_Reference>
  <disp:Display_Direction>
    <disp:horizontal_display_axis>Sample</disp:horizontal_display_axis>
    <disp:horizontal_display_direction>Left to Right</disp:horizontal_display_direction>
    <disp:vertical_display_axis>Line</disp:vertical_display_axis>
    <disp:vertical_display_direction>Top to Bottom</disp:vertical_display_direction>
  </disp:Display_Direction>
  </disp:Display_Settings>
  <img:Imaging>
    <img:Imaging_Instrument_Parameters>
     <img:exposure_duration unit="s">1.488e-003</img:exposure_duration>
    </img:Imaging_Instrument_Parameters>
    <Local_Internal_Reference>
     <local_identifier_reference>Image_Array_Object</local_identifier_reference>
     <local_reference_type>imaging_parameters_to_image_object</local_reference_type>
    </Local_Internal_Reference>
    <img:Image_Product_Information>
     <img:Filter>
      <img:filter_name>{{MainLabel.IsisCube.BandBin.FilterName.Value}}</img:filter_name>
      <img:filter_id>{{MainLabel.IsisCube.BandBin.FilterName.Value}}</img:filter_id>
      <img:bandwidth unit="{{MainLabel.IsisCube.BandBin.Width.Units}}">{{MainLabel.IsisCube.BandBin.Width.Value}}</img:bandwidth>
      {% if exists("MainLabel.IsisCube.BandBin.Venter") -%}
      <img:center_filter_wavelength unit="{{MainLabel.IsisCube.BandBin.Venter.Units}}">{{MainLabel.IsisCube.BandBin.Center.Value}}</img:center_filter_wavelength>
      {% endif -%}
     </img:Filter>
    </img:Image_Product_Information>
   </img:Imaging>
  {% if exists("MainLabel.IsisCube.Mapping") -%}
    {% include "mappingGroup.tpl" %}
  {% endif -%}
  {% if exists("MainLabel.IsisCube.Mosaic") -%}
  <geom:Geometry>
    <geom:Image_Display_Geometry>
     <geom:Display_Direction>
      <geom:horizontal_display_axis>Sample</geom:horizontal_display_axis>
      <geom:horizontal_display_direction>Left to Right</geom:horizontal_display_direction>
      <geom:vertical_display_axis>Line</geom:vertical_display_axis>
      <geom:vertical_display_direction>Top to Bottom</geom:vertical_display_direction>
     </geom:Display_Direction>
     <geom:Object_Orientation_North_East>
      <geom:north_azimuth unit="deg">{{MainLabel.IsisCube.Mosaic.NorthAzimuth.Value}}</geom:north_azimuth>
      <geom:east_azimuth unit="deg">0.0</geom:east_azimuth>
      <geom:Reference_Frame_Identification>
       <geom:name>J2000</geom:name>
      </geom:Reference_Frame_Identification>
     </geom:Object_Orientation_North_East>
    </geom:Image_Display_Geometry>
    <geom:Geometry_Orbiter>
     <geom:geometry_reference_time_utc>{{MainLabel.IsisCube.Mosaic.StartTime.Value}}Z</geom:geometry_reference_time_utc>
     <geom:Surface_Geometry>
      <geom:Surface_Geometry_Specific>
       <geom:subsolar_azimuth unit="deg">{{MainLabel.IsisCube.Mosaic.SubSolarAzimuth.Value}}</geom:subsolar_azimuth>
      </geom:Surface_Geometry_Specific>
     </geom:Surface_Geometry>
     <geom:Illumination_Geometry>
      <geom:Illumination_Specific>
       <geom:reference_pixel_location>Center</geom:reference_pixel_location>
       <geom:incidence_angle unit="deg">{{MainLabel.IsisCube.Mosaic.IncidenceAngle.Value}}</geom:incidence_angle>
       <geom:emission_angle unit="deg">{{MainLabel.IsisCube.Mosaic.EmissionAngle.Value}}</geom:emission_angle>
       <geom:phase_angle unit="deg">{{MainLabel.IsisCube.Mosaic.PhaseAngle.Value}}</geom:phase_angle>
      </geom:Illumination_Specific>
     </geom:Illumination_Geometry>
    </geom:Geometry_Orbiter>
    <geom:SPICE_Kernel_Files>
     <geom:SPICE_Kernel_Identification>
      <geom:spice_kernel_file_name>em16_cassis.tm</geom:spice_kernel_file_name>
     </geom:SPICE_Kernel_Identification>
    </geom:SPICE_Kernel_Files>
   </geom:Geometry>
  {% endif -%}
  </Discipline_Area>
 </Observation_Area>
 <File_Area_Observational>
  <File>
   <file_name>{{MainLabel.IsisCube.Archive.FileName.Value}}.img</file_name>
  </File>
  {# still need to include possible 3DArray
  <Array_3D_Image>
   <local_identifier>Array_3D_Image</local_identifier>
   <offset unit="byte">0</offset>
   <axes>3</axes>
   <axis_index_order>Last Index Fastest</axis_index_order>
   <Element_Array>
    <data_type>{{pixelType}}</data_type>
    <scaling_factor>1.0</scaling_factor>
    <value_offset>0.0</value_offset>
   </Element_Array>
   <Axis_Array>
    <axis_name>Band</axis_name>
    <elements>{{MainLabel.IsisCube.Core.Dimensions.Bands.Value}}</elements>
    <sequence_number>1</sequence_number>
   </Axis_Array>
   <Axis_Array>
    <axis_name>Line</axis_name>
    <elements>{{MainLabel.IsisCube.Core.Dimensions.Lines.Value}}</elements>
    <sequence_number>2</sequence_number>
   </Axis_Array>
   <Axis_Array>
    <axis_name>Sample</axis_name>
    <elements>{{MainLabel.IsisCube.Core.Dimensions.Samples.Value}}</elements>
    <sequence_number>3</sequence_number>
   </Axis_Array>
   <Special_Constants>
     <missing_constant>-3.40282265508890445e+38</missing_constant>
     <high_instrument_saturation>-3.40282326356119256e+38</high_instrument_saturation>
     <high_representation_saturation>-3.4028234663852886e+38</high_representation_saturation>
     <low_instrument_saturation>-3.40282306073709653e+38</low_instrument_saturation>
     <low_representation_saturation>-3.40282285791300049e+38</low_representation_saturation>
   </Special_Constants>
  </Array_3D_Image> #}
  <Array_2D_Image>
    <axes>2</axes>
    <Axis_Array>
      <axis_name>Sample</axis_name>
      <sequence_number>1</sequence_number>
      <elements>2048</elements>
    </Axis_Array>
    <Axis_Array>
      <axis_name>Line</axis_name>
      <sequence_number>2</sequence_number>
      <elements>256</elements>
    </Axis_Array>
    <Element_Array>
      <data_type>IEEE754LSBSingle</data_type>
      <idl_data_type>4</idl_data_type>
      <order>First_Index_Fastest</order>
      <unit>DN</unit>
      <scaling_factor> 1.00</scaling_factor>
      <offset> 0.00</offset>
    </Element_Array>
  </Array_2D_Image>
 </File_Area_Observational>
</Product_Observational>
