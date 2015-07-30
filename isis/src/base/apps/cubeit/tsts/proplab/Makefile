APPNAME = cubeit

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/inputPropLab.lis; \
	$(APPNAME) proplab=$(INPUT)/isisTruthDifferentLabel.cub   \
	fromlist=$(OUTPUT)/inputPropLab.lis              \
	to=$(OUTPUT)/cubeitProplabTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/cubeitProplabTruth.cub      \
	to=$(OUTPUT)/labels.pvl > /dev/null; 
	$(RM) $(OUTPUT)/inputPropLab.lis;  	\
