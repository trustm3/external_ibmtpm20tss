/********************************************************************************/
/*										*/
/*			 Object Templates					*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id$			*/
/*										*/
/* (c) Copyright IBM Corporation 2016.						*/
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

/* These are templates suitable for creating typical objects.  The functions are shared by create
   and createprimary

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <tss2/tss.h>
#include <tss2/tssutils.h>
#include <tss2/tssresponsecode.h>
#include <tss2/tssmarshal.h>

#include "objecttemplates.h"

static TPM_RC getPolicy(TPMT_PUBLIC *publicArea,
			const char *policyFilename);

/* asymPublicTemplate() is a template for an ECC or RSA 2048 key.

   It can create these types:

   TYPE_ST:   RSA storage key
   TYPE_DEN:  RSA decryption key (not storage key, NULL scheme)
   TYPE_DEO:  RSA decryption key (not storage key, OAEP scheme)
   TYPE_SI:   signing key (unrestricted)
   TYPE_SIR:  signing key (restricted)
   TYPE_GP:   general purpose key

   If restricted, it uses the RSASSA padding scheme
*/

TPM_RC asymPublicTemplate(TPMT_PUBLIC *publicArea,	/* output */
			  TPMA_OBJECT objectAttributes,	/* default, can be overridden here */
			  int keyType,			/* see above */
			  TPMI_ALG_PUBLIC algPublic,	/* RSA or ECC */	
			  TPMI_ECC_CURVE curveID,	/* for ECC */
			  TPMI_ALG_HASH nalg,		/* Name algorithm */
			  TPMI_ALG_HASH halg,		/* hash algorithm */
			  const char *policyFilename)	/* binary policy, NULL means empty */
{
    TPM_RC			rc = 0;

    if (rc == 0) {
	publicArea->objectAttributes = objectAttributes;
	
	/* Table 185 - TPM2B_PUBLIC inPublic */
	/* Table 184 - TPMT_PUBLIC publicArea */
	publicArea->type = algPublic;		/* RSA or ECC */
	publicArea->nameAlg = nalg;

	/* Table 32 - TPMA_OBJECT objectAttributes */
	publicArea->objectAttributes.val |= TPMA_OBJECT_SENSITIVEDATAORIGIN;
	publicArea->objectAttributes.val |= TPMA_OBJECT_USERWITHAUTH;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_ADMINWITHPOLICY;

	switch (keyType) {
	  case TYPE_DEN:
	  case TYPE_DEO:
	    publicArea->objectAttributes.val &= ~TPMA_OBJECT_SIGN;
	    publicArea->objectAttributes.val |= TPMA_OBJECT_DECRYPT;
	    publicArea->objectAttributes.val &= ~TPMA_OBJECT_RESTRICTED;
	    break;
	  case TYPE_ST:
	    publicArea->objectAttributes.val &= ~TPMA_OBJECT_SIGN;
	    publicArea->objectAttributes.val |= TPMA_OBJECT_DECRYPT;
	    publicArea->objectAttributes.val |= TPMA_OBJECT_RESTRICTED;
	    break;
	  case TYPE_SI:
	    publicArea->objectAttributes.val |= TPMA_OBJECT_SIGN;
	    publicArea->objectAttributes.val &= ~TPMA_OBJECT_DECRYPT;
	    publicArea->objectAttributes.val &= ~TPMA_OBJECT_RESTRICTED;
	    break;
	  case TYPE_SIR:
	    publicArea->objectAttributes.val |= TPMA_OBJECT_SIGN;
	    publicArea->objectAttributes.val &= ~TPMA_OBJECT_DECRYPT;
	    publicArea->objectAttributes.val |= TPMA_OBJECT_RESTRICTED;
	    break;
	  case TYPE_GP:
	    publicArea->objectAttributes.val |= TPMA_OBJECT_SIGN;
	    publicArea->objectAttributes.val |= TPMA_OBJECT_DECRYPT;
	    publicArea->objectAttributes.val &= ~TPMA_OBJECT_RESTRICTED;
	    break;
	}
    }
    if (rc == 0) {
	/* Table 72 -  TPM2B_DIGEST authPolicy */
	/* policy set separately */

	/* Table 182 - Definition of TPMU_PUBLIC_PARMS parameters */
	if (algPublic == TPM_ALG_RSA) {
	    /* Table 180 - Definition of {RSA} TPMS_RSA_PARMS rsaDetail */
	    /* Table 129 - Definition of TPMT_SYM_DEF_OBJECT Structure symmetric */
	    switch (keyType) {
	      case TYPE_DEN:
	      case TYPE_DEO:
	      case TYPE_SI:
	      case TYPE_SIR:
	      case TYPE_GP:
		/* Non-storage keys must have TPM_ALG_NULL for the symmetric algorithm */
		publicArea->parameters.rsaDetail.symmetric.algorithm = TPM_ALG_NULL;
		break;
	      case TYPE_ST:
		publicArea->parameters.rsaDetail.symmetric.algorithm = TPM_ALG_AES;
		/* Table 125 - TPMU_SYM_KEY_BITS keyBits */
		publicArea->parameters.rsaDetail.symmetric.keyBits.aes = 128;
		/* Table 126 - TPMU_SYM_MODE mode */
		publicArea->parameters.rsaDetail.symmetric.mode.aes = TPM_ALG_CFB;
		break;
	    }

	    /* Table 155 - Definition of {RSA} TPMT_RSA_SCHEME scheme */
	    switch (keyType) {
	      case TYPE_DEN:
	      case TYPE_GP:
	      case TYPE_ST:
	      case TYPE_SI:
		publicArea->parameters.rsaDetail.scheme.scheme = TPM_ALG_NULL;
		break;
	      case TYPE_DEO:
		publicArea->parameters.rsaDetail.scheme.scheme = TPM_ALG_OAEP;
		/* Table 152 - Definition of TPMU_ASYM_SCHEME details */
		/* Table 152 - Definition of TPMU_ASYM_SCHEME rsassa */
		/* Table 142 - Definition of {RSA} Types for RSA Signature Schemes */
		/* Table 135 - Definition of TPMS_SCHEME_HASH hashAlg */
		publicArea->parameters.rsaDetail.scheme.details.oaep.hashAlg = halg;
		break;
	      case TYPE_SIR:
		publicArea->parameters.rsaDetail.scheme.scheme = TPM_ALG_RSASSA;
		/* Table 152 - Definition of TPMU_ASYM_SCHEME details */
		/* Table 152 - Definition of TPMU_ASYM_SCHEME rsassa */
		/* Table 142 - Definition of {RSA} Types for RSA Signature Schemes */
		/* Table 135 - Definition of TPMS_SCHEME_HASH hashAlg */
		publicArea->parameters.rsaDetail.scheme.details.rsassa.hashAlg = halg;
		break;
	    }
	
	    /* Table 159 - Definition of {RSA} (TPM_KEY_BITS) TPMI_RSA_KEY_BITS Type keyBits */
	    publicArea->parameters.rsaDetail.keyBits = 2048;
	    publicArea->parameters.rsaDetail.exponent = 0;
	    /* Table 177 - TPMU_PUBLIC_ID unique */
	    /* Table 177 - Definition of TPMU_PUBLIC_ID */
	    publicArea->unique.rsa.t.size = 0;
	}
	else {	/* algPublic == TPM_ALG_ECC */
	    /* Table 181 - Definition of {ECC} TPMS_ECC_PARMS Structure eccDetail */
	    /* Table 129 - Definition of TPMT_SYM_DEF_OBJECT Structure symmetric */
	    switch (keyType) {
	      case TYPE_DEN:
	      case TYPE_DEO:
	      case TYPE_SI:
	      case TYPE_SIR:
	      case TYPE_GP:
		/* Non-storage keys must have TPM_ALG_NULL for the symmetric algorithm */
		publicArea->parameters.eccDetail.symmetric.algorithm = TPM_ALG_NULL;
		break;
	      case TYPE_ST:
		publicArea->parameters.eccDetail.symmetric.algorithm = TPM_ALG_AES;
		/* Table 125 - TPMU_SYM_KEY_BITS keyBits */
		publicArea->parameters.eccDetail.symmetric.keyBits.aes = 128;
		/* Table 126 - TPMU_SYM_MODE mode */
		publicArea->parameters.eccDetail.symmetric.mode.aes = TPM_ALG_CFB;
		break;
	    }
	    /* Table 166 - Definition of (TPMT_SIG_SCHEME) {ECC} TPMT_ECC_SCHEME Structure scheme */
	    /* Table 164 - Definition of (TPM_ALG_ID) {ECC} TPMI_ALG_ECC_SCHEME Type scheme */
	    switch (keyType) {
	      case TYPE_GP:
	      case TYPE_SI:
		publicArea->parameters.eccDetail.scheme.scheme = TPM_ALG_NULL;
		/* Table 165 - Definition of {ECC} (TPM_ECC_CURVE) TPMI_ECC_CURVE Type */
		/* Table 10 - Definition of (UINT16) {ECC} TPM_ECC_CURVE Constants <IN/OUT, S> curveID */
		publicArea->parameters.eccDetail.curveID = curveID;
		/* Table 150 - Definition of TPMT_KDF_SCHEME Structure kdf */
		/* Table 64 - Definition of (TPM_ALG_ID) TPMI_ALG_KDF Type */
		publicArea->parameters.eccDetail.kdf.scheme = TPM_ALG_NULL;
		break;
	      case TYPE_SIR:
		publicArea->parameters.eccDetail.scheme.scheme = TPM_ALG_ECDSA;
		/* Table 152 - Definition of TPMU_ASYM_SCHEME details */
		/* Table 143 - Definition of {ECC} Types for ECC Signature Schemes */
		publicArea->parameters.eccDetail.scheme.details.ecdsa.hashAlg = halg;
		/* Table 165 - Definition of {ECC} (TPM_ECC_CURVE) TPMI_ECC_CURVE Type */
		/* Table 10 - Definition of (UINT16) {ECC} TPM_ECC_CURVE Constants <IN/OUT, S> curveID */
		publicArea->parameters.eccDetail.curveID = curveID;
		/* Table 150 - Definition of TPMT_KDF_SCHEME Structure kdf */
		/* Table 64 - Definition of (TPM_ALG_ID) TPMI_ALG_KDF Type */
		publicArea->parameters.eccDetail.kdf.scheme = TPM_ALG_NULL;
		/* Table 149 - Definition of TPMU_KDF_SCHEME Union <IN/OUT, S> */
		/* Table 148 - Definition of Types for KDF Schemes, hash-based key-
		   or mask-generation functions */
		/* Table 135 - Definition of TPMS_SCHEME_HASH Structure hashAlg */
		publicArea->parameters.eccDetail.kdf.details.mgf1.hashAlg = halg;
		break;
	      case TYPE_DEN:
	      case TYPE_DEO:
		printf("Decryption key type for ECC keys not implemented yet\n");
		rc = TPM_RC_VALUE;
		/* FIXME keys other than signing are wrong, not implemented yet */
		publicArea->parameters.rsaDetail.scheme.scheme = TPM_ALG_NULL;
		/* Table 152 - Definition of TPMU_ASYM_SCHEME details */
		break;
	      case TYPE_ST:
		printf("Storage key for ECC keys not implemented yet\n");
		rc = TPM_RC_VALUE;
		/* FIXME keys other than signing are wrong, not implemented yet */
		publicArea->parameters.rsaDetail.scheme.scheme = TPM_ALG_NULL;
		break;
	    }
	    /* Table 177 - TPMU_PUBLIC_ID unique */
	    /* Table 177 - Definition of TPMU_PUBLIC_ID */
	    publicArea->unique.ecc.x.t.size = 0;
	    publicArea->unique.ecc.y.t.size = 0;
	}
    }
    if (rc == 0) {
	rc = getPolicy(publicArea, policyFilename);
    }
    return rc;
}

/* symmetricCipherTemplate() is a template for an AES 128 CFB key

 */

TPM_RC symmetricCipherTemplate(TPMT_PUBLIC *publicArea,		/* output */
			       TPMA_OBJECT objectAttributes,	/* default, can be overridden here */
			       TPMI_ALG_HASH nalg,		/* Name algorithm */
			       int rev116,			/* TPM rev 116 compatibility, sets SIGN */
			       const char *policyFilename)	/* binary policy, NULL means empty */
{
    TPM_RC rc = 0;
    
    if (rc == 0) {
	publicArea->objectAttributes = objectAttributes;

	/* Table 185 - TPM2B_PUBLIC inPublic */
	/* Table 184 - TPMT_PUBLIC publicArea */
	publicArea->type = TPM_ALG_SYMCIPHER;
	publicArea->nameAlg = nalg;
	/* Table 32 - TPMA_OBJECT objectAttributes */
	/* rev 116 used DECRYPT for both decrypt and encrypt.  After 116, encrypt required SIGN */
	if (!rev116) {
	    /* actually encrypt */
	    publicArea->objectAttributes.val |= TPMA_OBJECT_SIGN;
	}
	publicArea->objectAttributes.val |= TPMA_OBJECT_DECRYPT;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_RESTRICTED;
	publicArea->objectAttributes.val |= TPMA_OBJECT_SENSITIVEDATAORIGIN;
	publicArea->objectAttributes.val |= TPMA_OBJECT_USERWITHAUTH;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_ADMINWITHPOLICY;
	/* Table 72 -  TPM2B_DIGEST authPolicy */
	/* policy set separately */
	/* Table 182 - Definition of TPMU_PUBLIC_PARMS parameters */
	{
	    /* Table 131 - Definition of TPMS_SYMCIPHER_PARMS symDetail */
	    {
		/* Table 129 - Definition of TPMT_SYM_DEF_OBJECT sym */
		/* Table 62 - Definition of (TPM_ALG_ID) TPMI_ALG_SYM_OBJECT Type */
		publicArea->parameters.symDetail.sym.algorithm = TPM_ALG_AES;
		/* Table 125 - Definition of TPMU_SYM_KEY_BITS Union */
		publicArea->parameters.symDetail.sym.keyBits.aes = 128;
		/* Table 126 - Definition of TPMU_SYM_MODE Union */
		publicArea->parameters.symDetail.sym.mode.aes = TPM_ALG_CFB;
	    }
	}
	/* Table 177 - TPMU_PUBLIC_ID unique */
	/* Table 72 - Definition of TPM2B_DIGEST Structure */
	publicArea->unique.sym.t.size = 0; 
    }
    if (rc == 0) {
	rc = getPolicy(publicArea, policyFilename);
    }
    return rc;
}

/* keyedHashPublicTemplate() is a template for a HMAC key

   The key is not restricted
*/

TPM_RC keyedHashPublicTemplate(TPMT_PUBLIC *publicArea,		/* output */
			       TPMA_OBJECT objectAttributes,	/* default, can be overridden here */
			       TPMI_ALG_HASH nalg,		/* Name algorithm */
			       TPMI_ALG_HASH halg,		/* hash algorithm */
			       const char *policyFilename)	/* binary policy, NULL means empty */
{
    TPM_RC			rc = 0;

    if (rc == 0) {
	publicArea->objectAttributes = objectAttributes;

	/* Table 185 - TPM2B_PUBLIC inPublic */
	/* Table 184 - TPMT_PUBLIC publicArea->*/
	/* Table 176 - Definition of (TPM_ALG_ID) TPMI_ALG_PUBLIC Type */
	publicArea->type = TPM_ALG_KEYEDHASH;
	/* Table 59 - Definition of (TPM_ALG_ID) TPMI_ALG_HASH Type  */
	publicArea->nameAlg = nalg;
	/* Table 32 - TPMA_OBJECT objectAttributes */
	publicArea->objectAttributes.val |= TPMA_OBJECT_SIGN;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_DECRYPT;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_RESTRICTED;
	publicArea->objectAttributes.val |= TPMA_OBJECT_SENSITIVEDATAORIGIN;
	publicArea->objectAttributes.val |= TPMA_OBJECT_USERWITHAUTH;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_ADMINWITHPOLICY;
	/* Table 72 -  TPM2B_DIGEST authPolicy */
	/* policy set separately */
	{
	    /* Table 182 - Definition of TPMU_PUBLIC_PARMS Union <IN/OUT, S> */
	    /* Table 178 - Definition of TPMS_KEYEDHASH_PARMS Structure */
	    /* Table 141 - Definition of TPMT_KEYEDHASH_SCHEME Structure */
	    /* Table 137 - Definition of (TPM_ALG_ID) TPMI_ALG_KEYEDHASH_SCHEME Type */
	    publicArea->parameters.keyedHashDetail.scheme.scheme = TPM_ALG_HMAC;
	    /* Table 140 - Definition of TPMU_SCHEME_KEYEDHASH Union <IN/OUT, S> */
	    /* Table 138 - Definition of Types for HMAC_SIG_SCHEME */
	    /* Table 135 - Definition of TPMS_SCHEME_HASH Structure */
	    publicArea->parameters.keyedHashDetail.scheme.details.hmac.hashAlg = halg;
	}
	/* Table 177 - TPMU_PUBLIC_ID unique */
	/* Table 72 - Definition of TPM2B_DIGEST Structure */
	publicArea->unique.sym.t.size = 0; 
    }
    if (rc == 0) {
	rc = getPolicy(publicArea, policyFilename);
    }
    return rc;
}

/* blPublicTemplate() is a template for a sealed data blob.

*/

TPM_RC blPublicTemplate(TPMT_PUBLIC *publicArea,	/* output */
			TPMA_OBJECT objectAttributes,	/* default, can be overridden here */
			TPMI_ALG_HASH nalg,		/* Name algorithm */
			const char *policyFilename)	/* binary policy, NULL means empty */
{
    TPM_RC			rc = 0;

    if (rc == 0) {
	publicArea->objectAttributes = objectAttributes;

	/* Table 185 - TPM2B_PUBLIC inPublic */
	/* Table 184 - TPMT_PUBLIC publicArea->*/
	/* Table 176 - Definition of (TPM_ALG_ID) TPMI_ALG_PUBLIC Type */
	publicArea->type = TPM_ALG_KEYEDHASH;
	/* Table 59 - Definition of (TPM_ALG_ID) TPMI_ALG_HASH Type  */
	publicArea->nameAlg = nalg;
	/* Table 32 - TPMA_OBJECT objectAttributes */
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_SIGN;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_DECRYPT;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_RESTRICTED;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_SENSITIVEDATAORIGIN;
	publicArea->objectAttributes.val |= TPMA_OBJECT_USERWITHAUTH;
	publicArea->objectAttributes.val &= ~TPMA_OBJECT_ADMINWITHPOLICY;
	/* Table 72 -  TPM2B_DIGEST authPolicy */
	/* policy set separately */
	{
	    /* Table 182 - Definition of TPMU_PUBLIC_PARMS Union <IN/OUT, S> */
	    /* Table 178 - Definition of TPMS_KEYEDHASH_PARMS Structure */
	    /* Table 141 - Definition of TPMT_KEYEDHASH_SCHEME Structure */
	    /* Table 137 - Definition of (TPM_ALG_ID) TPMI_ALG_KEYEDHASH_SCHEME Type */
	    publicArea->parameters.keyedHashDetail.scheme.scheme = TPM_ALG_NULL;
	    /* Table 140 - Definition of TPMU_SCHEME_KEYEDHASH Union <IN/OUT, S> */
	}
	/* Table 177 - TPMU_PUBLIC_ID unique */
	/* Table 72 - Definition of TPM2B_DIGEST Structure */
	publicArea->unique.sym.t.size = 0; 
    }
    if (rc == 0) {
	rc = getPolicy(publicArea, policyFilename);
    }
    return rc;
}

static TPM_RC getPolicy(TPMT_PUBLIC *publicArea,
			const char *policyFilename)
{
    TPM_RC rc = 0;

    if (rc == 0) {
	if (policyFilename != NULL) {
	    rc = TSS_File_Read2B(&publicArea->authPolicy.b,
				 sizeof(TPMU_HA),
				 policyFilename);
	}
	else {
	    publicArea->authPolicy.t.size = 0;	/* default empty policy */
	}
    }
    return rc;
}

void printUsageTemplate(void)
{
    printf("\tAsymmetric Key Algorithm\n");
    printf("\t\t-rsa (default)\n");
    printf("\t\t-ecc curve\n");
    printf("\t\t\tbnp256\n");
    printf("\t\t\tnistp256\n");
    printf("\t\t\tnistp384\n");
    printf("\n");
    printf("\tKey attributes\n");
    printf("\n");
    printf("\t\t-bl data blob for unseal (create only)\n");
    printf("\t\t\t-if data file name\n");
    printf("\t\t-den decryption, RSA, not storage, NULL scheme\n");
    printf("\t\t-deo decryption, RSA, not storage, OAEP scheme\n");
    printf("\t\t-des encryption/decryption, AES symmetric\n");
    printf("\t\t\t[-116 for TPM rev 116 compatibility]\n");
    printf("\t\t-st storage\n");
    printf("\t\t\t[default for primary keys]\n");
    printf("\t\t-si signing\n");
    printf("\t\t-sir restricted signing\n");
    printf("\t\t-kh keyed hash (hmac)\n");
    printf("\t\t-gp general purpose, not storage\n");
    printf("\n");
    printf("\t\t-kt (can be specified more than once)\n"
	   "\t\t\tf fixedTPM \n"
	   "\t\t\tp fixedParent \n");
    printf("\t\t[-da object subject to DA protection) (default no)]\n");
    printf("\t[-pol policy file (default empty)]\n");
    printf("\n");
    printf("\t[-nalg name hash algorithm [sha1, sha256, sha384] (default sha256)]\n");
    printf("\t[-halg scheme hash algorithm [sha1, sha256, sha384] (default sha256)]\n");
    return;	
}
