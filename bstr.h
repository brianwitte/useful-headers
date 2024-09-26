/* Copyright 2002-2010 Paul Hsieh
 *
 * This file is derived from bstrlib AKA bstring:
 * https://github.com/msteinert/bstring/tree/eb3b7aed6a9dfdd4c3e2a36af9b395d773b3fec4
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *    3. Neither the name of bstrlib nor the names of its contributors may be
 *       used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * GNU General Public License Version 2 (the "GPL").
 */

#ifndef BSTR_H
#define BSTR_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stddef.h>
#include <stdarg.h>

/* Constants for return values */
#define BSTR_OK 0
#define BSTR_ERR -1

/* Structure representing a bstr */
typedef struct tagbstr {
	int		mlen;   /* Maximum length (allocated buffer size) */
	int		slen;   /* Current length of the string */
	unsigned char * data;   /* Pointer to the character data */
} *bstr;

typedef struct bstr_list {
	int	qty;
	int	mlen;
	bstr *	entry;
} bstr_list;

struct gen_bstr_list {
	struct bstr_list *	bl;     // Pointer to the list of bstrings
	bstr			b;      // The original bstring being split
};


#define bdataofse(b, o, e) \
	(((b) == (void *)0 || (b)->data == (void *)0) \
		? (char *)(e) \
		: ((char *)(b)->data) + (o))

/**
 * Returns the char * data portion of the bstring b offset by ofs.
 *
 * If b is NULL, NULL is returned.
 */
#define bdataofs(b, o) \
	(bdataofse((b), (o), (void *)0))

/**
 * Returns the char * data portion of the bstring b.
 *
 * If b is NULL, err is returned.
 */
#define bdatae(b, e) \
	(bdataofse(b, 0, e))

/**
 * Returns the char * data portion of the bstring b.
 *
 * If b is NULL, NULL is returned.
 */
#define bdata(b) \
	(bdataofs(b, 0))

static inline const char *bstr_data(const bstr s)
{
	return bdata(s);
}


/* Function prototypes */
static inline int snap_up_size(int i);
static inline void block_copy(void *dst, const void *src, int len);
static inline bstr bstr_copy(const bstr b);
static inline bstr blk_to_bstr(const void *blk, int len);
static inline bstr bstr_from_cstr(const char *str);
static inline int bstr_destroy(bstr b);
static inline int bstr_alloc(bstr b, int olen);
static inline int bstr_assign(bstr a, const bstr b);
static inline void bstr_append(bstr dest, const char *src);
static inline int bstr_append_char(bstr str, unsigned char c);
static inline int bstr_concat(bstr b0, const bstr b1);
static inline int bstr_insert(bstr b1, int pos, const bstr b2, unsigned char fill);
static inline bstr bstr_mid(const bstr b, int left, int len);
static inline int bstr_rchr(const bstr b, int c, int pos);
static inline int bstr_cmp(const bstr b0, const bstr b1);
static inline int bstr_find(const bstr b1, int pos, const bstr b2);
static inline int bstr_trunc(bstr b, int n);
static inline int bstr_icmp(const bstr b0, const bstr b1);
static inline int bstr_tolower(bstr b);
static inline int bstr_assign_mid(bstr dest, const bstr src, int start, int len);
static inline int bstr_format(bstr b, const char *fmt, ...);
static inline int bstr_vformat(bstr b, const char *fmt, va_list args);
static inline int bstr_ncmp(const bstr b0, const bstr b1, int n);
static inline int bstr_spn(const bstr b, const bstr accept);
static inline int bstr_cspn(const bstr b, const bstr reject);
static inline int bstr_list_alloc_min(struct bstr_list *list, int msz);
static inline int bstr_list_alloc(struct bstr_list *list, int msz);
static inline int bstr_list_callback(void *parm, int ofs, int len);
static inline struct bstr_list *bstr_list_create(void);
static inline int bstr_list_destroy(struct bstr_list *list);
static inline int bstr_split_cb(const bstr str, unsigned char split_char, int pos, int (*callback)(void *parm, int ofs, int len), void *parm);
static inline struct bstr_list *bstr_split(const bstr str, unsigned char split_char);
static inline int bstr_splits_cb(const bstr str, const bstr split_str, int pos, int (*callback)(void *parm, int ofs, int len), void *parm);
static inline struct bstr_list *bstr_splits(const bstr str, const bstr split_str);
static inline int bstr_split_str_cb(const bstr str, const bstr split_str, int pos, int (*callback)(void *parm, int ofs, int len), void *parm);
static inline struct bstr_list *bstr_split_str(const bstr str, const bstr split_str);
static inline bstr bstr_join(const struct bstr_list *list, const bstr sep);

/* Helper macros */
#define downcase(c) (tolower((unsigned char)(c)))
#define bstr_chr(b, c) bstr_rchr((b), (c), 0)
#define bstr_len(b) ((b) ? (b)->slen : -1)

/* Compute the snapped size for a given requested size. */
static inline int snap_up_size(int i)
{
	if (i < 8) {
		i = 8;
	} else {
		unsigned int j = (unsigned int)i;
		j |= (j >> 1);
		j |= (j >> 2);
		j |= (j >> 4);
		j |= (j >> 8);
#if (UINT_MAX != 0xffff)
		j |= (j >> 16);
#if (UINT_MAX > 0xffffffffUL)
		j |= (j >> 32);
#endif
#endif
		j++;
		if ((int)j >= i)
			i = (int)j;
	}
	return i;
}

/* Just a length-safe wrapper for memmove. */
static inline void block_copy(void *dst, const void *src, int len)
{
	if (len > 0)
		memmove(dst, src, len);
}

static inline bstr bstr_copy(const bstr b)
{
	if (!b || b->slen < 0 || !b->data) return NULL;

	bstr b0 = malloc(sizeof(struct tagbstr));
	if (!b0) return NULL;

	int i = b->slen;
	int j = snap_up_size(i + 1);
	b0->data = malloc(j);
	if (!b0->data) {
		j = i + 1;
		b0->data = malloc(j);
		if (!b0->data) {
			free(b0);
			return NULL;
		}
	}

	b0->mlen = j;
	b0->slen = i;
	if (i) memcpy(b0->data, b->data, i);
	b0->data[b0->slen] = '\0';
	return b0;
}

static inline bstr blk_to_bstr(const void *blk, int len)
{
	if (!blk || len < 0) return NULL;

	bstr b = malloc(sizeof(struct tagbstr));
	if (!b) return NULL;

	b->slen = len;
	int i = len + (2 - (len != 0));
	i = snap_up_size(i);
	b->mlen = i;
	b->data = malloc(b->mlen);
	if (!b->data) {
		free(b);
		return NULL;
	}
	if (len > 0) memcpy(b->data, blk, len);
	b->data[len] = '\0';
	return b;
}

static inline bstr bstr_from_cstr(const char *str)
{
	if (!str) return NULL;

	size_t j = strlen(str);
	int i = snap_up_size((int)(j + (2 - (j != 0))));
	if (i <= (int)j) return NULL;

	bstr b = malloc(sizeof(struct tagbstr));
	if (!b) return NULL;

	b->slen = (int)j;
	b->mlen = i;
	b->data = malloc(b->mlen);
	if (!b->data) {
		free(b);
		return NULL;
	}
	memcpy(b->data, str, j + 1);
	return b;
}

static inline int bstr_destroy(bstr b)
{
	if (!b || b->slen < 0 || b->mlen <= 0 || b->mlen < b->slen || !b->data)
		return BSTR_ERR;
	free(b->data);
	b->slen = -1;
	b->mlen = -__LINE__;
	b->data = NULL;
	free(b);
	return BSTR_OK;
}

static inline int bstr_alloc(bstr b, int olen)
{
	if (!b || !b->data || b->slen < 0 || b->mlen <= 0 || b->mlen < b->slen || olen <= 0)
		return BSTR_ERR;

	if (olen < b->mlen) return BSTR_OK;

	int len = snap_up_size(olen);
	if (len <= b->mlen) return BSTR_OK;

	unsigned char *x;

	if (7 * b->mlen < 8 * b->slen) {
retry:
		x = realloc(b->data, len);
		if (!x) {
			len = olen;
			x = realloc(b->data, len);
			if (!x) return BSTR_ERR;
		}
	} else {
		x = malloc(len);
		if (!x) goto retry;

		if (b->slen) memcpy(x, b->data, b->slen);
		free(b->data);
	}

	b->data = x;
	b->mlen = len;
	b->data[b->slen] = '\0';

	return BSTR_OK;
}

static inline int bstr_assign(bstr a, const bstr b)
{
	if (!b || !b->data || b->slen < 0) return BSTR_ERR;
	if (b->slen != 0) {
		if (bstr_alloc(a, b->slen) != BSTR_OK) return BSTR_ERR;
		memmove(a->data, b->data, b->slen);
	} else {
		if (!a || !a->data || a->mlen < a->slen || a->slen < 0 || a->mlen == 0) return BSTR_ERR;
	}
	a->data[b->slen] = '\0';
	a->slen = b->slen;
	return BSTR_OK;
}


static int bstr_catblk(bstr b, const void *s, int len)
{
	int nl;

	if (!b || !b->data ||
	    b->slen < 0 || b->mlen < b->slen ||
	    b->mlen <= 0 || !s || len < 0)
		return BSTR_ERR;
	if (0 > (nl = b->slen + len))
		/* Overflow? */
		return BSTR_ERR;
	if (b->mlen <= nl && 0 > bstr_alloc(b, nl + 1))
		return BSTR_ERR;
	block_copy(&b->data[b->slen], s, len);
	b->slen = nl;
	b->data[nl] = (unsigned char)'\0';
	return BSTR_OK;
}

static inline int bstr_catcstr(bstr b, const char *s)
{
	char *d;
	int i, l;

	if (b == NULL || b->data == NULL ||
	    b->slen < 0 || b->mlen < b->slen
	    || b->mlen <= 0 || s == NULL)
		return BSTR_ERR;
	/* Optimistically concatenate directly */
	l = b->mlen - b->slen;
	d = (char *)&b->data[b->slen];
	for (i = 0; i < l; ++i) {
		if ((*d++ = *s++) == '\0') {
			b->slen += i;
			return BSTR_OK;
		}
	}
	b->slen += i;
	/* Need to explicitely resize and concatenate tail */
	return bstr_catblk(b, s, strlen(s));
}



static int bstr_conchar(bstr b, char c)
{
	int d;

	if (!b)
		return BSTR_ERR;
	d = b->slen;
	if ((d | (b->mlen - d)) < 0 || bstr_alloc(b, d + 2) != BSTR_OK)
		return BSTR_ERR;
	b->data[d] = (unsigned char)c;
	b->data[d + 1] = (unsigned char)'\0';
	b->slen++;
	return BSTR_OK;
}

static inline void bstr_append(bstr s, const char *suffix)
{
	bstr_catcstr(s, suffix);
}

static inline int bstr_append_char(bstr s, unsigned char c)
{
	return bstr_conchar(s, c);
}

static inline int bstr_concat(bstr b0, const bstr b1)
{
	if (!b0 || !b1 || !b0->data || !b1->data) return BSTR_ERR;

	int d = b0->slen;
	int len = b1->slen;
	if ((d | (b0->mlen - d) | len | (d + len)) < 0) return BSTR_ERR;

	if (b0->mlen <= d + len + 1) {
		ptrdiff_t pd = b1->data - b0->data;
		bstr aux = b1;
		if (0 <= pd && pd < b0->mlen) {
			aux = bstr_copy(b1);
			if (!aux) return BSTR_ERR;
		}
		if (bstr_alloc(b0, d + len + 1) != BSTR_OK) {
			if (aux != b1) bstr_destroy(aux);
			return BSTR_ERR;
		}
	}
	block_copy(&b0->data[d], &b1->data[0], len);
	b0->data[d + len] = '\0';
	b0->slen = d + len;
	return BSTR_OK;
}

// Continue to apply `static inline` to the rest of your function definitions...





static inline int bstr_insert(bstr b1, int pos, const bstr b2, unsigned char fill)
{
	if (pos < 0 || !b1 || !b2 || b1->slen < 0 || b2->slen < 0 || b1->mlen < b1->slen || b1->mlen <= 0) return BSTR_ERR;

	ptrdiff_t pd = b2->data - b1->data;
	bstr aux = b2;
	if (pd >= 0 && pd < b1->mlen) {
		aux = bstr_copy(b2);
		if (!aux) return BSTR_ERR;
	}

	int d = b1->slen + aux->slen;
	int l = pos + aux->slen;
	if ((d | l) < 0) {
		if (aux != b2) bstr_destroy(aux);
		return BSTR_ERR;
	}

	if (l > d) {
		if (bstr_alloc(b1, l + 1) != BSTR_OK) {
			if (aux != b2) bstr_destroy(aux);
			return BSTR_ERR;
		}
		memset(b1->data + b1->slen, (int)fill, (size_t)(pos - b1->slen));
		b1->slen = l;
	} else {
		if (bstr_alloc(b1, d + 1) != BSTR_OK) {
			if (aux != b2) bstr_destroy(aux);
			return BSTR_ERR;
		}
		block_copy(b1->data + l, b1->data + pos, d - l);
		b1->slen = d;
	}

	block_copy(b1->data + pos, aux->data, aux->slen);
	b1->data[b1->slen] = '\0';
	if (aux != b2) bstr_destroy(aux);
	return BSTR_OK;
}

static inline bstr bstr_mid(const bstr b, int left, int len)
{
	if (!b || b->slen < 0 || !b->data) return NULL;

	if (left < 0) {
		len += left;
		left = 0;
	}
	if (len > b->slen - left) len = b->slen - left;
	if (len <= 0) return bstr_from_cstr("");

	return blk_to_bstr(b->data + left, len);
}

static inline int bstr_rchr(const bstr b, int c, int pos)
{
	if (!b || !b->data || b->slen <= pos || pos < 0) return BSTR_ERR;

	for (int i = pos; i >= 0; i--)
		if (b->data[i] == (unsigned char)c) return i;
	return BSTR_ERR;
}

static inline int bstr_cmp(const bstr b0, const bstr b1)
{
	if (!b0 || !b1 || !b0->data || !b1->data || b0->slen < 0 || b1->slen < 0) return SHRT_MIN;

	int n = (b0->slen < b1->slen) ? b0->slen : b1->slen;
	for (int i = 0; i < n; i++) {
		int v = ((char)b0->data[i]) - ((char)b1->data[i]);
		if (v != 0) return v;
		if (b0->data[i] == '\0') return BSTR_OK;
	}
	return (b0->slen > b1->slen) - (b1->slen > b0->slen);
}

static inline int bstr_find(const bstr b1, int pos, const bstr b2)
{
	if (!b1 || !b1->data || b1->slen < 0 || !b2 || !b2->data || b2->slen < 0) return BSTR_ERR;

	int i = pos;
	int lf = b1->slen - b2->slen + 1;
	if (lf <= pos) return BSTR_ERR;

	unsigned char *d0 = b2->data;
	unsigned char *d1 = b1->data;
	unsigned char c0 = d0[0];
	int ll = b2->slen;

	for (; i < lf; i++)
		if (c0 == d1[i] && memcmp(d1 + i, d0, ll) == 0) return i;
	return BSTR_ERR;
}

static inline int bstr_trunc(bstr b, int n)
{
	if (n < 0 || !b || !b->data || b->mlen < b->slen || b->slen < 0 || b->mlen <= 0) return BSTR_ERR;

	if (b->slen > n) {
		b->slen = n;
		b->data[n] = '\0';
	}
	return BSTR_OK;
}

static inline int bstr_icmp(const bstr b0, const bstr b1)
{
	if (!b0 || !b1 || !b0->data || !b1->data || b0->slen < 0 || b1->slen < 0) return SHRT_MIN;

	int n = (b0->slen < b1->slen) ? b0->slen : b1->slen;
	for (int i = 0; i < n; i++) {
		int v = downcase(b0->data[i]) - downcase(b1->data[i]);
		if (v != 0) return v;
	}

	return (b0->slen > b1->slen) - (b1->slen > b0->slen);
}

static inline int bstr_tolower(bstr b)
{
	if (!b || !b->data || b->mlen < b->slen || b->slen < 0 || b->mlen <= 0) return BSTR_ERR;

	for (int i = 0; i < b->slen; i++)
		b->data[i] = (unsigned char)downcase(b->data[i]);
	return BSTR_OK;
}

static inline int bstr_assign_mid(bstr dest, const bstr src, int start, int len)
{
	if (!dest || !src || !src->data || src->slen < 0 || start < 0 || len < 0) return BSTR_ERR;

	if (start >= src->slen) return bstr_trunc(dest, 0);

	int new_len = len;
	if (start + len > src->slen)
		new_len = src->slen - start;

	if (bstr_alloc(dest, new_len) != BSTR_OK) return BSTR_ERR;

	memmove(dest->data, src->data + start, new_len);
	dest->data[new_len] = '\0';
	dest->slen = new_len;

	return BSTR_OK;
}

static inline int bstr_format(bstr b, const char *fmt, ...)
{
	if (!b || !fmt) return BSTR_ERR;

	va_list args;
	va_start(args, fmt);

	int required_len = vsnprintf(NULL, 0, fmt, args) + 1;
	va_end(args);

	if (required_len <= 0) return BSTR_ERR;

	if (bstr_alloc(b, required_len) != BSTR_OK) return BSTR_ERR;

	va_start(args, fmt);
	vsnprintf((char *)b->data, required_len, fmt, args);
	va_end(args);

	b->slen = required_len - 1;
	return BSTR_OK;
}

static inline int bstr_vformat(bstr b, const char *fmt, va_list args)
{
	if (!b || !fmt) return BSTR_ERR;

	va_list args_copy;
	va_copy(args_copy, args);
	int required_len = vsnprintf(NULL, 0, fmt, args_copy) + 1;
	va_end(args_copy);

	if (required_len <= 0) return BSTR_ERR;

	if (bstr_alloc(b, required_len) != BSTR_OK) return BSTR_ERR;

	vsnprintf((char *)b->data, required_len, fmt, args);
	b->slen = required_len - 1;
	return BSTR_OK;
}

static inline int bstr_ncmp(const bstr b0, const bstr b1, int n)
{
	if (!b0 || !b1 || !b0->data || !b1->data || b0->slen < 0 || b1->slen < 0 || n < 0) return SHRT_MIN;

	int len = (b0->slen < b1->slen) ? b0->slen : b1->slen;
	if (n < len) len = n;

	for (int i = 0; i < len; i++) {
		int v = ((unsigned char)b0->data[i]) - ((unsigned char)b1->data[i]);
		if (v != 0) return v;
	}

	return (n > len) ? ((b0->slen > b1->slen) - (b1->slen > b0->slen)) : 0;
}

static inline int bstr_spn(const bstr b, const bstr accept)
{
	if (!b || !accept || !b->data || !accept->data || b->slen < 0 || accept->slen < 0) return BSTR_ERR;

	int i;
	for (i = 0; i < b->slen; i++)
		if (!memchr(accept->data, b->data[i], accept->slen))
			break;
	return i;
}

static inline int bstr_cspn(const bstr b, const bstr reject)
{
	if (!b || !reject || !b->data || !reject->data || b->slen < 0 || reject->slen < 0) return BSTR_ERR;

	int i;
	for (i = 0; i < b->slen; i++)
		if (memchr(reject->data, b->data[i], reject->slen))
			break;
	return i;
}

static inline int bstr_list_alloc_min(struct bstr_list *list, int msz)
{
	if (!list || msz <= 0 || !list->entry || list->qty < 0 ||
	    list->mlen <= 0 || list->qty > list->mlen)
		return BSTR_ERR;

	// The new minimum size should be at least as large as the number of entries
	if (msz < list->qty)
		msz = list->qty;

	if (list->mlen == msz)
		return BSTR_OK;

	// Ensure we don't decrease mlen below what it was previously set
	if (msz < list->mlen)
		return BSTR_OK;

	size_t new_size = ((size_t)msz) * sizeof(bstr);

	if (new_size < (size_t)msz)
		return BSTR_ERR;

	bstr *new_entry = realloc(list->entry, new_size);
	if (!new_entry)
		return BSTR_ERR;

	list->mlen = msz;
	list->entry = new_entry;
	return BSTR_OK;
}

static inline int bstr_list_alloc(struct bstr_list *list, int msz)
{
	if (!list || msz <= 0 || !list->entry || list->qty < 0 ||
	    list->mlen <= 0 || list->qty > list->mlen)
		return BSTR_ERR;

	if (list->mlen >= msz)
		return BSTR_OK;

	int smsz = snap_up_size(msz);
	size_t new_size = ((size_t)smsz) * sizeof(bstr);

	if (new_size < (size_t)smsz)
		return BSTR_ERR;

	bstr *new_entry = realloc(list->entry, new_size);
	if (!new_entry) {
		smsz = msz;
		new_size = ((size_t)smsz) * sizeof(bstr);
		new_entry = realloc(list->entry, new_size);
		if (!new_entry)
			return BSTR_ERR;
	}

	list->mlen = smsz;
	list->entry = new_entry;
	return BSTR_OK;
}

static inline int bstr_list_callback(void *parm, int ofs, int len)
{
	struct gen_bstr_list *g = (struct gen_bstr_list *)parm;
	bstr substring = bstr_mid(g->b, ofs, len);

	if (substring == NULL) return BSTR_ERR;

	if (g->bl->qty >= g->bl->mlen) {
		if (bstr_list_alloc(g->bl, g->bl->mlen * 2) != BSTR_OK) {
			bstr_destroy(substring);
			return BSTR_ERR;
		}
	}

	g->bl->entry[g->bl->qty] = substring;
	g->bl->qty++;
	return BSTR_OK;
}

static inline struct bstr_list *bstr_list_create(void)
{
	struct bstr_list *list = malloc(sizeof(struct bstr_list));

	if (list) {
		list->entry = (bstr *)malloc(1 * sizeof(bstr));
		if (!list->entry) {
			free(list);
			list = NULL;
		} else {
			list->qty = 0;
			list->mlen = 1;
		}
	}
	return list;
}

static inline int bstr_list_destroy(struct bstr_list *list)
{
	if (!list || list->qty < 0)
		return BSTR_ERR;
	for (int i = 0; i < list->qty; i++) {
		if (list->entry[i]) {
			bstr_destroy(list->entry[i]);
			list->entry[i] = NULL;
		}
	}
	list->qty = -1;
	list->mlen = -1;
	free(list->entry);
	list->entry = NULL;
	free(list);
	return BSTR_OK;
}

static inline int bstr_split_cb(const bstr str, unsigned char split_char, int pos,
				int (*callback)(void *parm, int ofs, int len), void *parm)
{
	if (!str || !str->data || str->slen < 0 || !callback)
		return BSTR_ERR;

	int start = pos;
	for (int i = pos; i < str->slen; i++) {
		if (str->data[i] == split_char) {
			if (callback(parm, start, i - start) < 0)
				return BSTR_ERR;
			start = i + 1;
		}
	}

	if (start <= str->slen)
		return callback(parm, start, str->slen - start);

	return BSTR_OK;
}

static inline struct bstr_list *bstr_split(const bstr str, unsigned char split_char)
{
	struct gen_bstr_list g;

	if (!str || !str->data || str->slen < 0)
		return NULL;

	g.bl = malloc(sizeof(struct bstr_list));
	if (!g.bl)
		return NULL;

	g.bl->mlen = 4;
	g.bl->entry = malloc(g.bl->mlen * sizeof(bstr));
	if (!g.bl->entry) {
		free(g.bl);
		return NULL;
	}

	g.b = (bstr)str;
	g.bl->qty = 0;

	if (bstr_split_cb(str, split_char, 0, bstr_list_callback, &g) < 0) {
		bstr_list_destroy(g.bl);
		return NULL;
	}

	return g.bl;
}

static inline int bstr_splits_cb(const bstr str, const bstr split_str, int pos,
				 int (*callback)(void *parm, int ofs, int len), void *parm)
{
	if (!str || !str->data || str->slen < 0 ||
	    !split_str || !split_str->data || split_str->slen < 0 || !callback)
		return BSTR_ERR;

	int start = pos;
	for (int i = pos; i < str->slen; i++) {
		for (int j = 0; j < split_str->slen; j++) {
			if (str->data[i] == split_str->data[j]) {
				if (callback(parm, start, i - start) < 0)
					return BSTR_ERR;
				start = i + 1;
				break;
			}
		}
	}

	if (start <= str->slen)
		return callback(parm, start, str->slen - start);

	return BSTR_OK;
}

static inline struct bstr_list *bstr_splits(const bstr str, const bstr split_str)
{
	struct gen_bstr_list g;

	if (!str || str->slen < 0 || !str->data ||
	    !split_str || split_str->slen < 0 || !split_str->data)
		return NULL;

	g.bl = malloc(sizeof(struct bstr_list));
	if (!g.bl)
		return NULL;

	g.bl->mlen = 4;
	g.bl->entry = malloc(g.bl->mlen * sizeof(bstr));
	if (!g.bl->entry) {
		free(g.bl);
		return NULL;
	}

	g.b = (bstr)str;
	g.bl->qty = 0;

	if (bstr_splits_cb(str, split_str, 0, bstr_list_callback, &g) < 0) {
		bstr_list_destroy(g.bl);
		return NULL;
	}

	return g.bl;
}

static inline int bstr_split_str_cb(const bstr str, const bstr split_str, int pos,
				    int (*callback)(void *parm, int ofs, int len), void *parm)
{
	if (!str || !str->data || str->slen < 0 ||
	    !split_str || !split_str->data || split_str->slen < 0 || !callback)
		return BSTR_ERR;

	int start = pos;
	while (start <= str->slen - split_str->slen) {
		int found = 1;
		for (int j = 0; j < split_str->slen; j++) {
			if (str->data[start + j] != split_str->data[j]) {
				found = 0;
				break;
			}
		}

		if (found) {
			if (callback(parm, pos, start - pos) < 0)
				return BSTR_ERR;
			pos = start + split_str->slen;
			start = pos;
		} else {
			start++;
		}
	}

	if (pos <= str->slen)
		return callback(parm, pos, str->slen - pos);

	return BSTR_OK;
}

static inline struct bstr_list *bstr_split_str(const bstr str, const bstr split_str)
{
	struct gen_bstr_list g;

	if (!str || !str->data || str->slen < 0)
		return NULL;

	g.bl = malloc(sizeof(struct bstr_list));
	if (!g.bl)
		return NULL;

	g.bl->mlen = 4;
	g.bl->entry = malloc(g.bl->mlen * sizeof(bstr));
	if (!g.bl->entry) {
		free(g.bl);
		return NULL;
	}

	g.b = (bstr)str;
	g.bl->qty = 0;

	if (bstr_split_str_cb(str, split_str, 0, bstr_list_callback, &g) < 0) {
		bstr_list_destroy(g.bl);
		return NULL;
	}

	return g.bl;
}

static inline bstr bstr_join(const struct bstr_list *list, const bstr sep)
{
	if (list == NULL || list->qty < 0)
		return NULL;
	if (sep != NULL && (sep->slen < 0 || sep->data == NULL))
		return NULL;

	int total_length = 1; // For null terminator
	for (int i = 0; i < list->qty; i++) {
		if (list->entry[i]->slen < 0)
			return NULL; // Invalid input
		total_length += list->entry[i]->slen;
		if (total_length < 0)
			return NULL; // Overflow
	}

	if (sep != NULL)
		total_length += (list->qty - 1) * sep->slen;

	bstr result = malloc(sizeof(struct tagbstr));
	if (!result)
		return NULL; // Out of memory

	result->data = malloc(total_length);
	if (!result->data) {
		free(result);
		return NULL;
	}

	result->mlen = total_length;
	result->slen = total_length - 1;

	int current_pos = 0;
	for (int i = 0; i < list->qty; i++) {
		if (i > 0 && sep != NULL) {
			memcpy(result->data + current_pos, sep->data, sep->slen);
			current_pos += sep->slen;
		}
		memcpy(result->data + current_pos, list->entry[i]->data, list->entry[i]->slen);
		current_pos += list->entry[i]->slen;
	}

	result->data[current_pos] = '\0';
	return result;
}

#endif /* BSTR_H */
