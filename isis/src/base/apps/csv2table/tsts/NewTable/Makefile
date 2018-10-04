APPNAME = csv2table

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/isisTruth.cub $(OUTPUT)/isisTruth.cub;
	$(APPNAME) csv=$(INPUT)/test.csv \
	tablename="TestTable" \
	to=$(OUTPUT)/isisTruth.cub > /dev/null;
	tabledump from=$(OUTPUT)/isisTruth.cub \
	to=$(OUTPUT)/output.csv \
	NAME="TestTable" > /dev/null;
	rm $(OUTPUT)/isisTruth.cub;
