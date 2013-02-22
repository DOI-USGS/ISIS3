APPNAME = socetlinescankeywords

include $(ISISROOT)/make/isismake.tsts

commands:
#Test LRO-NACR, pole image
	$(APPNAME) from=$(INPUT)/M139722912RE.lev1.8bit.cub \
	  to=$(OUTPUT)/M139722912RE_keywords.txt > /dev/null;

#Test LRO-NACL, image aquired while rotating the camera
	$(APPNAME) from=$(INPUT)/M140944356LE.lev1.8bit.cub \
	  to=$(OUTPUT)/M140944356LE_keywords.txt > /dev/null;
