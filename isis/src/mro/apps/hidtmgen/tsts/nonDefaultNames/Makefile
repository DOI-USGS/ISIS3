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
	$(LS) $(INPUT)/PSP*.cub > $(OUTPUT)/orthoInputList.txt;
	echo $(OUTPUT)/ORTHO1.IMG > $(OUTPUT)/orthoOutputList.txt;
	echo $(OUTPUT)/ORTHO2.IMG >> $(OUTPUT)/orthoOutputList.txt;
	echo $(OUTPUT)/ORTHO3.IMG >> $(OUTPUT)/orthoOutputList.txt;
	echo $(OUTPUT)/ORTHO4.IMG >> $(OUTPUT)/orthoOutputList.txt;
	$(APPNAME) DEFAULTNAMES=FALSE \
	           DTM=$(INPUT)/DTM_Zumba_1m_forPDS.cub \
	           DTMTO=$(OUTPUT)/DTM.IMG \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           ORTHOTOLIST=$(OUTPUT)/orthoOutputList.txt \
	           ORTHOPRODUCTIDLIST=$(INPUT)/orthoOutputProductIds.lis \
	           PARAMSPVL=$(INPUT)/parameters.pvl \
	           DTM_PRODUCT_ID=DtmProduct \
	           > /dev/null;
	
	catlab from=$(OUTPUT)/DTM.IMG \
	       to=$(OUTPUT)/DTM.pvl \
	       > /dev/null;
	catlab from=$(OUTPUT)/ORTHO1.IMG \
	       to=$(OUTPUT)/ORTHO1.pvl \
	       > /dev/null;
	catlab from=$(OUTPUT)/ORTHO2.IMG \
	       to=$(OUTPUT)/ORTHO2.pvl \
	       > /dev/null;
	catlab from=$(OUTPUT)/ORTHO3.IMG \
	       to=$(OUTPUT)/ORTHO3.pvl \
	       > /dev/null;
	catlab from=$(OUTPUT)/ORTHO4.IMG \
	       to=$(OUTPUT)/ORTHO4.pvl \
	       > /dev/null;
	
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
	
	$(RM) $(OUTPUT)/DTM.IMG;
	$(RM) $(OUTPUT)/ORTHO1.IMG;
	$(RM) $(OUTPUT)/ORTHO2.IMG;
	$(RM) $(OUTPUT)/ORTHO3.IMG;
	$(RM) $(OUTPUT)/ORTHO4.IMG;
	$(RM) $(OUTPUT)/orthoInputList.txt;
	$(RM) $(OUTPUT)/orthoOutputList.txt;

