APPNAME = csv2table

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/isisTruth.cub $(OUTPUT)/isisTruth.cub;
	$(APPNAME) csv=$(INPUT)/test.csv \
	label=$(INPUT)/label.pvl \
	tablename="TestTable" \
	to=$(OUTPUT)/isisTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/isisTruth.cub | \
	sed -n '/Object = Table/,/End_Object/p' > \
	$(OUTPUT)/table.pvl;
	rm $(OUTPUT)/isisTruth.cub;
