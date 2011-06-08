APPNAME = editlab

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/input.pvl $(OUTPUT)/input.pvl;
	$(APPNAME) from=$(OUTPUT)/input.pvl option=addkey grpname=Numbers keyword=Z value=32;
	$(APPNAME) from=$(OUTPUT)/input.pvl option=setkey grpname=Numbers keyword=X value=9;
	$(APPNAME) from=$(OUTPUT)/input.pvl option=modkey grpname=Numbers keyword=Z value=42;
	$(APPNAME) from=$(OUTPUT)/input.pvl option=modkey grpname=Numbers keyword=Y value=1;
