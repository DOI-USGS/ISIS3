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
	ls $(INPUT)/ESP*.cub > $(OUTPUT)/orthoInputList.txt;
	$(APPNAME) DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers.txt \
	           > /dev/null;

	catlab from=$(OUTPUT)/DTEED_042252_1930_042753_1930_A01.IMG \
	       to=$(OUTPUT)/DTEED_042252_1930_042753_1930_A01.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042753_1930_IRB_B_26_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042753_1930_IRB_B_26_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042753_1930_IRB_D_59_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042753_1930_IRB_D_59_ORTHO.pvl \
	       > /dev/null;  
	
	pds2isis from=$(OUTPUT)/DTEED_042252_1930_042753_1930_A01.IMG \
	         to=$(OUTPUT)/DTEED_042252_1930_042753_1930_A01.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042753_1930_IRB_B_26_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042753_1930_IRB_B_26_ORTHO.cub \
	         > /dev/null;  
	pds2isis from=$(OUTPUT)/ESP_042753_1930_IRB_D_59_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042753_1930_IRB_D_59_ORTHO.cub \
	         > /dev/null;  
	
	$(RM) $(OUTPUT)/DTEED_042252_1930_042753_1930_A01.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042753_1930_IRB_B_26_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042753_1930_IRB_D_59_ORTHO.IMG;
	$(RM) $(OUTPUT)/orthoInputList.txt;

