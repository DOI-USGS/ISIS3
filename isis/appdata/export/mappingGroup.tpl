<cart:Cartography>
<Local_Internal_Reference>
  <local_identifier_reference>Image_Array_Object</local_identifier_reference>
  <local_reference_type>cartography_parameters_to_image_object</local_reference_type>
</Local_Internal_Reference>
<cart:Spatial_Domain>
  <cart:Bounding_Coordinates>
  <cart:west_bounding_coordinate unit="deg">{{MainLabel.IsisCube.Mapping.MinimumLongitude.Value}}</cart:west_bounding_coordinate>
  <cart:east_bounding_coordinate unit="deg">{{MainLabel.IsisCube.Mapping.MaximumLongitude.Value}}</cart:east_bounding_coordinate>
  <cart:north_bounding_coordinate unit="deg">{{MainLabel.IsisCube.Mapping.MaximumLatitude.Value}}</cart:north_bounding_coordinate>
  <cart:south_bounding_coordinate unit="deg">{{MainLabel.IsisCube.Mapping.MinimumLatitude.Value}}</cart:south_bounding_coordinate>
  </cart:Bounding_Coordinates>
</cart:Spatial_Domain>
<cart:Spatial_Reference_Information>
  <cart:Horizontal_Coordinate_System_Definition>
  <cart:Planar>
      <cart:Map_Projection>
        <cart:map_projection_name>{{MainLabel.IsisCube.Mapping.ProjectionName.Value}}</cart:map_projection_name>
        <cart:{{MainLabel.IsisCube.Mapping.ProjectionName.Value}}>
        </cart:{{MainLabel.IsisCube.Mapping.ProjectionName.Value}}>
      </cart:Map_Projection>
    <cart:Planar_Coordinate_Information>
    <cart:planar_coordinate_encoding_method>Coordinate Pair</cart:planar_coordinate_encoding_method>
    <cart:Coordinate_Representation>
      <cart:pixel_resolution_x unit="m/pixel">{{MainLabel.IsisCube.Mapping.PixelResolution.Value}}</cart:pixel_resolution_x>
      <cart:pixel_resolution_y unit="m/pixel">{{MainLabel.IsisCube.Mapping.PixelResolution.Value}}</cart:pixel_resolution_y>
      <cart:pixel_scale_x unit="pixel/deg">{{MainLabel.IsisCube.Mapping.Scale.Value}}</cart:pixel_scale_x>
      <cart:pixel_scale_y unit="pixel/deg">{MainLabel.IsisCube.Mapping.Scale.Value}}</cart:pixel_scale_y>
    </cart:Coordinate_Representation>
    </cart:Planar_Coordinate_Information>
    <cart:Geo_Transformation>
    <cart:upperleft_corner_x unit="m">{{MainLabel.IsisCube.Mapping.UpperLeftCornerX.Value}}</cart:upperleft_corner_x>
    <cart:upperleft_corner_y unit="m">{{MainLabel.IsisCube.Mapping.UpperLeftCornerY.Value}}</cart:upperleft_corner_y>
    </cart:Geo_Transformation>
  </cart:Planar>
  <cart:Geodetic_Model>
    <cart:latitude_type>{{MainLabel.IsisCube.Mapping.LatitudeType.Value}}</cart:latitude_type>
    <cart:semi_major_radius unit="m">{{MainLabel.IsisCube.Mapping.EquatorialRadius.Value}}</cart:semi_major_radius>
    <cart:semi_minor_radius unit="m">{{MainLabel.IsisCube.Mapping.EquatorialRadius.Value}}</cart:semi_minor_radius>
    <cart:polar_radius unit="m">{{MainLabel.IsisCube.Mapping.PolarRadius.Value}}</cart:polar_radius>
    <cart:longitude_direction>{{MainLabel.IsisCube.Mapping.LongitudeDirection.Value}}</cart:longitude_direction>
  </cart:Geodetic_Model>
  </cart:Horizontal_Coordinate_System_Definition>
</cart:Spatial_Reference_Information>
</cart:Cartography>