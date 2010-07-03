APPNAME = editlab
editlabTruth3.txt.IGNORELINES=ByteOrder
tempLab3.txt.IGNORELINES=Bytes ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	makecube \
	  to=$(OUTPUT)/tempCube.cub \
	  samples=1 \
	  lines=13 \
	  bands=13 > /dev/null;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addg grpname=NewGroup comment="New Group";
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key1 value=null comment="New Keyword";
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key2 value=value ;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=modkey grpname=NewGroup keyword=Key2 value=value units=meters comment="Hello World";
	catlab from=$(OUTPUT)/tempCube.cub >& $(OUTPUT)/tempLab3.txt;
	head -n 31 $(OUTPUT)/tempLab3.txt > $(OUTPUT)/editlabTruth3.txt;
