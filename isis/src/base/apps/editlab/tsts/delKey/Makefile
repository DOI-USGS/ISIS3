# This test adds a new group, adds key1 and key2 to the group, and deletes key1
APPNAME = editlab
editlabTruth2.txt.IGNORELINES = ByteOrder TileSamples TileLines StartByte

include $(ISISROOT)/make/isismake.tsts

commands:
	makecube \
	  to=$(OUTPUT)/tempCube.cub \
	  samples=1 \
	  lines=13 \
	  bands=13 > /dev/null;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addg grpname=NewGroup;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key1 value=null;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key2 value=value;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=delkey grpname=NewGroup keyword=Key1;
	catlab from=$(OUTPUT)/tempCube.cub >& $(OUTPUT)/tempLab1.txt;
	head -n 30 $(OUTPUT)/tempLab1.txt > $(OUTPUT)/editlabTruth2.txt;
	cathist from=$(OUTPUT)/tempCube.cub >& $(OUTPUT)/history.pvl;
	$(RM) $(OUTPUT)/tempCube.cub;
	$(RM) $(OUTPUT)/tempLab1.txt;
