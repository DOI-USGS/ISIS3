# This test will print errors thrown by the hicrop application.
#
#   "Unable to crop the given cube [" + inputFileName using the hicrop program."
#   TEST A: 
#     if (instId.UpCase() != "HIRISE")
#         "Input cube has invalid InstrumentId. A HiRise image is required."
#   TEST B: 
#     if(firstValidTime >= lastValidTime)
#         "Unable to calculate start and end lines. The start ET value  is 
#          greater than or equal to the stop ET value."
#   TEST C: 
#     if (cropStartTime < ckCoverage.first )
#         "Invalid start/stop times. These times fall outside of CK file time 
#          coverage.
#   TEST D: 
#     if (cropStopTime > ckCoverage.second)
#         "Invalid start/stop times. These times fall outside of CK file time 
#          coverage.
#   TEST E: 
#     if (numberOfBodies != 1)
#         "Unable to find start and stop times using the given CK file. This 
#          application only works with single body CK files."
#   TEST F: 
#     if (numberOfIntervals != 1)
#         "Unable to find start and stop times using the given CK file. This 
#          application only works with single interval body CK files."
#   TEST G: 
#     if(cropEndLine < cropStartLine)
#         "Calculated Start/End Lines are invalid. End line must be greater than
#          or equal to start line."
.IGNORE:
APPNAME = hicrop

include $(ISISROOT)/make/isismake.tsts

commands:
#   TEST A: 
	echo -e "Error Test A:" > $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(INPUT)/LRONAC_M139722912RE_cropped.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519_hijitterMaster5.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped.cub \
	source=ck 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST B: given start time is >= given stop time 
	echo -e "\nError Test B:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519_hijitterMaster5.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped.cub \
	source=time \
    start=264289117.1 \
    stop=264289117.0 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST C: given start time is before ck start time
	echo -e "\nError Test C:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519_hijitterMaster5.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped.cub \
	source=time \
    start=0.0 \
    stop=264289117.2 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST D: given stop time is after ck stop time
	echo -e "\nError Test D:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519_hijitterMaster5.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped.cub \
	source=time \
    start=264289110.0 \
    stop=300000000.0 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST E: 
#   TEST F: 
#   TEST G: 
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/error_temp.txt;
