include common.mk


$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p bin)
$(module add gcc-7.3.0)
$(shell export LD_LIBRARY_PATH=$(LIBEVENT_LD_DIR):$LD_LIBRARY_PATH)
$(shell cp play_files/* .)

all:



ifneq ($(WITH_CONSTANTS), 0)
	(make -C lib/ BASE_DIR=${CURDIR} ODIR=$(OBJ_DIR))
	(make -C module/ BASE_DIR=${CURDIR} ODIR=$(OBJ_DIR) OFILE=$(COMPILE_CONF_FILE))
endif
	(make -C lib/ BASE_DIR=${CURDIR} ODIR=$(OBJ_DIR))
	(make -C main/ BASE_DIR=${CURDIR} ODIR=$(OBJ_DIR) BDIR=$(BIN_DIR))

clean:
	(make clean -C lib/ BASE_DIR=${CURDIR} ODIR=$(OBJ_DIR))
	(make clean -C main/ BASE_DIR=${CURDIR} ODIR=$(OBJ_DIR))
	(make clean -C module/ BASE_DIR=${CURDIR} ODIR=$(OBJ_DIR))
	rm -rf bin $(OBJ_DIR)
	rm -f *~ *.o *#* *.txt

