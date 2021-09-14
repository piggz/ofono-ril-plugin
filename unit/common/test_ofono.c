/*
 *  oFono - Open Source Telephony
 *
 *  Copyright (C) 2021 Jolla Ltd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 */

#include <ofono/conf.h>
#include <ofono/misc.h>
#include <ofono/log.h>

#include <gutil_log.h>

char *ofono_conf_get_string(GKeyFile *file, const char *group, const char *key)
{
	char *val = g_key_file_get_string(file, group, key, NULL);

	if (!val && strcmp(group, OFONO_COMMON_SETTINGS_GROUP)) {
		/* Check the common section */
		val = g_key_file_get_string(file, OFONO_COMMON_SETTINGS_GROUP,
			key, NULL);
	}
	return val;
}

void ofono_dbg(const struct ofono_debug_desc *desc, const char *format, ...)
{
	va_list va;

	va_start(va, format);
	gutil_logv(NULL, GLOG_LEVEL_DEBUG, format, va);
	va_end(va);
}

void ofono_warn(const char *format, ...)
{
	va_list va;

	va_start(va, format);
	gutil_logv(NULL, GLOG_LEVEL_WARN, format, va);
	va_end(va);
}

void ofono_error(const char *format, ...)
{
	va_list va;

	va_start(va, format);
	gutil_logv(NULL, GLOG_LEVEL_ERR, format, va);
	va_end(va);
}

void ofono_encode_hex(const void *in, unsigned int size, char out[])
{
	static const char hex[] = "0123456789ABCDEF";
	const guchar* bytes = in;
	char* dest = out;
	guint i = 0;

	while (i < size) {
		const guchar b = bytes[i++];

		*dest++ = hex[(b >> 4) & 0xf];
		*dest++ = hex[b & 0xf];
	}
	*dest = 0;
}

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 8
 * indent-tabs-mode: t
 * End:
 */
