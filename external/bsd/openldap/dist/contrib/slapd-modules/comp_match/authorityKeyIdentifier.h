/*	$NetBSD: authorityKeyIdentifier.h,v 1.1.1.2 2010/03/08 02:14:20 lukem Exp $	*/


#include "asn-incl.h"
/*
 *    authorityKeyIdentifier.h
 *    "AuthorityKeyIdentifierDefinition" ASN.1 module encode/decode/extracting/matching/free C src.
 *    This file was generated by modified eSMACC compiler Sat Dec 11 10:15:39 2004
 *    The generated files are strongly encouraged to be
 *    compiled as a module for OpenLDAP Software
 */

#ifndef _authorityKeyIdentifier_h_
#define _authorityKeyIdentifier_h_




#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
#pragma warning( disable : 4101 )
#endif
#include "componentlib.h"
typedef ComponentOcts ComponentKeyIdentifier; /* OCTET STRING */

#define MatchingComponentKeyIdentifier MatchingComponentOcts

#define ExtractingComponentKeyIdentifier ExtractingComponentOcts

#define BDecComponentKeyIdentifier BDecComponentOcts

#define GDecComponentKeyIdentifier GDecComponentOcts


typedef ComponentInt ComponentCertificateSerialNumber; /* INTEGER */

#define MatchingComponentCertificateSerialNumber MatchingComponentInt

#define ExtractingComponentCertificateSerialNumber ExtractingComponentInt

#define BDecComponentCertificateSerialNumber BDecComponentInt

#define GDecComponentCertificateSerialNumber GDecComponentInt


typedef struct OtherName /* SEQUENCE */
{
	Syntax* syntax;
	ComponentDesc* comp_desc;
	struct berval identifier;
	char id_buf[MAX_IDENTIFIER_LEN];
	ComponentOid type_id; /* OBJECT IDENTIFIER */
	ComponentAnyDefinedBy value; /* [0] EXPLICIT ANY DEFINED BY type-id */
} ComponentOtherName;

int MatchingComponentOtherName PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentOtherName PROTO (( void* mem_op, ComponentReference *cr, ComponentOtherName *comp ));


int BDecComponentOtherName PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentOtherName **v, AsnLen *bytesDecoded, int mode));


int GDecComponentOtherName PROTO (( void* mem_op, GenBuf * b, ComponentOtherName **v, AsnLen *bytesDecoded, int mode));



typedef struct ORAddress /* SEQUENCE */
{
	Syntax* syntax;
	ComponentDesc* comp_desc;
	struct berval identifier;
	char id_buf[MAX_IDENTIFIER_LEN];
	ComponentOid type_id; /* OBJECT IDENTIFIER */
	ComponentAnyDefinedBy value; /* ANY DEFINED BY type-id */
	ComponentOcts extension; /* OCTET STRING OPTIONAL */
} ComponentORAddress;

int MatchingComponentORAddress PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentORAddress PROTO (( void* mem_op, ComponentReference *cr, ComponentORAddress *comp ));


int BDecComponentORAddress PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentORAddress **v, AsnLen *bytesDecoded, int mode));


int GDecComponentORAddress PROTO (( void* mem_op, GenBuf * b, ComponentORAddress **v, AsnLen *bytesDecoded, int mode));



typedef struct AttributeTypeAndValue /* SEQUENCE */
{
	Syntax* syntax;
	ComponentDesc* comp_desc;
	struct berval identifier;
	char id_buf[MAX_IDENTIFIER_LEN];
	ComponentOid type; /* OBJECT IDENTIFIER */
	ComponentAnyDefinedBy value; /* ANY DEFINED BY type */
} ComponentAttributeTypeAndValue;

int MatchingComponentAttributeTypeAndValue PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentAttributeTypeAndValue PROTO (( void* mem_op, ComponentReference *cr, ComponentAttributeTypeAndValue *comp ));


int BDecComponentAttributeTypeAndValue PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentAttributeTypeAndValue **v, AsnLen *bytesDecoded, int mode));


int GDecComponentAttributeTypeAndValue PROTO (( void* mem_op, GenBuf * b, ComponentAttributeTypeAndValue **v, AsnLen *bytesDecoded, int mode));



typedef struct DirectoryString /* CHOICE */
{
	Syntax* syntax;
	ComponentDesc* comp_desc;
	struct berval identifier;
	char id_buf[MAX_IDENTIFIER_LEN];
    enum DirectoryStringChoiceId
    {
        DIRECTORYSTRING_TELETEXSTRING,
        DIRECTORYSTRING_PRINTABLESTRING,
        DIRECTORYSTRING_UNIVERSALSTRING,
        DIRECTORYSTRING_UTF8STRING,
        DIRECTORYSTRING_BMPSTRING
    } choiceId;
    union DirectoryStringChoiceUnion
    {
	ComponentTeletexString* teletexString; /* TeletexString SIZE 1..MAX */
	ComponentPrintableString* printableString; /* PrintableString SIZE 1..MAX */
	ComponentUniversalString* universalString; /* UniversalString SIZE 1..MAX */
	ComponentUTF8String* utf8String; /* UTF8String SIZE 1..MAX */
	ComponentBMPString* bmpString; /* BMPString SIZE 1..MAX */
    } a;
} ComponentDirectoryString;

int MatchingComponentDirectoryString PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentDirectoryString PROTO (( void* mem_op, ComponentReference *cr, ComponentDirectoryString *comp ));


int BDecComponentDirectoryString PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentDirectoryString **v, AsnLen *bytesDecoded, int mode));


int GDecComponentDirectoryString PROTO (( void* mem_op, GenBuf * b, ComponentDirectoryString **v, AsnLen *bytesDecoded, int mode));



typedef struct EDIPartyName /* SEQUENCE */
{
	Syntax* syntax;
	ComponentDesc* comp_desc;
	struct berval identifier;
	char id_buf[MAX_IDENTIFIER_LEN];
	ComponentDirectoryString* nameAssigner; /* [0] DirectoryString OPTIONAL */
	ComponentDirectoryString* partyName; /* [1] DirectoryString */
} ComponentEDIPartyName;

int MatchingComponentEDIPartyName PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentEDIPartyName PROTO (( void* mem_op, ComponentReference *cr, ComponentEDIPartyName *comp ));


int BDecComponentEDIPartyName PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentEDIPartyName **v, AsnLen *bytesDecoded, int mode));


int GDecComponentEDIPartyName PROTO (( void* mem_op, GenBuf * b, ComponentEDIPartyName **v, AsnLen *bytesDecoded, int mode));



typedef ComponentList ComponentRelativeDistinguishedName; /* SET OF AttributeTypeAndValue */

int MatchingComponentRelativeDistinguishedName PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentRelativeDistinguishedName PROTO (( void* mem_op, ComponentReference *cr, ComponentRelativeDistinguishedName *comp ));


int BDecComponentRelativeDistinguishedName PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentRelativeDistinguishedName **v, AsnLen *bytesDecoded, int mode));


int GDecComponentRelativeDistinguishedName PROTO (( void* mem_op, GenBuf * b, ComponentRelativeDistinguishedName **v, AsnLen *bytesDecoded, int mode));



typedef ComponentList ComponentRDNSequence; /* SEQUENCE OF RelativeDistinguishedName */

int MatchingComponentRDNSequence PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentRDNSequence PROTO (( void* mem_op, ComponentReference *cr, ComponentRDNSequence *comp ));


int BDecComponentRDNSequence PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentRDNSequence **v, AsnLen *bytesDecoded, int mode));


int GDecComponentRDNSequence PROTO (( void* mem_op, GenBuf * b, ComponentRDNSequence **v, AsnLen *bytesDecoded, int mode));



typedef struct Name /* CHOICE */
{
	Syntax* syntax;
	ComponentDesc* comp_desc;
	struct berval identifier;
	char id_buf[MAX_IDENTIFIER_LEN];
    enum NameChoiceId
    {
        NAME_RDNSEQUENCE
    } choiceId;
    union NameChoiceUnion
    {
	ComponentRDNSequence* rdnSequence; /* RDNSequence */
    } a;
} ComponentName;

int MatchingComponentName PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentName PROTO (( void* mem_op, ComponentReference *cr, ComponentName *comp ));


int BDecComponentName PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentName **v, AsnLen *bytesDecoded, int mode));


int GDecComponentName PROTO (( void* mem_op, GenBuf * b, ComponentName **v, AsnLen *bytesDecoded, int mode));



typedef struct GeneralName /* CHOICE */
{
	Syntax* syntax;
	ComponentDesc* comp_desc;
	struct berval identifier;
	char id_buf[MAX_IDENTIFIER_LEN];
    enum GeneralNameChoiceId
    {
        GENERALNAME_OTHERNAME,
        GENERALNAME_RFC822NAME,
        GENERALNAME_DNSNAME,
        GENERALNAME_X400ADDRESS,
        GENERALNAME_DIRECTORYNAME,
        GENERALNAME_EDIPARTYNAME,
        GENERALNAME_UNIFORMRESOURCEIDENTIFIER,
        GENERALNAME_IPADDRESS,
        GENERALNAME_REGISTEREDID
    } choiceId;
    union GeneralNameChoiceUnion
    {
	ComponentOtherName* otherName; /* [0] OtherName */
	ComponentIA5String* rfc822Name; /* [1] IA5String */
	ComponentIA5String* dNSName; /* [2] IA5String */
	ComponentORAddress* x400Address; /* [3] ORAddress */
	ComponentName* directoryName; /* [4] Name */
	ComponentEDIPartyName* ediPartyName; /* [5] EDIPartyName */
	ComponentIA5String* uniformResourceIdentifier; /* [6] IA5String */
	ComponentOcts* iPAddress; /* [7] OCTET STRING */
	ComponentOid* registeredID; /* [8] OBJECT IDENTIFIER */
    } a;
} ComponentGeneralName;

int MatchingComponentGeneralName PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentGeneralName PROTO (( void* mem_op, ComponentReference *cr, ComponentGeneralName *comp ));


int BDecComponentGeneralName PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentGeneralName **v, AsnLen *bytesDecoded, int mode));


int GDecComponentGeneralName PROTO (( void* mem_op, GenBuf * b, ComponentGeneralName **v, AsnLen *bytesDecoded, int mode));



typedef ComponentList ComponentGeneralNames; /* SEQUENCE SIZE 1..MAX OF GeneralName */

int MatchingComponentGeneralNames PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentGeneralNames PROTO (( void* mem_op, ComponentReference *cr, ComponentGeneralNames *comp ));


int BDecComponentGeneralNames PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentGeneralNames **v, AsnLen *bytesDecoded, int mode));


int GDecComponentGeneralNames PROTO (( void* mem_op, GenBuf * b, ComponentGeneralNames **v, AsnLen *bytesDecoded, int mode));



typedef struct AuthorityKeyIdentifier /* SEQUENCE */
{
	Syntax* syntax;
	ComponentDesc* comp_desc;
	struct berval identifier;
	char id_buf[MAX_IDENTIFIER_LEN];
	ComponentKeyIdentifier keyIdentifier; /* [0] KeyIdentifier OPTIONAL */
	ComponentGeneralNames* authorityCertIssuer; /* [1] GeneralNames OPTIONAL */
	ComponentCertificateSerialNumber* authorityCertSerialNumber; /* [2] CertificateSerialNumber OPTIONAL */
} ComponentAuthorityKeyIdentifier;

int MatchingComponentAuthorityKeyIdentifier PROTO (( char *oid, ComponentSyntaxInfo *, ComponentSyntaxInfo *v2 ));


void* ExtractingComponentAuthorityKeyIdentifier PROTO (( void* mem_op, ComponentReference *cr, ComponentAuthorityKeyIdentifier *comp ));


int BDecComponentAuthorityKeyIdentifier PROTO ((void* mem_op, GenBuf * b, AsnTag tagId0, AsnLen elmtLen0, ComponentAuthorityKeyIdentifier **v, AsnLen *bytesDecoded, int mode));


int GDecComponentAuthorityKeyIdentifier PROTO (( void* mem_op, GenBuf * b, ComponentAuthorityKeyIdentifier **v, AsnLen *bytesDecoded, int mode));



/* ========== Object Declarations ========== */


/* ========== Object Set Declarations ========== */
#ifdef __cplusplus
extern "C" {
#endif

#endif /* conditional include of authorityKeyIdentifier.h */
