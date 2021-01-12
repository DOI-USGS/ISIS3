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
	$(LS) $(INPUT)/ESP*.cub > $(OUTPUT)/orthoInputList.txt;
	$(APPNAME) DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers.txt \
	           ENDIAN=MSB \
	           NULL=FALSE \
	           LIS=TRUE \
	           LRS=TRUE \
	           HIS=TRUE \
	           HRS=TRUE \
	           DTMBITTYPE=8BIT \
	           ORTHOBITTYPE=32BIT \
	           > /dev/null;

	catlab from=$(OUTPUT)/DTEED_042252_1930_042753_1930_A31.IMG \
	       to=$(OUTPUT)/DTEED_042252_1930_042753_1930_A31_MSB_8BIT.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO_MSB_32BIT.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO_MSB_32BIT.pvl \
	       > /dev/null;  
	
	pds2isis from=$(OUTPUT)/DTEED_042252_1930_042753_1930_A31.IMG \
	         to=$(OUTPUT)/DTEED_042252_1930_042753_1930_A31_MSB_8BIT.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO_MSB_32BIT.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO_MSB_32BIT.cub \
	         > /dev/null;
	
	$(RM) $(OUTPUT)/DTEED_042252_1930_042753_1930_A31.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG;

	$(APPNAME) DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params2.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers.txt \
	           DTMBITTYPE=U16BIT \
	           ORTHOBITTYPE=S16BIT \
	           NULL=TRUE \
	           LIS=TRUE \
	           LRS=TRUE \
	           HIS=TRUE \
	           HRS=TRUE \
	           > /dev/null;

	catlab from=$(OUTPUT)/DTEED_042252_1930_042753_1930_A07.IMG \
	       to=$(OUTPUT)/DTEED_042252_1930_042753_1930_A07_U16BIT.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO_S16BIT.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO_S16BIT.pvl \
	       > /dev/null;  
	
	pds2isis from=$(OUTPUT)/DTEED_042252_1930_042753_1930_A07.IMG \
	         to=$(OUTPUT)/DTEED_042252_1930_042753_1930_A07_U16BIT.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO_S16BIT.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO_S16BIT.cub \
	         > /dev/null;
	
	$(RM) $(OUTPUT)/DTEED_042252_1930_042753_1930_A07.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG;

	$(APPNAME) DEFAULTNAMES=TRUE \
	           OUTPUTDIR=$(OUTPUT) \
	           DTM=$(INPUT)/Ares4_Marth_Crater_3557E_126N_ngate_03.cub \
	           ORTHOFROMLIST=$(OUTPUT)/orthoInputList.txt \
	           PARAMSPVL=$(INPUT)/params2.pvl \
	           ORTHOSEQUENCENUMBERLIST=$(INPUT)/sequenceNumbers.txt \
	           DTMBITTYPE=S16BIT \
	           ORTHOBITTYPE=U16BIT \
	           NULL=FALSE \
	           LIS=FALSE \
	           LRS=FALSE \
	           HIS=FALSE \
	           HRS=FALSE \
	           > /dev/null;

	catlab from=$(OUTPUT)/DTEED_042252_1930_042753_1930_A07.IMG \
	       to=$(OUTPUT)/DTEED_042252_1930_042753_1930_A07_S16BIT.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO_U16BIT.pvl \
	       > /dev/null;  
	catlab from=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG \
	       to=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO_U16BIT.pvl \
	       > /dev/null;  
	
	pds2isis from=$(OUTPUT)/DTEED_042252_1930_042753_1930_A07.IMG \
	         to=$(OUTPUT)/DTEED_042252_1930_042753_1930_A07_S16BIT.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO_U16BIT.cub \
	         > /dev/null;
	pds2isis from=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG \
	         to=$(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO_U16BIT.cub \
	         > /dev/null;
	
	$(RM) $(OUTPUT)/DTEED_042252_1930_042753_1930_A07.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_B_41_ORTHO.IMG;
	$(RM) $(OUTPUT)/ESP_042252_1930_IRB_D_31_ORTHO.IMG;
	$(RM) $(OUTPUT)/orthoInputList.txt;
