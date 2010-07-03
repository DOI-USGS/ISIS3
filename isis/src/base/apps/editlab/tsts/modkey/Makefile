APPNAME = editlab
editlabTruth1.txt.IGNORELINES=ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	makecube \
	  to=$(OUTPUT)/tempCube.cub \
	  samples=1 \
	  lines=13 \
	  bands=13 > /dev/null;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addg grpname=NewGroup ;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key1 value=null ;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=addkey grpname=NewGroup keyword=Key2 value=value ;
	$(APPNAME) from=$(OUTPUT)/tempCube.cub option=modkey grpname=NewGroup keyword=Key2 value=value units=meters;
	catlab from=$(OUTPUT)/tempCube.cub >& $(OUTPUT)/tempLab1.txt;
	head -n 31 $(OUTPUT)/tempLab1.txt > $(OUTPUT)/editlabTruth1.txt;
