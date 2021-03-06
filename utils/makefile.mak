#################################################################################
#										#
#			Windows MinGW TPM2 Makefile				#
#			     Written by Ken Goldman				#
#		       IBM Thomas J. Watson Research Center			#
#	      $Id: makefile.mak 730 2016-08-23 21:09:53Z kgoldman $		#
#										#
# (c) Copyright IBM Corporation 2015.						#
# 										#
# All rights reserved.								#
# 										#
# Redistribution and use in source and binary forms, with or without		#
# modification, are permitted provided that the following conditions are	#
# met:										#
# 										#
# Redistributions of source code must retain the above copyright notice,	#
# this list of conditions and the following disclaimer.				#
# 										#
# Redistributions in binary form must reproduce the above copyright		#
# notice, this list of conditions and the following disclaimer in the		#
# documentation and/or other materials provided with the distribution.		#
# 										#
# Neither the names of the IBM Corporation nor the names of its			#
# contributors may be used to endorse or promote products derived from		#
# this software without specific prior written permission.			#
# 										#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS		#
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT		#
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR		#
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT		#
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,	#
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT		#
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,		#
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY		#
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT		#
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE		#
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.		#
#										#
#################################################################################

# C compiler

CC = "c:/program files/mingw/bin/gcc.exe"

# compile - common flags for TSS library and applications

CCFLAGS = 					\
	-DTPM_WINDOWS				\
	-I. 					\
	-I"c:/program files/MinGW/include"	\
	-I"c:/program files/openssl/include"	\

# compile - for TSS library

CCLFLAGS = 	-I../src -DTPM_TSS

# link - common flags flags TSS library and applications

LNFLAGS =					\
	-D_MT					\
	-DTPM_WINDOWS				\
	-I"c:/program files/MinGW/include"	\
	-I"c:/program files/openssl/include"	\
	-I.

# link - for TSS library

LNLFLAGS = 

# link - for applications, TSS path, TSS and OpenSSl libraries

LNAFLAGS = 

LNLIBS = 	"c:/program files/openssl/lib/mingw/libeay32.a" \
		"c:/program files/openssl/lib/mingw/ssleay32.a" \
		"c:/program files/MinGW/lib/libws2_32.a"

# executable extension

EXE=.exe

# shared library

LIBTSS=libtss.dll

include makefile-common

# Uncomment for TBSI

# CCFLAGS +=	-DTPM_WINDOWS_TBSI		\
# 		-DTPM_WINDOWS_TBSI_WIN8		\
# 		-D_WIN32_WINNT=0x0600

# TSS_OBJS += tsstbsi.o 

# LNLIBS += C:\PROGRA~2\WI3CF2~1\8.0\Lib\win8\um\x86\Tbs.lib
# #LNLIBS += c:/progra~1/Micros~2/Windows/v7.1/lib/Tbs.lib

# default build target

all:	$(ALL)

# TSS shared library source

tss.o: 		$(TSS_HEADERS) tss.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tss.c
tssproperties.o: $(TSS_HEADERS) tssproperties.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tssproperties.c
tssauth.o: 	$(TSS_HEADERS) tssauth.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tssauth.c
tssmarshal.o: 	$(TSS_HEADERS) tssmarshal.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tssmarshal.c
tsscrypto.o: 	$(TSS_HEADERS) tsscrypto.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tsscrypto.c
tssutils.o: 	$(TSS_HEADERS) tssutils.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tssutils.c
tsssocket.o: 	$(TSS_HEADERS) tsssocket.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tsssocket.c
tssdev.o: 	$(TSS_HEADERS) tssdev.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tssdev.c
tsstransmit.o: 	$(TSS_HEADERS) tsstransmit.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tsstransmit.c
tssresponsecode.o: $(TSS_HEADERS) tssresponsecode.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tssresponsecode.c
tssccattributes.o: $(TSS_HEADERS) tssccattributes.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tssccattributes.c
fail.o: 	$(TSS_HEADERS) fail.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) fail.c
tssprint.o: 	$(TSS_HEADERS) tssprint.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) tssprint.c
Unmarshal.o: 	$(TSS_HEADERS) Unmarshal.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) Unmarshal.c

CommandAttributeData.o: 	$(TSS_HEADERS) ../src/CommandAttributeData.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) ../src/CommandAttributeData.c
Commands.o: 	../src/Commands.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) ../src/Commands.c
CpriHash.o: 	../src/CpriHash.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) ../src/CpriHash.c
CpriSym.o: 	../src/CpriSym.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) ../src/CpriSym.c

# TSS shared library build

$(LIBTSS): 	$(TSS_OBJS)
		$(CC) $(LNFLAGS) $(LNLFLAGS) -shared -o $(LIBTSS) $(TSS_OBJS) \
		-Wl,--out-implib,libtss.a $(LNLIBS)

.PHONY:		clean
.PRECIOUS:	%.o

clean:		
		rm -f *.o  *~ 	\
		$(LIBTSS)	\
		$(ALL)

create.exe:	create.o objecttemplates.o $(LIBTSS) 
		$(CC) $(LNFLAGS) -L. -ltss $< -o $@ applink.o objecttemplates.o $(LNLIBS) $(LIBTSS) 

createprimary.exe:	createprimary.o objecttemplates.o  $(LIBTSS) 
		$(CC) $(LNFLAGS) -L. -ltss $< -o $@ applink.o objecttemplates.o $(LNLIBS) $(LIBTSS) 

eventextend.exe:	eventextend.o eventlib.o $(LIBTSS) 
		$(CC) $(LNFLAGS) -L. -ltss $< -o $@ applink.o eventlib.o $(LNLIBS) $(LIBTSS) 

createek.exe:	createek.o ekutils.o $(LIBTSS) 
		$(CC) $(LNFLAGS) -L. -ltss $< -o $@ applink.o ekutils.o $(LNLIBS) $(LIBTSS)

pprovision.exe:	pprovision.o ekutils.o $(LIBTSS) 
		$(CC) $(LNFLAGS) -L. -ltss $< -o $@ applink.o ekutils.o $(LNLIBS) $(LIBTSS)

%.exe:		%.o applink.o $(LIBTSS)
		$(CC) $(LNFLAGS) -L. -ltss $< -o $@ applink.o $(LNLIBS) $(LIBTSS)

%.o:		%.c
		$(CC) $(CCFLAGS) $< -o $@
