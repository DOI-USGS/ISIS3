# Please note, the input data was reduced from its original size, in doing so
# the Scale and PixelResolution was changed, for the purpose of testing it
# needed to be the original values, so, the values were edited. This means
# the values for those two keywords are incorrect.
#
# This test is testing an equitorial projection, of primary importance is the
# labels and details such as quoting on keywords, the standard for the output
# is very specific.

APPNAME = hidtmgen

include $(ISISROOT)/make/isismake.tsts

commands:
	# no input data
	echo -e "Error Test: no DTM or orthos given" > $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers1.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# empty ortho given
	echo -e "\nError Test: given ORTHOFROMLIST is empty" >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(INPUT)/empty.lis \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers1.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(LS) $(INPUT)/ESP*cub > $(OUTPUT)/orthoInputList.txt;

	# given DTM is not a DTM
	echo -e "\nError Test: given DTM is not a DTM" >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/ESP_042252_1930_3-BAND_COLOR_2m_o_croppedForTest.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers1.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# given DTM is not a supported projection
	echo -e "\nError Test: given DTM is not not a supported projection" \
	  >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/molaMarsPlanetaryRadius0005.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers1.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# given sequence list size doesn't match ortho list size
	echo -e "\nError Test: given sequence list size doesn't match ortho list size" \
	  >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers2.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# given ortho to list size doesn't match ortho from list size
	echo -e "\nError Test: given ortho to list size doesn't match from list size" \
	  >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=FALSE \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           DTM_PRODUCT_ID=xyz \
	           DTMTO=$(OUTPUT)/xyz.IMG \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOTOLIST=$(INPUT)/sequenceNumbers2.txt \
	           ORTHOPRODUCTIDLIST=$(INPUT)/sequenceNumbers1.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# given ortho product id list size doesn't match ortho from list size
	echo -e "\nError Test: given ortho product ID list size doesn't match from list size" \
	  >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=FALSE \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           DTM_PRODUCT_ID=xyz \
	           DTMTO=$(OUTPUT)/xyz.IMG \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOTOLIST=$(INPUT)/sequenceNumbers1.txt \
	           ORTHOPRODUCTIDLIST=$(INPUT)/sequenceNumbers2.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# given PARAMSPVL has invalid producing institution
	echo -e "\nError Test: given PARAMSPVL has invalid producing institution" \
	  >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/invalidProducingInstitution.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers1.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# given PARAMSPVL has invalid version
	echo -e "\nError Test: given PARAMSPVL has invalid version" \
	  >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/invalidVersionId.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers1.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# given ORTHOFROMLIST cube has invalid band size
	echo -e "\nError Test: given DTM has invalid band size" \
	  >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/DTM_2Bands.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers1.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	# given ORTHOFROMLIST cube has invalid band size
	$(LS) $(INPUT)/2Band*cub >> $(OUTPUT)/orthoInputList.txt;
	echo -e "\nError Test: given ORTHOFROMLIST cube has invalid band size" \
	  >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	           DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers2.txt \
	           2>> $(OUTPUT)/error_temp.txt \
	           > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;
	$(RM) $(OUTPUT)/orthoInputList.txt;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_D_09_ORTHO.IMG;
	$(RM) $(OUTPUT)/DTEED_042252_1930_042753_1930_A04.IMG;
	$(RM) $(OUTPUT)/xyz.IMG;
