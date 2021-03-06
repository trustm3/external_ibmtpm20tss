################################################################################
#										#
#		Linux TPM2 Utilities Makefile					#
#			     Written by Ken Goldman				#
#		       IBM Thomas J. Watson Research Center			#
#	      $Id: makefile 740 2016-08-29 18:26:38Z kgoldman $			#
#										#
# (c) Copyright IBM Corporation 2016						#
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

CC = /usr/bin/gcc

# compile - common flags for TSS library and applications

CCFLAGS = 	-DTPM_POSIX

#	-DTPM_INTERFACE_TYPE_DEFAULT="\"dev\""

# compile - for TSS library

CCLFLAGS = 	-I. -I../src -DTPM_TSS

# compile - for applications

CCAFLAGS = 	-I.

# link - common flags flags TSS library and applications

LNFLAGS = 	-DTPM_POSIX		\
		-L.

# link - for TSS library

LNLFLAGS = 

# link - for applications, TSS path, TSS and OpenSSl libraries

LNAFLAGS = -Wl,-rpath,.

LNALIBS =  -ltss -lcrypto

# shared library

LIBTSS=libtss.so

# executable extension

EXE=

include makefile-common

# default build target

all:	$(ALL)

# TSS library source shared with TPM (for TSS library)

Unmarshal.c:			../src/Unmarshal.c
				sed '/TPM_TSS/a #define TPM_TSS' ../src/Unmarshal.c > Unmarshal.c
CpriSym.c:			../src/CpriSym.c
				ln -s ../src/CpriSym.c .
CpriHash.c:			../src/CpriHash.c
				ln -s ../src/CpriHash.c .
Commands.c:			../src/Commands.c
				ln -s ../src/Commands.c .
CommandAttributeData.c:		../src/CommandAttributeData.c
				ln -s ../src/CommandAttributeData.c .

# TSS shared library source

tss.o: 		$(TSS_HEADERS) tss.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tss.c
tssproperties.o: $(TSS_HEADERS) tssproperties.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tssproperties.c
tssauth.o: 	$(TSS_HEADERS) tssauth.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tssauth.c
tssmarshal.o: 	$(TSS_HEADERS) tssmarshal.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tssmarshal.c
tsscrypto.o: 	$(TSS_HEADERS) tsscrypto.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tsscrypto.c
tssutils.o: 	$(TSS_HEADERS) tssutils.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tssutils.c
tsssocket.o: 	$(TSS_HEADERS) tsssocket.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tsssocket.c
tssdev.o: 	$(TSS_HEADERS) tssdev.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tssdev.c
tsstransmit.o: 	$(TSS_HEADERS) tsstransmit.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tsstransmit.c
tssresponsecode.o: $(TSS_HEADERS) tssresponsecode.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tssresponsecode.c
tssccattributes.o: $(TSS_HEADERS) tssccattributes.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tssccattributes.c
fail.o: 	$(TSS_HEADERS) fail.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC fail.c
tssprint.o: 	$(TSS_HEADERS) tssprint.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC tssprint.c
Unmarshal.o: 	$(TSS_HEADERS) Unmarshal.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC Unmarshal.c
Commands.o: 	$(TSS_HEADERS) Commands.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC Commands.c
CommandAttributeData.o: 	$(TSS_HEADERS) CommandAttributeData.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC CommandAttributeData.c
CpriHash.o: 	$(TSS_HEADERS) CpriHash.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC CpriHash.c
CpriSym.o: 	$(TSS_HEADERS) CpriSym.c
		$(CC) $(CCFLAGS) $(CCLFLAGS) -fPIC CpriSym.c

# TSS shared library build

$(LIBTSS): 	$(TSS_OBJS)
		$(CC) $(LNFLAGS) $(LNLFLAGS) -shared -o $(LIBTSS) $(TSS_OBJS)

.PHONY:		clean
.PRECIOUS:	%.o

clean:		
		rm -f *.o  *~ 	\
		../src/*.o	\
		h*.bin		\
		$(ALL)

# applications

activatecredential:	tss2/tss.h activatecredential.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) activatecredential.o $(LNALIBS) -o activatecredential
eventextend:		eventextend.o eventlib.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) eventextend.o eventlib.o $(LNALIBS) -o eventextend
certify:		tss2/tss.h certify.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) certify.o $(LNALIBS) -o certify
changeeps:		tss2/tss.h changeeps.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) changeeps.o $(LNALIBS) -o changeeps
changepps:		tss2/tss.h changepps.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) changepps.o $(LNALIBS) -o changepps
clear:			tss2/tss.h clear.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) clear.o $(LNALIBS) -o clear
clearcontrol:		tss2/tss.h clearcontrol.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) clearcontrol.o $(LNALIBS) -o clearcontrol
clockrateadjust:	tss2/tss.h clockrateadjust.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) clockrateadjust.o $(LNALIBS) -o clockrateadjust
clockset:		tss2/tss.h clockset.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) clockset.o $(LNALIBS) -o clockset
contextload:		tss2/tss.h contextload.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) contextload.o $(LNALIBS) -o contextload
contextsave:		tss2/tss.h contextsave.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) contextsave.o $(LNALIBS) -o contextsave
create:			tss2/tss.h create.o objecttemplates.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) create.o objecttemplates.o $(LNALIBS) -o create
createprimary:		tss2/tss.h createprimary.o objecttemplates.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) createprimary.o objecttemplates.o $(LNALIBS) -o createprimary
dictionaryattacklockreset:		tss2/tss.h dictionaryattacklockreset.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) dictionaryattacklockreset.o $(LNALIBS) -o dictionaryattacklockreset
dictionaryattackparameters:		tss2/tss.h dictionaryattackparameters.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) dictionaryattackparameters.o $(LNALIBS) -o dictionaryattackparameters
duplicate:		tss2/tss.h duplicate.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) duplicate.o $(LNALIBS) -o duplicate 
eccparameters:		tss2/tss.h eccparameters.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) eccparameters.o $(LNALIBS) -o eccparameters 
encryptdecrypt:		tss2/tss.h encryptdecrypt.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) encryptdecrypt.o $(LNALIBS) -o encryptdecrypt	
eventsequencecomplete:	tss2/tss.h eventsequencecomplete.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) eventsequencecomplete.o $(LNALIBS) -o eventsequencecomplete	
evictcontrol:		tss2/tss.h evictcontrol.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) evictcontrol.o $(LNALIBS) -o evictcontrol	
flushcontext:		tss2/tss.h flushcontext.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) flushcontext.o $(LNALIBS) -o flushcontext
getcommandauditdigest:	tss2/tss.h getcommandauditdigest.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) getcommandauditdigest.o $(LNALIBS) -o getcommandauditdigest
getcapability:		tss2/tss.h getcapability.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) getcapability.o $(LNALIBS) -o getcapability
getrandom:		tss2/tss.h getrandom.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) getrandom.o $(LNALIBS) -o getrandom
getsessionauditdigest:	tss2/tss.h getsessionauditdigest.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) getsessionauditdigest.o $(LNALIBS) -o getsessionauditdigest
gettime:		tss2/tss.h gettime.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) gettime.o $(LNALIBS) -o gettime
hashsequencestart:	tss2/tss.h hashsequencestart.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) hashsequencestart.o $(LNALIBS) -o hashsequencestart
hash:			tss2/tss.h hash.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) hash.o $(LNALIBS) -o hash
hierarchycontrol:	tss2/tss.h hierarchycontrol.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) hierarchycontrol.o $(LNALIBS) -o hierarchycontrol
hierarchychangeauth:	tss2/tss.h hierarchychangeauth.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) hierarchychangeauth.o $(LNALIBS) -o hierarchychangeauth
hmac:			tss2/tss.h hmac.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) hmac.o $(LNALIBS) -o hmac
hmacstart:		tss2/tss.h hmacstart.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) hmacstart.o $(LNALIBS) -o hmacstart
import:			tss2/tss.h import.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) import.o $(LNALIBS) -o import
load:			tss2/tss.h load.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) load.o $(LNALIBS) -o load
loadexternal:		tss2/tss.h loadexternal.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) loadexternal.o $(LNALIBS) -o loadexternal
makecredential:		tss2/tss.h makecredential.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) makecredential.o $(LNALIBS) -o makecredential
nvcertify:		tss2/tss.h nvcertify.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvcertify.o $(LNALIBS) -o nvcertify
nvchangeauth:		tss2/tss.h nvchangeauth.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvchangeauth.o $(LNALIBS) -o nvchangeauth
nvdefinespace:		tss2/tss.h nvdefinespace.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvdefinespace.o $(LNALIBS) -o nvdefinespace
nvextend:		tss2/tss.h nvextend.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvextend.o $(LNALIBS) -o nvextend
nvglobalwritelock:	tss2/tss.h nvglobalwritelock.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvglobalwritelock.o $(LNALIBS) -o nvglobalwritelock
nvincrement:		tss2/tss.h nvincrement.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvincrement.o $(LNALIBS) -o nvincrement
nvread:			tss2/tss.h nvread.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvread.o $(LNALIBS) -o nvread
nvreadlock:		tss2/tss.h nvreadlock.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvreadlock.o $(LNALIBS) -o nvreadlock
nvreadpublic:		tss2/tss.h nvreadpublic.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvreadpublic.o $(LNALIBS) -o nvreadpublic
nvsetbits:		tss2/tss.h nvsetbits.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvsetbits.o $(LNALIBS) -o nvsetbits
nvundefinespace:	tss2/tss.h nvundefinespace.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvundefinespace.o $(LNALIBS) -o nvundefinespace
nvundefinespacespecial:	tss2/tss.h nvundefinespacespecial.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvundefinespacespecial.o $(LNALIBS) -o nvundefinespacespecial
nvwrite:		tss2/tss.h nvwrite.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvwrite.o $(LNALIBS) -o nvwrite
nvwritelock:		tss2/tss.h nvwritelock.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) nvwritelock.o $(LNALIBS) -o nvwritelock
objectchangeauth:	tss2/tss.h objectchangeauth.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) objectchangeauth.o $(LNALIBS) -o objectchangeauth
pcrallocate: 		tss2/tss.h pcrallocate.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) pcrallocate.o $(LNALIBS) -o pcrallocate
pcrevent: 		tss2/tss.h pcrevent.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) pcrevent.o $(LNALIBS) -o pcrevent
pcrextend: 		tss2/tss.h pcrextend.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) pcrextend.o $(LNALIBS) -o pcrextend
pcrread: 		tss2/tss.h pcrread.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) pcrread.o $(LNALIBS) -o pcrread
pcrreset: 		tss2/tss.h pcrreset.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) pcrreset.o $(LNALIBS) -o pcrreset
policyauthorize:	tss2/tss.h policyauthorize.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policyauthorize.o $(LNALIBS) -o policyauthorize
policyauthvalue:	tss2/tss.h policyauthvalue.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policyauthvalue.o $(LNALIBS) -o policyauthvalue
policycommandcode:	tss2/tss.h policycommandcode.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policycommandcode.o $(LNALIBS) -o policycommandcode
policycphash:		tss2/tss.h policycphash.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policycphash.o $(LNALIBS) -o policycphash
policycountertimer :	tss2/tss.h policycountertimer.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policycountertimer.o $(LNALIBS) -o policycountertimer
policygetdigest:	tss2/tss.h policygetdigest.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policygetdigest.o $(LNALIBS) -o policygetdigest
policymaker:		tss2/tss.h policymaker.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policymaker.o $(LNALIBS) -o policymaker
policymakerpcr:		tss2/tss.h policymakerpcr.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policymakerpcr.o $(LNALIBS) -o policymakerpcr
policynv:		tss2/tss.h policynv.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policynv.o $(LNALIBS) -o policynv
policynvwritten:	tss2/tss.h policynvwritten.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policynvwritten.o $(LNALIBS) -o policynvwritten
policyor:		tss2/tss.h policyor.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policyor.o $(LNALIBS) -o policyor
policypassword:		tss2/tss.h policypassword.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policypassword.o $(LNALIBS) -o policypassword
policypcr:		tss2/tss.h policypcr.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policypcr.o $(LNALIBS) -o policypcr
policyrestart:		tss2/tss.h policyrestart.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policyrestart.o $(LNALIBS) -o policyrestart
policysigned:		tss2/tss.h policysigned.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policysigned.o $(LNALIBS) -o policysigned
policysecret:		tss2/tss.h policysecret.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policysecret.o $(LNALIBS) -o policysecret
policyticket:		tss2/tss.h policyticket.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) policyticket.o $(LNALIBS) -o policyticket
quote:			tss2/tss.h quote.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) quote.o $(LNALIBS) -o quote
powerup:		tss2/tss.h powerup.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) powerup.o $(LNALIBS) -o powerup
readclock:		tss2/tss.h readclock.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) readclock.o $(LNALIBS) -o readclock
readpublic:		tss2/tss.h readpublic.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) readpublic.o $(LNALIBS) -o readpublic
returncode:		tss2/tss.h returncode.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) returncode.o $(LNALIBS) -o returncode
rewrap:			tss2/tss.h rewrap.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) rewrap.o $(LNALIBS) -o rewrap
rsadecrypt: 		tss2/tss.h rsadecrypt.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) rsadecrypt.o $(LNALIBS) -o rsadecrypt
rsaencrypt: 		tss2/tss.h rsaencrypt.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) rsaencrypt.o $(LNALIBS) -o rsaencrypt
sequenceupdate:		tss2/tss.h sequenceupdate.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) sequenceupdate.o $(LNALIBS) -o sequenceupdate
sequencecomplete:	tss2/tss.h sequencecomplete.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) sequencecomplete.o $(LNALIBS) -o sequencecomplete
setprimarypolicy:	tss2/tss.h setprimarypolicy.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) setprimarypolicy.o $(LNALIBS) -o setprimarypolicy
shutdown:		tss2/tss.h shutdown.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) shutdown.o $(LNALIBS) -o shutdown
sign:			tss2/tss.h sign.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) sign.o $(LNALIBS) -o sign
startauthsession:	tss2/tss.h startauthsession.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) startauthsession.o $(LNALIBS) -o startauthsession
startup:		tss2/tss.h startup.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) startup.o $(LNALIBS) -o startup
stirrandom:		tss2/tss.h stirrandom.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) stirrandom.o $(LNALIBS) -o stirrandom
unseal:			tss2/tss.h unseal.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) unseal.o $(LNALIBS) -o unseal
verifysignature:	tss2/tss.h verifysignature.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) verifysignature.o $(LNALIBS) -o verifysignature
signapp:		tss2/tss.h signapp.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) signapp.o $(LNALIBS) -o signapp
writeapp:		tss2/tss.h writeapp.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) writeapp.o $(LNALIBS) -o writeapp
createek:		createek.o ekutils.o libtss.so
			$(CC) $(LNFLAGS) $(LNAFLAGS) createek.o ekutils.o $(LNALIBS) -o createek


t:		t.o
		$(CC) $(LNFLAGS) $(LNAFLAGS) t.o $(LNALIBS) -o t

# for applications, not for TSS library

%.o:		%.c tss2/tss.h 
		$(CC) $(CCFLAGS) $(CCAFLAGS) $< -o $@

