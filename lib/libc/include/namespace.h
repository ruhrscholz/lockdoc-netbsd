/*	$NetBSD: namespace.h,v 1.15 1998/10/13 15:05:02 kleink Exp $	*/

/*-
 * Copyright (c) 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_

#include <sys/cdefs.h>

#define catclose	_catclose
#define catgets		_catgets
#define catopen		_catopen
#define err		_err
#define errx		_errx
#define strtoq		_strtoq
#define strtouq		_strtouq
#define sys_errlist	_sys_errlist
#define sys_nerr	_sys_nerr
#define sys_siglist	_sys_siglist
#define verr		_verr
#define verrx		_verrx
#define vwarn		_vwarn
#define vwarnx		_vwarnx
#define warn		_warn
#define warnx		_warnx

#ifdef __weak_alias
#define a64l			_a64l
#define asctime_r		_asctime_r
#define alarm			_alarm
#define alphasort		_alphasort
#define authnone_create		_authnone_create
#define authunix_create		_authunix_create
#define authunix_create_default _authunix_create_default
#define basename		_basename
#define bindresvport		_bindresvport
#define bm_comp			_bm_comp
#define bm_exec			_bm_exec
#define bm_free			_bm_free
#define callrpc			_callrpc
#define cfgetispeed		_cfgetispeed
#define cfgetospeed		_cfgetospeed
#define cfmakeraw		_cfmakeraw
#define cfsetispeed		_cfsetispeed
#define cfsetospeed		_cfsetospeed
#define cfsetspeed		_cfsetspeed
#define cgetcap			_cgetcap
#define cgetclose		_cgetclose
#define cgetent			_cgetent
#define cgetfirst		_cgetfirst
#define cgetmatch		_cgetmatch
#define cgetnext		_cgetnext
#define cgetnum			_cgetnum
#define cgetset			_cgetset
#define cgetstr			_cgetstr
#define cgetustr		_cgetustr
#define clnt_broadcast		_clnt_broadcast
#define clnt_create		_clnt_create
#define clnt_pcreateerror	_clnt_pcreateerror
#define clnt_perrno		_clnt_perrno
#define clnt_perror		_clnt_perror
#define clnt_spcreateerror	_clnt_spcreateerror
#define clnt_sperrno		_clnt_sperrno
#define clnt_sperror		_clnt_sperror
#define clntraw_create		_clntraw_create
#define clnttcp_create		_clnttcp_create
#define clntudp_bufcreate	_clntudp_bufcreate
#define clntudp_create		_clntudp_create
#define closedir		_closedir
#define closelog		_closelog
#define confstr			_confstr
#define ctermid			_ctermid
#define ctime_r			_ctime_r
#define daemon			_daemon
#define dbm_clearerr		_dbm_clearerr
#define dbm_close		_dbm_close
#define dbm_delete		_dbm_delete
#define dbm_dirfno		_dbm_dirfno
#define dbm_error		_dbm_error
#define dbm_fetch		_dbm_fetch
#define dbm_firstkey		_dbm_firstkey
#define dbm_nextkey		_dbm_nextkey
#define dbm_open		_dbm_open
#define dbm_store		_dbm_store
#define dbopen			_dbopen
#define devname			_devname
#define dirname			_dirname
#define drand48			_drand48
#define endfsent		_endfsent
#define endgrent		_endgrent
#define endnetent		_endnetent
#define endnetgrent		_endnetgrent
#define endprotoent		_endprotoent
#define endpwent		_endpwent
#define endrpcent		_endrpcent
#define endservent		_endservent
#define endttyent		_endttyent
#define endusershell		_endusershell
#define erand48			_erand48
#define ether_aton		_ether_aton
#define ether_hostton		_ether_hostton
#define ether_line		_ether_line
#define ether_ntoa		_ether_ntoa
#define ether_ntohost		_ether_ntohost
#define execl			_execl
#define execle			_execle
#define execlp			_execlp
#define execv			_execv
#define execvp			_execvp
#define fdopen			_fdopen
#define fnmatch			_fnmatch
#define ftok			_ftok
#define fts_children		_fts_children
#define fts_close		_fts_close
#define fts_open		_fts_open
#define fts_read		_fts_read
#define fts_set			_fts_set
#define get_myaddress		_get_myaddress
#define getbsize		_getbsize
#define getcwd			_getcwd
#define getdiskbyname		_getdiskbyname
#define getdtablesize		_getdtablesize
#define getfsent		_getfsent
#define getfsfile		_getfsfile
#define getfsspec		_getfsspec
#define getgrent		_getgrent
#define getgrgid		_getgrgid
#define getgrnam		_getgrnam
#define getgrouplist		_getgrouplist
#define gethostname		_gethostname
#define getloadavg		_getloadavg
#define getlogin		_getlogin
#define getmntinfo		_getmntinfo
#define getmode			_getmode
#define getnetbyaddr		_getnetbyaddr
#define getnetbyname		_getnetbyname
#define getnetent		_getnetent
#define getnetgrent		_getnetgrent
#define getopt			_getopt
#define getpagesize		_getpagesize
#define getpass			_getpass
#define getprotobyname		_getprotobyname
#define getprotobynumber	_getprotobynumber
#define getprotoent		_getprotoent
#define getpwent		_getpwent
#define getpwnam		_getpwnam
#define getpwuid		_getpwuid
#define getrpcbyname		_getrpcbyname
#define getrpcbynumber		_getrpcbynumber
#define getrpcent		_getrpcent
#define getrpcport		_getrpcport
#define getservbyname		_getservbyname
#define getservbyport		_getservbyport
#define getservent		_getservent
#define getsubopt		_getsubopt
#define getttyent		_getttyent
#define getttynam		_getttynam
#define getusershell		_getusershell
#define glob			_glob
#define globfree		_globfree
#define gmtime_r		_gmtime_r
#define group_from_gid		_group_from_gid
#define heapsort		_heapsort
#define inet_lnaof		_inet_lnaof
#define inet_makeaddr		_inet_makeaddr
#define inet_net_ntop		_inet_net_ntop
#define inet_net_pton		_inet_net_pton
#define inet_neta		_inet_neta
#define inet_netof		_inet_netof
#define inet_network		_inet_network
#define inet_nsap_addr		_inet_nsap_addr
#define inet_nsap_ntoa		_inet_nsap_ntoa
#define inet_ntoa		_inet_ntoa
#define inet_ntop		_inet_ntop
#define inet_pton		_inet_pton
#define initgroups		_initgroups
#define initstate		_initstate
#define innetgr			_innetgr
#define isatty			_isatty
#define isinf			_isinf
#define isnan			_isnan
#define jrand48			_jrand48
#define l64a			_l64a
#define lcong48			_lcong48
#define localtime_r		_localtime_r
#define lockf			_lockf
#define lrand48			_lrand48
#define mergesort		_mergesort
#define mpool_close		_mpool_close
#define mpool_filter		_mpool_filter
#define mpool_get		_mpool_get
#define mpool_new		_mpool_new
#define mpool_open		_mpool_open
#define mpool_put		_mpool_put
#define mpool_sync		_mpool_sync
#define mrand48			_mrand48
#define nice			_nice
#define nlist			_nlist
#define nrand48			_nrand48
#define offtime			_offtime
#define opendir			_opendir
#define openlog			_openlog
#define pause			_pause
#define pclose			_pclose
#define perror			_perror
#define pmap_getmaps		_pmap_getmaps
#define pmap_getport		_pmap_getport
#define pmap_rmtcall		_pmap_rmtcall
#define pmap_set		_pmap_set
#define pmap_unset		_pmap_unset
#define popen			_popen
#define posix2time		_posix2time
#define psignal			_psignal
#define putenv			_putenv
#define qabs			_qabs
#define qdiv			_qdiv
#define radixsort		_radixsort
#define random			_random
#define readdir			_readdir
#define realpath		_realpath
#define regcomp			_regcomp
#define regerror		_regerror
#define regexec			_regexec
#define regfree			_regfree
#define registerrpc		_registerrpc
#define rewinddir		_rewinddir
#define scandir			_scandir
#define seed48			_seed48
#define seekdir			_seekdir
#define setdomainname		_setdomainname
#define setenv			_setenv
#define setfsent		_setfsent
#define setgrent		_setgrent
#define setgroupent		_setgroupent
#define sethostname		_sethostname
#define setlogmask		_setlogmask
#define setmode			_setmode
#define setnetent		_setnetent
#define setnetgrent		_setnetgrent
#define setpassent		_setpassent
#define setproctitle		_setproctitle
#define setprotoent		_setprotoent
#define setpwent		_setpwent
#define setrpcent		_setrpcent
#define setservent		_setservent
#define setstate		_setstate
#define setttyent		_setttyent
#define setusershell		_setusershell
#define shm_open		_shm_open
#define shm_unlink		_shm_unlink
#define siginterrupt		_siginterrupt
#define sl_add			_sl_add
#define sl_find			_sl_find
#define sl_free			_sl_free
#define sl_init			_sl_init
#define sleep			_sleep
#define snprintf		_snprintf
#define sradixsort		_sradixsort
#define srand48			_srand48
#define srandom			_srandom
#define strcasecmp		_strcasecmp
#define strncasecmp		_strncasecmp
#define strptime		_strptime
#define strsignal		_strsignal
#define strtok_r		_strtok_r
#define strunvis		_strunvis
#define strvis			_strvis
#define strvisx			_strvisx
#define svc_getreq		_svc_getreq
#define svc_getreqset		_svc_getreqset
#define svc_register		_svc_register
#define svc_run			_svc_run
#define svc_sendreply		_svc_sendreply
#define svc_unregister		_svc_unregister
#define svcerr_auth		_svcerr_auth
#define svcerr_decode		_svcerr_decode
#define svcerr_noproc		_svcerr_noproc
#define svcerr_noprog		_svcerr_noprog
#define svcerr_progvers		_svcerr_progvers
#define svcerr_systemerr	_svcerr_systemerr
#define svcerr_weakauth		_svcerr_weakauth
#define svcfd_create		_svcfd_create
#define svcraw_create		_svcraw_create
#define svctcp_create		_svctcp_create
#define svcudp_bufcreate	_svcudp_bufcreate
#define svcudp_create		_svcudp_create
#define svcudp_enablecache	_svcudp_enablecache
#define sysconf			_sysconf
#define sysctl			_sysctl
#define syslog			_syslog
#define tcdrain			_tcdrain
#define tcflow			_tcflow
#define tcflush			_tcflush
#define tcgetattr		_tcgetattr
#define tcgetpgrp		_tcgetpgrp
#define tcgetsid		_tcgetsid
#define tcsendbreak		_tcsendbreak
#define tcsetattr		_tcsetattr
#define tcsetpgrp		_tcsetpgrp
#define telldir			_telldir
#define time			_time
#define time2posix		_time2posix
#define timegm			_timegm
#define timelocal		_timelocal
#define timeoff			_timeoff
#define times			_times
#define timezone		_timezone
#define ttyname			_ttyname
#define ttyslot			_ttyslot
#define tzset			_tzset
#define tzsetwall		_tzsetwall
#define ualarm			_ualarm
#define uname			_uname
#define unsetenv		_unsetenv
#define unvis			_unvis
#define user_from_uid		_user_from_uid
#define usleep			_usleep
#define utime			_utime
#define valloc			_valloc
#define vis			_vis
#define vsnprintf		_vsnprintf
#define vsyslog			_vsyslog
#define wait			_wait
#define wait3			_wait3
#define waitpid			_waitpid
#define xdr_accepted_reply	_xdr_accepted_reply
#define xdr_array		_xdr_array
#define xdr_authunix_parms	_xdr_authunix_parms
#define xdr_bool		_xdr_bool
#define xdr_bytes		_xdr_bytes
#define xdr_callhdr		_xdr_callhdr
#define xdr_callmsg		_xdr_callmsg
#define xdr_char		_xdr_char
#define xdr_datum		_xdr_datum
#define xdr_des_block		_xdr_des_block
#define xdr_domainname		_xdr_domainname
#define xdr_double		_xdr_double
#define xdr_enum		_xdr_enum
#define xdr_float		_xdr_float
#define xdr_free		_xdr_free
#define xdr_int			_xdr_int
#define xdr_int16_t		_xdr_int16_t
#define xdr_int32_t		_xdr_int32_t
#define xdr_long		_xdr_long
#define xdr_mapname		_xdr_mapname
#define xdr_netobj		_xdr_netobj
#define xdr_opaque		_xdr_opaque
#define xdr_opaque_auth		_xdr_opaque_auth
#define xdr_peername		_xdr_peername
#define xdr_pmap		_xdr_pmap
#define xdr_pmaplist		_xdr_pmaplist
#define xdr_pointer		_xdr_pointer
#define xdr_reference		_xdr_reference
#define xdr_rejected_reply	_xdr_rejected_reply
#define xdr_replymsg		_xdr_replymsg
#define xdr_rmtcall_args	_xdr_rmtcall_args
#define xdr_rmtcallres		_xdr_rmtcallres
#define xdr_short		_xdr_short
#define xdr_string		_xdr_string
#define xdr_u_char		_xdr_u_char
#define xdr_u_int		_xdr_u_int
#define xdr_u_int16_t		_xdr_u_int16_t
#define xdr_u_int32_t		_xdr_u_int32_t
#define xdr_u_long		_xdr_u_long
#define xdr_u_short		_xdr_u_short
#define xdr_union		_xdr_union
#define xdr_vector		_xdr_vector
#define xdr_void		_xdr_void
#define xdr_wrapstring		_xdr_wrapstring
#define xdr_yp_inaddr		_xdr_yp_inaddr
#define xdr_ypall		_xdr_ypall
#define xdr_ypbind_resp		_xdr_ypbind_resp
#define xdr_ypbind_setdom	_xdr_ypbind_setdom
#define xdr_ypdomain_wrap_string	_xdr_ypdomain_wrap_string
#define xdr_ypmap_parms		_xdr_ypmap_parms
#define xdr_ypmap_wrap_string	_xdr_ypmap_wrap_string
#define xdr_ypmaplist		_xdr_ypmaplist
#define xdr_ypowner_wrap_string _xdr_ypowner_wrap_string
#define xdr_yppushresp_xfr	_xdr_yppushresp_xfr
#define xdr_ypreq_key		_xdr_ypreq_key
#define xdr_ypreq_nokey		_xdr_ypreq_nokey
#define xdr_ypreq_xfr		_xdr_ypreq_xfr
#define xdr_ypresp_key_val	_xdr_ypresp_key_val
#define xdr_ypresp_maplist	_xdr_ypresp_maplist
#define xdr_ypresp_master	_xdr_ypresp_master
#define xdr_ypresp_order	_xdr_ypresp_order
#define xdr_ypresp_val		_xdr_ypresp_val
#define xdrmem_create		_xdrmem_create
#define xdrrec_create		_xdrrec_create
#define xdrrec_endofrecord	_xdrrec_endofrecord
#define xdrrec_eof		_xdrrec_eof
#define xdrrec_skiprecord	_xdrrec_skiprecord
#define xdrstdio_create		_xdrstdio_create
#define xprt_register		_xprt_register
#define xprt_unregister		_xprt_unregister
#define yp_all			_yp_all
#define yp_bind			_yp_bind
#define yp_first		_yp_first
#define yp_get_default_domain	_yp_get_default_domain
#define yp_maplist		_yp_maplist
#define yp_master		_yp_master
#define yp_match		_yp_match
#define yp_next			_yp_next
#define yp_order		_yp_order
#define yp_unbind		_yp_unbind
#define yperr_string		_yperr_string
#define ypprot_err		_ypprot_err
#endif

#endif
