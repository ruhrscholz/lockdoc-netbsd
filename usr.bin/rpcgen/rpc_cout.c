/*	$NetBSD: rpc_cout.c,v 1.19 2002/01/31 22:43:57 tv Exp $	*/
/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user or with the express written consent of
 * Sun Microsystems, Inc.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#include <sys/cdefs.h>
#if defined(__RCSID) && !defined(lint)
#if 0
static char sccsid[] = "@(#)rpc_cout.c 1.13 89/02/22 (C) 1987 SMI";
#else
__RCSID("$NetBSD: rpc_cout.c,v 1.19 2002/01/31 22:43:57 tv Exp $");
#endif
#endif

/*
 * rpc_cout.c, XDR routine outputter for the RPC protocol compiler
 */
#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rpc_scan.h"
#include "rpc_parse.h"
#include "rpc_util.h"

static int findtype __P((definition *, char *));
static int undefined __P((char *));
static void print_generic_header __P((char *, int));
static void print_header __P((definition *));
static void print_prog_header __P((proc_list *));
static void print_trailer __P((void));
static void print_ifopen __P((int, char *));
static void print_ifarg __P((char *));
static void print_ifsizeof __P((char *, char *));
static void print_ifclose __P((int));
static void print_ifstat __P((int, char *, char *, relation, char *, char *, char *));
static void emit_enum __P((definition *));
static void emit_program __P((definition *));
static void emit_union __P((definition *));
static void emit_struct __P((definition *));
static void emit_typedef __P((definition *));
static void print_stat __P((int, declaration *));

/*
 * Emit the C-routine for the given definition
 */
void
emit(def)
	definition *def;
{
	if (def->def_kind == DEF_CONST) {
		return;
	}
	if (def->def_kind == DEF_PROGRAM) {
		emit_program(def);
		return;
	}
	if (def->def_kind == DEF_TYPEDEF) {
		/* now we need to handle declarations like struct typedef foo
		 * foo; since we dont want this to be expanded into 2 calls to
		 * xdr_foo */

		if (strcmp(def->def.ty.old_type, def->def_name) == 0)
			return;
	};

	print_header(def);

	switch (def->def_kind) {
	case DEF_UNION:
		emit_union(def);
		break;
	case DEF_ENUM:
		emit_enum(def);
		break;
	case DEF_STRUCT:
		emit_struct(def);
		break;
	case DEF_TYPEDEF:
		emit_typedef(def);
		break;
	case DEF_PROGRAM:
	case DEF_CONST:
		errx(1, "Internal error %s, %d: Case %d not handled\n",
		    __FILE__, __LINE__, def->def_kind);
		break;
	}
	print_trailer();
}

static int
findtype(def, type)
	definition *def;
	char   *type;
{

	if (def->def_kind == DEF_PROGRAM || def->def_kind == DEF_CONST) {
		return (0);
	} else {
		return (streq(def->def_name, type));
	}
}

static int
undefined(type)
	char   *type;
{
	definition *def;

	def = (definition *) FINDVAL(defined, type, findtype);


	return (def == NULL);
}

static void
print_generic_header(procname, pointerp)
	char   *procname;
	int     pointerp;
{
	f_print(fout, "\n");
	f_print(fout, "bool_t\n");
	if (Cflag) {
		f_print(fout, "xdr_%s(", procname);
		f_print(fout, "XDR *xdrs, ");
		f_print(fout, "%s ", procname);
		if (pointerp)
			f_print(fout, "*");
		f_print(fout, "objp)\n{\n");
	} else {
		f_print(fout, "xdr_%s(xdrs, objp)\n", procname);
		f_print(fout, "\tXDR *xdrs;\n");
		f_print(fout, "\t%s ", procname);
		if (pointerp)
			f_print(fout, "*");
		f_print(fout, "objp;\n{\n");
	}
}

static void
print_header(def)
	definition *def;
{
	print_generic_header(def->def_name,
	    def->def_kind != DEF_TYPEDEF ||
	    !isvectordef(def->def.ty.old_type, def->def.ty.rel));
}

static void
print_prog_header(plist)
	proc_list *plist;
{
	print_generic_header(plist->args.argname, 1);
}

static void
print_trailer()
{
	f_print(fout, "\treturn (TRUE);\n");
	f_print(fout, "}\n");
}


static void
print_ifopen(indent, name)
	int     indent;
	char   *name;
{
	char _t_kludge[32];

	/*
	 * XXX Solaris seems to strip the _t. No idea why.
	 */
	if (!strcmp(name, "rpcprog_t") || !strcmp(name, "rpcvers_t") ||
	    !strcmp(name, "rpcproc_t") || !strcmp(name, "rpcprot_t") ||
	    !strcmp(name, "rpcport_t") || !strcmp(name, "rpcpinline_t")) {
		strncpy(_t_kludge, name, strlen(name) - 2);
		name = _t_kludge;
	}
	tabify(fout, indent);
	f_print(fout, "if (!xdr_%s(xdrs", name);
}

static void
print_ifarg(arg)
	char   *arg;
{
	f_print(fout, ", %s", arg);
}

static void
print_ifsizeof(prefix, type)
	char   *prefix;
	char   *type;
{
	if (streq(type, "bool")) {
		f_print(fout, ", sizeof(bool_t), (xdrproc_t)xdr_bool");
	} else {
		f_print(fout, ", sizeof(");
		if (undefined(type) && prefix) {
			f_print(fout, "%s ", prefix);
		}
		f_print(fout, "%s), (xdrproc_t)xdr_%s", type, type);
	}
}

static void
print_ifclose(indent)
	int     indent;
{
	f_print(fout, "))\n");
	tabify(fout, indent);
	f_print(fout, "\treturn (FALSE);\n");
}

static void
print_ifstat(indent, prefix, type, rel, amax, objname, name)
	int     indent;
	char   *prefix;
	char   *type;
	relation rel;
	char   *amax;
	char   *objname;
	char   *name;
{
	char   *alt = NULL;

	switch (rel) {
	case REL_POINTER:
		print_ifopen(indent, "pointer");
		print_ifarg("(char **)");
		f_print(fout, "%s", objname);
		print_ifsizeof(prefix, type);
		break;
	case REL_VECTOR:
		if (streq(type, "string")) {
			alt = "string";
		} else
			if (streq(type, "opaque")) {
				alt = "opaque";
			}
		if (alt) {
			print_ifopen(indent, alt);
			print_ifarg(objname);
		} else {
			print_ifopen(indent, "vector");
			print_ifarg("(char *)");
			f_print(fout, "%s", objname);
		}
		print_ifarg(amax);
		if (!alt) {
			print_ifsizeof(prefix, type);
		}
		break;
	case REL_ARRAY:
		if (streq(type, "string")) {
			alt = "string";
		} else
			if (streq(type, "opaque")) {
				alt = "bytes";
			}
		if (streq(type, "string")) {
			print_ifopen(indent, alt);
			print_ifarg(objname);
		} else {
			if (alt) {
				print_ifopen(indent, alt);
			} else {
				print_ifopen(indent, "array");
			}
			print_ifarg("(char **)");
			if (*objname == '&') {
				f_print(fout, "%s.%s_val, (u_int *)%s.%s_len",
				    objname, name, objname, name);
			} else {
				f_print(fout, "&%s->%s_val, (u_int *)&%s->%s_len",
				    objname, name, objname, name);
			}
		}
		print_ifarg(amax);
		if (!alt) {
			print_ifsizeof(prefix, type);
		}
		break;
	case REL_ALIAS:
		print_ifopen(indent, type);
		print_ifarg(objname);
		break;
	}
	print_ifclose(indent);
}
/* ARGSUSED */
static void
emit_enum(def)
	definition *def;
{
	fprintf(fout, "\n");
	print_ifopen(1, "enum");
	print_ifarg("(enum_t *)objp");
	print_ifclose(1);
}

static void
emit_program(def)
	definition *def;
{
	decl_list *dl;
	version_list *vlist;
	proc_list *plist;

	for (vlist = def->def.pr.versions; vlist != NULL; vlist = vlist->next)
		for (plist = vlist->procs; plist != NULL; plist = plist->next) {
			if (!newstyle || plist->arg_num < 2)
				continue;	/* old style, or single
						 * argument */
			print_prog_header(plist);
			for (dl = plist->args.decls; dl != NULL;
			    dl = dl->next)
				print_stat(1, &dl->decl);
			print_trailer();
		}
}


static void
emit_union(def)
	definition *def;
{
	declaration *dflt;
	case_list *cl;
	declaration *cs;
	char   *object;
	const char vecformat[] = "objp->%s_u.%s";
	const char format[] = "&objp->%s_u.%s";

	f_print(fout, "\n");
	print_stat(1, &def->def.un.enum_decl);
	f_print(fout, "\tswitch (objp->%s) {\n", def->def.un.enum_decl.name);
	for (cl = def->def.un.cases; cl != NULL; cl = cl->next) {
		f_print(fout, "\tcase %s:\n", cl->case_name);
		if (cl->contflag == 1)	/* a continued case statement */
			continue;
		cs = &cl->case_decl;
		if (!streq(cs->type, "void")) {
			object = alloc(strlen(def->def_name) + strlen(format) +
			    strlen(cs->name) + 1);
			if (isvectordef(cs->type, cs->rel)) {
				s_print(object, vecformat, def->def_name,
				    cs->name);
			} else {
				s_print(object, format, def->def_name,
				    cs->name);
			}
			print_ifstat(2, cs->prefix, cs->type, cs->rel,
			    cs->array_max, object, cs->name);
			free(object);
		}
		f_print(fout, "\t\tbreak;\n");
	}
	dflt = def->def.un.default_decl;
	f_print(fout, "\tdefault:\n");
	if (dflt != NULL) {
		if (!streq(dflt->type, "void")) {
			object = alloc(strlen(def->def_name) + strlen(format) +
			    strlen(dflt->name) + 1);
			if (isvectordef(dflt->type, dflt->rel)) {
				s_print(object, vecformat, def->def_name,
				    dflt->name);
			} else {
				s_print(object, format, def->def_name,
				    dflt->name);
			}
			print_ifstat(2, dflt->prefix, dflt->type, dflt->rel,
			    dflt->array_max, object, dflt->name);
			free(object);
		}
		f_print(fout, "\t\tbreak;\n");
	} else {
		f_print(fout, "\t\treturn (FALSE);\n");
	}

	f_print(fout, "\t}\n");
}

static void
emit_struct(def)
	definition *def;
{
	decl_list *dl;
	int     i, j, size, flag;
	decl_list *cur = NULL, *psav;
	bas_type *ptr;
	char   *sizestr, *plus;
	char    ptemp[256];
	int     can_inline;


	if (doinline == 0) {
		fprintf(fout, "\n");
		for (dl = def->def.st.decls; dl != NULL; dl = dl->next)
			print_stat(1, &dl->decl);
		return;
	}
	size = 0;
	can_inline = 0;
	for (dl = def->def.st.decls; dl != NULL; dl = dl->next)
		if ((dl->decl.prefix == NULL) &&
		    ((ptr = find_type(dl->decl.type)) != NULL) &&
		    ((dl->decl.rel == REL_ALIAS) || (dl->decl.rel == REL_VECTOR))) {

			if (dl->decl.rel == REL_ALIAS)
				size += ptr->length;
			else {
				can_inline = 1;
				break;	/* can be inlined */
			};
		} else {
			if (size >= doinline) {
				can_inline = 1;
				break;	/* can be inlined */
			}
			size = 0;
		}
	if (size > doinline)
		can_inline = 1;

	if (can_inline == 0) {	/* can not inline, drop back to old mode */
		fprintf(fout, "\n");
		for (dl = def->def.st.decls; dl != NULL; dl = dl->next)
			print_stat(1, &dl->decl);
		return;
	};

	/* May cause lint to complain. but  ... */
	f_print(fout, "\tint32_t *buf;\n");

	flag = PUT;
	f_print(fout, "\n\tif (xdrs->x_op == XDR_ENCODE) {\n");

	for (j = 0; j < 2; j++) {
		i = 0;
		size = 0;
		sizestr = NULL;
		for (dl = def->def.st.decls; dl != NULL; dl = dl->next) {	/* xxx */

			/* now walk down the list and check for basic types */
			if ((dl->decl.prefix == NULL) && ((ptr = find_type(dl->decl.type)) != NULL) && ((dl->decl.rel == REL_ALIAS) || (dl->decl.rel == REL_VECTOR))) {
				if (i == 0)
					cur = dl;
				i++;

				if (dl->decl.rel == REL_ALIAS)
					size += ptr->length;
				else {
					/* this is required to handle arrays */

					if (sizestr == NULL)
						plus = "";
					else
						plus = " + ";

					if (ptr->length != 1)
						s_print(ptemp, "%s%s * %d", plus, dl->decl.array_max, ptr->length);
					else
						s_print(ptemp, "%s%s", plus, dl->decl.array_max);

					/* now concatenate to sizestr !!!! */
					if (sizestr == NULL)
						sizestr = strdup(ptemp);
					else {
						sizestr = (char *) realloc(sizestr, strlen(sizestr) + strlen(ptemp) + 1);
						if (sizestr == NULL) {

							f_print(stderr, "Fatal error : no memory\n");
							crash();
						};
						sizestr = strcat(sizestr, ptemp);	/* build up length of
											 * array */

					}
				}

			} else {
				if (i > 0)  {
					if (sizestr == NULL && size < doinline) {
						/* don't expand into inline
						 * code if size < doinline */
						while (cur != dl) {
							print_stat(2, &cur->decl);
							cur = cur->next;
						}
					} else {



						/* were already looking at a
						 * xdr_inlineable structure */
						if (sizestr == NULL)
							f_print(fout, "\t\tbuf = (int32_t *)XDR_INLINE(xdrs, %d * BYTES_PER_XDR_UNIT);\n",
							    size);
						else
							if (size == 0)
								f_print(fout,
								    "\t\tbuf = (int32_t *)XDR_INLINE(xdrs, %s * BYTES_PER_XDR_UNIT);\n",
								    sizestr);
							else
								f_print(fout,
								    "\t\tbuf = (int32_t *)XDR_INLINE(xdrs, (%d + %s) * BYTES_PER_XDR_UNIT);\n",
								    size, sizestr);

						f_print(fout, "\t\tif (buf == NULL) {\n");

						psav = cur;
						while (cur != dl) {
							print_stat(3, &cur->decl);
							cur = cur->next;
						}

						f_print(fout, "\t\t} else {\n");

						cur = psav;
						while (cur != dl) {
							emit_inline(&cur->decl, flag);
							cur = cur->next;
						}

						f_print(fout, "\t\t}\n");
					}
				}
				size = 0;
				i = 0;
				sizestr = NULL;
				print_stat(2, &dl->decl);
			}

		}
		if (i > 0) {
			if (sizestr == NULL && size < doinline) {
				/* don't expand into inline code if size <
				 * doinline */
				while (cur != dl) {
					print_stat(2, &cur->decl);
					cur = cur->next;
				}
			} else {

				/* were already looking at a xdr_inlineable
				 * structure */
				if (sizestr == NULL)
					f_print(fout, "\t\tbuf = (int32_t *)XDR_INLINE(xdrs, %d * BYTES_PER_XDR_UNIT);\n",
					    size);
				else
					if (size == 0)
						f_print(fout,
						    "\t\tbuf = (int32_t *)XDR_INLINE(xdrs, %s * BYTES_PER_XDR_UNIT);\n",
						    sizestr);
					else
						f_print(fout,
						    "\t\tbuf = (int32_t *)XDR_INLINE(xdrs, (%d + %s) * BYTES_PER_XDR_UNIT);\n",
						    size, sizestr);

				f_print(fout, "\t\tif (buf == NULL) {\n");

				psav = cur;
				while (cur != NULL) {
					print_stat(3, &cur->decl);
					cur = cur->next;
				}
				f_print(fout, "\t\t} else {\n");

				cur = psav;
				while (cur != dl) {
					emit_inline(&cur->decl, flag);
					cur = cur->next;
				}

				f_print(fout, "\t\t}\n");

			}
		}
		if (flag == PUT) {
			flag = GET;
			f_print(fout, "\t} else if (xdrs->x_op == XDR_DECODE) {\n");
		}
	}

	f_print(fout, "\t} else {\n");

	/* now take care of XDR_FREE case */

	for (dl = def->def.st.decls; dl != NULL; dl = dl->next)
		print_stat(2, &dl->decl);

	f_print(fout, "\t}\n");
}

static void
emit_typedef(def)
	definition *def;
{
	char   *prefix = def->def.ty.old_prefix;
	char   *type = def->def.ty.old_type;
	char   *amax = def->def.ty.array_max;
	relation rel = def->def.ty.rel;

	fprintf(fout, "\n");
	print_ifstat(1, prefix, type, rel, amax, "objp", def->def_name);
}

static void
print_stat(indent, dec)
	declaration *dec;
	int     indent;
{
	char   *prefix = dec->prefix;
	char   *type = dec->type;
	char   *amax = dec->array_max;
	relation rel = dec->rel;
	char    name[256];

	if (isvectordef(type, rel)) {
		s_print(name, "objp->%s", dec->name);
	} else {
		s_print(name, "&objp->%s", dec->name);
	}
	print_ifstat(indent, prefix, type, rel, amax, name, dec->name);
}


void
emit_inline(decl, flag)
	declaration *decl;
	int     flag;
{

/*check whether an array or not */

	switch (decl->rel) {
	case REL_ALIAS:
		emit_single_in_line(decl, flag, REL_ALIAS);
		break;
	case REL_VECTOR:
		f_print(fout, "\t\t\t{\n");
		f_print(fout, "\t\t\t\tint i;\n");
		f_print(fout, "\t\t\t\t%s *genp;\n", decl->type);
		f_print(fout, "\n");
		f_print(fout, "\t\t\t\tfor (i = 0, genp = objp->%s;\n",
		    decl->name);
		f_print(fout, "\t\t\t\t    i < %s; i++) {\n\t\t",
		    decl->array_max);
		emit_single_in_line(decl, flag, REL_VECTOR);
		f_print(fout, "\t\t\t\t}\n\t\t\t}\n");
		break;
	case REL_ARRAY:
	case REL_POINTER:
		errx(1, "Internal error %s, %d: Case %d not handled\n",
		    __FILE__, __LINE__, decl->rel);
	}
}

void
emit_single_in_line(decl, flag, rel)
	declaration *decl;
	int     flag;
	relation rel;
{
	char   *upp_case;
	int     freed = 0;

	if (flag == PUT)
		f_print(fout, "\t\t\tIXDR_PUT_");
	else
		if (rel == REL_ALIAS)
			f_print(fout, "\t\t\tobjp->%s = IXDR_GET_", decl->name);
		else
			f_print(fout, "\t\t\t*genp++ = IXDR_GET_");

	upp_case = upcase(decl->type);

	/* hack  - XX */
	if (strcmp(upp_case, "INT") == 0) {
		free(upp_case);
		freed = 1;
		upp_case = "LONG";
	}
	if (strcmp(upp_case, "U_INT") == 0) {
		free(upp_case);
		freed = 1;
		upp_case = "U_LONG";
	}
	if (flag == PUT) {
		if (rel == REL_ALIAS)
			f_print(fout, "%s(buf, objp->%s);\n", upp_case, decl->name);
		else
			f_print(fout, "%s(buf, *genp++);\n", upp_case);

	} else
		f_print(fout, "%s(buf);\n", upp_case);
	if (!freed)
		free(upp_case);

}


char   *
upcase(str)
	char   *str;
{
	char   *ptr, *hptr;


	ptr = (char *) malloc(strlen(str) + 1);
	if (ptr == (char *) NULL) {
		f_print(stderr, "malloc failed\n");
		exit(1);
	};

	hptr = ptr;
	while (*str != '\0')
		*ptr++ = toupper(*str++);

	*ptr = '\0';
	return (hptr);

}
