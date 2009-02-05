/*
 * Copyright (c) 2005-2008 Nominet UK (www.nic.uk)
 * All rights reserved.
 * Contributors: Ben Laurie, Rachel Willmer. The Contributors have asserted
 * their moral rights under the UK Copyright Design and Patents Act 1988 to
 * be recorded as the authors of this copyright work.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** \file
 */

#ifndef OPS_UTIL_H
#define OPS_UTIL_H

#include "openpgpsdk/types.h"
#include "openpgpsdk/create.h"
#include "openpgpsdk/packet-parse.h"

#include <stdlib.h>

void            hexdump(const unsigned char *, size_t, const char *);

/*
 * These macros code ensures that you are casting what you intend to cast.
 * It works because in "a ? b : c", b and c must have the same type.
 * This is a copy of the macro defined in openssl/asn1.h.
 */
#ifndef CHECKED_PTR_OF
#define CHECKED_PTR_OF(type, p) ((void*) (1 ? p : (type *)0))
#endif
#define CHECKED_INSTANCE_OF(type, p) (1 ? p : (type)0)

/* number of elements in an array */
#define OPS_ARRAY_SIZE(a)	(sizeof(a)/sizeof(*(a)))

const char     *ops_str_from_map(int, ops_map_t *);

int             ops_set_debug_level(const char *);
int             ops_get_debug_level(const char *);

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION	"20090201"
#endif

const char     *ops_get_version(void);

#ifndef __UNCONST
#define __UNCONST(a)   ((void *)(unsigned long)(const void *)(a))
#endif

#endif
