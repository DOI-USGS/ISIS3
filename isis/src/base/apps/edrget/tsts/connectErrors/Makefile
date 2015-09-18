APPNAME = edrget

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) \
	  url=pdsimage2.wr.usgs.gov/cdroms/Viking_Orbiter/vo_1004/f253sxx/f253s05.imq \
	  topath=$(OUPTUT) \
	  >& $(OUTPUT)/noScheme.txt` ]; then \
	  true; \
	fi;
	if [ `$(APPNAME) \
	  url=ftp://pdsimage2.wr.usgs.gov/cdroms/Viking_Orbiter/vo_1004/f253sxx/f25.imq \
	  topath=$(OUPTUT) \
	  >& $(OUTPUT)/ftpFileDNE.txt` ]; then \
	  true; \
	fi;
	if [ `$(APPNAME) \
	  url=ftp:// \
	  >& $(OUTPUT)/ftpURLEmpty.txt` ]; then \
	  true; \
	fi;
	if [ `$(APPNAME) \
	  url=ftp://pdsimage2.wr.usgs.gov/cdroms/Viking_Orbiter/vo_1004/f253sxx/ \
	  >& $(OUTPUT)/ftpFileNameEmpty.txt` ]; then \
	  true; \
	fi;
	if [ `$(APPNAME) \
	  url=ftp://~~~~~jdkjkjks/ \
	  >& $(OUTPUT)/ftpURLInvalid.txt` ]; then \
	  true; \
	fi;
	if [ `$(APPNAME) \
	  url=http://pdsimage2.wr.usgs.gov/cdroms/Viking_Orbiter/vo_1004/f253sxx/f26.imq \
	  topath=$(OUTPUT) \
	  >& $(OUTPUT)/httpFileDNEtmp.txt` ]; then \
	  true; \
	fi; \
	$(TAIL) '-1' < $(OUTPUT)/httpFileDNEtmp.txt > $(OUTPUT)/httpFileDNE.txt; \
	$(RM) $(OUTPUT)/httpFileDNEtmp.txt; \
	if [ `$(APPNAME) \
	  url=http:// \
	  >& $(OUTPUT)/httpURLEmpty.txt` ]; then \
	  true; \
	fi;
	if [ `$(APPNAME) \
	  url=http://pdsimage2.wr.usgs.gov/cdroms/Viking_Orbiter/vo_1004/f253sxx/ \
	  >& $(OUTPUT)/httpFileNameEmpty.txt` ]; then \
	  true; \
	fi;
	if [ `$(APPNAME) \
	  url=http://~~~~~jdkjkjks/ \
	  >& $(OUTPUT)/httpURLInvalid.txt` ]; then \
	  true; \
	fi;