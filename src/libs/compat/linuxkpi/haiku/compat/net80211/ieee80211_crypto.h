/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_NET80211_IEEE80211_CRYPTO_H_
#define _LKPI_HAIKU_NET80211_IEEE80211_CRYPTO_H_

/*
 * Extends freebsd_wlan's <net80211/ieee80211_crypto.h> with the key
 * accessors newer FreeBSD net80211 grew and LinuxKPI's linux_80211.c
 * uses; they read the same fields the older struct always had.
 */

#include_next <net80211/ieee80211_crypto.h>


static __inline int
ieee80211_crypto_get_key_len(const struct ieee80211_key* k)
{
	return k->wk_keylen;
}


static __inline const uint8_t*
ieee80211_crypto_get_key_data(const struct ieee80211_key* k)
{
	return k->wk_key;
}


static __inline int
ieee80211_crypto_get_key_txmic_len(const struct ieee80211_key* k)
{
	return k->wk_cipher->ic_miclen;
}


static __inline int
ieee80211_crypto_get_key_rxmic_len(const struct ieee80211_key* k)
{
	return k->wk_cipher->ic_miclen;
}


static __inline const uint8_t*
ieee80211_crypto_get_key_txmic_data(const struct ieee80211_key* k)
{
	return k->wk_txmic;
}


static __inline const uint8_t*
ieee80211_crypto_get_key_rxmic_data(const struct ieee80211_key* k)
{
	return k->wk_rxmic;
}


#endif /* _LKPI_HAIKU_NET80211_IEEE80211_CRYPTO_H_ */
