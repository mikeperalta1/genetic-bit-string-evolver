
#	Custom functions
define say
	$(info [BitEvolver] $1)
endef
define error
	$(error [BitEvolver] $1)
endef
define die
	$(call error,$1)
	$(exit 1)
endef


#	Demand BUILD_DIR and BIN_DIR
ifeq ($(BUILD_DIR),)
$(call die,Please provide BUILD_DIR)
endif
ifeq ($(BIN_DIR),)
$(call die,Please provide BIN_DIR)
endif


#
CC=g++
CFLAGS= -c -std=c++11 -Wall -I..


#
OBJECT_PREFIX=BitEvolver_


#
default:	default-say
default:	release
default:
	$(call say,Default target finished)
default-say:
	$(call say,Using default target: release)


#
release:	release-say
release:	CFLAGS+= -O2
release:	build-objects
release:
	$(call say,Done building RELEASE)
release-say:
	$(call say,Building RELEASE)

#
debug:		debug-say
debug:		CFLAGS+= -g -g3
debug:		build-objects
debug:
	$(call say,Done building DEBUG)
debug-say:
	$(call say,Building DEBUG)


#
build-objects:	\
				$(BUILD_DIR)/$(OBJECT_PREFIX)Random.o \
				$(BUILD_DIR)/$(OBJECT_PREFIX)Population.o \
				$(BUILD_DIR)/$(OBJECT_PREFIX)Breeder.o \
				$(BUILD_DIR)/$(OBJECT_PREFIX)Chromosome.o
	$(call say,Done building objects)


#	Population.o
$(BUILD_DIR)/$(OBJECT_PREFIX)Population.o:	\
											Population.h \
											Population.cpp \
											Defines.h Enums.h Includes.h \
											Random.h \
											Chromosome.h
	$(CC) -o $@ \
		Population.cpp \
		$(CFLAGS)
	$(call say,Built $@)


#	Breeder.o
$(BUILD_DIR)/$(OBJECT_PREFIX)Breeder.o:	\
											Breeder.h \
											Breeder.cpp \
											Defines.h Enums.h Includes.h \
											Random.h \
											Chromosome.h
	$(CC) -o $@ \
		Breeder.cpp \
		$(CFLAGS)
	$(call say,Built $@)


#	Chromosome.o
$(BUILD_DIR)/$(OBJECT_PREFIX)Chromosome.o:	\
											Chromosome.h \
											Chromosome.cpp \
											Defines.h Enums.h Includes.h \
											Random.h
	$(CC) -o $@ \
		Chromosome.cpp \
		$(CFLAGS)
	$(call say,Built $@)


#	Random.o
$(BUILD_DIR)/$(OBJECT_PREFIX)Random.o:	\
											Random.h \
											Random.cpp \
											Defines.h Enums.h Includes.h
	$(CC) -o $@ \
		Random.cpp \
		$(CFLAGS)
	$(call say,Built $@)







