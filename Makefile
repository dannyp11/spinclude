# src data --------------------------------------------------------
SRC_DIR := src
BIN     := spinclude
# -----------------------------------------------------------------

# Install data ----------------------------------------------------
DESTDIR ?=/usr/local
INSTALLDIR_BIN=$(DESTDIR)/bin/
# -----------------------------------------------------------------

.PHONY: all

all:
	$(MAKE) -C $(SRC_DIR) -j4
	ln -sf $(SRC_DIR)/$(BIN) $(BIN)

check:	
	$(MAKE) check -C $(SRC_DIR)
	
clean:
	$(MAKE) clean -C $(SRC_DIR)
	rm -f $(BIN)

install: all
	mkdir -p $(INSTALLDIR_BIN)
	install $(SRC_DIR)/$(BIN) $(INSTALLDIR_BIN)

uninstall:
	rm -f $(INSTALLDIR_BIN)/$(BIN)	

