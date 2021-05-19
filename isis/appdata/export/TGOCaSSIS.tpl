<?xml version="1.0" encoding="utf-8"?>
<?xml-model href="http://pds.nasa.gov/pds4/disp/v1/PDS4_DISP_1700.sch" schemetypens="http://purl.oclc.org/dsdl/schematron"?>
<?xml-model href="http://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1800.sch" schemetypens="http://purl.oclc.org/dsdl/schematron"?>
<Product_Observational>
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
  <FSW_HEADER>
  <PEHK_HEADER>
  <DERIVED_HEADER_DATA>
   <PixelsPossiblySaturated>{{MainLabel.IsisCube.Archive.PixelsPossiblySaturated.Value}}</PixelsPossiblySaturated>
   <Filter>{{MainLabel.IsisCube.BandBin.FilterName.Value}}</Filter>
  </DERIVED_HEADER_DATA>
 </CaSSIS_Header>
 <Identification_Area>
  <logical_identifier>urn:nasa:pds:TBD:TBD:TBD</logical_identifier>
  <version_id>1.0</version_id>
  <title>PDS4 product exported from ISIS3 cube.</title>
  <information_model_version>1.8.0.0</information_model_version>
  <product_class>Product_Observational</product_class>
  <Modification_History>
   <Modification_Detail>
  </Modification_History>
 </Identification_Area>
 <Observation_Area>
  <Time_Coordinates>
   <start_date_time>{{MainLabel.IsisCube.Instrument.StartTime.Value}}</start_date_time>
   {% if exists("MainLabel.IsisCube.Instrument.StopTime") %}
   <stop_date_time>{{MainLabel.IsisCube.Instrument.StopTime.Value}}Z</stop_date_time>
   {% else %}
   <stop_date_time>{{MainLabel.IsisCube.Instrument.StartTime.Value}}Z</stop_date_time>
   {% endif %}
  </Time_Coordinates>
  <Investigation_Area>
   <name>{{MainLabel.IsisCube.Instrument.SpacecraftName.Value}}</name>
   <type>Mission</type>
   <Internal_Reference>
    <lid_reference>urn:nasa:pds:TBD</lid_reference>
    <reference_type>data_to_investigation</reference_type>
   </Internal_Reference>
  </Investigation_Area>
  <Observing_System>
   <name>{{MainLabel.IsisCube.Instrument.SpacecraftName.Value}} {{MainLabel.IsisCube.Instrument.InstrumentId.Value}}</name>
   <Observing_System_Component>
    <name>{{MainLabel.IsisCube.Instrument.SpacecraftName.Value}}</name>
    <type>Spacecraft</type>
   </Observing_System_Component>
   <Observing_System_Component>
    <name>{{MainLabel.IsisCube.Instrument.InstrumentId.Value}}</name>
    <type>Instrument</type>
   </Observing_System_Component>
  </Observing_System>
  <Target_Identification>
   <name>{{MainLabel.IsisCube.Instrument.TargetName.Value}}</name>
   <type>{{GeneratedJson.targetType}}</type>
  </Target_Identification>
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
  </Discipline_Area>
 </Observation_Area>
 <File_Area_Observational>
  <File>
   <file_name>{{MainLabel.IsisCube.Archive.FileName.Value}}.img</file_name>
  </File>
  <Array_3D_Image>
   <local_identifier>Array_3D_Image</local_identifier>
   <offset unit="byte">0</offset>
   <axes>3</axes>
   <axis_index_order>Last Index Fastest</axis_index_order>
   <Element_Array>
    <data_type>{{GeneratedJson.pixelType}}</data_type>
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
  </Array_3D_Image>
 </File_Area_Observational>
</Product_Observational>

