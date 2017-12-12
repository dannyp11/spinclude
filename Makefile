# src data --------------------------------------------------------
SRC_DIR := src
BIN      = spinclude 
UTIL     = tarjan-util
# -----------------------------------------------------------------

# Install data ----------------------------------------------------
DESTDIR ?=/usr/local
INSTALLDIR_BIN=$(DESTDIR)/bin/
# -----------------------------------------------------------------

.PHONY: all

all:
	$(MAKE) -C $(SRC_DIR) -j4
	ln -sf $(SRC_DIR)/$(BIN) $(BIN)
	ln -sf $(SRC_DIR)/$(UTIL) $(UTIL)

check:
	@if [ ! -d submodules/googletest/googletest ]; then \
	    echo "ERROR: GoogleTest not found, did you forget to run" ; \
	    echo "  git submodule update --init --recursive ?"; \
	    exit 1 ;\
	fi
	$(MAKE) check -C $(SRC_DIR)
	
clean:
	$(MAKE) clean -C $(SRC_DIR)
	rm -f $(BIN) $(UTIL)

install: all
	mkdir -p $(INSTALLDIR_BIN)
	install $(SRC_DIR)/$(BIN) $(INSTALLDIR_BIN)

uninstall:
	rm -f $(INSTALLDIR_BIN)/$(BIN)	

