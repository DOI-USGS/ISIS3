APPNAME = socetlinescankeywords

include $(ISISROOT)/make/isismake.tsts

commands:
#Test LRO-NACR, pole image
	$(APPNAME) from=$(INPUT)/M139722912RE.lev1.8bit.cub \
	  to=$(OUTPUT)/M139722912RE_keywords.txt > /dev/null;
	cat $(OUTPUT)/M139722912RE_keywords.txt | \
	sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	$(OUTPUT)/sed_M139722912RE_keywords.txt
	$(RM) $(OUTPUT)/M139722912RE_keywords.txt 

#Test LRO-NACL, image aquired while rotating the camera
	$(APPNAME) from=$(INPUT)/M140944356LE.lev1.8bit.cub \
	  to=$(OUTPUT)/M140944356LE_keywords.txt > /dev/null;
	cat $(OUTPUT)/M140944356LE_keywords.txt | \
	sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	$(OUTPUT)/sed_M140944356LE_keywords.txt
	$(RM) $(OUTPUT)/M140944356LE_keywords.txt 
