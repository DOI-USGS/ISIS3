<div id="main">

<div id="content">

<div class="contextual">

</div>

<div class="wiki wiki-page">

<span id="Isis-Cue-Label"></span>

# Isis Cue Label [¶](#Isis-Cue-Label-)

    Object = IsisCube
      Object = Core
        StartByte   = 65537
        Format      = Tile
        TileSamples = 128
        TileLines   = 128
    
        Group = Dimensions
          Samples = 5000
          Lines   = 7168
          Bands   = 1
        End_Group
    
        Group = Pixels
          Type       = SignedWord
          ByteOrder  = Lsb
          Base       = 0.0
          Multiplier = 1.0
        End_Group
      End_Object
    
      Group = Instrument
        SpacecraftName        = Mars_Reconnaissance_Orbiter
        InstrumentId          = CTX
        TargetName            = Mars
        MissionPhaseName      = PSP
        StartTime             = 2006-11-19T04:01:27.976
        SpacecraftClockCount  = 0848376106:209
        OffsetModeId          = 196/187/185
        LineExposureDuration  = 1.877 <MSEC>
        FocalPlaneTemperature = 291.8 <K>
        SampleBitModeId       = SQROOT
        SpatialSumming        = 1
        SampleFirstPixel      = 0
      End_Group
    
      Group = Archive
        DataSetId           = MRO-M-CTX-2-EDR-L0-V1.0
        ProductId           = P01_001472_1747_XI_05S146W
        ProducerId          = MRO_CTX_TEAM
        ProductCreationTime = 2007-05-18T22:41:04
        OrbitNumber         = 1472
      End_Group
    
      Group = BandBin
        FilterName = BroadBand
        Center     = 0.65 <micrometers>
        Width      = 0.15 <micrometers>
      End_Group
    
      Group = Kernels
        NaifFrameCode       = -74021
        LeapSecond          = $base/kernels/lsk/naif0008.tls
        TargetAttitudeShape = $base/kernels/pck/pck00008.tpc
        TargetPosition      = Table
        InstrumentPointing  = Table
        Instrument          = Null
        SpacecraftClock     = $mro/kernels/sclk/MRO_SCLKSCET.00022.65536.tsc
        InstrumentPosition  = Table
        InstrumentAddendum  = $mro/kernels/iak/mroctxAddendum001.ti
        ShapeModel          = $base/dems/molaMarsPlanetaryRadius0002.cub
      End_Group
    End_Object

</div>

<div style="clear:both;">

</div>

</div>

</div>
