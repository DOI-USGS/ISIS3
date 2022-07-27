<?xml version="1.0" encoding="UTF-8"?>

<?xml-model href="http://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1C00.sch"
    schematypens="http://purl.oclc.org/dsdl/schematron"?>
<?xml-model href="http://pds.nasa.gov/pds4/disp/v1/PDS4_DISP_1B00.sch"
    schemetypens="http://purl.oclc.org/dsdl/schematron"?>
<?xml-model href="http://pds.nasa.gov/pds4/img/v1/PDS4_IMG_1D00_1730.sch"
    schemetypens="http://purl.oclc.org/dsdl/schematron"?>
<?xml-model 
	  href="./PDS4_LRO_1D00_1000.sch" 
	    schematypens="http://purl.oclc.org/dsdl/schematron"?>

<Product_Observational
    xmlns="http://pds.nasa.gov/pds4/pds/v1"
    xmlns:pds="http://pds.nasa.gov/pds4/pds/v1"
    xmlns:lro="http://pds.nasa.gov/pds4/mission/lro/v1"
    xmlns:disp="http://pds.nasa.gov/pds4/disp/v1"
    xmlns:img="http://pds.nasa.gov/pds4/img/v1" 
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    
    xsi:schemaLocation="http://pds.nasa.gov/pds4/pds/v1 http://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1C00.xsd
                        http://pds.nasa.gov/pds4/disp/v1 http://pds.nasa.gov/pds4/disp/v1/PDS4_DISP_1B00.xsd
                        http://pds.nasa.gov/pds4/mission/lro/v1 ./PDS4_LRO_1D00_1000.xsd
                        http://pds.nasa.gov/pds4/img/v1 http://pds.nasa.gov/pds4/img/v1/PDS4_IMG_1D00_1730.xsd">

    <Identification_Area>
        <logical_identifier>{{ default(ExtraXml.LrocNacEdrIsis.ProductObservational.IdentificationArea.LogicalIdentifier, "TBD NO VALUE AVAILABLE") -}}</logical_identifier>
        <version_id>1.0</version_id>
        <title>LUNAR RECONNAISSANCE ORBITER CAMERA NAC EDR product</title>
        <information_model_version>1.12.0.0</information_model_version>
        <product_class>Product_Observational</product_class>
        <Alias_List>
            <Alias>
                <alternate_id>{% if exists("MainLabel.IsisCube.Archive.ProductId.Value") -%}{{MainLabel.IsisCube.Archive.ProductId.Value}}{% endif -%}</alternate_id>
                <comment>Original filename at PDS3 archive, on-line at LROC Node</comment>
            </Alias>
            <Alias>
                <alternate_id>{% if exists("MainLabel.IsisCube.Archive.OriginalProductId.Value") -%}{{MainLabel.IsisCube.Archive.OriginalProductId.Value}}{% endif -%}</alternate_id>
                <comment>PDS3 SOURCE_PRODUCT_ID</comment>
            </Alias>
        </Alias_List>
        <Modification_History>
            <Modification_Detail>
                <modification_date>{{currentTime}}</modification_date>
                <version_id>1.0</version_id>
                <description>
                    This PDS4 labeled (detached label) product (orig PDS3 image) is a result
                    of develpment activity for eventual LUNAR RECONNAISSANCE ORBITER CAMERA NAC EDR migration to PDS4.
                </description>
            </Modification_Detail>
        </Modification_History>
    </Identification_Area>

    <Observation_Area>
        <Time_Coordinates>
            <start_date_time>{{MainLabel.IsisCube.Instrument.StartTime.Value}}Z</start_date_time>
            <stop_date_time>{{MainLabel.IsisCube.Instrument.StopTime.Value}}Z</stop_date_time>
        </Time_Coordinates>
        <Primary_Result_Summary>
            <purpose>Science</purpose>
            <processing_level>Partially Processed</processing_level>
            <description>The Lunar Reconnaissance Orbiter Camera (LROC) Wide Angle Camera (WAC) and Narrow Angle Cameras (NACs) 
                are on the NASA Lunar Reconnaissance Orbiter (LRO). The WAC is a 7-color push-frame camera (100 and 400 m/pixel 
                visible and UV, respectively), while the two NACs are monochrome narrow-angle linescan imagers (0.5 m/pixel). 
                The primary mission of LRO is to obtain measurements of the Moon that will enable future lunar human exploration.
                The overarching goals of the LROC investigation include landing site identification and certification, mapping 
                of permanently polar shadowed and sunlit regions, meter-scale mapping of polar regions, global multispectral 
                imaging, a global morphology base map, characterization of regolith properties, and determination of current 
                impact hazards.
            </description>
        </Primary_Result_Summary>
        <Investigation_Area>
            <name>Product_Observational</name>
            <type>Mission</type>
            <Internal_Reference>
                <lid_reference>{{ default(ExtraXml.LrocNacEdrIsis.ProductObservational.ObservationArea.InvestigationArea.InternalReference.LidReference, "TBD NO VALUE AVAILABLE") -}}</lid_reference>
                <reference_type>data_to_investigation</reference_type>
                <comment>This is the PDS4 logical identifier for LRO</comment>
            </Internal_Reference>
        </Investigation_Area>
        <Observing_System>
            <Observing_System_Component>
                <name>LUNAR RECONNAISSANCE ORBITER CAMERA</name>
                <type>Instrument</type>
                <Internal_Reference>
                    <lid_reference>{{default(ExtraXml.LrocNacEdrIsis.ProductObservational.ObservationArea.ObservingSystem.ObservingSystemComponent.InternalReference.LidReference, "TBD NO VALUE AVAILABLE") -}}</lid_reference>
                    <reference_type>is_instrument</reference_type>
                </Internal_Reference>
            </Observing_System_Component>
        </Observing_System>
        <Target_Identification>
            <name>{{MainLabel.IsisCube.Instrument.TargetName.Value}}</name>
            <type>Satellite</type>
            <Internal_Reference>
                <lid_reference>{{default(ExtraXml.LrocNacEdrIsis.ProductObservational.ObservationArea.TargetIdentification.InternalReference.LidReference, "TBD NO VALUE AVAILABLE") -}}</lid_reference>
                <reference_type>data_to_target</reference_type>
            </Internal_Reference>
        </Target_Identification>

        <Mission_Area>
            <lro:LRO_Parameters>
                <!-- parameters for all LRO products -->
                <lro:product_type>CDR</lro:product_type>
                <lro:product_version_id>{{default(MainLabel.IsisCube.Archive.ProductVersionId.Value, "TBD NO VALUE AVAILABLE") -}}</lro:product_version_id> 
                <lro:orbit_number>{{default(MainLabel.IsisCube.Archive.OrbitNumber.Value, "TBD NO VALUE AVAILABLE") -}}</lro:orbit_number>
                <lro:mission_phase_name>{{default(MainLabel.IsisCube.Instrument.MissionPhaseName.Value, "TBD NO VALUE AVAILABLE") -}}</lro:mission_phase_name>
                <lro:spacecraft_clock_start_count>{{default(MainLabel.IsisCube.Instrument.SpacecraftClockStartCount.Value, "TBD NO VALUE AVAILABLE") -}}</lro:spacecraft_clock_start_count>
                <lro:spacecraft_clock_stop_count>{{default(MainLabel.IsisCube.Instrument.SpacecraftClockStopCount.Value, "TBD NO VALUE AVAILABLE") -}}</lro:spacecraft_clock_stop_count>
                <lro:producer_institution_name>Arizona State University</lro:producer_institution_name>
                
                <lro:LROC_Parameters>
                    <!-- Parameters specific to LROC -->
                    <lro:upload_id>{{default(OriginalLabel.UPLOAD_ID.Value, "TBD NO VALUE AVAILABLE") -}}</lro:upload_id>
                    <lro:rationale_desc>{{default(MainLabel.IsisCube.Archive.RationaleDescription.Value, "TBD NO VALUE AVAILABLE") -}}</lro:rationale_desc>
                    <lro:data_quality_id>{{default(MainLabel.IsisCube.Archive.DataQualityId.Value, "TBD NO VALUE AVAILABLE") -}}</lro:data_quality_id>
                    <lro:data_quality_desc>
                        {{default(OriginalLabel.DATA_QUALITY_DESC.Value, "TBD NO VALUE AVAILABLE") -}}
                    </lro:data_quality_desc>
                    
                    <!-- NAC specific attributes -->
                    <lro:preroll_time>{{default(MainLabel.IsisCube.Instrument.PrerollTime.Value, "TBD NO VALUE AVAILABLE") -}}Z</lro:preroll_time>
                    <lro:spacecraft_clock_preroll_count>{{default(MainLabel.IsisCube.Instrument.SpacecraftClockPrerollCount.Value, "TBD NO VALUE AVAILABLE") -}}</lro:spacecraft_clock_preroll_count>
                    <lro:frame_id>{{default(MainLabel.IsisCube.Instrument.FrameId.Value, "TBD NO VALUE AVAILABLE") -}}</lro:frame_id>
                    
                    <!-- NAC environment attributes -->
                    <lro:temperature_scs unit="{{default(MainLabel.IsisCube.Instrument.TemperatureSCS.Units, "TBD NO VALUE AVAILABLE") -}}">{{default(MainLabel.IsisCube.Instrument.TemperatureSCS.Value, "TBD NO VALUE AVAILABLE") -}}</lro:temperature_scs> 
                    <lro:temperature_fpa unit="{{default(MainLabel.IsisCube.Instrument.TemperatureFPA.Units, "TBD NO VALUE AVAILABLE") -}}">{{default(MainLabel.IsisCube.Instrument.TemperatureFPA.Value, "TBD NO VALUE AVAILABLE") -}}</lro:temperature_fpa> 
                    <lro:temperature_fpga unit="{{default(MainLabel.IsisCube.Instrument.TemperatureFPGA.Units, "TBD NO VALUE AVAILABLE") -}}">{{default(MainLabel.IsisCube.Instrument.TemperatureFPGA.Value, "TBD NO VALUE AVAILABLE") -}}</lro:temperature_fpga> 
                    <lro:temperature_telescope unit="{{default(MainLabel.IsisCube.Instrument.TemperatureTelescope.Units, "TBD NO VALUE AVAILABLE") -}}">{{default(MainLabel.IsisCube.Instrument.TemperatureTelescope.Value, "TBD NO VALUE AVAILABLE") -}}</lro:temperature_telescope>
                    <lro:temperature_scs_raw>{{default(MainLabel.IsisCube.Instrument.TemperatureSCSRaw.Value, "TBD NO VALUE AVAILABLE") -}}</lro:temperature_scs_raw>
                    <lro:temperature_fpa_raw>{{default(MainLabel.IsisCube.Instrument.TemperatureFPARaw.Value, "TBD NO VALUE AVAILABLE") -}}</lro:temperature_fpa_raw>
                    <lro:temperature_fpga_raw>{{default(MainLabel.IsisCube.Instrument.TemperatureFPGARaw.Value, "TBD NO VALUE AVAILABLE") -}}</lro:temperature_fpga_raw>
                    <lro:temperature_telescope_raw>{{default(MainLabel.IsisCube.Instrument.TemperatureTelescopeRaw.Value, "TBD NO VALUE AVAILABLE") -}}</lro:temperature_telescope_raw>
                    
                    <!-- NAC imaging attributes -->
                    <lro:line_exposure_duration unit="{{default(MainLabel.IsisCube.Instrument.LineExposureDuration.Units, "TBD NO VALUE AVAILABLE") -}}">{{default(MainLabel.IsisCube.Instrument.LineExposureDuration.Value, "TBD NO VALUE AVAILABLE") -}}</lro:line_exposure_duration>
                    <lro:line_exposure_code>{{default(OriginalLabel.LRO_LINE_EXPOSURE_CODE.Value, "TBD NO VALUE AVAILABLE") -}}</lro:line_exposure_code>
                    <lro:dac_reset_level>{{default(OriginalLabel.LRO_DAC_RESET_LEVEL.Value, "TBD NO VALUE AVAILABLE") -}}</lro:dac_reset_level>
                    <lro:channel_a_offset>{{default(OriginalLabel.LRO_CHANNEL_A_OFFSET.Value, "TBD NO VALUE AVAILABLE") -}}</lro:channel_a_offset>
                    <lro:channel_b_offset>{{default(OriginalLabel.LRO_CHANNEL_B_OFFSET.Value, "TBD NO VALUE AVAILABLE") -}}</lro:channel_b_offset>
                    <lro:compand_code>{{default(OriginalLabel.LRO_COMPAND_CODE.Value, "TBD NO VALUE AVAILABLE") -}}</lro:compand_code>
                    <lro:line_code>{{default(OriginalLabel.LRO_LINE_CODE.Value, "TBD NO VALUE AVAILABLE") -}}</lro:line_code>
                    <lro:bterm1>{{default(OriginalLabel.LRO_BTERM.Value.0, "TBD NO VALUE AVAILABLE") -}}</lro:bterm1>
                    <lro:bterm2>{{default(OriginalLabel.LRO_BTERM.Value.1, "TBD NO VALUE AVAILABLE") -}}</lro:bterm2>
                    <lro:bterm3>{{default(OriginalLabel.LRO_BTERM.Value.2, "TBD NO VALUE AVAILABLE") -}}</lro:bterm3>
                    <lro:bterm4>{{default(OriginalLabel.LRO_BTERM.Value.3, "TBD NO VALUE AVAILABLE") -}}</lro:bterm4>
                    <lro:bterm5>{{default(OriginalLabel.LRO_BTERM.Value.4, "TBD NO VALUE AVAILABLE") -}}</lro:bterm5>
                    <lro:mterm1>{{default(OriginalLabel.LRO_MTERM.Value.0, "TBD NO VALUE AVAILABLE") -}}</lro:mterm1>
                    <lro:mterm2>{{default(OriginalLabel.LRO_MTERM.Value.1, "TBD NO VALUE AVAILABLE") -}}</lro:mterm2>
                    <lro:mterm3>{{default(OriginalLabel.LRO_MTERM.Value.2, "TBD NO VALUE AVAILABLE") -}}</lro:mterm3>
                    <lro:mterm4>{{default(OriginalLabel.LRO_MTERM.Value.3, "TBD NO VALUE AVAILABLE") -}}</lro:mterm4>
                    <lro:mterm5>{{default(OriginalLabel.LRO_MTERM.Value.4, "TBD NO VALUE AVAILABLE") -}}</lro:mterm5>
                    <lro:xterm1>{{default(OriginalLabel.LRO_XTERM.Value.0, "TBD NO VALUE AVAILABLE") -}}</lro:xterm1>
                    <lro:xterm2>{{default(OriginalLabel.LRO_XTERM.Value.1, "TBD NO VALUE AVAILABLE") -}}</lro:xterm2>
                    <lro:xterm3>{{default(OriginalLabel.LRO_XTERM.Value.2, "TBD NO VALUE AVAILABLE") -}}</lro:xterm3>
                    <lro:xterm4>{{default(OriginalLabel.LRO_XTERM.Value.3, "TBD NO VALUE AVAILABLE") -}}</lro:xterm4>
                    <lro:xterm5>{{default(OriginalLabel.LRO_XTERM.Value.4, "TBD NO VALUE AVAILABLE") -}}</lro:xterm5>
                    <lro:compression_flag>{{default(OriginalLabel.LRO_COMPRESSION_FLAG.Value, "TBD NO VALUE AVAILABLE") -}}</lro:compression_flag>
                    <lro:mode>{{default(OriginalLabel.LRO_MODE.Value, "TBD NO VALUE AVAILABLE") -}}</lro:mode>
                    
                    
                </lro:LROC_Parameters>
            </lro:LRO_Parameters>
        </Mission_Area>
        
        <Discipline_Area>

            <img:Imaging>
                <Local_Internal_Reference>
                    <local_identifier_reference>DATA_FILE</local_identifier_reference>
                    <local_reference_type>imaging_parameters_to_image_object</local_reference_type>
                </Local_Internal_Reference>
                <img:Optical_Filter>
                    <img:bandwidth unit="{{default(MainLabel.IsisCube.BandBin.Width.Units, "TBD NO VALUE AVAILABLE") -}}">{{default(MainLabel.IsisCube.BandBin.Width.Value, "TBD NO VALUE AVAILABLE") -}}</img:bandwidth>
                    <img:center_filter_wavelength unit="{{default(MainLabel.IsisCube.BandBin.Center.Units, "TBD NO VALUE AVAILABLE") -}}">{{default(MainLabel.IsisCube.BandBin.Center.Value, "TBD NO VALUE AVAILABLE") -}}</img:center_filter_wavelength>
                </img:Optical_Filter>
                <img:Sampling>
                    <img:crosstrack_summing>{{default(MainLabel.IsisCube.Instrument.SpatialSumming.Value, "TBD NO VALUE AVAILABLE") -}}</img:crosstrack_summing>
                </img:Sampling>
            </img:Imaging>  
            
            <disp:Display_Settings>
                <Local_Internal_Reference>
                    <local_identifier_reference>Array_2D_Image</local_identifier_reference>
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
    <Reference_List>
        <External_Reference>
            <doi>10.1007/s11214-010-9634-2</doi>
            <reference_text>Robinson, M. S.; Brylow, S. M.; Tschimmel, M.; Humm, D.; Lawrence, S. J.; Thomas, P. C.; 
                Denevi, B. W.; Bowman-Cisneros, E.; Zerr, J.; Ravine, M. A.; Caplinger, M. A.; Ghaemi, F. T.; Schaffner, J. A.; 
                Malin, M. C.; Mahanti, P.; Bartels, A.; Anderson, J.; Tran, T. N.; Eliason, E. M.; McEwen, A. S. Turtle, E.; 
                Jolliff, B. L.; Hiesinger, H., 2010, "Lunar Reconnaissance Orbiter Camera (LROC) Instrument Overview", Space 
                Science Reviews, Volume 150, Issue 1-4, pp. 81-124
            </reference_text>
            <description>instrument overview</description>
        </External_Reference>
    </Reference_List>
    <File_Area_Observational>
        <File>
            <file_name>{{imageFileName}}</file_name>
            <local_identifier>DATA_FILE</local_identifier>
            <creation_date_time>{{currentTime}}Z</creation_date_time>
            <file_size unit="byte">{{outputFileSize}}</file_size>
            <md5_checksum>{{md5Hash}}</md5_checksum>
            <comment>LUNAR RECONNAISSANCE ORBITER CAMERA NAC EDR product.</comment>
        </File>
        <Header>
            <offset unit="byte">0</offset>
            <object_length unit="byte">{{default(MainLabel.Label.Bytes.Value, "TBD NO VALUE AVAILABLE") -}}</object_length>
            <parsing_standard_id>ISIS</parsing_standard_id>
            <description>ISIS attached header</description>
        </Header>
        <Array_2D_Image>
            <local_identifier>Array_2D_Image</local_identifier>
            <offset unit="byte">{{default(MainLabel.IsisCube.Core.StartByte.Value - 1, "TBD NO VALUE AVAILABLE") -}}</offset>
            <axes>2</axes>
            <axis_index_order>Last Index Fastest</axis_index_order>
            <Element_Array>
                <data_type>{{default(MainLabel.IsisCube.Core.Pixels.Type.Value, "TBD NO VALUE AVAILABLE") -}}</data_type>
                <unit>RAW_INSTRUMENT_COUNT</unit>
            </Element_Array>
                <Axis_Array>
                <axis_name>Line</axis_name>
                <elements>{{default(MainLabel.IsisCube.Core.Dimensions.Lines.Value, "TBD NO VALUE AVAILABLE") -}}</elements>
                <sequence_number>1</sequence_number>
            </Axis_Array>
            <Axis_Array>
                <axis_name>Sample</axis_name>
                <elements>{{default(MainLabel.IsisCube.Core.Dimensions.Samples.Value, "TBD NO VALUE AVAILABLE") -}}</elements>
                <sequence_number>2</sequence_number>
            </Axis_Array>
        </Array_2D_Image>
    </File_Area_Observational>

</Product_Observational>