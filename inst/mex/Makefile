# Build static GMP and MPFR libraries in this directory.

GMP_VER = 6.2.1
MPFR_VER = 4.1.0

INST_DIR = $(shell pwd)
BUILD_DIR := ${INST_DIR}/.build
ENSURE_BUILD_DIR := $(shell mkdir -p ${BUILD_DIR})

GMP_DIR = gmp-${GMP_VER}
GMP_TAR = ${GMP_DIR}.tar.bz2
GMP_URL = https://gmplib.org/download/gmp/${GMP_TAR}
GMP_PATH = ${BUILD_DIR}/${GMP_DIR}

MPFR_DIR = mpfr-${MPFR_VER}
MPFR_TAR = ${MPFR_DIR}.tar.gz
MPFR_URL = https://www.mpfr.org/mpfr-4.1.0/${MPFR_TAR}
MPFR_PATH = ${BUILD_DIR}/${MPFR_DIR}

CURL_OPTS = --fail --location --silent --show-error --output

HEADER = gmp.h mpfr.h mpf2mpfr.h
STATIC_LIBS = libgmp.a libmpfr.a

UNCRUSTIFY_FILES = $(wildcard *.c) $(wildcard *.h)

all: $(patsubst %,${BUILD_DIR}/lib/%,${STATIC_LIBS})
	cp $(patsubst %,${BUILD_DIR}/lib/%,${STATIC_LIBS}) ${INST_DIR}
	cp $(patsubst %,${BUILD_DIR}/include/%,${HEADER})  ${INST_DIR}

${BUILD_DIR}/${GMP_TAR}:
	curl ${CURL_OPTS} ${BUILD_DIR}/${GMP_TAR} ${GMP_URL}

${BUILD_DIR}/${MPFR_TAR}:
	curl ${CURL_OPTS} ${BUILD_DIR}/${MPFR_TAR} ${MPFR_URL}

${BUILD_DIR}/lib/libgmp.a: ${BUILD_DIR}/${GMP_TAR}
	$(RM) -R ${GMP_PATH}
	cd ${BUILD_DIR} && tar -xf $<
	cd ${GMP_PATH} && ./configure \
	  --prefix=${BUILD_DIR} \
	  --libdir=${BUILD_DIR}/lib \
	  --with-pic=yes
	cd ${GMP_PATH} && $(MAKE)
	cd ${GMP_PATH} && $(MAKE) check
	cd ${GMP_PATH} && $(MAKE) install

${BUILD_DIR}/lib/libmpfr.a: ${BUILD_DIR}/${MPFR_TAR} ${BUILD_DIR}/lib/libgmp.a
	$(RM) -R ${MPFR_PATH}
	cd ${BUILD_DIR} && tar -xf $<
	cd ${MPFR_PATH} && ./configure \
	  --prefix=${BUILD_DIR} \
	  --libdir=${BUILD_DIR}/lib \
	  --with-pic=yes \
	  --with-gmp-include=${BUILD_DIR}/include \
	  --with-gmp-lib=${BUILD_DIR}/lib
	cd ${MPFR_PATH} && $(MAKE)
	cd ${MPFR_PATH} && $(MAKE) check
	cd ${MPFR_PATH} && $(MAKE) install

format:
	for i in ${UNCRUSTIFY_FILES}; do \
	  uncrustify -c uncrustify.cfg --no-backup $$i; \
	done
