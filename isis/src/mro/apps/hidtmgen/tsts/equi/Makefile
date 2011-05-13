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
	$(APPNAME) DTM=$(INPUT)/DTM_Zumba_1m_forPDS.cub \
	  ORTHO1=$(INPUT)/PSP_002118_1510_1m_o_forPDS.cub \
	  ORTHO2=$(INPUT)/PSP_002118_1510_25cm_o_forPDS.cub \
  	  ORTHO3=$(INPUT)/PSP_003608_1510_1m_o_forPDS.cub \
	  ORTHO4=$(INPUT)/PSP_003608_1510_25cm_o_forPDS.cub \
	  OUTPUTDIR=$(OUTPUT) \
	  PRODUCT_VERSION_ID=1.0 \
	  RATIONALE_DESC=Rationale \
	  > /dev/null;

	head -n 93 $(OUTPUT)/DTEEC_002118_1510_003608_1510_A01.IMG \
	  > $(OUTPUT)/DTEEC_002118_1510_003608_1510_A01.pvl;
	head -n 92 $(OUTPUT)/DT1EA_002118_1510_003608_1510_A01.IMG \
	  > $(OUTPUT)/DT1EA_002118_1510_003608_1510_A01.pvl;
	head -n 92 $(OUTPUT)/DT1EC_002118_1510_003608_1510_A01.IMG \
	  > $(OUTPUT)/DT1EC_002118_1510_003608_1510_A01.pvl;
	head -n 92 $(OUTPUT)/DT2EA_002118_1510_003608_1510_A01.IMG \
	  > $(OUTPUT)/DT2EA_002118_1510_003608_1510_A01.pvl;
	head -n 92 $(OUTPUT)/DT2EC_002118_1510_003608_1510_A01.IMG \
	  > $(OUTPUT)/DT2EC_002118_1510_003608_1510_A01.pvl;
	
	pds2isis from=$(OUTPUT)/DTEEC_002118_1510_003608_1510_A01.IMG \
	  to=$(OUTPUT)/DTEEC_002118_1510_003608_1510_A01.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/DT1EA_002118_1510_003608_1510_A01.IMG \
	  to=$(OUTPUT)/DT1EA_002118_1510_003608_1510_A01.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/DT1EC_002118_1510_003608_1510_A01.IMG \
	  to=$(OUTPUT)/DT1EC_002118_1510_003608_1510_A01.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/DT2EA_002118_1510_003608_1510_A01.IMG \
	  to=$(OUTPUT)/DT2EA_002118_1510_003608_1510_A01.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/DT2EC_002118_1510_003608_1510_A01.IMG \
	  to=$(OUTPUT)/DT2EC_002118_1510_003608_1510_A01.cub \
	  > /dev/null;  

	rm $(OUTPUT)/DTEEC_002118_1510_003608_1510_A01.IMG;
	rm $(OUTPUT)/DT1EA_002118_1510_003608_1510_A01.IMG;
	rm $(OUTPUT)/DT1EC_002118_1510_003608_1510_A01.IMG;
	rm $(OUTPUT)/DT2EA_002118_1510_003608_1510_A01.IMG;
	rm $(OUTPUT)/DT2EC_002118_1510_003608_1510_A01.IMG;

