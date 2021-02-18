APPNAME = cubeit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(ECHO) $(INPUT)/isisTruth.cub+2 > $(OUTPUT)/inputList.lis;
	$(ECHO) $(INPUT)/isisTruth.cub >> $(OUTPUT)/inputList.lis;
	$(ECHO) $(INPUT)/isisTruth.cub+1 >> $(OUTPUT)/inputList.lis;
	$(APPNAME) fromlist=$(OUTPUT)/inputList.lis \
	  to=$(OUTPUT)/cubeitTruth1.cub > /dev/null;
	$(RM) $(OUTPUT)/inputList.lis;
