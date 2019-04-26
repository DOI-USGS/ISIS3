< Thoughts: should we rename this as: Kaguya Terrain Camera development notes and have another page for the standard pipeline. Two different audiences >

# Corrected Values 

Some keys have an original and corrected version. The corrected version must be preferred over the original keys. 

* `CORRECTED_SC_CLOCK_START_COUNT` vs `SPACECRAFT_CLOCK_START_COUNT`
* `CORRECTED_SC_CLOCK_STOP_COUNT` vs `SPACECRAFT_CLOCK_STOP_COUNT`
* `CORRECTED_START_TIME` vs `START_TIME`
* `CORRECTED_STOP_TIME` vs `STOP_TIME`            
* `CORRECTED_SAMPLING_INTERVAL` vs `LINE_EXPOSURE_DURATION`

Using the non-corrected data will give you an incorrect camera model. For example, `LINE_EXPOSURE_DURATION` is, in some images, almost half of what `CORRECTED_SAMPLING_INTERVAL` evaluates to. The resulting computed image would be scaled incorrectly in the line direction. It is not known why there this discrepancy exists. 

## Image Start Time discrepancy 
`CORRECTED_SC_CLOCK_START_COUNT` and `CORRECTED_START_TIME` would in theory be the same value for a given image, but in practice differ on the order of hundredths of a second (0.001,) which corresponds to n lines. `CORRECTED_SC_CLOCK_START_COUNT` was selected to be used as the Start Time for the sensor model in both ISIS and ALE, based on a history of spacecraft clock counts being more accurate in other missions.

# Kaguya TC's many IKIDs

Kaguya seems to have many different IK's depending on combinations of operating modes (stereo vs mono operation, compression type, and swath mode). From Kaguya's IK kernel:

                                                     End
                                               Start Pixel
        Sensor                                 Pixel (+dummy)  NAIF ID
        -----------------------------------------------------------------
        LISM_TC1                                  1  4096      -131351
        LISM_TC2                                  1  4096      -131371
        LISM_TC1_WDF  (Double DCT Full)           1  4096      -131352
        LISM_TC1_WTF  (Double Through Full)       1  1600      -131353
        LISM_TC1_SDF  (Single DCT Full)           1  4096      -131354
        LISM_TC1_STF  (Single Through Full)       1  3208      -131355
        LISM_TC1_WDN  (Double DCT Nominal)      297  3796(+4)  -131356
        LISM_TC1_WTN  (Double Through Nominal)  297  1896      -131357
        LISM_TC1_SDN  (Single DCT Nominal)      297  3796(+4)  -131358
        LISM_TC1_STN  (Single Through Nominal)  297  3504      -131359
        LISM_TC1_WDH  (Double DCT Half)        1172  2921(+2)  -131360
        LISM_TC1_WTH  (Double Through Half)    1172  2771      -131361
        LISM_TC1_SDH  (Single DCT Half)        1172  2921(+2)  -131362
        LISM_TC1_STH  (Single Through Half)    1172  2923      -131363
        LISM_TC1_SSH  (Single SP_support Half) 1172  2921      -131364

        LISM_TC2_WDF  (Double DCT Full)           1  4096      -131372
        LISM_TC2_WTF  (Double Through Full)       1  1600      -131373
        LISM_TC2_SDF  (Single DCT Full)           1  4096      -131374
        LISM_TC2_STF  (Single Through Full)       1  3208      -131375
        LISM_TC2_WDN  (Double DCT Nominal)      297  3796(+4)  -131376
        LISM_TC2_WTN  (Double Through Nominal)  297  1896      -131377
        LISM_TC2_SDN  (Single DCT Nominal)      297  3796(+4)  -131378
        LISM_TC2_STN  (Single Through Nominal)  297  3504      -131379
        LISM_TC2_WDH  (Double DCT Half)        1172  2921(+2)  -131380
        LISM_TC2_WTH  (Double Through Half)    1172  2771      -131381
        LISM_TC2_SDH  (Single DCT Half)        1172  2921(+2)  -131382
        LISM_TC2_STH  (Single Through Half)    1172  2923      -131383
        LISM_TC2_SSH  (Single SP_support Half) 1172  2921      -131384

Single means only one camera was enabled, double means both TC1 and TC2 were enabled. All images are decompressed so compression mode is a non-issue. Nominal, Full and Half refer to the different swath modes. According to the Jaxa team, the modes do not really impact the camera model with the exception of the swath mode which determines the starting sample. For many SPICE `gdpool` calls (focal lengths, pixel size, CCD center, distortion coefficients), the `LISM_TC1` and `LISM_TC2` IKIDs should be used as these do not vary with the different operating modes, including swath mode. 

# Kaguya TC's Distortion Model

The equation to be used for the Kaguya TC distortion model and associated coefficients were located in the instrument kernel `SEL_TC_V01.TI`:

```
Distortion information
---------------------------------------------------------------------------
  Line-of-sight vector of pixel no. n can be expressed as below.

   Distortion coefficients information:
     INS<INSTID>_DISTORTION_COEF_X  = ( a0, a1, a2, a3)
     INS<INSTID>_DISTORTION_COEF_Y  = ( b0, b1, b2, b3),

   Distance r from the center:
     r = - (n - INS<INSTID>_CENTER) * INS<INSTID>_PIXEL_SIZE.

   Line-of-sight vector v is calculated as
     v[X] = INS<INSTID>BORESIGHT[X]
            +a0 +a1*r +a2*r^2 +a3*r^3 ,
     v[Y] = INS<INSTID>BORESIGHT[Y]
            +r +a0 +a1*r +a2*r^2 +a3*r^3 ,
     v[Z] = INS<INSTID>BORESIGHT[Z] .


   \begindata

      INS-131351_DISTORTION_COEF_X  = (
                                      -9.6499e-4,  9.8441e-4,
                                       8.5773e-6, -3.7438e-6
                                      )
      INS-131351_DISTORTION_COEF_Y  = (
                                      -1.3796e-3,  1.3502e-5,
                                       2.7251e-6, -6.1938e-6
                                      )
      INS-131371_DISTORTION_COEF_X  = (
                                       2.9786e-3, 7.7836e-5,
                                       3.9265e-6, -4.4088e-6
                                      )
      INS-131371_DISTORTION_COEF_Y  = (
                                       9.2410e-4, -1.1994e-4,
                                       2.9281e-5, -3.7239e-7
                                      )

```

There were a couple of what appear to be transcription errors in distortion equations from the IK above, so a slightly edited IK,`SEL_TC_V02.TI`, was created as part of this work. The actual distortion equations used are: 

```
   Line-of-sight vector v is calculated as
     v[X] = INS<INSTID>BORESIGHT[X]
            +a0 +a1*r +a2*r^2 +a3*r^3 ,
     v[Y] = INS<INSTID>BORESIGHT[Y]
            +b0 +b1*r +b2*r^2 +b3*r^3 ,
     v[Z] = INS<INSTID>BORESIGHT[Z] .
```

The same distortion equation is used in both the ISIS and ALE/CSM sensor models.

# Discrepancies in Archived Data 

There is a difference between the `.sl2` files stored under `/work/projects` and images obtained from the Jaxa's product search (https://darts.isas.jaxa.jp/planet/pdap/selene/product_search.html#). Images in Jaxa's product search use slightly different labels (e.g. Line exposure duration is a scalar in the `.sl2` files and a one element list in the Jaxa product search labels). The product search images also have detached labels compared to the `.sl2` files which have attached labels. This basically means we need to use slightly different processes depending on where the image originated from. 
< Do either/both CSM or ISIS handle both?? >

# Processing Kaguya TC images in ISIS
<Insert what we know about the file naming conventions "S" vs "W" what N and E are...>

The archive files we have available locally are in:
    /work/projects/jaxa0[1234]
NOTE: These files are not in the same format as those downloaded from the JAXA site.

< Kelvin and Lisa may be able to add more about this >

The example Kaguya TC file used in the examples below was pulled from:
````/work/projects/jaxa01/TC_w_Level2B0/01/2008/12/07/TC1W2B0_01_05188N259E0020.sl2````

Each observation is stored in a "tar" file with the extension "sl2" instead of "tar". The tar contains the following files:
* TC1W2B0_01_05188N259E0020.igz - This is a GNU zipped image file with an attached PDS3 like PVL label. Traditionally this would have an extension of "gz". Linux gunzip complains about the "igz" extension. The file can be renamed with an extension of "gz, and then unzipped using gunzip. Once unzipped, the ISIS3 PVL and Import classes have been able to parse these labels and image data. NOTE: Comments below about flipping and mirroring do NOT apply to this image data.
* TC1W2B0_01_05188N259E0020.jpg - This is a browse image of the observation possibly flipped and mirrored to put north up.
* TC1W2B0_01_05188N259E0020.ctg - Catalog file containing basic information about the observation.
* TC1W2B0_01_05188N259E0020.lbl - The label file for the observation (i.e., not just the image)


The standard workflow for processing Kaguya TC images in ISIS is as follows:
* Un-tar and decompress the data set:
```
tar xvf TCxxxxx_xx_xxxxxNxxxExxxx.sl2
mv TCxxxxx_xx_xxxxxNxxxExxxx.igz TCxxxxx_xx_xxxxxNxxxExxxx.gz
gunzip TCxxxxx_xx_xxxxxNxxxExxxx.gz
```
* Convert the image to an ISIS3 cube

```kaguyatc2isis from=TCxxxxx_xx_xxxxxNxxxExxxx.lbl to=TCxxxxx_xx_xxxxxNxxxExxxx.cub```

* Add SPICE data to the cube. By default spiceinit uses only reconstructed CK and SPK SPICE data. The ISIS3 data area for kaguya contains a smithed CK file from Goddard: SEL_MAIN_GRGM900C_L270_DIRALT_2019-02-13_TYPE13.bsp. This SPK contains updated spacecraft position information for the extended part of the mission (2009). 

To use the default SPK data:

```spiceinit fr=TCxxxxx_xx_xxxxxNxxxExxxx.cub```

To use the updated SPK data for observations covered by the times ranges in the smithed SPK:

```spiceinit fr=TCxxxxx_xx_xxxxxNxxxExxxx.cub spksmithed=TRUE```

NOTE: The updated Goddard SPK covers the following times:

```
    Time = ("2008 DEC 27 05:31:05.183794 TDB",
            "2009 JAN 01 04:01:05.183936 TDB")
    Time = ("2009 JAN 01 04:01:06.183936 TDB",
            "2009 FEB 20 20:01:06.185240 TDB")
    Time = ("2009 FEB 20 23:46:06.185243 TDB",
            "2009 MAR 19 18:01:06.185602 TDB")
    Time = ("2009 MAR 19 22:01:06.185603 TDB",
            "2009 APR 16 19:01:06.185615 TDB")
    Time = ("2009 APR 16 21:31:06.185614 TDB",
            "2009 JUN 08 12:31:06.184717 TDB")
    Time = ("2009 JUN 08 12:37:36.184717 TDB",
            "2009 JUN 10 19:31:06.184658 TDB")

```

Project the data to a map projections:

```cam2map from=TCxxxxx_xx_xxxxxNxxxExxxx.cub to=TCxxxxx_xx_xxxxxNxxxExxxx_Proj.cub map=Some_Map_Projection_Template pixres=mpp res=X```
