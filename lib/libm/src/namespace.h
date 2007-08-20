/* $NetBSD: namespace.h,v 1.2 2007/08/20 16:01:39 drochner Exp $ */

#define atan2 _atan2
#define atan2f _atan2f
#define hypot _hypot
#define hypotf _hypotf

#define exp _exp
#define expf _expf
#define log _log
#define logf _logf

#if 0 /* not yet - need to review use in machdep code first */
#define sin _sin
#define sinf _sinf
#define cos _cos
#define cosf _cosf
#endif /* notyet */
#define sinh _sinh
#define sinhf _sinhf
#define cosh _cosh
#define coshf _coshf
#define asin _asin
#define asinf _asinf

#define casin _casin
#define casinf _casinf
#define catan _catan
#define catanf _catanf
