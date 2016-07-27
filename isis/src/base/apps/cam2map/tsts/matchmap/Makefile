APPNAME = cam2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/vesta.cub \
		to=$(OUTPUT)/output.cub \
		map=$(INPUT)/vestaEquirectangular.map \
		matchmap=yes \
		>& /dev/null;
