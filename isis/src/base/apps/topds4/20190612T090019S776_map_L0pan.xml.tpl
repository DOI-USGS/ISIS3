<?xml version="1.0" encoding="UTF-8"?>
<?xml-model href="https://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1700.sch" schematypens="http://purl.oclc.org/dsdl/schematron" ?>
<?xml-model href="https://pds.nasa.gov/pds4/disp/v1/PDS4_DISP_1700.sch" schematypens="http://purl.oclc.org/dsdl/schematron" ?>
<?xml-model href="https://pds.nasa.gov/pds4/geom/v1/PDS4_GEOM_1700_1401.sch" schematypens="http://purl.oclc.org/dsdl/schematron" ?>
<?xml-model href="https://pds.nasa.gov/pds4/orex/v1/orex_ldd_OREX_1300.sch" schematypens="http://purl.oclc.org/dsdl/schematron" ?>
<Product_Observational
    xmlns="http://pds.nasa.gov/pds4/pds/v1"
    xmlns:pds="http://pds.nasa.gov/pds4/pds/v1"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:disp="http://pds.nasa.gov/pds4/disp/v1"
    xmlns:geom="http://pds.nasa.gov/pds4/geom/v1"
    xmlns:orex="http://pds.nasa.gov/pds4/mission/orex/v1"
    xsi:schemaLocation=
    "http://pds.nasa.gov/pds4/pds/v1 https://pds.nasa.gov/pds4/pds/v1/PDS4_PDS_1700.xsd
    http://pds.nasa.gov/pds4/disp/v1 https://pds.nasa.gov/pds4/disp/v1/PDS4_DISP_1700.xsd
    http://pds.nasa.gov/pds4/geom/v1 https://pds.nasa.gov/pds4/geom/v1/PDS4_GEOM_1700_1401.xsd
    http://pds.nasa.gov/pds4/mission/orex/v1 https://pds.nasa.gov/pds4/mission/orex/v1/PDS4_OREX_1300.xsd">
    <File_Area_Observational>
        <File>
            <file_name>filename</file_name>
            <creation_date_time>dateandtime</creation_date_time>
            <file_size unit="byte">filesize</file_size>
        </File>
        <Header>
            <offset unit="byte">headeroffset</offset>
            <object_length unit="byte">headerLength</object_length>
            <parsing_standard_id>fileType</parsing_standard_id>
        </Header>
        <Array_2D_Image>
            <local_identifier>Active Area</local_identifier>
            <offset unit="byte">imageOffsetBytes</offset>
            <axes>numAxes</axes>
            <axis_index_order>Last Index Fastest</axis_index_order>
            <description>OCAMS image 1024 by 1024 pixel active array.</description>
            <Element_Array>
                <data_type>bitType</data_type>
                <unit>DN</unit>
                <scaling_factor>1</scaling_factor>
                <value_offset>WHAT IS THIS</value_offset>
            </Element_Array>
            <Axis_Array>
                <axis_name>Line</axis_name>
                <elements>{{MainLabel.IsisCube.Core.Dimensions.Lines.Value}}</elements>
                <sequence_number>1</sequence_number>
            </Axis_Array>
            <Axis_Array>
                <axis_name>Sample</axis_name>
                <elements>{{MainLabel.IsisCube.Core.Dimensions.Samples.Value}}</elements>
                <sequence_number>2</sequence_number>
            </Axis_Array>
        </Array_2D_Image>
    </File_Area_Observational>
</Product_Observational>
