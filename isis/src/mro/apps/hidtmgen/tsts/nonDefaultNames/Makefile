# Please note, the input data was reduced from its original size, in doing so
# the Scale and PixelResolution was changed, for the purpose of testing it
# needed to be the original values, so, the values were edited. This means
# the values for those two keywords are incorrect.
#
# This test is meant to test the functionality of the application withregards
# to non standard names. The labels are really the important part, details
# such as quoting.

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
	  DEFAULTNAMES=FALSE \
	  DTMTO=$(OUTPUT)/DTM.IMG \
	  ORTHO1TO=$(OUTPUT)/ORTHO1.IMG \
	  ORTHO2TO=$(OUTPUT)/ORTHO2.IMG \
	  ORTHO3TO=$(OUTPUT)/ORTHO3.IMG \
	  ORTHO4TO=$(OUTPUT)/ORTHO4.IMG \
	  DTM_PRODUCT_ID=DtmProduct \
	  ORTHO1_PRODUCT_ID=Ortho1Product \
	  ORTHO2_PRODUCT_ID=Ortho2Product \
	  ORTHO3_PRODUCT_ID=Ortho3Product \
	  ORTHO4_PRODUCT_ID=Ortho4Product \
	  SOURCE_PRODUCT_ID=SourceId \
	  > /dev/null;
	
	head -n 93 $(OUTPUT)/DTM.IMG \
	  > $(OUTPUT)/DTM.pvl;
	head -n 92 $(OUTPUT)/ORTHO1.IMG \
	  > $(OUTPUT)/ORTHO1.pvl;
	head -n 92 $(OUTPUT)/ORTHO2.IMG \
	  > $(OUTPUT)/ORTHO2.pvl;
	head -n 92 $(OUTPUT)/ORTHO3.IMG \
	  > $(OUTPUT)/ORTHO3.pvl;
	head -n 92 $(OUTPUT)/ORTHO4.IMG \
	  > $(OUTPUT)/ORTHO4.pvl;
	
	pds2isis from=$(OUTPUT)/DTM.IMG \
	  to=$(OUTPUT)/DTM.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/ORTHO1.IMG \
	  to=$(OUTPUT)/ORTHO1.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/ORTHO2.IMG \
	  to=$(OUTPUT)/ORTHO2.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/ORTHO3.IMG \
	  to=$(OUTPUT)/ORTHO3.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/ORTHO4.IMG \
	  to=$(OUTPUT)/ORTHO4.cub \
	  > /dev/null;  
	
	rm $(OUTPUT)/DTM.IMG;
	rm $(OUTPUT)/ORTHO1.IMG;
	rm $(OUTPUT)/ORTHO2.IMG;
	rm $(OUTPUT)/ORTHO3.IMG;
	rm $(OUTPUT)/ORTHO4.IMG;

