/* GNU gettext - internationalization aids
   Copyright (C) 1995, 1996, 1997, 1998 Free Software Foundation, Inc.

   This file was written by Peter Miller <millerp@canb.auug.org.au>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free SoftwareFoundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "po-lex.h"
#include "str-list.h"

/* According to Sun's Uniforum proposal the default message domain is
   named `messages'.  */
#define MESSAGE_DOMAIN_DEFAULT "messages"


/* Is current msgid a format string?  */
enum is_c_format
{
  undecided,
  yes,
  no,
  possible,
  impossible
};

typedef struct message_variant_ty message_variant_ty;
struct message_variant_ty
{
  const char *domain;
  lex_pos_ty pos;
  const char *msgstr;
};

typedef struct message_ty message_ty;
struct message_ty
{
  /* Plain comments (#) appearing before the message.  */
  string_list_ty *comment;

  /* Extracted comments (#.) appearing before the message.  */
  string_list_ty *comment_dot;

  /* File position comments (#:) appearing before the message, one for
     each unique file position instance, sorted by file name and then
     by line.  */
  size_t filepos_count;
  lex_pos_ty *filepos;

  /* Informations from special comments (e.g. generated by msgmerge).  */
  int is_fuzzy;
  enum is_c_format is_c_format;

  /* Do we want the string to be wrapped in the emitted PO file?  */
  enum is_c_format do_wrap;

  /* The msgid string.  */
  const char *msgid;

  /* The msgstr strings, one for each observed domain in the file.  */
  size_t variant_count;
  message_variant_ty *variant;

  /* Used for checking that messages have been used, in the msgcmp and
     msgmerge programs.  */
  int used;

  /* If set the message is obsolete and while writing out it should be
     commented out.  */
  int obsolete;
};

message_ty *message_alloc PARAMS ((char *msgid));
void message_free PARAMS ((message_ty *));

message_variant_ty *message_variant_search PARAMS ((message_ty *mp,
						    const char *domain));
void message_variant_append PARAMS ((message_ty *mp, const char *domain,
				     const char *msgstr,
				     const lex_pos_ty *pp));
void message_comment_append PARAMS ((message_ty *, const char *));
void message_comment_dot_append PARAMS ((message_ty *, const char *));
message_ty *message_copy PARAMS ((message_ty *));
message_ty *message_merge PARAMS ((message_ty *def, message_ty *ref));
void message_comment_filepos PARAMS ((message_ty *, const char *, size_t));
void message_print_style_indent PARAMS ((void));
void message_print_style_uniforum PARAMS ((void));
void message_print_style_escape PARAMS ((int));


typedef struct message_list_ty message_list_ty;
struct message_list_ty
{
  message_ty **item;
  size_t nitems;
  size_t nitems_max;
};

message_list_ty *message_list_alloc PARAMS ((void));
void message_list_free PARAMS ((message_list_ty *));
void message_list_append PARAMS ((message_list_ty *, message_ty *));
void message_list_delete_nth PARAMS ((message_list_ty *, size_t));
message_ty *message_list_search PARAMS ((message_list_ty *, const char *));
message_ty *message_list_search_fuzzy PARAMS ((message_list_ty *,
					       const char *));
void message_list_print PARAMS ((message_list_ty *, const char *, int, int));
void message_list_sort_by_msgid PARAMS ((message_list_ty *));
void message_list_sort_by_filepos PARAMS ((message_list_ty *));

enum is_c_format parse_c_format_description_string PARAMS ((const char *s));
enum is_c_format parse_c_width_description_string PARAMS ((const char *s));
int possible_c_format_p PARAMS ((enum is_c_format));
void message_page_width_set PARAMS ((size_t width));

#endif /* message.h */
