APPNAME = dawnvir2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/VIR_IR_1A_1_332974737_1.LBL \
         to=$(OUTPUT)/VIR_IR_1A_1_332974737_1.cub > /dev/null;
	catlab from=$(OUTPUT)/VIR_IR_1A_1_332974737_1.cub >& $(OUTPUT)/labels.txt;
