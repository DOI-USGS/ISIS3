APPNAME = isiscomplete

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) isiscomplete > $(OUTPUT)/self_completion.txt;
	$(APPNAME) mirror > $(OUTPUT)/mirror_completion.txt;
	$(ECHO) `$(APPNAME) isisui` | $(SED) s/\\\/.\*\\\/// > $(OUTPUT)/isisui_completion.txt;
	$(APPNAME) qview > $(OUTPUT)/qview_completion.txt;
	$(APPNAME) fx > $(OUTPUT)/fx_completion.txt;
	$(APPNAME) isiscomplete mirror qview fx \
	  > $(OUTPUT)/combined_completion.txt;
