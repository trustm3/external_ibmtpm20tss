/********************************************************************************/
/*										*/
/*			    TSS Configuration Properties			*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: tssproperties.c 682 2016-07-15 18:49:19Z kgoldman $		*/
/*										*/
/* (c) Copyright IBM Corporation 2015.						*/
/*										*/
/* All rights reserved.								*/
/* 										*/
/* Redistribution and use in source and binary forms, with or without		*/
/* modification, are permitted provided that the following conditions are	*/
/* met:										*/
/* 										*/
/* Redistributions of source code must retain the above copyright notice,	*/
/* this list of conditions and the following disclaimer.			*/
/* 										*/
/* Redistributions in binary form must reproduce the above copyright		*/
/* notice, this list of conditions and the following disclaimer in the		*/
/* documentation and/or other materials provided with the distribution.		*/
/* 										*/
/* Neither the names of the IBM Corporation nor the names of its		*/
/* contributors may be used to endorse or promote products derived from		*/
/* this software without specific prior written permission.			*/
/* 										*/
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS		*/
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT		*/
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR	*/
/* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT		*/
/* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,	*/
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT		*/
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,	*/
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY	*/
/* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT		*/
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE	*/
/* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.		*/
/********************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>

#include <tss2/tss.h>
#include <tss2/tsstransmit.h>
#include <tss2/tsscrypto.h>

#include <tss2/tssproperties.h>

/* local prototypes */

static TPM_RC TSS_SetTraceLevel(const char *value);
static TPM_RC TSS_SetDataDirectory(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetCommandPort(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetPlatformPort(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetCommandUds(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetPlatformUds(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetServerName(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetServerType(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetInterfaceType(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetDevice(TSS_CONTEXT *tssContext, const char *value);
static TPM_RC TSS_SetEncryptSessions(TSS_CONTEXT *tssContext, const char *value);

/* globals for the library */

/* tracing is global to avoid passing the context into every function call */
int tssVerbose = TRUE;		/* initial value so TSS_Properties_Init errors emit message */
int tssVverbose = FALSE;

/* This is a total hack to ensure that the global verbose flags are only set once.  It's used by the
   two entry points to the TSS, TSS_Create() and TSS_SetProperty() */

int tssFirstCall = TRUE;

/* defaults for global settings */

#ifndef TPM_TRACE_LEVEL_DEFAULT 	
#define TPM_TRACE_LEVEL_DEFAULT 	"0"
#endif

#ifndef TPM_COMMAND_PORT_DEFAULT
#define TPM_COMMAND_PORT_DEFAULT 	"2321"		/* default for MS simulator */
#endif

#ifndef TPM_PLATFORM_PORT_DEFAULT
#define TPM_PLATFORM_PORT_DEFAULT 	"2322"		/* default for MS simulator */
#endif

#ifndef TPM_COMMAND_UDS_DEFAULT
#define TPM_COMMAND_UDS_DEFAULT		"/data/tpm2_simulator/tpm2_commands.uds"	/* default for IDS */
#endif

#ifndef TPM_PLATFORM_UDS_DEFAULT
#define TPM_PLATFORM_UDS_DEFAULT	"/data/tpm2_simulator/tpm2_platform.uds"	/* default for IDS */
#endif

#ifndef TPM_SERVER_NAME_DEFAULT
#define TPM_SERVER_NAME_DEFAULT		"localhost"	/* default to local machine */
#endif

#ifndef TPM_SERVER_TYPE_DEFAULT
#define TPM_SERVER_TYPE_DEFAULT		"mssim"		/* default to MS simulator format */
#endif

#ifndef TPM_DATA_DIR_DEFAULT
#define TPM_DATA_DIR_DEFAULT		"."		/* default to current working directory */
#endif

#ifndef TPM_INTERFACE_TYPE_DEFAULT
#define TPM_INTERFACE_TYPE_DEFAULT	"socsim"	/* default to MS simulator interface */
#endif

#ifndef TPM_DEVICE_DEFAULT
#ifdef TPM_POSIX
#define TPM_DEVICE_DEFAULT		"/dev/tpm0"	/* default to Linux device driver */
#endif
#ifdef TPM_WINDOWS
#define TPM_DEVICE_DEFAULT		"tddl.dll"	/* default to Windows TPM interface dll */
#endif
#endif

#ifndef TPM_ENCRYPT_SESSIONS_DEFAULT
#define TPM_ENCRYPT_SESSIONS_DEFAULT	"1"
#endif

/* TSS_GlobalProperties_Init() sets the global verbose trace flags at the first entry points to the
   TSS */

TPM_RC TSS_GlobalProperties_Init(void)
{
    TPM_RC		rc = 0;
    const char 		*value;

    /* trace level is global, tssContext can be null */
    if (rc == 0) {
	value = getenv("TPM_TRACE_LEVEL");
	rc = TSS_SetTraceLevel(value);
    }
    return rc;
}


/* TSS_Properties_Init() sets the initial TSS_CONTEXT properties based on either the environment
   variables (if set) or the defaults (if not).
*/

TPM_RC TSS_Properties_Init(TSS_CONTEXT *tssContext)
{
    TPM_RC		rc = 0;
    const char 		*value;

    if (rc == 0) {
	tssContext->tssAuthContext = NULL;
	tssContext->tssFirstTransmit = TRUE;	/* connection not opened */
#ifdef TPM_WINDOWS
	tssContext->sock_fd = INVALID_SOCKET;
#endif
#ifdef TPM_POSIX
	tssContext->sock_fd = -1;
#endif
	tssContext->dev_fd = -1;
#ifdef TPM_WINDOWS
#ifdef TPM_WINDOWS_TBSI
	tssContext->hContext = 0;	/* FIXME:  Guess at an illegal value */
#endif
#endif
    }
    /* data directory */
    if (rc == 0) {
	value = getenv("TPM_DATA_DIR");
	rc = TSS_SetDataDirectory(tssContext, value);
    }
    /* flag whether session state should be encrypted */
    if (rc == 0) {
	value = getenv("TPM_ENCRYPT_SESSIONS");
	rc = TSS_SetEncryptSessions(tssContext, value);
    }
    /* TPM socket command port */
    if (rc == 0) {
	value = getenv("TPM_COMMAND_PORT");
	rc = TSS_SetCommandPort(tssContext, value);
    }
    /* TPM simulator socket platform port */
    if (rc == 0) {
	value = getenv("TPM_PLATFORM_PORT");
	rc = TSS_SetPlatformPort(tssContext, value);
    }
    /* TPM socket command UDS */
    if (rc == 0) {
       value = getenv("TPM_COMMAND_UDS");
       rc = TSS_SetCommandUds(tssContext, value);
    }
    /* TPM simulator socket platform UDS */
    if (rc == 0) {
       value = getenv("TPM_PLATFORM_UDS");
       rc = TSS_SetPlatformUds(tssContext, value);
    }
    /* TPM socket host name */
    if (rc == 0) {
	value = getenv("TPM_SERVER_NAME");
	rc = TSS_SetServerName(tssContext, value);
    }
    /* TPM socket server type */
    if (rc == 0) {
	value = getenv("TPM_SERVER_TYPE");
	rc = TSS_SetServerType(tssContext, value);
    }
    /* TPM interface type */
    if (rc == 0) {
	value = getenv("TPM_INTERFACE_TYPE");
	rc = TSS_SetInterfaceType(tssContext, value);
    }
    /* TPM device within the interface type */
    if (rc == 0) {
	value = getenv("TPM_DEVICE");
	rc = TSS_SetDevice(tssContext, value);
    }
    return rc;
}

/* TSS_SetProperty() sets the property to the value.

   The format of the property and value the same as that of the environment variable.

   A NULL value sets the property to the default.
*/

TPM_RC TSS_SetProperty(TSS_CONTEXT *tssContext,
		       int property,
		       const char *value)
{
    TPM_RC		rc = 0;

    /* at the first call to the TSS, initialize global variables */
    if (tssFirstCall) {
	/* crypto module initializations */
	if (rc == 0) {
	    rc = TSS_Crypto_Init();
	}
	if (rc == 0) {
	    rc = TSS_GlobalProperties_Init();
	}
	tssFirstCall = FALSE;
    }
    if (rc == 0) {
	switch (property) {
	  case TPM_TRACE_LEVEL:
	    rc = TSS_SetTraceLevel(value);
	    break;
	  case TPM_DATA_DIR:
	    rc = TSS_SetDataDirectory(tssContext, value);
	    break;
	  case TPM_COMMAND_PORT:	
	    rc = TSS_SetCommandPort(tssContext, value);
	    break;
	  case TPM_PLATFORM_PORT:	
	    rc = TSS_SetPlatformPort(tssContext, value);
	    break;
	  case TPM_COMMAND_UDS: 
	     rc = TSS_SetCommandUds(tssContext, value);
	     break;
	  case TPM_PLATFORM_UDS:        
	    rc = TSS_SetPlatformUds(tssContext, value);
	    break;
	  case TPM_SERVER_NAME:		
	    rc = TSS_SetServerName(tssContext, value);
	    break;
	  case TPM_SERVER_TYPE:		
	    rc = TSS_SetServerType(tssContext, value);
	    break;
	  case TPM_INTERFACE_TYPE:
	    rc = TSS_SetInterfaceType(tssContext, value);
	    break;
	  case TPM_DEVICE:
	    rc = TSS_SetDevice(tssContext, value);
	    break;
	  case TPM_ENCRYPT_SESSIONS:
	    rc = TSS_SetEncryptSessions(tssContext, value);
	    break;
	  default:
	    rc = TSS_RC_BAD_PROPERTY;
	}
    }
    return rc;
}

/* TSS_SetTraceLevel() sets the trace level.

   0:	no printing
   1:	error primting
   2:	trace printing
*/

static TPM_RC TSS_SetTraceLevel(const char *value)
{
    TPM_RC		rc = 0;
    int			irc;
    int 		level;

    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_TRACE_LEVEL_DEFAULT;
	}
    }
    if (rc == 0) {
	irc = sscanf(value, "%u", &level);
	if (irc != 1) {
	    if (tssVerbose) printf("TSS_SetTraceLevel: Error, value invalid\n");
	    rc = TSS_RC_BAD_PROPERTY_VALUE;
	}
    }
    if (rc == 0) {
	switch (level) {
	  case 0:
	    tssVerbose = FALSE;
	    tssVverbose = FALSE;
	    break;
	  case 1:
	    tssVerbose = TRUE;
	    tssVverbose = FALSE;
	    break;
	  default:
	    tssVerbose = TRUE;
	    tssVverbose = TRUE;
	    break;
	}
    }
    return rc;
}

static TPM_RC TSS_SetDataDirectory(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC		rc = 0;

    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_DATA_DIR_DEFAULT;
	}
    }
    if (rc == 0) {
	tssContext->tssDataDirectory = value;
	/* FIXME check length, don't hard code max length, use max path size */
    }
    return rc;
}

static TPM_RC TSS_SetCommandPort(TSS_CONTEXT *tssContext, const char *value)
{
    int			irc;
    TPM_RC		rc = 0;

    /* close an open connection before changing property */
    if (rc == 0) {
	rc = TSS_Close(tssContext);
    }
    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_COMMAND_PORT_DEFAULT;
	}
    }
    if (rc == 0) {
	irc = sscanf(value, "%hu", &tssContext->tssCommandPort);
	if (irc != 1) {
	    if (tssVerbose) printf("TSS_SetCommandPort: Error, value invalid\n");
	    rc = TSS_RC_BAD_PROPERTY_VALUE;
	}
    }
    return rc;
}

static TPM_RC TSS_SetPlatformPort(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC		rc = 0;
    int			irc;

    /* close an open connection before changing property */
    if (rc == 0) {
	rc = TSS_Close(tssContext);
    }
    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_PLATFORM_PORT_DEFAULT;
	}
    }
    if (rc == 0) {
	irc = sscanf(value, "%hu", &tssContext->tssPlatformPort);
	if (irc != 1) {
	    if (tssVerbose) printf("TSS_SetPlatformPort: Error, , value invalid\n");
	    rc = TSS_RC_BAD_PROPERTY_VALUE;
	}
    }
    return rc;
}

static TPM_RC TSS_SetCommandUds(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC             rc = 0;

    /* close an open connection before changing property */
    if (rc == 0) {
       rc = TSS_Close(tssContext);
    }
    if (rc == 0) {
       if (value == NULL) {
           value = TPM_COMMAND_UDS_DEFAULT;
       }
    }
    if (rc == 0) {
       tssContext->tssCommandUds = value;
    }
    return rc;
}

static TPM_RC TSS_SetPlatformUds(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC             rc = 0;

    /* close an open connection before changing property */
    if (rc == 0) {
       rc = TSS_Close(tssContext);
    }
    if (rc == 0) {
       if (value == NULL) {
           value = TPM_PLATFORM_UDS_DEFAULT;
       }
    }
    if (rc == 0) {
       tssContext->tssPlatformUds = value;
    }
    return rc;
}

static TPM_RC TSS_SetServerName(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC		rc = 0;

    /* close an open connection before changing property */
    if (rc == 0) {
	rc = TSS_Close(tssContext);
    }
    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_SERVER_NAME_DEFAULT;
	}
    }
    if (rc == 0) {
	tssContext->tssServerName = value;
    }
    return rc;
}

static TPM_RC TSS_SetServerType(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC		rc = 0;

    /* close an open connection before changing property */
    if (rc == 0) {
	rc = TSS_Close(tssContext);
    }
    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_SERVER_TYPE_DEFAULT;
	}
    }
    if (rc == 0) {
	tssContext->tssServerType = value;
    }
    return rc;
}

static TPM_RC TSS_SetInterfaceType(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC		rc = 0;

    /* close an open connection before changing property */
    if (rc == 0) {
	rc = TSS_Close(tssContext);
    }
    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_INTERFACE_TYPE_DEFAULT;
	}
    }
    if (rc == 0) {
	tssContext->tssInterfaceType = value;
    }
    return rc;
}

static TPM_RC TSS_SetDevice(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC		rc = 0;

    /* close an open connection before changing property */
    if (rc == 0) {
	rc = TSS_Close(tssContext);
    }
    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_DEVICE_DEFAULT;
	}
    }
    if (rc == 0) {
	tssContext->tssDevice = value;
    }
    return rc;
}

static TPM_RC TSS_SetEncryptSessions(TSS_CONTEXT *tssContext, const char *value)
{
    TPM_RC		rc = 0;
    int			irc;

    if (rc == 0) {
	if (value == NULL) {
	    value = TPM_ENCRYPT_SESSIONS_DEFAULT;
	}
    }
    if (rc == 0) {
	irc = sscanf(value, "%u", &tssContext->tssEncryptSessions);
	if (irc != 1) {
	    if (tssVerbose) printf("TSS_SetEncryptSessions: Error, value invalid\n");
	    rc = TSS_RC_BAD_PROPERTY_VALUE;
	}
    }
    return rc;
}
