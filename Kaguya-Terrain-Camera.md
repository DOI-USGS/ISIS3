

# Corrected Values 

Some keys have an original and corrected version. The corrected version must be preferred over the original keys. 

* `CORRECTED_SC_CLOCK_START_COUNT` vs `SPACECRAFT_CLOCK_START_COUNT`
* `CORRECTED_SC_CLOCK_STOP_COUNT` vs `SPACECRAFT_CLOCK_STOP_COUNT`
* `CORRECTED_START_TIME` vs `START_TIME`
* `CORRECTED_STOP_TIME` vs `STOP_TIME`            
* `CORRECTED_SAMPLING_INTERVAL` vs `LINE_EXPOSURE_DURATION`

Using the non-corrected data will give you an incorrect camera model. For example, `LINE_EXPOSURE_DURATION` is, in some images, almost half of what `CORRECTED_SAMPLING_INTERVAL` evaluates to. The resulting computed image would be scaled incorrectly in the line direction. It is not known why there this discrepancy exists. 

# Kaguya TC's many IKIDs

Kaguya seems to have many different IK's depending on combinations of operating modes (stereo vs mono operation, compression type, and swath mode). From Kaguya IK kernel:

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

Single means only one camera was enabled, double means both TC1 and TC2 were enabled. All images are decompressed so compression mode is a non-issue. Nominal, Full and Half refer to the different swatch modes. According to the Jaxa team, the modes do not really impact the camera model with the exception of the swatch mode which determines the starting sample. For many `gdpool` calls (focal lengths, pixel size, CCD center, distortion coefficients), the `LISM_TC1` and `LISM_TC2` IKIDs should be used as these do not vary with the different operating modes, including swatch mode. 

# Discrepancies in Archived Data 

There is a difference between the `.sl2` files stored under `/work/projects` and images obtained from the Jaxa's product search (https://darts.isas.jaxa.jp/planet/pdap/selene/product_search.html#). Images in Jaxa's product search use slightly different labels (e.g. Line exposure duration is a scalar in the `.sl2` files and a one element list in the Jaxa product search labels). The product search images also have detached labels compared to the `.sl2` files which have attached labels. This basically means we need to use slightly different processes depending on where the image originated from. 
