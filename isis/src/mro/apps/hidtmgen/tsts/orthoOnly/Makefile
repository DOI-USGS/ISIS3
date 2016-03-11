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
	ls $(INPUT)/*.cub > $(OUTPUT)/orthoFromList.txt;
	$(APPNAME) DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           ORTHOFROMLIST=$(OUTPUT)/orthoFromList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers.txt \
	           > /dev/null;

	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_B_23_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_B_23_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_D_01_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_D_01_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042753_1930_IRB_B_67_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042753_1930_IRB_B_67_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042753_1930_IRB_D_45_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042753_1930_IRB_D_45_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_009404_2635_RED_A_98_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_009404_2635_RED_A_98_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_009404_2635_RED_C_89_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_009404_2635_RED_C_89_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_010221_2635_RED_A_54_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_010221_2635_RED_A_54_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_010221_2635_RED_C_76_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_010221_2635_RED_C_76_ORTHO.pvl \
	       > /dev/null;  
	
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_B_23_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_D_01_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042753_1930_IRB_B_67_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042753_1930_IRB_D_45_ORTHO.IMG;
	$(RM) $(OUTPUT)/PSP_009404_2635_RED_A_98_ORTHO.IMG
	$(RM) $(OUTPUT)/PSP_009404_2635_RED_C_89_ORTHO.IMG
	$(RM) $(OUTPUT)/PSP_010221_2635_RED_A_54_ORTHO.IMG
	$(RM) $(OUTPUT)/PSP_010221_2635_RED_C_76_ORTHO.IMG
	$(RM) $(OUTPUT)/orthoFromList.txt;

