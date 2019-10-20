/**
 * PList
 *
 * plist.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "plist.h"
#include <string.h>

ssize_t __plist_find(const void *list, const void *item, size_t item_size, int (*cmp)(const void *, const void *)) {
	const plist_t *plist = __PLIST_L2P(list);
	if (plist == NULL) {
		return -1;
	}

	for (size_t i = 0; i < plist->length; i++) {
		void *list_item = plist->data + i * item_size;
		if (cmp == NULL) {
			if (memcmp(list_item, item, item_size) == 0) {
				return i;
			}
		} else if (cmp(list_item, item) == 0) {
			return i;
		}
	}

	return -1;
}
