/*	$NetBSD: print-sctp.c,v 1.3 2001/06/27 02:46:25 simonb Exp $	*/

/* Copyright (c) 2001 NETLAB, Temple University
 * Copyright (c) 2001 Protocol Engineering Lab, University of Delaware
 *
 * Jerry Heinz <gheinz@astro.temple.edu>
 * John Fiore <jfiore@joda.cis.temple.edu>
 * Armando L. Caro Jr. <acaro@cis.udel.edu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static const char rcsid[] =
"@(#) Header: /tcpdump/master/tcpdump/print-sctp.c,v 1.3 2001/05/09 02:42:50 itojun Exp (NETLAB/PEL)";
#else
__RCSID("$NetBSD: print-sctp.c,v 1.3 2001/06/27 02:46:25 simonb Exp $");
#endif
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/param.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <unistd.h>
#include "sctpHeader.h"
#include "sctpConstants.h"
#include <assert.h>

#if __STDC__
struct mbuf;
struct rtentry;
#endif
#include <net/if.h>

#include <netinet/in.h>

#include <stdio.h>
#include <string.h>

#include "interface.h"
#include "addrtoname.h"
#include "extract.h"			/* must come after interface.h */
#include "ip.h"
#ifdef INET6
#include "ip6.h"
#endif

void sctp_print(const u_char *bp,        /* beginning of sctp packet */
		const u_char *bp2,       /* beginning of enclosing */
		u_int sctpPacketLength)  /* ip packet */
{ 
  const struct sctpHeader *sctpPktHdr;
  const struct ip *ip;
#ifdef INET6
  const struct ip6_hdr *ip6;
#endif
  const u_char *cp;
  void *endPacketPtr;
  u_short sourcePort, destPort;
  int chunkCount;
  struct sctpChunkDesc *chunkDescPtr;
  void *nextChunk;

  sctpPktHdr = (struct sctpHeader*) bp;
  endPacketPtr = ((u_char*)((u_char*)sctpPktHdr+sctpPacketLength));
  
  if( (u_long) endPacketPtr > (u_long) snapend)
    endPacketPtr = (void *) snapend;
  ip = (struct ip *)bp2;
#ifdef INET6
  if (IP_V(ip) == 6)
    ip6 = (struct ip6_hdr *)bp2;
  else
    ip6 = NULL;
#endif /*INET6*/
  cp = (u_char *)(sctpPktHdr + 1);
  if (cp > snapend)
    { 
      printf("[|sctp]");
      return; 
    } 

  if (sctpPacketLength < sizeof(struct sctpHeader)) 
    {
      (void)printf("truncated-sctp - %ld bytes missing!", 
		   (long)sctpPacketLength-sizeof(struct sctpHeader));
      return;
    }
  
  /*    sctpPacketLength -= sizeof(struct sctpHeader);  packet length  */
  /*  			      is now only as long as the payload  */

  sourcePort = ntohs(sctpPktHdr->source);
  destPort = ntohs(sctpPktHdr->destination);
  
#ifdef INET6
  if (ip6) {
    if (ip6->ip6_nxt == IPPROTO_SCTP) {
      (void)printf("%s.%d > %s.%d: sctp",
        ip6addr_string(&ip6->ip6_src),
        sourcePort,
        ip6addr_string(&ip6->ip6_dst),
        destPort);
    } else {
      (void)printf("%d > %d: sctp",
        sourcePort, destPort);
    }
  } else
#endif /*INET6*/
  {
    if (ip->ip_p == IPPROTO_SCTP) {
      (void)printf("%s.%d > %s.%d: sctp",
        ipaddr_string(&ip->ip_src),
        sourcePort,
        ipaddr_string(&ip->ip_dst),
        destPort);
    } else {
      (void)printf("%d > %d: sctp",
        sourcePort, destPort);
    }
  }
  fflush(stdout);

  if (vflag < 2)
	return;
  
  /* cycle through all chunks, printing information on each one */
  for (chunkCount = 0, 
	 chunkDescPtr = (struct sctpChunkDesc *) ( (u_char*) sctpPktHdr +
						   sizeof(struct sctpHeader));
       chunkDescPtr != NULL &&
	 ( (void *)  ((u_char *) chunkDescPtr + sizeof(struct sctpChunkDesc))
	   <= endPacketPtr);
       
       chunkDescPtr = (struct sctpChunkDesc *) nextChunk, chunkCount++)
    {
      u_short align;
      u_char *chunkEnd;
      
      chunkEnd = ((u_char*)chunkDescPtr + ntohs(chunkDescPtr->chunkLength));
      
      align=ntohs(chunkDescPtr->chunkLength) % 4;
      if (align != 0)
	align = 4 - align;

      nextChunk = (void *) (chunkEnd + align);

      printf("\n\t%d) ", chunkCount+1);
      switch (chunkDescPtr->chunkID)
	{
	case SCTP_DATA :
	  {
	    struct sctpDataPart *dataHdrPtr;
	    
	    printf("[DATA] ");
	    
	    if ((chunkDescPtr->chunkFlg & SCTP_DATA_UNORDERED) 
		== SCTP_DATA_UNORDERED)
	      printf("(U)");

	    if ((chunkDescPtr->chunkFlg & SCTP_DATA_FIRST_FRAG) 
		== SCTP_DATA_FIRST_FRAG)
	      printf("(B)");
	    
	    if ((chunkDescPtr->chunkFlg & SCTP_DATA_LAST_FRAG) 
		== SCTP_DATA_LAST_FRAG)
	      printf("(E)");

	    if( ((chunkDescPtr->chunkFlg & SCTP_DATA_UNORDERED) 
		 == SCTP_DATA_UNORDERED) 
		||
		((chunkDescPtr->chunkFlg & SCTP_DATA_FIRST_FRAG) 
		 == SCTP_DATA_FIRST_FRAG)
		||
		((chunkDescPtr->chunkFlg & SCTP_DATA_LAST_FRAG) 
		 == SCTP_DATA_LAST_FRAG) )
	      printf(" ");

	    dataHdrPtr=(struct sctpDataPart*)(chunkDescPtr+1);
			     
	    printf("[TSN: %u] ", (u_int32_t)ntohl(dataHdrPtr->TSN));
	    printf("[SID: %u] ", ntohs(dataHdrPtr->streamId));
	    printf("[SSEQ %u] ", ntohs(dataHdrPtr->sequence));
	    printf("[PPID 0x%x] ", (u_int32_t)ntohl(dataHdrPtr->payloadtype));
	    fflush(stdout);

	    if (vflag)		/* if verbose output is specified */
	      {		           /* at the command line */
		char *payloadPtr;
		
		printf("[Payload: {");
		fflush(stdout);

		payloadPtr = (char *) (++dataHdrPtr);
		write(STDOUT_FILENO, payloadPtr, 
		      htons(chunkDescPtr->chunkLength)-1 -
		      sizeof(struct sctpDataPart)-sizeof(struct sctpChunkDesc));
		printf("}] ");
		fflush(stdout);
	      }
	    break;
	  }
	case SCTP_INITIATION :
	  {
	    struct sctpInitiation *init;

	    printf("[INIT] ");
	    init=(struct sctpInitiation*)(chunkDescPtr+1);
	    printf("[init tag: %u] ", (u_int32_t)ntohl(init->initTag));
	    printf("[rwnd: %u] ", (u_int32_t)ntohl(init->rcvWindowCredit));
	    printf("[OS: %u] ", ntohs(init->NumPreopenStreams));
	    printf("[MIS: %u] ", ntohs(init->MaxInboundStreams));
	    printf("[init TSN: %u] ", (u_int32_t)ntohl(init->initialTSN));

#if(0) /* ALC you can add code for optional params here */
	    if( (init+1) < chunkEnd )
	      printf(" @@@@@ UNFINISHED @@@@@@%s\n",
		     "Optional params present, but not printed.");
#endif
	    break;
	  }
	case SCTP_INITIATION_ACK :
	  {
	    struct sctpInitiation *init;
	    
	    printf("[INIT ACK] ");
	    init=(struct sctpInitiation*)(chunkDescPtr+1);
	    printf("[init tag: %u] ", (u_int32_t)ntohl(init->initTag));
	    printf("[rwnd: %u] ", (u_int32_t)ntohl(init->rcvWindowCredit));
	    printf("[OS: %u] ", ntohs(init->NumPreopenStreams));
	    printf("[MIS: %u] ", ntohs(init->MaxInboundStreams));
	    printf("[init TSN: %u] ", (u_int32_t)ntohl(init->initialTSN));
	    
#if(0) /* ALC you can add code for optional params here */
	    if( (init+1) < chunkEnd )
	      printf(" @@@@@ UNFINISHED @@@@@@%s\n",
		     "Optional params present, but not printed.");
#endif
	    break;
	  }
	case SCTP_SELECTIVE_ACK:
	  {
	    struct sctpSelectiveAck *sack;
	    struct sctpSelectiveFrag *frag; 
	    int fragNo, tsnNo;
	    u_long *dupTSN;

	    printf("[SACK] ");
	    sack=(struct sctpSelectiveAck*)(chunkDescPtr+1);
	    printf("[cum ack %u] ", (u_int32_t)ntohl(sack->highestConseqTSN));
	    printf("[a_rwnd %u] ", (u_int32_t)ntohl(sack->updatedRwnd));
	    printf("[#gap acks %u] ", ntohs(sack->numberOfdesc));
	    printf("[#dup tsns %u] ", ntohs(sack->numDupTsns));
	    
	    
	    /* print gaps */
	    for (frag = ( (struct sctpSelectiveFrag *)
			  ((struct sctpSelectiveAck *) sack+1)),
		   fragNo=0;
		 (void *)frag < nextChunk && fragNo < ntohs(sack->numberOfdesc);
		 frag++, fragNo++)
	      printf("\n\t\t[gap ack block #%d: start = %u, end = %u] ", 
		     fragNo+1,
		     (u_int32_t)(ntohl(sack->highestConseqTSN) + ntohs(frag->fragmentStart)),
		     (u_int32_t)(ntohl(sack->highestConseqTSN) + ntohs(frag->fragmentEnd)));
	    

	    /* print duplicate TSNs */
	    for (dupTSN = (u_long*)frag, tsnNo=0; 
		 (void *) dupTSN < nextChunk && tsnNo<ntohs(sack->numDupTsns);
		 dupTSN++, tsnNo++)
	      printf("\n\t\t[dup TSN #%u: %u] ", tsnNo+1,
	          (u_int32_t)ntohl(*dupTSN));

	    break;
	  }
	case SCTP_HEARTBEAT_REQUEST :
	  {
	    struct sctpHBsender *hb;

	    hb=(struct sctpHBsender*)chunkDescPtr;

	    printf("[HB REQ] ");
	    
	    break;
	  }
	case SCTP_HEARTBEAT_ACK :
	  printf("[HB ACK] ");
	  break;
	case SCTP_ABORT_ASSOCIATION :
	  printf("[ABORT] ");
	  break;
	case SCTP_SHUTDOWN :
	  printf("[SHUTDOWN] ");
	  break;
	case SCTP_SHUTDOWN_ACK :
	  printf("[SHUTDOWN ACK] ");
	  break;
	case SCTP_OPERATION_ERR :
	  printf("[OP ERR] ");
	  break;
	case SCTP_COOKIE_ECHO :
	  printf("[COOKIE ECHO] ");
	  break;
	case SCTP_COOKIE_ACK :
	  printf("[COOKIE ACK] ");
	  break;
	case SCTP_ECN_ECHO :
	  printf("[ECN ECHO] ");
	  break;
	case SCTP_ECN_CWR : 
	  printf("[ECN CWR] ");
	  break;
	case SCTP_SHUTDOWN_COMPLETE :
	  printf("[SHUTDOWN COMPLETE] ");
	  break;
	case SCTP_FORWARD_CUM_TSN :
	  printf("[FOR CUM TSN] ");
	  break;
	case SCTP_RELIABLE_CNTL :
	  printf("[REL CTRL] ");
	  break;
	case SCTP_RELIABLE_CNTL_ACK :
	  printf("[REL CTRL ACK] ");
	  break;
	default :
	  printf("[Unknown chunk type: 0x%x]", chunkDescPtr->chunkID);
	  return;
	}
    }
}
