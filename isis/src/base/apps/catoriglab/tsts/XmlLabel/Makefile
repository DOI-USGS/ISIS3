APPNAME = catoriglab
# history 2018-09-05 Kristin Berry - Added code to strip out XML attributes from test, since their order can chage. 

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/tgoCaSSISImage.cub \
	  APPEND=false TO=$(OUTPUT)/XmlLabelTemp.txt > /dev/null;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	  $(OUTPUT)/XmlLabelTemp.txt \
	  > $(OUTPUT)/tempLabel1.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	  $(OUTPUT)/tempLabel1.txt \
	  > $(OUTPUT)/tempLabel2.txt;
	$(SED) 's+\Science_Facets.*>+\Science_Facets>+' \
	  $(OUTPUT)/tempLabel2.txt \
	  > $(OUTPUT)/XmlLabel.txt; 
	$(RM) $(OUTPUT)/XmlLabelTemp.txt;
	$(RM) $(OUTPUT)/tempLabel1.txt;
	$(RM) $(OUTPUT)/tempLabel2.txt;

