# Please note, the input data was reduced from its original size, in doing so
# the Scale and PixelResolution was changed, for the purpose of testing it
# needed to be the original values, so, the values were edited. This means
# the values for those two keywords are incorrect.
APPNAME = hidtmgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) DTM=$(INPUT)/Polar_Crater_1_1m_ngate_edited2_forPDS.cub \
	  ORTHO1=$(INPUT)/PSP_009404_2635_RED_NLstretch_1m_o_forPDS.cub \
	  ORTHO2=$(INPUT)/PSP_009404_2635_RED_NLstretch_25cm_o_forPDS.cub \
  	  ORTHO3=$(INPUT)/PSP_010221_2635_RED_NLstretch_1m_o_forPDS.cub \
	  ORTHO4=$(INPUT)/PSP_010221_2635_RED_NLstretch_25cm_o_forPDS.cub \
	  OUTPUTDIR=$(OUTPUT) \
	  PARAMSPVL=$(INPUT)/parameters.pvl \
	  RATIONALE_DESC=Rationale \
	  > /dev/null;
	
	head -n 83 $(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.IMG \
	  > $(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.pvl;
	head -n 82 $(OUTPUT)/DT1PA_009404_2635_010221_2635_Z12.IMG \
	  > $(OUTPUT)/DT1PA_009404_2635_010221_2635_Z12.pvl;
	head -n 82 $(OUTPUT)/DT1PC_009404_2635_010221_2635_Z12.IMG \
	  > $(OUTPUT)/DT1PC_009404_2635_010221_2635_Z12.pvl;
	head -n 82 $(OUTPUT)/DT2PA_009404_2635_010221_2635_Z12.IMG \
	  > $(OUTPUT)/DT2PA_009404_2635_010221_2635_Z12.pvl;
	head -n 82 $(OUTPUT)/DT2PC_009404_2635_010221_2635_Z12.IMG \
	  > $(OUTPUT)/DT2PC_009404_2635_010221_2635_Z12.pvl;
	
	pds2isis from=$(OUTPUT)/DT1PA_009404_2635_010221_2635_Z12.IMG \
	  to=$(OUTPUT)/DT1PA_009404_2635_010221_2635_Z12.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/DT1PC_009404_2635_010221_2635_Z12.IMG \
	  to=$(OUTPUT)/DT1PC_009404_2635_010221_2635_Z12.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/DT2PA_009404_2635_010221_2635_Z12.IMG \
	  to=$(OUTPUT)/DT2PA_009404_2635_010221_2635_Z12.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/DT2PC_009404_2635_010221_2635_Z12.IMG \
	  to=$(OUTPUT)/DT2PC_009404_2635_010221_2635_Z12.cub \
	  > /dev/null;  
	pds2isis from=$(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.IMG \
	  to=$(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.cub \
	  > /dev/null;  
	
	rm $(OUTPUT)/DT1PA_009404_2635_010221_2635_Z12.IMG;
	rm $(OUTPUT)/DT1PC_009404_2635_010221_2635_Z12.IMG;
	rm $(OUTPUT)/DT2PA_009404_2635_010221_2635_Z12.IMG;
	rm $(OUTPUT)/DT2PC_009404_2635_010221_2635_Z12.IMG;
	rm $(OUTPUT)/DTEPC_009404_2635_010221_2635_Z12.IMG;

