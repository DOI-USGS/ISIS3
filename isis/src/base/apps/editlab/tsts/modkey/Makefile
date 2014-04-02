# This test creates a new group, adds a keyword called key1 to the group, and 
# adds 2 keywords called key2 to the group.
APPNAME = editlab
editlabTruth1.txt.IGNORELINES = ByteOrder TileSamples TileLines StartByte

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
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=modkey grpname=NewGroup keyword=Key2 value=value units=meters;
	catlab from=$(OUTPUT)/tempCube.cub to=$(OUTPUT)/tempLab1.txt > /dev/null;
	head -n 31 $(OUTPUT)/tempLab1.txt > $(OUTPUT)/editlabTruth1.txt;
	cathist from=$(OUTPUT)/tempCube.cub to=$(OUTPUT)/history.pvl > /dev/null;
	$(RM) $(OUTPUT)/tempCube.cub;
	$(RM) $(OUTPUT)/tempLab1.txt;
