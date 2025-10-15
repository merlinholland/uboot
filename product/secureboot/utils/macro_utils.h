// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __MACRO_UTILS_H__
#define __MACRO_UTILS_H__

/**
 * judge param passing to function.
 */
#define var_is_null_return(p, ret)                         \
	do {                                                    \
		if ((p) == NULL) {                               \
			printf("%s-%d: %s param is null\n", __FUNCTION__, __LINE__, #p);  \
			return (ret);             \
		}                                                   \
	} while (0)

#ifndef check_goto
#define check_goto(val, label, ...) \
	do { \
		if ((val)) { \
			__VA_ARGS__; \
			goto label; \
		} \
	} while (0)
#endif

#ifndef check_return
#define check_return(val, ret, ...) \
	do { \
		if ((val)) { \
			__VA_ARGS__; \
			return (ret); \
		} \
	} while (0)
#endif

#endif
