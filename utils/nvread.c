/********************************************************************************/
/*										*/
/*			    NV Read		 				*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: nvread.c 730 2016-08-23 21:09:53Z kgoldman $			*/
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

/* 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <tss2/tss.h>
#include <tss2/tssutils.h>
#include <tss2/tssresponsecode.h>
#include <tss2/tssprint.h>

static TPM_RC readNvBufferMax(TSS_CONTEXT *tssContext,
			      uint32_t *nvBufferMax);
static void printUsage(void);

int verbose = FALSE;

int main(int argc, char *argv[])
{
    TPM_RC			rc = 0;
    int				i;    /* argc iterator */
    TSS_CONTEXT			*tssContext = NULL;
    NV_Read_In 			in;
    NV_Read_Out			out;
    uint16_t 			offset = 0;			/* default 0 */
    uint16_t 			readLength = 0;			/* bytes to read */
    char 			hierarchyAuthChar = 0;
    const char 			*datafilename = NULL;
    TPMI_RH_NV_INDEX		nvIndex = 0;
    const char			*nvPassword = NULL; 		/* default no password */
    TPMI_SH_AUTH_SESSION    	sessionHandle0 = TPM_RS_PW;
    unsigned int		sessionAttributes0 = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle1 = TPM_RH_NULL;
    unsigned int		sessionAttributes1 = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle2 = TPM_RH_NULL;
    unsigned int		sessionAttributes2 = 0;
    unsigned char 		*readBuffer = NULL; 
    uint32_t 			nvBufferMax;
    uint16_t 			bytesRead;			/* bytes read so far */
    int				done = FALSE;
   
    TSS_SetProperty(NULL, TPM_TRACE_LEVEL, "1");

    for (i=1 ; (i<argc) && (rc == 0) ; i++) {
	if (strcmp(argv[i],"-pwdn") == 0) {
	    i++;
	    if (i < argc) {
		nvPassword = argv[i];
	    }
	    else {
		printf("-pwdn option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-hia") == 0) {
	    i++;
	    if (i < argc) {
		hierarchyAuthChar = argv[i][0];
	    }
	    else {
		printf("Missing parameter for -hia\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-ha") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &nvIndex);
	    }
	    else {
		printf("Missing parameter for -ha\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i], "-of")  == 0) {
	    i++;
	    if (i < argc) {
		datafilename = argv[i];
	    } else {
		printf("-of option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-off") == 0) {
	    i++;
	    if (i < argc) {
		offset = atoi(argv[i]);
		/* FIXME range check */
	    }
	    else {
		printf("-off option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-sz") == 0) {
	    i++;
	    if (i < argc) {
		readLength = atoi(argv[i]);
	    }
	    else {
		printf("-sz option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se0") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle0);
	    }
	    else {
		printf("Missing parameter for -se0\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes0);
		if (sessionAttributes0 > 0xff) {
		    printf("Out of range session attributes for -se0\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se0\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se1") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle1);
	    }
	    else {
		printf("Missing parameter for -se1\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes1);
		if (sessionAttributes1 > 0xff) {
		    printf("Out of range session attributes for -se1\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se1\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se2") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle2);
	    }
	    else {
		printf("Missing parameter for -se2\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes2);
		if (sessionAttributes2 > 0xff) {
		    printf("Out of range session attributes for -se2\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se2\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-h") == 0) {
	    printUsage();
	}
	else if (strcmp(argv[i],"-v") == 0) {
	    verbose = TRUE;
	    TSS_SetProperty(NULL, TPM_TRACE_LEVEL, "2");
	}
	else {
	    printf("\n%s is not a valid option\n", argv[i]);
	    printUsage();
	}
    }
    if ((nvIndex >> 24) != TPM_HT_NV_INDEX) {
	printf("NV index handle not specified or out of range, MSB not 01\n");
	printUsage();
    }
    /* Authorization handle */
    if (rc == 0) {
	if (hierarchyAuthChar == 'o') {
	    in.authHandle = TPM_RH_OWNER;  
	}
	else if (hierarchyAuthChar == 'p') {
	    in.authHandle = TPM_RH_PLATFORM;  
	}
	else if (hierarchyAuthChar == 0) {
	    in.authHandle = nvIndex;
	}
	else {
	    printf("\n");
	    printUsage();
	}
    }
    if (rc == 0) {
	if (readLength > 0) {	
	    readBuffer = malloc(readLength);		/* freed @1 */
	    if (readBuffer == NULL) {
		printf("Cannot malloc %u bytes for read buffer\n", readLength);
		exit(1);	
	    }
	}
	else {
	    readBuffer = NULL;
	}
    }
    /* Start a TSS context */
    if (rc == 0) {
	rc = TSS_Create(&tssContext);
    }
    /* data may have to be read in chunks.  Read the TPM_PT_NV_BUFFER_MAX, the chunk size */
    if (rc == 0) {
	rc = readNvBufferMax(tssContext,
			     &nvBufferMax);
    }    
    if (rc == 0) {
	in.nvIndex = nvIndex;
	in.offset = offset;	/* start at supplied offset */
	bytesRead = 0;		/* bytes read so far */
    }
    /* call TSS to execute the command */
    while ((rc == 0) && !done) {
	if (rc == 0) {
	    /* read a chunk */
	    in.offset += bytesRead;
	    if ((uint32_t)(readLength - bytesRead) < nvBufferMax) {
		in.size = readLength - bytesRead;	/* last chunk */
	    }
	    else {
		in.size = nvBufferMax;		/* next chunk */
	    }
	}
	if (rc == 0) {
	    rc = TSS_Execute(tssContext,
			     (RESPONSE_PARAMETERS *)&out,
			     (COMMAND_PARAMETERS *)&in,
			     NULL,
			     TPM_CC_NV_Read,
			     sessionHandle0, nvPassword, sessionAttributes0,
			     sessionHandle1, NULL, sessionAttributes1,
			     sessionHandle2, NULL, sessionAttributes2,
			     TPM_RH_NULL, NULL, 0);
	}
	/* copy the results to the read buffer */
	if (rc == 0) {
	    memcpy(readBuffer + bytesRead, out.data.b.buffer, out.data.b.size);
	    bytesRead += out.data.b.size;
	    if (bytesRead == readLength) {
		done = TRUE;
	    }
	}
    }
    {
	TPM_RC rc1 = TSS_Delete(tssContext);
	if (rc == 0) {
	    rc = rc1;
	}
    }
    if ((rc == 0) && (datafilename != NULL)) {
	rc = TSS_File_WriteBinaryFile(readBuffer, readLength, datafilename);
    }
    if (rc == 0) {
	if (verbose) printf("nvread: success\n");
	TSS_PrintAll("nvread: data", readBuffer, readLength);
    }
    else {
	const char *msg;
	const char *submsg;
	const char *num;
	printf("nvread: failed, rc %08x\n", rc);
	TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	printf("%s%s%s\n", msg, submsg, num);
	rc = EXIT_FAILURE;
    }
    free(readBuffer);	/* @1 */
    return rc;
}

static TPM_RC readNvBufferMax(TSS_CONTEXT *tssContext,
			      uint32_t *nvBufferMax)
{
    TPM_RC			rc = 0;
    GetCapability_In 		in;
    GetCapability_Out		out;

    in.capability = TPM_CAP_TPM_PROPERTIES;
    in.property = TPM_PT_NV_BUFFER_MAX;
    in.propertyCount = 1;	/* ask for one property */
    if (rc == 0) {
	rc = TSS_Execute(tssContext,
			 (RESPONSE_PARAMETERS *)&out, 
			 (COMMAND_PARAMETERS *)&in,
			 NULL,
			 TPM_CC_GetCapability,
			 TPM_RH_NULL, NULL, 0);
    }
    /* sanity check that the property name is correct, demo of how to parse the structure */
    if (rc == 0) {
	if (out.capabilityData.data.tpmProperties.tpmProperty[0].property == TPM_PT_NV_BUFFER_MAX) {
	    *nvBufferMax = out.capabilityData.data.tpmProperties.tpmProperty[0].value;
	    if (verbose) printf("readNvBufferMax: %u\n", *nvBufferMax);
	}
	else {
	    printf("readNvBufferMax: wrong property returned: %08x\n",
		   out.capabilityData.data.tpmProperties.tpmProperty[0].property);
	    *nvBufferMax = 512;
	}
    }
    else {
	const char *msg;
	const char *submsg;
	const char *num;
	printf("getcapability: failed, rc %08x\n", rc);
	TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	printf("%s%s%s\n", msg, submsg, num);
	rc = EXIT_FAILURE;
    }
    return rc;
}

static void printUsage(void)
{
    printf("\n");
    printf("nvread\n");
    printf("\n");
    printf("Runs TPM2_NV_Read\n");
    printf("\n");
    printf("\t[-hia hierarchy authorization (o, p)(default index authorization)]\n");
    printf("\t-ha NV index handle\n");
    printf("\t[-pwdn password for NV index (default empty)]\n");
    printf("\t[-sz data size (default 0)]\n");
    printf("\t[-off offset (default 0)]\n");
    printf("\t[-of data file]\n");
    printf("\n");
    printf("\t-se[0-2] session handle / attributes (default PWAP)\n");
    printf("\t\t01 continue\n");
    exit(1);	
}
