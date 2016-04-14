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
	$(LS) $(INPUT)/PSP*.cub > $(OUTPUT)/orthoInputList.txt;
	$(APPNAME) DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/DTM_Zumba_1m_forPDS.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers.txt \
	           > /dev/null;

	catlab from=$(OUTPUT)/DTEEC_002118_1510_003608_1510_A02.IMG \
	       to=$(OUTPUT)/DTEEC_002118_1510_003608_1510_A02.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_002118_1510_RED_C_01_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_002118_1510_RED_C_01_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_002118_1510_RED_A_02_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_002118_1510_RED_A_02_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_003608_1510_RED_C_03_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_003608_1510_RED_C_03_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_003608_1510_RED_A_04_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_003608_1510_RED_A_04_ORTHO.pvl \
	       > /dev/null;  
	
	pds2isis from=$(OUTPUT)/DTEEC_002118_1510_003608_1510_A02.IMG \
	         to=$(OUTPUT)/DTEEC_002118_1510_003608_1510_A02.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/PSP_002118_1510_RED_C_01_ORTHO.IMG \
	         to=$(OUTPUT)/PSP_002118_1510_RED_C_01_ORTHO.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/PSP_002118_1510_RED_A_02_ORTHO.IMG \
	         to=$(OUTPUT)/PSP_002118_1510_RED_A_02_ORTHO.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/PSP_003608_1510_RED_C_03_ORTHO.IMG \
	         to=$(OUTPUT)/PSP_003608_1510_RED_C_03_ORTHO.cub \
	         > /dev/null;  
	pds2isis from=$(OUTPUT)/PSP_003608_1510_RED_A_04_ORTHO.IMG \
	         to=$(OUTPUT)/PSP_003608_1510_RED_A_04_ORTHO.cub \
	         > /dev/null;  

	$(RM) $(OUTPUT)/DTEEC_002118_1510_003608_1510_A02.IMG;
	$(RM) $(OUTPUT)/PSP_002118_1510_RED_C_01_ORTHO.IMG;
	$(RM) $(OUTPUT)/PSP_002118_1510_RED_A_02_ORTHO.IMG;
	$(RM) $(OUTPUT)/PSP_003608_1510_RED_C_03_ORTHO.IMG;
	$(RM) $(OUTPUT)/PSP_003608_1510_RED_A_04_ORTHO.IMG;
	$(RM) $(OUTPUT)/orthoInputList.txt;

