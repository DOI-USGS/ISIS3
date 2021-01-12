# Please note, the input data was reduced from its original size, in doing so
# the Scale and PixelResolution was changed, for the purpose of testing it
# needed to be the original values, so, the values were edited. This means
# the values for those two keywords are incorrect.
APPNAME = hidtmgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/PSP*.cub > $(OUTPUT)/orthoInputList.txt;
	echo "1" >  $(OUTPUT)/sequence.txt;
	echo "2" >> $(OUTPUT)/sequence.txt;
	echo "3" >> $(OUTPUT)/sequence.txt;
	echo "4" >> $(OUTPUT)/sequence.txt;
	$(APPNAME) OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Polar_Crater_1_1m_ngate_edited2_forPDS.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           ORTHOSEQUENCE=$(OUTPUT)/sequence.txt \
	           PARAMSPVL=$(INPUT)/parameters.pvl \
	           > /dev/null;
	
	catlab from=$(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.IMG \
	       to=$(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_009404_2635_RED_C_1_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_009404_2635_RED_C_1_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_009404_2635_RED_A_2_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_009404_2635_RED_A_2_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_010221_2635_RED_C_3_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_010221_2635_RED_C_3_ORTHO.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/PSP_010221_2635_RED_A_4_ORTHO.IMG \
	       to=$(OUTPUT)/PSP_010221_2635_RED_A_4_ORTHO.pvl \
	       > /dev/null;  
	
	pds2isis from=$(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.IMG \
	         to=$(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.cub \
	         > /dev/null;  
	pds2isis from=$(OUTPUT)/PSP_009404_2635_RED_C_1_ORTHO.IMG \
	         to=$(OUTPUT)/PSP_009404_2635_RED_C_1_ORTHO.cub \
	         > /dev/null;  
	pds2isis from=$(OUTPUT)/PSP_009404_2635_RED_A_2_ORTHO.IMG \
	         to=$(OUTPUT)/PSP_009404_2635_RED_A_2_ORTHO.cub \
	         > /dev/null;  
	pds2isis from=$(OUTPUT)/PSP_010221_2635_RED_C_3_ORTHO.IMG \
	         to=$(OUTPUT)/PSP_010221_2635_RED_C_3_ORTHO.cub \
	         > /dev/null;  
	pds2isis from=$(OUTPUT)/PSP_010221_2635_RED_A_4_ORTHO.IMG \
	         to=$(OUTPUT)/PSP_010221_2635_RED_A_4_ORTHO.cub \
	         > /dev/null;  
	
	$(RM) $(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.IMG;
	$(RM) $(OUTPUT)/PSP_009404_2635_RED_C_1_ORTHO.IMG;
	$(RM) $(OUTPUT)/PSP_009404_2635_RED_A_2_ORTHO.IMG;
	$(RM) $(OUTPUT)/PSP_010221_2635_RED_C_3_ORTHO.IMG;
	$(RM) $(OUTPUT)/PSP_010221_2635_RED_A_4_ORTHO.IMG;
	$(RM) $(OUTPUT)/orthoInputList.txt;
	$(RM) $(OUTPUT)/sequence.txt;

