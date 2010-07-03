APP1NAME = moc2isis
APP2NAME = spiceinit
APP3NAME = campt
APP4NAME = getsn

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APP1NAME) from= $(INPUT)/ab102302.img \
	  to= $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub > /dev/null

	$(APP2NAME) from= $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub \
	  to= $(OUTPUT)/ab102302.img--finalOutput.pvl SAMPLE=2 LINE=2 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub \
	  to= $(OUTPUT)/ab102302.img--finalOutput.pvl SAMPLE=335 LINE=2 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub \
	  to= $(OUTPUT)/ab102302.img--finalOutput.pvl SAMPLE=335 LINE=639 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub \
	  to= $(OUTPUT)/ab102302.img--finalOutput.pvl SAMPLE=2 LINE=639 ALLOWOUTSIDE=no \
	  > /dev/null

	$(APP3NAME) from= $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub \
	  to= $(OUTPUT)/ab102302.img--finalOutput.pvl SAMPLE=194 LINE=335 ALLOWOUTSIDE=no \
	  > /dev/null

	echo -e "\nOutput of getsn..." > $(OUTPUT)/ab102302.img--getsnOutput.txt && \
          $(APP4NAME) from= $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub \
          >> $(OUTPUT)/ab102302.img--getsnOutput.txt

	$(RM) $(OUTPUT)/ab102302.img--moc2isis-spiceinit.cub > /dev/null
	$(RM) $(OUTPUT)/../print.prt > /dev/null
