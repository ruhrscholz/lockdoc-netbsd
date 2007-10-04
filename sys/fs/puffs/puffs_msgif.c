/*	$NetBSD: puffs_msgif.c,v 1.44 2007/10/04 21:20:47 pooka Exp $	*/

/*
 * Copyright (c) 2005, 2006, 2007  Antti Kantee.  All Rights Reserved.
 *
 * Development of this software was supported by the
 * Google Summer of Code program and the Ulla Tuominen Foundation.
 * The Google SoC project was mentored by Bill Studenmund.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: puffs_msgif.c,v 1.44 2007/10/04 21:20:47 pooka Exp $");

#include <sys/param.h>
#include <sys/fstrans.h>
#include <sys/malloc.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <sys/lock.h>
#include <sys/proc.h>

#include <fs/puffs/puffs_msgif.h>
#include <fs/puffs/puffs_sys.h>

/*
 * waitq data structures
 */

/*
 * While a request is going to userspace, park the caller within the
 * kernel.  This is the kernel counterpart of "struct puffs_req".
 */
struct puffs_park {
	struct puffs_req	*park_preq;	/* req followed by buf	*/
	uint64_t		park_id;	/* duplicate of preq_id */

	size_t			park_copylen;	/* userspace copylength	*/
	size_t			park_maxlen;	/* max size in comeback */

	parkdone_fn		park_done;
	void			*park_donearg;

	int			park_flags;
	int			park_refcount;

	kcondvar_t		park_cv;
	kmutex_t		park_mtx;

	TAILQ_ENTRY(puffs_park) park_entries;
};
#define PARKFLAG_WAITERGONE	0x01
#define PARKFLAG_DONE		0x02
#define PARKFLAG_ONQUEUE1	0x04
#define PARKFLAG_ONQUEUE2	0x08
#define PARKFLAG_CALL		0x10
#define PARKFLAG_WANTREPLY	0x20

static struct pool_cache parkpc;
static struct pool parkpool;

static int
makepark(void *arg, void *obj, int flags)
{
	struct puffs_park *park = obj;

	mutex_init(&park->park_mtx, MUTEX_DEFAULT, IPL_NONE);
	cv_init(&park->park_cv, "puffsrpl");

	return 0;
}

static void
nukepark(void *arg, void *obj)
{
	struct puffs_park *park = obj;

	cv_destroy(&park->park_cv);
	mutex_destroy(&park->park_mtx);
}

void
puffs_msgif_init()
{

	pool_init(&parkpool, sizeof(struct puffs_park), 0, 0, 0,
	    "puffprkl", &pool_allocator_nointr, IPL_NONE);
	pool_cache_init(&parkpc, &parkpool, makepark, nukepark, NULL);
}

void
puffs_msgif_destroy()
{

	pool_cache_destroy(&parkpc);
	pool_destroy(&parkpool);
}

void *
puffs_park_alloc(int waitok)
{
	struct puffs_park *park;

	park = pool_cache_get(&parkpc, waitok ? PR_WAITOK : PR_NOWAIT);
	if (park) {
		park->park_refcount = 1;
		mutex_enter(&park->park_mtx);
	}

	return park;
}

static void
puffs_park_reference(struct puffs_park *park)
{

	mutex_enter(&park->park_mtx);
	park->park_refcount++;
}

void
puffs_park_release(void *arg, int fullnuke)
{
	struct puffs_park *park = arg;

	KASSERT(mutex_owned(&park->park_mtx));
	--park->park_refcount;

	mutex_exit(&park->park_mtx);
	if (park->park_refcount == 0 || fullnuke)
		pool_cache_put(&parkpc, park);
}

#ifdef PUFFSDEBUG
static void
parkdump(struct puffs_park *park)
{

	DPRINTF(("park %p, preq %p, id %" PRIu64 "\n"
	    "\tcopy %zu, max %zu - done: %p/%p\n"
	    "\tflags 0x%08x, refcount %d, cv/mtx: %p/%p\n",
	    park, park->park_preq, park->park_id,
	    park->park_copylen, park->park_maxlen,
	    park->park_done, park->park_donearg,
	    park->park_flags, park->park_refcount,
	    &park->park_cv, &park->park_mtx));
}

static void
parkqdump(struct puffs_wq *q, int dumpall)
{
	struct puffs_park *park;
	int total = 0;

	TAILQ_FOREACH(park, q, park_entries) {
		if (dumpall)
			parkdump(park);
		total++;
	}
	DPRINTF(("puffs waitqueue at %p dumped, %d total\n", q, total));

}
#endif /* PUFFSDEBUG */

/*
 * Converts a non-FAF op to a FAF.  This simply involves making copies
 * of the park and request structures and tagging the request as a FAF.
 * It is safe to block here, since the original op is not a FAF.
 */
static void
puffs_reqtofaf(struct puffs_park *park)
{
	struct puffs_req *newpreq;

	KASSERT((park->park_preq->preq_opclass & PUFFSOPFLAG_FAF) == 0);

	MALLOC(newpreq, struct puffs_req *, park->park_copylen,
	    M_PUFFS, M_ZERO | M_WAITOK);

	memcpy(newpreq, park->park_preq, park->park_copylen);

	park->park_preq = newpreq;
	park->park_preq->preq_opclass |= PUFFSOPFLAG_FAF;
	park->park_flags &= ~PARKFLAG_WANTREPLY;
}


/*
 * kernel-user-kernel waitqueues
 */

static int touser(struct puffs_mount *, struct puffs_park *, uint64_t);

uint64_t
puffs_getreqid(struct puffs_mount *pmp)
{
	uint64_t rv;

	mutex_enter(&pmp->pmp_lock);
	rv = pmp->pmp_nextreq++;
	mutex_exit(&pmp->pmp_lock);

	return rv;
}

/* vfs request */
int
puffs_vfstouser(struct puffs_mount *pmp, int optype, void *kbuf, size_t buflen)
{
	struct puffs_park *park;

	park = puffs_park_alloc(1);
	park->park_preq = kbuf;

	park->park_preq->preq_opclass = PUFFSOP_VFS; 
	park->park_preq->preq_optype = optype;

	park->park_maxlen = park->park_copylen = buflen;
	park->park_flags = 0;

	return touser(pmp, park, puffs_getreqid(pmp));
}

void
puffs_suspendtouser(struct puffs_mount *pmp, int status)
{
	struct puffs_vfsreq_suspend *pvfsr_susp;
	struct puffs_park *park;

	pvfsr_susp = malloc(sizeof(struct puffs_vfsreq_suspend),
	    M_PUFFS, M_WAITOK | M_ZERO);
	park = puffs_park_alloc(1);

	pvfsr_susp->pvfsr_status = status;
	park->park_preq = (struct puffs_req *)pvfsr_susp;

	park->park_preq->preq_opclass = PUFFSOP_VFS | PUFFSOPFLAG_FAF;
	park->park_preq->preq_optype = PUFFS_VFS_SUSPEND;

	park->park_maxlen = park->park_copylen
	    = sizeof(struct puffs_vfsreq_suspend);
	park->park_flags = 0;

	(void)touser(pmp, park, 0);
}

/*
 * vnode level request
 */
int
puffs_vntouser(struct puffs_mount *pmp, int optype,
	void *kbuf, size_t buflen, size_t maxdelta,
	struct vnode *vp_opc, struct vnode *vp_aux)
{
	struct puffs_park *park;
	struct puffs_req *preq;
	void *cookie = VPTOPNC(vp_opc);
	struct puffs_node *pnode;
	int rv;

	park = puffs_park_alloc(1);
	park->park_preq = kbuf;

	park->park_preq->preq_opclass = PUFFSOP_VN; 
	park->park_preq->preq_optype = optype;
	park->park_preq->preq_cookie = cookie;

	park->park_copylen = buflen;
	park->park_maxlen = buflen + maxdelta;
	park->park_flags = 0;

	rv = touser(pmp, park, puffs_getreqid(pmp));

	/*
	 * Check if the user server requests that inactive be called
	 * when the time is right.
	 */
	preq = park->park_preq;
	if (preq->preq_setbacks & PUFFS_SETBACK_INACT_N1) {
		pnode = vp_opc->v_data;
		pnode->pn_stat |= PNODE_DOINACT;
	}
	if (preq->preq_setbacks & PUFFS_SETBACK_INACT_N2) {
		/* if no vp_aux, just ignore */
		if (vp_aux) {
			pnode = vp_aux->v_data;
			pnode->pn_stat |= PNODE_DOINACT;
		}
	}
	if (preq->preq_setbacks & PUFFS_SETBACK_NOREF_N1) {
		pnode = vp_opc->v_data;
		pnode->pn_stat |= PNODE_NOREFS;
	}
	if (preq->preq_setbacks & PUFFS_SETBACK_NOREF_N2) {
		/* if no vp_aux, just ignore */
		if (vp_aux) {
			pnode = vp_aux->v_data;
			pnode->pn_stat |= PNODE_NOREFS;
		}
	}

	return rv;
}

int
puffs_cookietouser(struct puffs_mount *pmp, int optype,
	void *kbuf, size_t buflen, void *cookie, int faf)
{
	struct puffs_park *park;

	park = puffs_park_alloc(1);
	park->park_preq = kbuf;

	park->park_preq->preq_opclass = PUFFSOP_VN |(faf ? PUFFSOPFLAG_FAF : 0);
	park->park_preq->preq_optype = optype;
	park->park_preq->preq_cookie = cookie;

	park->park_copylen = park->park_maxlen = buflen;
	park->park_flags = 0;

	return touser(pmp, park, puffs_getreqid(pmp));
}

/*
 * vnode level request, caller-controller req id
 */
int
puffs_vntouser_req(struct puffs_mount *pmp, int optype,
	void *kbuf, size_t buflen, size_t maxdelta,
	uint64_t reqid, struct vnode *vp_opc, struct vnode *vp_aux)
{
	struct puffs_park *park;
	void *cookie = VPTOPNC(vp_opc);

	park = puffs_park_alloc(1);
	park->park_preq = kbuf;

	park->park_preq->preq_opclass = PUFFSOP_VN; 
	park->park_preq->preq_optype = optype;
	park->park_preq->preq_cookie = cookie;

	park->park_copylen = buflen;
	park->park_maxlen = buflen + maxdelta;
	park->park_flags = 0;

	return touser(pmp, park, reqid);
}

void
puffs_vntouser_call(struct puffs_mount *pmp, int optype,
	void *kbuf, size_t buflen, size_t maxdelta,
	parkdone_fn donefn, void *donearg,
	struct vnode *vp_opc, struct vnode *vp_aux)
{
	struct puffs_park *park;
	void *cookie = VPTOPNC(vp_opc);

	park = puffs_park_alloc(1);
	park->park_preq = kbuf;

	park->park_preq->preq_opclass = PUFFSOP_VN; 
	park->park_preq->preq_optype = optype;
	park->park_preq->preq_cookie = cookie;

	park->park_copylen = buflen;
	park->park_maxlen = buflen + maxdelta;
	park->park_done = donefn;
	park->park_donearg = donearg;
	park->park_flags = PARKFLAG_CALL;

	(void) touser(pmp, park, puffs_getreqid(pmp));
}

/*
 * Notice: kbuf will be free'd later.  I must be allocated from the
 * kernel heap and it's ownership is shifted to this function from
 * now on, i.e. the caller is not allowed to use it anymore!
 */
void
puffs_vntouser_faf(struct puffs_mount *pmp, int optype,
	void *kbuf, size_t buflen, struct vnode *vp_opc)
{
	struct puffs_park *park;
	void *cookie = VPTOPNC(vp_opc);

	/* XXX: is it allowable to sleep here? */
	park = puffs_park_alloc(0);
	if (park == NULL)
		return; /* 2bad */

	park->park_preq = kbuf;

	park->park_preq->preq_opclass = PUFFSOP_VN | PUFFSOPFLAG_FAF;
	park->park_preq->preq_optype = optype;
	park->park_preq->preq_cookie = cookie;

	park->park_maxlen = park->park_copylen = buflen;
	park->park_flags = 0;

	(void)touser(pmp, park, 0);
}

void
puffs_cacheop(struct puffs_mount *pmp, struct puffs_park *park,
	struct puffs_cacheinfo *pcinfo, size_t pcilen, void *cookie)
{

	park->park_preq = (struct puffs_req *)pcinfo;
	park->park_preq->preq_opclass = PUFFSOP_CACHE | PUFFSOPFLAG_FAF;
	park->park_preq->preq_optype = PCACHE_TYPE_WRITE; /* XXX */
	park->park_preq->preq_cookie = cookie;

	park->park_maxlen = park->park_copylen = pcilen;
	park->park_flags = 0;

	(void)touser(pmp, park, 0); 
}

void
puffs_errnotify(struct puffs_mount *pmp, uint8_t type, int error,
	const char *str, void *cookie)
{
	struct puffs_park *park;
	struct puffs_error *perr;

	park = puffs_park_alloc(1);
	MALLOC(perr, struct puffs_error *, sizeof(struct puffs_error),
	    M_PUFFS, M_ZERO | M_WAITOK);

	perr->perr_error = error;
	strlcpy(perr->perr_str, str, sizeof(perr->perr_str));

	park->park_preq = (struct puffs_req *)perr;
	park->park_preq->preq_opclass = PUFFSOP_ERROR | PUFFSOPFLAG_FAF;
	park->park_preq->preq_optype = type;
	park->park_preq->preq_cookie = cookie;

	park->park_maxlen = park->park_copylen = sizeof(struct puffs_error);
	park->park_flags = 0;

	(void)touser(pmp, park, 0);
}

/*
 * Wait for the userspace ping-pong game in calling process context.
 *
 * This unlocks vnodes if they are supplied.  vp1 is the vnode
 * before in the locking order, i.e. the one which must be locked
 * before accessing vp2.  This is done here so that operations are
 * already ordered in the queue when vnodes are unlocked (I'm not
 * sure if that's really necessary, but it can't hurt).  Okok, maybe
 * there's a slight ugly-factor also, but let's not worry about that.
 */
static int
touser(struct puffs_mount *pmp, struct puffs_park *park, uint64_t reqid)
{
	struct lwp *l = curlwp;
	struct mount *mp;
	struct puffs_req *preq;
	int rv = 0;

	mp = PMPTOMP(pmp);
	preq = park->park_preq;
	preq->preq_id = park->park_id = reqid;
	preq->preq_buflen = ALIGN(park->park_maxlen);

	if (PUFFSOP_WANTREPLY(preq->preq_opclass))
		park->park_flags |= PARKFLAG_WANTREPLY;

	/*
	 * To support PCATCH, yet another movie: check if there are signals
	 * pending and we are issueing a non-FAF.  If so, return an error
	 * directly UNLESS we are issueing INACTIVE.  In that case, convert
	 * it to a FAF, fire off to the file server and return an error.
	 * Yes, this is bordering disgusting.  Barfbags are on me.
	 */
	if ((park->park_flags & PARKFLAG_WANTREPLY)
	   && (park->park_flags & PARKFLAG_CALL) == 0
	   && (l->l_flag & LW_PENDSIG) != 0 && sigispending(l, 0)) {
		if (PUFFSOP_OPCLASS(preq->preq_opclass) == PUFFSOP_VN
		    && preq->preq_optype == PUFFS_VN_INACTIVE) {
			puffs_reqtofaf(park);
			DPRINTF(("puffs touser: converted to FAF %p\n", park));
			rv = EINTR;
		} else {
			puffs_park_release(park, 0);
			return EINTR;
		}
	}

	/*
	 * test for suspension lock.
	 *
	 * Note that we *DO NOT* keep the lock, since that might block
	 * lock acquiring PLUS it would give userlandia control over
	 * the lock.  The operation queue enforces a strict ordering:
	 * when the fs server gets in the op stream, it knows things
	 * are in order.  The kernel locks can't guarantee that for
	 * userspace, in any case.
	 *
	 * BUT: this presents a problem for ops which have a consistency
	 * clause based on more than one operation.  Unfortunately such
	 * operations (read, write) do not reliably work yet.
	 *
	 * Ya, Ya, it's wrong wong wrong, me be fixink this someday.
	 *
	 * XXX: and there is one more problem.  We sometimes need to
	 * take a lazy lock in case the fs is suspending and we are
	 * executing as the fs server context.  This might happen
	 * e.g. in the case that the user server triggers a reclaim
	 * in the kernel while the fs is suspending.  It's not a very
	 * likely event, but it needs to be fixed some day.
	 */

	/*
	 * MOREXXX: once PUFFS_WCACHEINFO is enabled, we can't take
	 * the mutex here, since getpages() might be called locked.
	 */
	fstrans_start(mp, FSTRANS_NORMAL);
	mutex_enter(&pmp->pmp_lock);
	fstrans_done(mp);

	if (pmp->pmp_status != PUFFSTAT_RUNNING) {
		mutex_exit(&pmp->pmp_lock);
		puffs_park_release(park, 0);
		return ENXIO;
	}

#ifdef PUFFSDEBUG
	parkqdump(&pmp->pmp_req_touser, puffsdebug > 1);
	parkqdump(&pmp->pmp_req_replywait, puffsdebug > 1);
#endif

	TAILQ_INSERT_TAIL(&pmp->pmp_req_touser, park, park_entries);
	park->park_flags |= PARKFLAG_ONQUEUE1;
	puffs_mp_reference(pmp);
	pmp->pmp_req_touser_count++;
	mutex_exit(&pmp->pmp_lock);

	DPRINTF(("touser: req %" PRIu64 ", preq: %p, park: %p, "
	    "c/t: 0x%x/0x%x, f: 0x%x\n", preq->preq_id, preq, park,
	    preq->preq_opclass, preq->preq_optype, park->park_flags));

	cv_broadcast(&pmp->pmp_req_waiter_cv);
	selnotify(pmp->pmp_sel, 0);

	if ((park->park_flags & PARKFLAG_WANTREPLY)
	    && (park->park_flags & PARKFLAG_CALL) == 0) {
		int error;

		error = cv_wait_sig(&park->park_cv, &park->park_mtx);
		if (error) {
			park->park_flags |= PARKFLAG_WAITERGONE;
			if (park->park_flags & PARKFLAG_DONE) {
				rv = preq->preq_rv;
				puffs_park_release(park, 0);
			} else {
				/*
				 * ok, we marked it as going away, but
				 * still need to do queue ops.  take locks
				 * in correct order.
				 *
				 * We don't want to release our reference
				 * if it's on replywait queue to avoid error
				 * to file server.  putop() code will DTRT.
				 */
				KASSERT(park->park_flags &
				    (PARKFLAG_ONQUEUE1 | PARKFLAG_ONQUEUE2));
				mutex_exit(&park->park_mtx);

				mutex_enter(&pmp->pmp_lock);
				mutex_enter(&park->park_mtx);
				if (park->park_flags & PARKFLAG_ONQUEUE1) {
					TAILQ_REMOVE(&pmp->pmp_req_touser,
					    park, park_entries);
					pmp->pmp_req_touser_count--;
					park->park_flags &= ~PARKFLAG_ONQUEUE1;
				}
				if ((park->park_flags & PARKFLAG_ONQUEUE2) == 0)
					puffs_park_release(park, 0);
				else
					mutex_exit(&park->park_mtx);
				mutex_exit(&pmp->pmp_lock);

				rv = error;
			}
		} else {
			rv = preq->preq_rv;
			puffs_park_release(park, 0);
		}

		/*
		 * retake the lock and release.  This makes sure (haha,
		 * I'm humorous) that we don't process the same vnode in
		 * multiple threads due to the locks hacks we have in
		 * puffs_lock().  In reality this is well protected by
		 * the biglock, but once that's gone, well, hopefully
		 * this will be fixed for real.  (and when you read this
		 * comment in 2017 and subsequently barf, my condolences ;).
		 */
		if (rv == 0 && !fstrans_is_owner(mp)) {
			fstrans_start(mp, FSTRANS_NORMAL);
			fstrans_done(mp);
		}
	} else {
		mutex_exit(&park->park_mtx);
	}

	mutex_enter(&pmp->pmp_lock);
	puffs_mp_release(pmp);
	mutex_exit(&pmp->pmp_lock);

	return rv;
}


/*
 * getop: scan through queued requests until:
 *  1) max number of requests satisfied
 *     OR
 *  2) buffer runs out of space
 *     OR
 *  3) nonblocking is set AND there are no operations available
 *     OR
 *  4) at least one operation was transferred AND there are no more waiting
 */
int
puffs_getop(struct puffs_mount *pmp, struct puffs_reqh_get *phg, int nonblock)
{
	struct puffs_park *park;
	struct puffs_req *preq;
	uint8_t *bufpos;
	int error, donesome;

	donesome = error = 0;
	bufpos = phg->phg_buf;

	mutex_enter(&pmp->pmp_lock);
	while (phg->phg_nops == 0 || donesome != phg->phg_nops) {
 again:
		if (pmp->pmp_status != PUFFSTAT_RUNNING) {
			/* if we got some, they don't really matter anymore */
			error = ENXIO;
			goto out;
		}
		if (TAILQ_EMPTY(&pmp->pmp_req_touser)) {
			if (donesome)
				goto out;

			if (nonblock) {
				error = EWOULDBLOCK;
				goto out;
			}

			error = cv_wait_sig(&pmp->pmp_req_waiter_cv,
			    &pmp->pmp_lock);
			if (error)
				goto out;
			else
				goto again;
		}

		park = TAILQ_FIRST(&pmp->pmp_req_touser);
		puffs_park_reference(park);

		/* If it's a goner, don't process any furher */
		if (park->park_flags & PARKFLAG_WAITERGONE) {
			puffs_park_release(park, 0);
			continue;
		}

		preq = park->park_preq;
		if (phg->phg_buflen < preq->preq_buflen) {
			if (!donesome)
				error = E2BIG;
			puffs_park_release(park, 0);
			goto out;
		}

		TAILQ_REMOVE(&pmp->pmp_req_touser, park, park_entries);
		KASSERT(park->park_flags & PARKFLAG_ONQUEUE1);
		park->park_flags &= ~PARKFLAG_ONQUEUE1;
		pmp->pmp_req_touser_count--;
		KASSERT(pmp->pmp_req_touser_count >= 0);
		mutex_exit(&pmp->pmp_lock);

		DPRINTF(("puffsgetop: get op %" PRIu64 " (%d.), from %p "
		    "len %zu (buflen %zu), target %p\n", preq->preq_id,
		    donesome, preq, park->park_copylen, preq->preq_buflen,
		    bufpos));

		if ((error = copyout(preq, bufpos, park->park_copylen)) != 0) {
			DPRINTF(("puffs_getop: copyout failed: %d\n", error));
			/*
			 * ok, user server is probably trying to cheat.
			 * stuff op back & return error to user.  We need
			 * to take locks in the correct order.
			 */
			mutex_exit(&park->park_mtx);

			/*
			 * XXX: ONQUEUE1 | ONQUEUE2 invariant doesn't
			 * hold here
			 */

			mutex_enter(&pmp->pmp_lock);
			mutex_enter(&park->park_mtx);
			if ((park->park_flags & PARKFLAG_WAITERGONE) == 0) {
				 TAILQ_INSERT_HEAD(&pmp->pmp_req_touser, park,
				     park_entries);
				 park->park_flags |= PARKFLAG_ONQUEUE1;
				 pmp->pmp_req_touser_count++;
			}

			if (donesome)
				error = 0;
			puffs_park_release(park, 0);
			goto out;
		}
		bufpos += preq->preq_buflen;
		phg->phg_buflen -= preq->preq_buflen;
		donesome++;

		/* XXXfixme: taking this lock in the wrong order */
		mutex_enter(&pmp->pmp_lock);

		if (park->park_flags & PARKFLAG_WANTREPLY) {
			TAILQ_INSERT_TAIL(&pmp->pmp_req_replywait, park,
			    park_entries);
			park->park_flags |= PARKFLAG_ONQUEUE2;
			puffs_park_release(park, 0);
		} else {
			free(preq, M_PUFFS);
			puffs_park_release(park, 1);
		}
	}

 out:
	phg->phg_more = pmp->pmp_req_touser_count;
	mutex_exit(&pmp->pmp_lock);

	phg->phg_nops = donesome;

	return error;
}

int
puffs_putop(struct puffs_mount *pmp, struct puffs_reqh_put *php)
{
	struct puffs_park *park;
	struct puffs_req tmpreq;
	struct puffs_req *nextpreq;
	void *userbuf;
	uint64_t id;
	size_t reqlen;
	int donesome, error, wgone, release;

	donesome = error = wgone = 0;

	id = php->php_id;
	userbuf = php->php_buf;
	reqlen = php->php_buflen;

	mutex_enter(&pmp->pmp_lock);
	while (donesome != php->php_nops) {
		release = 0;
#ifdef PUFFSDEBUG
		DPRINTF(("puffsputop: searching for %" PRIu64 ", ubuf: %p, "
		    "len %zu\n", id, userbuf, reqlen));
#endif
		TAILQ_FOREACH(park, &pmp->pmp_req_replywait, park_entries) {
			if (park->park_id == id)
				break;
		}

		if (park == NULL) {
			DPRINTF(("puffsputop: no request: %" PRIu64 "\n", id));
			error = EINVAL;
			break;
		}

		puffs_park_reference(park);
		if (reqlen == 0 || reqlen > park->park_maxlen) {
			DPRINTF(("puffsputop: invalid buffer length: "
			    "%zu\n", reqlen));
			error = E2BIG;
			puffs_park_release(park, 0);
			break;
		}
		wgone = park->park_flags & PARKFLAG_WAITERGONE;

		/* check if it's still on the queue after acquiring lock */
		if (park->park_flags & PARKFLAG_ONQUEUE2) {
			TAILQ_REMOVE(&pmp->pmp_req_replywait, park,
			    park_entries);
			park->park_flags &= ~PARKFLAG_ONQUEUE2;
		}

		mutex_exit(&pmp->pmp_lock);

		/*
		 * If the caller has gone south, go to next, collect
		 * $200 and free the structure there instead of wakeup.
		 * We also need to copyin the header info.  Flag structure
		 * release to mode total and utter destruction.
		 */
		if (wgone) {
			DPRINTF(("puffs_putop: bad service - waiter gone for "
			    "park %p\n", park));
			error = copyin(userbuf, &tmpreq,
			    sizeof(struct puffs_req));
			release = 1;
			if (error)
				goto loopout;
			nextpreq = &tmpreq;
			goto next;
		}

		DPRINTF(("puffsputpop: copyin from %p to %p, len %zu\n",
		    userbuf, park->park_preq, reqlen));
		error = copyin(userbuf, park->park_preq, reqlen);
		if (error)
			goto loopout;
		nextpreq = park->park_preq;

 next:
		/* all's well, prepare for next op */
		id = nextpreq->preq_id;
		reqlen = nextpreq->preq_buflen;
		userbuf = nextpreq->preq_nextbuf;
		donesome++;

 loopout:
		if (error && !wgone)
			park->park_preq->preq_rv = error;

		if (park->park_flags & PARKFLAG_CALL) {
			DPRINTF(("puffsputopt: call for %p, arg %p\n",
			    park->park_preq, park->park_donearg));
			park->park_done(pmp,park->park_preq,park->park_donearg);
			release = 1;
		}

		if (!wgone) {
			DPRINTF(("puffs_putop: flagging done for "
			    "park %p\n", park));

			cv_signal(&park->park_cv);
		}
		park->park_flags |= PARKFLAG_DONE;
		puffs_park_release(park, release);

		mutex_enter(&pmp->pmp_lock);
		if (error)
			break;
		wgone = 0;
	}

	mutex_exit(&pmp->pmp_lock);
	php->php_nops -= donesome;

	return error;
}

/*
 * We're dead, kaput, RIP, slightly more than merely pining for the
 * fjords, belly-up, fallen, lifeless, finished, expired, gone to meet
 * our maker, ceased to be, etcetc.  YASD.  It's a dead FS!
 *
 * Caller must hold puffs mutex.
 */
void
puffs_userdead(struct puffs_mount *pmp)
{
	struct puffs_park *park, *park_next;

	/*
	 * Mark filesystem status as dying so that operations don't
	 * attempt to march to userspace any longer.
	 */
	pmp->pmp_status = PUFFSTAT_DYING;

	/* signal waiters on REQUEST TO file server queue */
	for (park = TAILQ_FIRST(&pmp->pmp_req_touser); park; park = park_next) {
		uint8_t opclass;

		puffs_park_reference(park);
		park_next = TAILQ_NEXT(park, park_entries);

		KASSERT(park->park_flags & PARKFLAG_ONQUEUE1);
		TAILQ_REMOVE(&pmp->pmp_req_touser, park, park_entries);
		park->park_flags &= ~PARKFLAG_ONQUEUE1;
		pmp->pmp_req_touser_count--;

		/*
		 * If the waiter is gone, we may *NOT* access preq anymore.
		 */
		if (park->park_flags & PARKFLAG_WAITERGONE) {
			KASSERT((park->park_flags & PARKFLAG_CALL) == 0);
			KASSERT(park->park_flags & PARKFLAG_WANTREPLY);
			puffs_park_release(park, 0);
		} else {
			opclass = park->park_preq->preq_opclass;
			park->park_preq->preq_rv = ENXIO;

			if (park->park_flags & PARKFLAG_CALL) {
				park->park_done(pmp, park->park_preq,
				    park->park_donearg);
				puffs_park_release(park, 1);
			} else if ((park->park_flags & PARKFLAG_WANTREPLY)==0) {
				free(park->park_preq, M_PUFFS);
				puffs_park_release(park, 1);
			} else {
				park->park_preq->preq_rv = ENXIO;
				cv_signal(&park->park_cv);
				puffs_park_release(park, 0);
			}
		}
	}

	/* signal waiters on RESPONSE FROM file server queue */
	for (park=TAILQ_FIRST(&pmp->pmp_req_replywait); park; park=park_next) {
		puffs_park_reference(park);
		park_next = TAILQ_NEXT(park, park_entries);

		KASSERT(park->park_flags & PARKFLAG_ONQUEUE2);
		KASSERT(park->park_flags & PARKFLAG_WANTREPLY);

		TAILQ_REMOVE(&pmp->pmp_req_replywait, park, park_entries);
		park->park_flags &= ~PARKFLAG_ONQUEUE2;

		/*
		 * If the waiter is gone, we may *NOT* access preq anymore.
		 */
		if (park->park_flags & PARKFLAG_WAITERGONE) {
			KASSERT((park->park_flags & PARKFLAG_CALL) == 0);
			puffs_park_release(park, 0);
		} else {
			park->park_preq->preq_rv = ENXIO;
			if (park->park_flags & PARKFLAG_CALL) {
				park->park_done(pmp, park->park_preq,
				    park->park_donearg);
				puffs_park_release(park, 1);
			} else {
				cv_signal(&park->park_cv);
				puffs_park_release(park, 0);
			}
		}
	}
}
