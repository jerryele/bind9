/*
 * Copyright (C) 1999, 2000  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* $Id: a_1.c,v 1.8 2000/04/27 00:02:45 tale Exp $ */

/* reviewed: Thu Mar 16 15:58:36 PST 2000 by brister */

#ifndef RDATA_HS_4_A_1_C
#define RDATA_HS_4_A_1_C

#include <string.h>

#include <isc/net.h>

#define RRTYPE_A_ATTRIBUTES (0)

static inline isc_result_t
fromtext_hs_a(dns_rdataclass_t rdclass, dns_rdatatype_t type,
	      isc_lex_t *lexer, dns_name_t *origin,
	      isc_boolean_t downcase, isc_buffer_t *target)
{
	isc_token_t token;
	struct in_addr addr;
	isc_region_t region;

	REQUIRE(type == 1);
	REQUIRE(rdclass == 4);

	UNUSED(origin);
	UNUSED(downcase);

	RETERR(gettoken(lexer, &token, isc_tokentype_string, ISC_FALSE));

	if (inet_aton(token.value.as_pointer, &addr) != 1)
		return (DNS_R_BADDOTTEDQUAD);
	isc_buffer_availableregion(target, &region);
	if (region.length < 4)
		return (ISC_R_NOSPACE);
	memcpy(region.base, &addr, 4);
	isc_buffer_add(target, 4);
	return (ISC_R_SUCCESS);
}

static inline isc_result_t
totext_hs_a(dns_rdata_t *rdata, dns_rdata_textctx_t *tctx, 
	    isc_buffer_t *target) 
{
	isc_region_t region;

	REQUIRE(rdata->type == 1);
	REQUIRE(rdata->rdclass == 4);
	REQUIRE(rdata->length == 4);

	UNUSED(tctx);

	isc_buffer_availableregion(target, &region);
	if (inet_ntop(AF_INET, rdata->data,
			  (char *)region.base, region.length) == NULL)
		return (ISC_R_NOSPACE);

	isc_buffer_add(target, strlen((char *)region.base));
	return (ISC_R_SUCCESS);
}

static inline isc_result_t
fromwire_hs_a(dns_rdataclass_t rdclass, dns_rdatatype_t type,
	      isc_buffer_t *source, dns_decompress_t *dctx,
	      isc_boolean_t downcase, isc_buffer_t *target)
{
	isc_region_t sregion;
	isc_region_t tregion;

	REQUIRE(type == 1);
	REQUIRE(rdclass == 4);

	UNUSED(dctx);
	UNUSED(downcase);


	isc_buffer_activeregion(source, &sregion);
	isc_buffer_availableregion(target, &tregion);
	if (sregion.length < 4)
		return (ISC_R_UNEXPECTEDEND);
	if (tregion.length < 4)
		return (ISC_R_NOSPACE);

	memcpy(tregion.base, sregion.base, 4);
	isc_buffer_forward(source, 4);
	isc_buffer_add(target, 4);
	return (ISC_R_SUCCESS);
}

static inline isc_result_t
towire_hs_a(dns_rdata_t *rdata, dns_compress_t *cctx, isc_buffer_t *target)
{
	isc_region_t region;

	REQUIRE(rdata->type == 1);
	REQUIRE(rdata->rdclass == 4);

	UNUSED(cctx);

	isc_buffer_availableregion(target, &region);
	if (region.length < rdata->length)
		return (ISC_R_NOSPACE);
	memcpy(region.base, rdata->data, rdata->length);
	isc_buffer_add(target, 4);
	return (ISC_R_SUCCESS);
}

static inline int
compare_hs_a(dns_rdata_t *rdata1, dns_rdata_t *rdata2)
{
	int order;
	
	REQUIRE(rdata1->type == rdata2->type);
	REQUIRE(rdata1->rdclass == rdata2->type);
	REQUIRE(rdata1->type == 1);
	REQUIRE(rdata1->rdclass == 4);

	order = memcmp(rdata1->data, rdata2->data, 4);
	if (order != 0)
		order = (order < 0) ? -1 : 1;

	return (order);
}

static inline isc_result_t
fromstruct_hs_a(dns_rdataclass_t rdclass, dns_rdatatype_t type, void *source,
	        isc_buffer_t *target)
{

	REQUIRE(type == 1);
	REQUIRE(rdclass == 4);

	UNUSED(source);
	UNUSED(target);

	return (ISC_R_NOTIMPLEMENTED);
}

static inline isc_result_t
tostruct_hs_a(dns_rdata_t *rdata, void *target, isc_mem_t *mctx)
{
	dns_rdata_hs_a_t *a = target;
	isc_uint32_t n;
	isc_region_t region;

	REQUIRE(rdata->type == 1);
	REQUIRE(rdata->rdclass == 4);

	UNUSED(mctx);

	a->common.rdclass = rdata->rdclass;
	a->common.rdtype = rdata->type;
	ISC_LINK_INIT(&a->common, link);

	dns_rdata_toregion(rdata, &region);
	n = uint32_fromregion(&region);
	a->in_addr.s_addr = htonl(n);

	return (ISC_R_SUCCESS);
}

static inline void
freestruct_hs_a(void *source)
{
	REQUIRE(source != NULL);
	REQUIRE(ISC_FALSE);	/*XXX*/

}

static inline isc_result_t
additionaldata_hs_a(dns_rdata_t *rdata, dns_additionaldatafunc_t add,
		    void *arg)
{
	REQUIRE(rdata->type == 1);
	REQUIRE(rdata->rdclass == 4);

	(void)add;
	(void)arg;

	return (ISC_R_SUCCESS);
}

static inline isc_result_t
digest_hs_a(dns_rdata_t *rdata, dns_digestfunc_t digest, void *arg)
{
	isc_region_t r;

	REQUIRE(rdata->type == 1);
	REQUIRE(rdata->rdclass == 4);

	dns_rdata_toregion(rdata, &r);

	return ((digest)(arg, &r));
}

#endif	/* RDATA_HS_4_A_1_C */
