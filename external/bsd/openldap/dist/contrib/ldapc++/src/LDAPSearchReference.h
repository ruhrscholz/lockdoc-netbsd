/*	$NetBSD: LDAPSearchReference.h,v 1.1.1.2 2010/03/08 02:14:20 lukem Exp $	*/

// OpenLDAP: pkg/ldap/contrib/ldapc++/src/LDAPSearchReference.h,v 1.4.10.1 2008/04/14 23:09:26 quanah Exp
/*
 * Copyright 2000, OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */


#ifndef LDAP_SEARCH_REFERENCE_H
#define LDAP_SEARCH_REFERENCE_H 

#include <LDAPMessage.h>
#include <LDAPUrlList.h>

class LDAPRequest;
class LDAPUrl;

/**
 * This class is used to represent Continuation References that were
 * returned during a SEARCH-Operation.
 */
class LDAPSearchReference : public LDAPMsg{

    public :
        /**
         * Constructor that create an object from the C-API structures
         */
        LDAPSearchReference(const LDAPRequest* req, LDAPMessage* msg);

        /**
         * The Destructor
         */
        ~LDAPSearchReference();

        /**
         * @returns The destination URLs that were send with this message
         */
        const LDAPUrlList& getUrls() const;

    private :
        LDAPUrlList m_urlList;
        LDAPSearchReference();
};



#endif //LDAP_SEARCH_REFERENCE_H
