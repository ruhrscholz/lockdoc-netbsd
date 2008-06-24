/*	$NetBSD: linux32_socket.c,v 1.7 2008/06/24 09:00:04 njoly Exp $ */

/*-
 * Copyright (c) 2006 Emmanuel Dreyfus, all rights reserved.
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
 *	This product includes software developed by Emmanuel Dreyfus
 * 4. The name of the author may not be used to endorse or promote 
 *    products derived from this software without specific prior written 
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE THE AUTHOR AND CONTRIBUTORS ``AS IS'' 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>

__KERNEL_RCSID(0, "$NetBSD: linux32_socket.c,v 1.7 2008/06/24 09:00:04 njoly Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/fstypes.h>
#include <sys/signal.h>
#include <sys/dirent.h>
#include <sys/kernel.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/proc.h>
#include <sys/ucred.h>
#include <sys/swap.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/filedesc.h>

#include <machine/types.h>

#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/ip_mroute.h>

#include <sys/syscallargs.h>

#include <compat/netbsd32/netbsd32.h>
#include <compat/netbsd32/netbsd32_ioctl.h>
#include <compat/netbsd32/netbsd32_conv.h>
#include <compat/netbsd32/netbsd32_syscallargs.h>
#include <compat/sys/sockio.h>

#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/common/linux_misc.h>
#include <compat/linux/common/linux_oldolduname.h>
#include <compat/linux/common/linux_ioctl.h>
#include <compat/linux/common/linux_sockio.h>
#include <compat/linux/linux_syscallargs.h>

#include <compat/linux32/common/linux32_types.h>
#include <compat/linux32/common/linux32_signal.h>
#include <compat/linux32/common/linux32_machdep.h>
#include <compat/linux32/common/linux32_sysctl.h>
#include <compat/linux32/common/linux32_socketcall.h>
#include <compat/linux32/common/linux32_ioctl.h>
#include <compat/linux32/linux32_syscallargs.h>

int
linux32_sys_socketpair(struct lwp *l, const struct linux32_sys_socketpair_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) domain;
		syscallarg(int) type;
		syscallarg(int) protocol;
		syscallarg(netbsd32_intp) rsv;
	} */
	struct linux_sys_socketpair_args ua;

	NETBSD32TO64_UAP(domain);
	NETBSD32TO64_UAP(type);
	NETBSD32TO64_UAP(protocol);
	NETBSD32TOP_UAP(rsv, int)

	return linux_sys_socketpair(l, &ua, retval);
}

int
linux32_sys_sendto(struct lwp *l, const struct linux32_sys_sendto_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_voidp) msg;
		syscallarg(int) len;
		syscallarg(int) flags;
		syscallarg(netbsd32_osockaddrp_t) to;
		syscallarg(int) tolen;
	} */
	struct linux_sys_sendto_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(msg, void);
	NETBSD32TO64_UAP(len);
	NETBSD32TO64_UAP(flags);
	NETBSD32TOP_UAP(to, struct osockaddr);
	NETBSD32TO64_UAP(tolen);

	return linux_sys_sendto(l, &ua, retval);
}


int
linux32_sys_recvfrom(struct lwp *l, const struct linux32_sys_recvfrom_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_voidp) buf;
		syscallarg(netbsd32_size_t) len;
		syscallarg(int) flags;
		syscallarg(netbsd32_osockaddrp_t) from;
		syscallarg(netbsd32_intp) fromlenaddr;
	} */
	struct linux_sys_recvfrom_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(buf, void);
	NETBSD32TO64_UAP(len);
	NETBSD32TO64_UAP(flags);
	NETBSD32TOP_UAP(from, struct osockaddr);
	NETBSD32TOP_UAP(fromlenaddr, unsigned int);

	return linux_sys_recvfrom(l, &ua, retval);
}

int
linux32_sys_setsockopt(struct lwp *l, const struct linux32_sys_setsockopt_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(int) level;
		syscallarg(int) optname;
		syscallarg(netbsd32_voidp) optval;
		syscallarg(int) optlen;
	} */
	struct linux_sys_setsockopt_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TO64_UAP(level);
	NETBSD32TO64_UAP(optname);
	NETBSD32TOP_UAP(optval, void);
	NETBSD32TO64_UAP(optlen);

	return linux_sys_setsockopt(l, &ua, retval);
}


int
linux32_sys_getsockopt(struct lwp *l, const struct linux32_sys_getsockopt_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(int) level;
		syscallarg(int) optname;
		syscallarg(netbsd32_voidp) optval;
		syscallarg(netbsd32_intp) optlen;
	} */
	struct linux_sys_getsockopt_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TO64_UAP(level);
	NETBSD32TO64_UAP(optname);
	NETBSD32TOP_UAP(optval, void);
	NETBSD32TOP_UAP(optlen, int);

	return linux_sys_getsockopt(l, &ua, retval);
}

int
linux32_sys_socket(struct lwp *l, const struct linux32_sys_socket_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) domain;
		syscallarg(int) type;
		syscallarg(int) protocol;
	} */
	struct linux_sys_socket_args ua;

	NETBSD32TO64_UAP(domain);
	NETBSD32TO64_UAP(type);
	NETBSD32TO64_UAP(protocol);

	return linux_sys_socket(l, &ua, retval);
}

int
linux32_sys_bind(struct lwp *l, const struct linux32_sys_bind_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_osockaddrp_t) name;
		syscallarg(int) namelen;
	} */
	struct linux_sys_bind_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(name, struct osockaddr)
	NETBSD32TO64_UAP(namelen);

	return linux_sys_bind(l, &ua, retval);
}

int
linux32_sys_connect(struct lwp *l, const struct linux32_sys_connect_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_osockaddrp_t) name;
		syscallarg(int) namelen;
	} */
	struct linux_sys_connect_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(name, struct osockaddr)
	NETBSD32TO64_UAP(namelen);

#ifdef DEBUG_LINUX
	printf("linux32_sys_connect: s = %d, name = %p, namelen = %d\n",
		SCARG(&ua, s), SCARG(&ua, name), SCARG(&ua, namelen));
#endif

	return linux_sys_connect(l, &ua, retval);
}

int
linux32_sys_accept(struct lwp *l, const struct linux32_sys_accept_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_osockaddrp_t) name;
		syscallarg(netbsd32_intp) anamelen;
	} */
	struct linux_sys_accept_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(name, struct osockaddr)
	NETBSD32TOP_UAP(anamelen, int);

	return linux_sys_accept(l, &ua, retval);
}

int
linux32_sys_getpeername(struct lwp *l, const struct linux32_sys_getpeername_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) fdes;
		syscallarg(netbsd32_sockaddrp_t) asa;
		syscallarg(netbsd32_intp) alen;
	} */
	struct linux_sys_getpeername_args ua;

	NETBSD32TO64_UAP(fdes);
	NETBSD32TOP_UAP(asa, struct sockaddr)
	NETBSD32TOP_UAP(alen, int);

	return linux_sys_getpeername(l, &ua, retval);
}

int
linux32_sys_getsockname(struct lwp *l, const struct linux32_sys_getsockname_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) fdec;
		syscallarg(netbsd32_charp) asa;
		syscallarg(netbsd32_intp) alen;
	} */
	struct linux_sys_getsockname_args ua;

	NETBSD32TO64_UAP(fdec);
	NETBSD32TOP_UAP(asa, char)
	NETBSD32TOP_UAP(alen, int);

	return linux_sys_getsockname(l, &ua, retval);
}

int
linux32_sys_sendmsg(struct lwp *l, const struct linux32_sys_sendmsg_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_msghdrp_t) msg;
		syscallarg(int) flags;
	} */
	struct linux_sys_sendmsg_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(msg, struct msghdr);
	NETBSD32TO64_UAP(flags);

	return linux_sys_sendmsg(l, &ua, retval);
}

int
linux32_sys_recvmsg(struct lwp *l, const struct linux32_sys_recvmsg_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_msghdrp_t) msg;
		syscallarg(int) flags;
	} */
	struct linux_sys_recvmsg_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(msg, struct msghdr);
	NETBSD32TO64_UAP(flags);

	return linux_sys_recvmsg(l, &ua, retval);
}

int
linux32_sys_send(struct lwp *l, const struct linux32_sys_send_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_voidp) buf;
		syscallarg(int) len;
		syscallarg(int) flags;
	} */
	struct sys_sendto_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(buf, void);
	NETBSD32TO64_UAP(len);
	NETBSD32TO64_UAP(flags);
	SCARG(&ua, to) = NULL;
	SCARG(&ua, tolen) = 0;

	return sys_sendto(l, &ua, retval);
}

int
linux32_sys_recv(struct lwp *l, const struct linux32_sys_recv_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) s;
		syscallarg(netbsd32_voidp) buf;
		syscallarg(int) len;
		syscallarg(int) flags;
	} */
	struct sys_recvfrom_args ua;

	NETBSD32TO64_UAP(s);
	NETBSD32TOP_UAP(buf, void);
	NETBSD32TO64_UAP(len);
	NETBSD32TO64_UAP(flags);
	SCARG(&ua, from) = NULL;
	SCARG(&ua, fromlenaddr) = NULL;

	return sys_recvfrom(l, &ua, retval);
}

int
linux32_ioctl_socket(struct lwp *l, const struct linux32_sys_ioctl_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) fd;
		syscallarg(u_long) com;
		syscallarg(void *) data;
	} */
	u_long com;
	int error = 0, isdev = 0, dosys = 1;
	struct netbsd32_ioctl_args ia;
	file_t *fp;
	struct vnode *vp;
	int (*ioctlf)(file_t *, u_long, void *);
	struct ioctl_pt pt;

	if ((fp = fd_getfile(SCARG(uap, fd))) == NULL)
		return (EBADF);

	if (fp->f_type == DTYPE_VNODE) {
		vp = (struct vnode *)fp->f_data;
		isdev = vp->v_type == VCHR;
	}

	/*
	 * Don't try to interpret socket ioctl calls that are done
	 * on a device filedescriptor, just pass them through, to
	 * emulate Linux behaviour. Use PTIOCLINUX so that the
	 * device will only handle these if it's prepared to do
	 * so, to avoid unexpected things from happening.
	 */
	if (isdev) {
		dosys = 0;
		ioctlf = fp->f_ops->fo_ioctl;
		pt.com = SCARG(uap, com);
		pt.data = (void *)NETBSD32PTR64(SCARG(uap, data));
		error = ioctlf(fp, PTIOCLINUX, &pt);
		/*
		 * XXX hack: if the function returns EJUSTRETURN,
		 * it has stuffed a sysctl return value in pt.data.
		 */
		if (error == EJUSTRETURN) {
			retval[0] = (register_t)pt.data;
			error = 0;
		}
		goto out;
	}

	com = SCARG(uap, com);
	retval[0] = 0;

	switch (com) {
	case LINUX_SIOCGIFCONF:
		SCARG(&ia, com) = OOSIOCGIFCONF32;
		break;
	case LINUX_SIOCGIFFLAGS:
		SCARG(&ia, com) = OSIOCGIFFLAGS;
		break;
	case LINUX_SIOCSIFFLAGS:
		SCARG(&ia, com) = OSIOCSIFFLAGS;
		break;
	case LINUX_SIOCGIFADDR:
		SCARG(&ia, com) = OOSIOCGIFADDR;
		break;
	case LINUX_SIOCGIFDSTADDR:
		SCARG(&ia, com) = OOSIOCGIFDSTADDR;
		break;
	case LINUX_SIOCGIFBRDADDR:
		SCARG(&ia, com) = OOSIOCGIFBRDADDR;
		break;
	case LINUX_SIOCGIFNETMASK:
		SCARG(&ia, com) = OOSIOCGIFNETMASK;
		break;
	case LINUX_SIOCADDMULTI:
		SCARG(&ia, com) = OSIOCADDMULTI;
		break;
	case LINUX_SIOCDELMULTI:
		SCARG(&ia, com) = OSIOCDELMULTI;
		break;
#ifdef notyet
	case LINUX_SIOCGIFHWADDR:
		error = linux_getifhwaddr(l, retval, SCARG(uap, fd),
		    SCARG(uap, data));
		dosys = 0;
		break;
#endif
	default:
		error = EINVAL;
	}

 out:
 	fd_putfile(SCARG(uap, fd));

	if (error == 0 && dosys) {
		SCARG(&ia, fd) = SCARG(uap, fd);
		SCARG(&ia, data) = SCARG(uap, data);
		error = netbsd32_ioctl(curlwp, &ia, retval);
	}

	return error;
}
