# This test creates a new group, adds 2 keyword called key1 to the group, 
# adds 2 keywords called key2 to the group. Then, the call to setkey on key1
# replaces the value and units of one of the of key1 keywords and deletes the 
# other
APPNAME = editlab
editlabTruth1.txt.IGNORELINES = ByteOrder TileSamples TileLines StartByte
tempLab1.txt.IGNORELINES = Bytes TileSamples TileLines StartByte
history.pvl.IGNORELINES = ExecutionDateTime HostName

include $(ISISROOT)/make/isismake.tsts

commands:
	makecube \
	  to=$(OUTPUT)/tempCube.cub \
	  samples=1 \
	  lines=13 \
	  bands=13 > /dev/null;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addg grpname=NewGroup ;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key1 value=null ;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key1 value=value units=meters;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key2 value=null ;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key2 value=value units=meters;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=setkey grpname=NewGroup keyword=Key1 value=setVal units=setMeters;
	catlab from=$(OUTPUT)/tempCube.cub >& $(OUTPUT)/tempLab1.txt;
	head -n 31 $(OUTPUT)/tempLab1.txt > $(OUTPUT)/editlabTruth1.txt;
	cathist from=$(OUTPUT)/tempCube.cub >& $(OUTPUT)/history.pvl;
