/*
 *  oFono - Open Source Telephony
 *
 *  Copyright (C) 2017-2021 Jolla Ltd.
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

#ifndef TEST_WATCH_H
#define TEST_WATCH_H

#include <ofono/watch.h>

enum test_watch_signal {
	TEST_WATCH_SIGNAL_MODEM_CHANGED,
	TEST_WATCH_SIGNAL_ONLINE_CHANGED,
	TEST_WATCH_SIGNAL_SIM_CHANGED,
	TEST_WATCH_SIGNAL_SIM_STATE_CHANGED,
	TEST_WATCH_SIGNAL_ICCID_CHANGED,
	TEST_WATCH_SIGNAL_IMSI_CHANGED,
	TEST_WATCH_SIGNAL_SPN_CHANGED,
	TEST_WATCH_SIGNAL_NETREG_CHANGED,
	TEST_WATCH_SIGNAL_COUNT
};

void test_watch_signal_queue(struct ofono_watch *w, enum test_watch_signal id);
void test_watch_emit_queued_signals(struct ofono_watch *w);
void test_watch_set_ofono_sim(struct ofono_watch *w, struct ofono_sim *sim);
void test_watch_set_ofono_iccid(struct ofono_watch *w, const char *iccid);
void test_watch_set_ofono_imsi(struct ofono_watch *w, const char *imsi);
void test_watch_set_ofono_spn(struct ofono_watch *w, const char *spn);
void test_watch_set_ofono_netreg(struct ofono_watch *w,
					struct ofono_netreg *netreg);

#endif /* TEST_WATCH_H */

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 8
 * indent-tabs-mode: t
 * End:
 */
