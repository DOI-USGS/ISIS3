APPNAME = tgocassisstitch

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/framelets.lis
	$(APPNAME) fromlist=$(OUTPUT)/framelets.lis \
	outputprefix=$(OUTPUT)/CAS-MCO > /dev/null;
	ls $(OUTPUT)/*.cub | sed 's/\.cub//' > $(OUTPUT)/frames.lis
	catlab from=$\$$\1.cub to=\$$\1.pvl -batchlist=$(OUTPUT)/frames.lis > /dev/null;
	rm $(OUTPUT)/framelets.lis
	rm $(OUTPUT)/frames.lis
