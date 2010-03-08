/*	$NetBSD: LDAPMessageQueue.h,v 1.1.1.2 2010/03/08 02:14:20 lukem Exp $	*/

// OpenLDAP: pkg/ldap/contrib/ldapc++/src/LDAPMessageQueue.h,v 1.5.10.2 2008/04/14 23:09:26 quanah Exp
/*
 * Copyright 2000, OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */


#ifndef LDAP_MESSAGE_QUEUE_H
#define LDAP_MESSAGE_QUEUE_H

#include <stack>

#include <LDAPUrlList.h>
#include <LDAPMessage.h>

class LDAPAsynConnection;
class LDAPRequest;
class LDAPSearchRequest;
class LDAPUrl;
typedef std::stack<LDAPRequest*> LDAPRequestStack;
typedef std::list<LDAPRequest*> LDAPRequestList;

/**
 * This class is created for the asynchronous LDAP-operations. And can be
 * used by the client to retrieve the results of an operation.
 */
class LDAPMessageQueue{
    public :

        /**
         * This creates a new LDAPMessageQueue. For a LDAP-request
         *
         * @param conn  The Request for that is queue can be used to get
         *              the results.
         */
        LDAPMessageQueue(LDAPRequest *conn);
        /**
         * Destructor
         */
        ~LDAPMessageQueue();

        /**
         * This method reads exactly one Message from the results of a
         * Request. 
         * @throws LDAPException
         * @return A pointer to an object of one of the classes that were
         *          derived from LDAPMsg. The user has to cast it to the
         *          correct type (e.g. LDAPResult or LDAPSearchResult)
         */           
        LDAPMsg* getNext();

        /**
         * For internat use only.
         *
         * The method is used to start the automatic referral chasing
         */
        LDAPRequest* chaseReferral(LDAPMsg* ref);

        /**
         * For internal use only
         *
         * The referral chasing algorithm needs this method to see the
         * currently active requests.
         */
        LDAPRequestStack* getRequestStack(); 
    
    private :
        LDAPRequestStack m_activeReq;
        LDAPRequestList m_issuedReq;
};
#endif //ifndef LDAP_MESSAGE_QUEUE_H

