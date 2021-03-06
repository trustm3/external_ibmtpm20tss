/********************************************************************************/
/*										*/
/*			    Create Primary	 				*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: createprimary.c 714 2016-08-11 21:46:03Z kgoldman $		*/
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
#include <tss2/tssmarshal.h>
#include <tss2/tssprint.h>

#include "objecttemplates.h"

static void printUsage(void);

int verbose = FALSE;

int main(int argc, char *argv[])
{
    TPM_RC			rc = 0;
    int				i;    /* argc iterator */
    TSS_CONTEXT			*tssContext = NULL;
    CreatePrimary_In 		in;
    CreatePrimary_Out 		out;
    char 			hierarchyChar = 'n';
    TPMI_RH_HIERARCHY		primaryHandle = TPM_RH_NULL;
    TPMA_OBJECT			objectAttributes;
    int				keyType = TYPE_ST;
    uint32_t 			keyTypeSpecified = 0;
    int				rev116 = FALSE;
    const char 			*uniqueFilename = NULL;
    TPMI_ALG_PUBLIC 		algPublic = TPM_ALG_RSA;
    TPMI_ALG_HASH		halg = TPM_ALG_SHA256;
    TPMI_ALG_HASH		nalg = TPM_ALG_SHA256;
    TPMI_ECC_CURVE		curveID = TPM_ECC_NONE;
    const char			*policyFilename = NULL;
    const char			*publicKeyFilename = NULL;
    const char 			*dataFilename = NULL;
    const char			*keyPassword = NULL; 
    const char			*parentPassword = NULL; 
    const char			*parentPasswordFilename = NULL; 
    const char			*parentPasswordPtr = NULL; 
    uint8_t			*parentPasswordBuffer = NULL;		/* for the free */
    size_t 			parentPasswordLength = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle0 = TPM_RS_PW;
    unsigned int		sessionAttributes0 = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle1 = TPM_RH_NULL;
    unsigned int		sessionAttributes1 = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle2 = TPM_RH_NULL;
    unsigned int		sessionAttributes2 = 0;
    
    TSS_SetProperty(NULL, TPM_TRACE_LEVEL, "1");

    /* command line argument defaults */
    objectAttributes.val = 0;
    objectAttributes.val |= TPMA_OBJECT_NODA;
    objectAttributes.val |= TPMA_OBJECT_SENSITIVEDATAORIGIN;
    objectAttributes.val |= TPMA_OBJECT_USERWITHAUTH;
    objectAttributes.val &= ~TPMA_OBJECT_ADMINWITHPOLICY;
    objectAttributes.val |= TPMA_OBJECT_RESTRICTED;
    objectAttributes.val |= TPMA_OBJECT_DECRYPT;
    objectAttributes.val &= ~TPMA_OBJECT_SIGN;
    objectAttributes.val |= TPMA_OBJECT_FIXEDTPM;
    objectAttributes.val |= TPMA_OBJECT_FIXEDPARENT;

    for (i=1 ; (i<argc) && (rc == 0) ; i++) {
	if (strcmp(argv[i],"-hi") == 0) {
	    i++;
	    if (i < argc) {
		hierarchyChar = argv[i][0];
	    }
	    else {
		printf("Missing parameter for -hi\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i], "-bl") == 0) {
	    keyType = TYPE_BL;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-den") == 0) {
	    keyType = TYPE_DEN;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-deo") == 0) {
	    keyType = TYPE_DEO;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-des") == 0) {
	    keyType = TYPE_DES;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-st") == 0) {
	    keyType = TYPE_ST;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-si") == 0) {
	    keyType = TYPE_SI;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-sir") == 0) {
	    keyType = TYPE_SIR;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-kh") == 0) {
	    keyType = TYPE_KH;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-gp") == 0) {
	    keyType = TYPE_GP;
	    keyTypeSpecified++;
	}
	else if (strcmp(argv[i], "-116") == 0) {
	    rev116 = TRUE;
	}
	else if (strcmp(argv[i], "-rsa") == 0) {
	    algPublic = TPM_ALG_RSA;
	}
	else if (strcmp(argv[i], "-ecc") == 0) {
	    algPublic = TPM_ALG_ECC;
	    i++;
	    if (i < argc) {
		if (strcmp(argv[i],"bnp256") == 0) {
		    curveID = TPM_ECC_BN_P256;
		}
		else if (strcmp(argv[i],"nistp256") == 0) {
		    curveID = TPM_ECC_NIST_P256;
		}
		else if (strcmp(argv[i],"nistp384") == 0) {
		    curveID = TPM_ECC_NIST_P384;
		}
		else {
		    printf("Bad parameter for -ecc\n");
		    printUsage();
		}
	    }
	    else {
		printf("-ecc option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i], "-kt") == 0) {
	    i++;
	    if (i < argc) {
		switch (argv[i][0]) {
		  case 'f':
		    objectAttributes.val |= TPMA_OBJECT_FIXEDTPM;
		    break;
		  case 'p':
		    objectAttributes.val |= TPMA_OBJECT_FIXEDPARENT;
		    break;
		  default:
		    printf("Bad parameter for -kt\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -kt\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i], "-da") == 0) {
	    objectAttributes.val &= ~TPMA_OBJECT_NODA;
	}
	else if (strcmp(argv[i],"-halg") == 0) {
	    i++;
	    if (i < argc) {
		if (strcmp(argv[i],"sha1") == 0) {
		    halg = TPM_ALG_SHA1;
		}
		else if (strcmp(argv[i],"sha256") == 0) {
		    halg = TPM_ALG_SHA256;
		}
		else if (strcmp(argv[i],"sha384") == 0) {
		    halg = TPM_ALG_SHA384;
		}
		else {
		    printf("Bad parameter for -halg\n");
		    printUsage();
		}
	    }
	    else {
		printf("-halg option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-nalg") == 0) {
	    i++;
	    if (i < argc) {
		if (strcmp(argv[i],"sha1") == 0) {
		    nalg = TPM_ALG_SHA1;
		}
		else if (strcmp(argv[i],"sha256") == 0) {
		    nalg = TPM_ALG_SHA256;
		}
		else if (strcmp(argv[i],"sha384") == 0) {
		    nalg = TPM_ALG_SHA384;
		}
		else {
		    printf("Bad parameter for -nalg\n");
		    printUsage();
		}
	    }
	    else {
		printf("-nalg option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-pwdp") == 0) {
	    i++;
	    if (i < argc) {
		parentPassword = argv[i];
	    }
	    else {
		printf("-pwdp option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-pwdpi") == 0) {
	    i++;
	    if (i < argc) {
		parentPasswordFilename = argv[i];
	    }
	    else {
		printf("-pwdpi option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-pwdk") == 0) {
	    i++;
	    if (i < argc) {
		keyPassword = argv[i];
	    }
	    else {
		printf("-pwdk option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-iu") == 0) {
	    i++;
	    if (i < argc) {
		uniqueFilename = argv[i];
	    }
	    else {
		printf("-iu option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-opu") == 0) {
	    i++;
	    if (i < argc) {
		publicKeyFilename = argv[i];
	    }
	    else {
		printf("-opu option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-pol") == 0) {
	    i++;
	    if (i < argc) {
		policyFilename = argv[i];
	    }
	    else {
		printf("-pol option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-if") == 0) {
	    i++;
	    if (i < argc) {
		dataFilename = argv[i];
	    }
	    else {
		printf("-if option needs a value\n");
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
    if (keyTypeSpecified > 1) {
	printf("Too many key attributes\n");
	printUsage();
    }
    switch (keyType) {
      case TYPE_BL:
	if (dataFilename == NULL) {
	    printf("-bl needs -if (sealed data object needs data to seal)\n");
	    printUsage();
	}
	break;
      case TYPE_ST:
      case TYPE_DEN:
      case TYPE_DEO:
      case TYPE_SI:
      case TYPE_SIR:
      case TYPE_GP:
	if (dataFilename != NULL) {
	    printf("asymmetric key cannot have -if (sensitive data)\n");
	    printUsage();
	}
      case TYPE_DES:
      case TYPE_KH:
	/* inSensitive optional for symmetric keys */
	break;
    }
    if (rc == 0) {
	if ((parentPassword != NULL) && (parentPasswordFilename != NULL)) {
	    printf("Cannot specify both -pwdp and -pwdpi\n");
	    printUsage();
	}
    }
    if (rc == 0) {
	/* command auth from string */
	if (parentPassword != NULL) {
	    parentPasswordPtr = parentPassword; 
	}
	/* command parent from file */
	else if (parentPasswordFilename != NULL) {
	    if (rc == 0) {
		/* must be freed by caller */
		rc = TSS_File_ReadBinaryFile(&parentPasswordBuffer,
					     &parentPasswordLength,
					     parentPasswordFilename);
	    }
	    if (rc == 0) {
		if (parentPasswordLength > sizeof(TPMU_HA)) {
		    printf("Password too long %u\n", (unsigned int)parentPasswordLength);
		    rc = TSS_RC_INSUFFICIENT_BUFFER;
		}
	    }
	    if (rc == 0) {
		parentPasswordPtr = (const char *)parentPasswordBuffer;
	    }
	}
	/* no command parent specified */
	else {
	    parentPasswordPtr = NULL;
	}
    }
    /* Table 50 - TPMI_RH_HIERARCHY primaryHandle */
    if (rc == 0) {
	if (hierarchyChar == 'e') {
	    primaryHandle = TPM_RH_ENDORSEMENT;
	}
	else if (hierarchyChar == 'o') {
	    primaryHandle = TPM_RH_OWNER;
	}
	else if (hierarchyChar == 'p') {
	    primaryHandle = TPM_RH_PLATFORM;
	}
	else if (hierarchyChar == 'n') {
	    primaryHandle = TPM_RH_NULL;
	}
	else {
	    printf("Bad parameter %c for -hi\n", hierarchyChar);
	    printUsage();
	}
	in.primaryHandle = primaryHandle;
    }
    /* Table 134 - TPM2B_SENSITIVE_CREATE inSensitive */
    if (rc == 0) {
	/* Table 133 - TPMS_SENSITIVE_CREATE */
	{
	    if (keyPassword == NULL) {
		in.inSensitive.t.sensitive.userAuth.t.size = 0;
	    }
	    else {
		rc = TSS_TPM2B_StringCopy(&in.inSensitive.t.sensitive.userAuth.b,
					  keyPassword, sizeof(TPMU_HA));
	    }
	}
    }
    if (rc == 0) {
	/* Table 132 - Definition of TPM2B_SENSITIVE_DATA Structure data */
	if (dataFilename != NULL) {
	    rc = TSS_File_Read2B(&in.inSensitive.t.sensitive.data.b,
				 MAX_SYM_DATA,
				 dataFilename);
	}
	else {
	    in.inSensitive.t.sensitive.data.t.size = 0;
	}
    }
    /* Table 185 - TPM2B_PUBLIC	inPublic */
    if (rc == 0) {
	switch (keyType) {
	  case TYPE_BL:
	    rc = blPublicTemplate(&in.inPublic.t.publicArea, objectAttributes,
				  nalg,
				  policyFilename);
	    break;
	  case TYPE_ST:
	  case TYPE_DEN:
	  case TYPE_DEO:
	  case TYPE_SI:
	  case TYPE_SIR:
	  case TYPE_GP:
	    rc = asymPublicTemplate(&in.inPublic.t.publicArea, objectAttributes,
				    keyType, algPublic, curveID, nalg, halg,
				    policyFilename);
	    break;
	  case TYPE_DES:
	    rc = symmetricCipherTemplate(&in.inPublic.t.publicArea, objectAttributes,
					 nalg, rev116,
					 policyFilename);
	    break;
	  case TYPE_KH:
	    rc = keyedHashPublicTemplate(&in.inPublic.t.publicArea, objectAttributes,
					 nalg, halg,
					 policyFilename);
	}
    }
    /* Table 177 - TPMU_PUBLIC_ID unique */
    /* Table 158 - TPM2B_PUBLIC_KEY_RSA rsa */
    if (rc == 0) {
	if (uniqueFilename != NULL) {
	    rc = TSS_File_Read2B(&in.inPublic.t.publicArea.unique.rsa.b,
				 MAX_RSA_KEY_BYTES,
				 uniqueFilename);
	}
	else {
	    in.inPublic.t.publicArea.unique.rsa.t.size = 0;
	}
    }
    /* TPM2B_DATA outsideInfo */
    if (rc == 0) {
	in.outsideInfo.t.size = 0;
    }
    /* Table 102 - TPML_PCR_SELECTION */
    /* TPML_PCR_SELECTION	creationPCR */
    if (rc == 0) {
	in.creationPCR.count = 0;
    }
    /* Start a TSS context */
    if (rc == 0) {
	rc = TSS_Create(&tssContext);
    }
    /* call TSS to execute the command */
    if (rc == 0) {
	rc = TSS_Execute(tssContext,
			 (RESPONSE_PARAMETERS *)&out,
			 (COMMAND_PARAMETERS *)&in,
			 NULL,
			 TPM_CC_CreatePrimary,
			 sessionHandle0, parentPasswordPtr, sessionAttributes0,
			 sessionHandle1, NULL, sessionAttributes1,
			 sessionHandle2, NULL, sessionAttributes2,
			 TPM_RH_NULL, NULL, 0);
    }
    {
	TPM_RC rc1 = TSS_Delete(tssContext);
	if (rc == 0) {
	    rc = rc1;
	}
    }
    /* save the public key */
    if ((rc == 0) && (publicKeyFilename != NULL)) {
	rc = TSS_File_WriteStructure(&out.outPublic,
				     (MarshalFunction_t)TSS_TPM2B_PUBLIC_Marshal,
				     publicKeyFilename);
    }
    if (rc == 0) {
	printf("Handle %08x\n", out.objectHandle);
	if (verbose) TSS_PrintAll("createprimary: public key",
				  out.outPublic.t.publicArea.unique.rsa.t.buffer,
				  out.outPublic.t.publicArea.unique.rsa.t.size);
	if (verbose) printf("createprimary: success\n");
    }
    else {
	const char *msg;
	const char *submsg;
	const char *num;
	printf("createprimary: failed, rc %08x\n", rc);
	TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	printf("%s%s%s\n", msg, submsg, num);
	rc = EXIT_FAILURE;
    }
    free(parentPasswordBuffer);
    parentPasswordBuffer = NULL;
    return rc;
}

static void printUsage(void)
{
    printf("\n");
    printf("createprimary creates a primary storage key\n");
    printf("\n");
    printf("Runs TPM2_CreatePrimary\n");
    printf("\n");
    printf("\t[-hi hierarchy e, o, p, n (default null)]\n");
    printf("\t[-pwdp password for hierarchy (default empty)]\n");
    printf("\t[-pwdpi password file name for hierarchy (default empty)]\n");
    printf("\t[-pwdk password for key (default empty)]\n");
    printf("\t[-iu inPublic unique field file (default none)]\n");
    printf("\t[-opu public key file name (default do not save)]\n");
    printf("\n");
    printUsageTemplate();
    printf("\n");
    printf("\t-se[0-2] session handle / attributes (default PWAP)\n");
    printf("\t\t01 continue\n");
    printf("\t\t20 command decrypt\n");
    printf("\t\t40 response encrypt\n");
    exit(1);	
}
