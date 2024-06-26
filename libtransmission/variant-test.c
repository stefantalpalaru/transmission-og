/*
 * This file Copyright (C) 2013-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 */

#include <ctype.h> /* isspace() */
#include <errno.h> /* EILSEQ */
#include <string.h> /* strlen(), strncmp() */

#include <event2/buffer.h>

#define __LIBTRANSMISSION_VARIANT_MODULE__

#include "transmission.h"
#include "utils.h" /* tr_free */
#include "variant.h"
#include "variant-common.h"

#include "libtransmission-test.h"

#ifndef _WIN32
#define STACK_SMASH_DEPTH (1 * 1000 * 1000)
#else
#define STACK_SMASH_DEPTH (100 * 1000)
#endif

static int testInt(void)
{
    uint8_t buf[128];
    int64_t val;
    int err;
    uint8_t const *end;

    /* good int string */
    tr_snprintf((char *)buf, sizeof(buf), "i64e");
    err = tr_bencParseInt(buf, buf + 4, &end, &val);
    check_int(err, ==, 0);
    check_int(val, ==, 64);
    check_ptr(end, ==, buf + 4);

    /* missing 'e' */
    end = NULL;
    val = 888;
    err = tr_bencParseInt(buf, buf + 3, &end, &val);
    check_int(err, ==, EILSEQ);
    check_int(val, ==, 888);
    check_ptr(end, ==, NULL);

    /* empty buffer */
    err = tr_bencParseInt(buf, buf + 0, &end, &val);
    check_int(err, ==, EILSEQ);
    check_int(val, ==, 888);
    check_ptr(end, ==, NULL);

    /* bad number */
    tr_snprintf((char *)buf, sizeof(buf), "i6z4e");
    err = tr_bencParseInt(buf, buf + 5, &end, &val);
    check_int(err, ==, EILSEQ);
    check_int(val, ==, 888);
    check_ptr(end, ==, NULL);

    /* negative number */
    tr_snprintf((char *)buf, sizeof(buf), "i-3e");
    err = tr_bencParseInt(buf, buf + 4, &end, &val);
    check_int(err, ==, 0);
    check_int(val, ==, -3);
    check_ptr(end, ==, buf + 4);

    /* zero */
    tr_snprintf((char *)buf, sizeof(buf), "i0e");
    err = tr_bencParseInt(buf, buf + 4, &end, &val);
    check_int(err, ==, 0);
    check_int(val, ==, 0);
    check_ptr(end, ==, buf + 3);

    /* no leading zeroes allowed */
    val = 0;
    end = NULL;
    tr_snprintf((char *)buf, sizeof(buf), "i04e");
    err = tr_bencParseInt(buf, buf + 4, &end, &val);
    check_int(err, ==, EILSEQ);
    check_int(val, ==, 0);
    check_ptr(end, ==, NULL);

    return 0;
}

static int testStr(void)
{
    uint8_t buf[128];
    int err;
    int n;
    uint8_t const *end;
    uint8_t const *str;
    size_t len;

    /* string len is designed to overflow */
    n = tr_snprintf((char *)buf, sizeof(buf), "%zu:boat", (size_t)(SIZE_MAX - 2));
    err = tr_bencParseStr(buf, buf + n, &end, &str, &len);
    check_int(err, ==, EILSEQ);
    check_uint(len, ==, 0);
    check_ptr(str, ==, NULL);
    check_ptr(end, ==, NULL);

    /* good string */
    n = tr_snprintf((char *)buf, sizeof(buf), "4:boat");
    err = tr_bencParseStr(buf, buf + n, &end, &str, &len);
    check_int(err, ==, 0);
    check_uint(len, ==, 4);
    check_mem(str, ==, "boat", len);
    check_ptr(end, ==, buf + 6);
    str = NULL;
    end = NULL;
    len = 0;

    /* string goes past end of buffer */
    err = tr_bencParseStr(buf, buf + (n - 1), &end, &str, &len);
    check_int(err, ==, EILSEQ);
    check_uint(len, ==, 0);
    check_ptr(str, ==, NULL);
    check_ptr(end, ==, NULL);

    /* empty string */
    n = tr_snprintf((char *)buf, sizeof(buf), "0:");
    err = tr_bencParseStr(buf, buf + n, &end, &str, &len);
    check_int(err, ==, 0);
    check_uint(len, ==, 0);
    check_uint(*str, ==, '\0');
    check_ptr(end, ==, buf + 2);
    str = NULL;
    end = NULL;
    len = 0;

    /* short string */
    n = tr_snprintf((char *)buf, sizeof(buf), "3:boat");
    err = tr_bencParseStr(buf, buf + n, &end, &str, &len);
    check_int(err, ==, 0);
    check_uint(len, ==, 3);
    check_mem(str, ==, "boa", len);
    check_ptr(end, ==, buf + 5);
    str = NULL;
    end = NULL;
    len = 0;

    return 0;
}

static int testString(char const *str, bool isGood)
{
    tr_variant val;
    char const *end = NULL;
    char *saved;
    size_t const len = strlen(str);
    size_t savedLen;
    int err;

    err = tr_variantFromBencFull(&val, str, len, NULL, &end);

    if (!isGood) {
        check_int(err, !=, 0);
    } else {
        check_int(err, ==, 0);
#if 0
        fprintf(stderr, "in: [%s]\n", str);
        fprintf(stderr, "out:\n%s", tr_variantToStr(&val, TR_VARIANT_FMT_JSON, NULL));
#endif
        check_ptr(end, ==, str + len);
        saved = tr_variantToStr(&val, TR_VARIANT_FMT_BENC, &savedLen);
        check_str(saved, ==, str);
        check_uint(savedLen, ==, len);
        tr_free(saved);
        tr_variantFree(&val);
    }

    return 0;
}

static int testParse(void)
{
    tr_variant val;
    tr_variant *child;
    tr_variant *child2;
    char buf[512];
    char const *end;
    int err;
    size_t len;
    int64_t i;
    char *saved;

    tr_snprintf((char *)buf, sizeof(buf), "i64e");
    err = tr_variantFromBencFull(&val, buf, sizeof(buf), NULL, &end);
    check_int(err, ==, 0);
    check(tr_variantGetInt(&val, &i));
    check_int(i, ==, 64);
    check_ptr(end, ==, buf + 4);
    tr_variantFree(&val);

    tr_snprintf((char *)buf, sizeof(buf), "li64ei32ei16ee");
    err = tr_variantFromBencFull(&val, buf, sizeof(buf), NULL, &end);
    check_int(err, ==, 0);
    check_ptr(end, ==, buf + strlen((char *)buf));
    check_uint(val.val.l.count, ==, 3);
    check(tr_variantGetInt(&val.val.l.vals[0], &i));
    check_int(i, ==, 64);
    check(tr_variantGetInt(&val.val.l.vals[1], &i));
    check_int(i, ==, 32);
    check(tr_variantGetInt(&val.val.l.vals[2], &i));
    check_int(i, ==, 16);
    saved = tr_variantToStr(&val, TR_VARIANT_FMT_BENC, &len);
    check_str(saved, ==, buf);
    tr_free(saved);
    tr_variantFree(&val);

    end = NULL;
    tr_snprintf((char *)buf, sizeof(buf), "lllee");
    err = tr_variantFromBencFull(&val, buf, sizeof(buf), NULL, &end);
    check_int(err, !=, 0);
    check_ptr(end, ==, NULL);

    end = NULL;
    tr_snprintf((char *)buf, sizeof(buf), "le");
    err = tr_variantFromBencFull(&val, buf, sizeof(buf), NULL, &end);
    check_int(err, ==, 0);
    check_ptr(end, ==, buf + 2);
    saved = tr_variantToStr(&val, TR_VARIANT_FMT_BENC, &len);
    check_str(saved, ==, "le");
    tr_free(saved);
    tr_variantFree(&val);

    if ((err = testString("llleee", true)) != 0) {
        return err;
    }

    if ((err = testString("d3:cow3:moo4:spam4:eggse", true)) != 0) {
        return err;
    }

    if ((err = testString("d4:spaml1:a1:bee", true)) != 0) {
        return err;
    }

    if ((err = testString("d5:greenli1ei2ei3ee4:spamd1:ai123e3:keyi214eee", true)) != 0) {
        return err;
    }

    if ((err = testString("d9:publisher3:bob17:publisher-webpage15:www.example.com18:publisher.location4:homee", true)) != 0) {
        return err;
    }

    if ((err = testString("d8:completei1e8:intervali1800e12:min intervali1800e5:peers0:e", true)) != 0) {
        return err;
    }

    if ((err = testString("d1:ai0e1:be", false)) != 0) /* odd number of children */
    {
        return err;
    }

    if ((err = testString("", false)) != 0) {
        return err;
    }

    if ((err = testString(" ", false)) != 0) {
        return err;
    }

    /* nested containers
     * parse an unsorted dict
     * save as a sorted dict */
    end = NULL;
    tr_snprintf((char *)buf, sizeof(buf), "lld1:bi32e1:ai64eeee");
    err = tr_variantFromBencFull(&val, buf, sizeof(buf), NULL, &end);
    check_int(err, ==, 0);
    check_ptr(end, ==, buf + strlen((char const *)buf));
    check_ptr((child = tr_variantListChild(&val, 0)), !=, NULL);
    check_ptr((child2 = tr_variantListChild(child, 0)), !=, NULL);
    saved = tr_variantToStr(&val, TR_VARIANT_FMT_BENC, &len);
    check_str(saved, ==, "lld1:ai64e1:bi32eeee");
    tr_free(saved);
    tr_variantFree(&val);

    /* too many endings */
    end = NULL;
    tr_snprintf((char *)buf, sizeof(buf), "leee");
    err = tr_variantFromBencFull(&val, buf, sizeof(buf), NULL, &end);
    check_int(err, ==, 0);
    check_ptr(end, ==, buf + 2);
    saved = tr_variantToStr(&val, TR_VARIANT_FMT_BENC, &len);
    check_str(saved, ==, "le");
    tr_free(saved);
    tr_variantFree(&val);

    /* no ending */
    end = NULL;
    tr_snprintf((char *)buf, sizeof(buf), "l1:a1:b1:c");
    err = tr_variantFromBencFull(&val, buf, strlen(buf), NULL, &end);
    check_int(err, !=, 0);

    /* incomplete string */
    end = NULL;
    tr_snprintf((char *)buf, sizeof(buf), "1:");
    err = tr_variantFromBencFull(&val, buf, strlen(buf), NULL, &end);
    check_int(err, !=, 0);

    return 0;
}

static void stripWhitespace(char *in)
{
    char *out = in;

    for (; !tr_str_is_empty(in); ++in) {
        if (!isspace(*in)) {
            *out++ = *in;
        }
    }

    *out = '\0';
}

static int testJSONSnippet(char const *benc_str, char const *expected)
{
    tr_variant top;
    char *serialized;
    struct evbuffer *buf;

    tr_variantFromBenc(&top, benc_str, strlen(benc_str));
    buf = tr_variantToBuf(&top, TR_VARIANT_FMT_JSON);
    serialized = (char *)evbuffer_pullup(buf, -1);
    stripWhitespace(serialized);
#if 0
    fprintf(stderr, "benc: %s\n", benc_str);
    fprintf(stderr, "json: %s\n", serialized);
    fprintf(stderr, "want: %s\n", expected);
#endif
    check_str(serialized, ==, expected);
    tr_variantFree(&top);
    evbuffer_free(buf);
    return 0;
}

static int testJSON(void)
{
    int val;
    char const *benc_str;
    char const *expected;

    benc_str = "i6e";
    expected = "6";

    if ((val = testJSONSnippet(benc_str, expected)) != 0) {
        return val;
    }

    benc_str = "d5:helloi1e5:worldi2ee";
    expected = "{\"hello\":1,\"world\":2}";

    if ((val = testJSONSnippet(benc_str, expected)) != 0) {
        return val;
    }

    benc_str = "d5:helloi1e5:worldi2e3:fooli1ei2ei3eee";
    expected = "{\"foo\":[1,2,3],\"hello\":1,\"world\":2}";

    if ((val = testJSONSnippet(benc_str, expected)) != 0) {
        return val;
    }

    benc_str = "d5:helloi1e5:worldi2e3:fooli1ei2ei3ed1:ai0eeee";
    expected = "{\"foo\":[1,2,3,{\"a\":0}],\"hello\":1,\"world\":2}";

    if ((val = testJSONSnippet(benc_str, expected)) != 0) {
        return val;
    }

    benc_str = "d4:argsd6:statusle7:status2lee6:result7:successe";
    expected = "{\"args\":{\"status\":[],\"status2\":[]},\"result\":\"success\"}";

    if ((val = testJSONSnippet(benc_str, expected)) != 0) {
        return val;
    }

    return 0;
}

static int testMerge(void)
{
    size_t len;
    tr_variant dest;
    tr_variant src;
    int64_t i;
    char const *s;
    tr_quark const i1 = tr_quark_new("i1", 2);
    tr_quark const i2 = tr_quark_new("i2", 2);
    tr_quark const i3 = tr_quark_new("i3", 2);
    tr_quark const i4 = tr_quark_new("i4", 2);
    tr_quark const s5 = tr_quark_new("s5", 2);
    tr_quark const s6 = tr_quark_new("s6", 2);
    tr_quark const s7 = tr_quark_new("s7", 2);
    tr_quark const s8 = tr_quark_new("s8", 2);

    /* initial dictionary (default values) */
    tr_variantInitDict(&dest, 10);
    tr_variantDictAddInt(&dest, i1, 1);
    tr_variantDictAddInt(&dest, i2, 2);
    tr_variantDictAddInt(&dest, i4, -35); /* remains untouched */
    tr_variantDictAddStr(&dest, s5, "abc");
    tr_variantDictAddStr(&dest, s6, "def");
    tr_variantDictAddStr(&dest, s7, "127.0.0.1"); /* remains untouched */

    /* new dictionary, will overwrite items in dest */
    tr_variantInitDict(&src, 10);
    tr_variantDictAddInt(&src, i1, 1); /* same value */
    tr_variantDictAddInt(&src, i2, 4); /* new value */
    tr_variantDictAddInt(&src, i3, 3); /* new key:value */
    tr_variantDictAddStr(&src, s5, "abc"); /* same value */
    tr_variantDictAddStr(&src, s6, "xyz"); /* new value */
    tr_variantDictAddStr(&src, s8, "ghi"); /* new key:value */

    tr_variantMergeDicts(&dest, /*const*/ &src);

    check(tr_variantDictFindInt(&dest, i1, &i));
    check_int(i, ==, 1);
    check(tr_variantDictFindInt(&dest, i2, &i));
    check_int(i, ==, 4);
    check(tr_variantDictFindInt(&dest, i3, &i));
    check_int(i, ==, 3);
    check(tr_variantDictFindInt(&dest, i4, &i));
    check_int(i, ==, -35);
    check(tr_variantDictFindStr(&dest, s5, &s, &len));
    check_uint(len, ==, 3);
    check_str(s, ==, "abc");
    check(tr_variantDictFindStr(&dest, s6, &s, &len));
    check_uint(len, ==, 3);
    check_str(s, ==, "xyz");
    check(tr_variantDictFindStr(&dest, s7, &s, &len));
    check_uint(len, ==, 9);
    check_str(s, ==, "127.0.0.1");
    check(tr_variantDictFindStr(&dest, s8, &s, &len));
    check_uint(len, ==, 3);
    check_str(s, ==, "ghi");

    tr_variantFree(&dest);
    tr_variantFree(&src);
    return 0;
}

static int testStackSmash(void)
{
    size_t len;
    int err;
    char *in;
    char const *end;
    tr_variant val;
    char *saved;
    int const depth = STACK_SMASH_DEPTH;

    in = tr_new(char, depth * 2 + 1);

    for (int i = 0; i < depth; ++i) {
        in[i] = 'l';
        in[depth + i] = 'e';
    }

    in[depth * 2] = '\0';
    err = tr_variantFromBencFull(&val, in, depth * 2, NULL, &end);
    check_int(err, ==, 0);
    check_ptr(end, ==, in + depth * 2);
    saved = tr_variantToStr(&val, TR_VARIANT_FMT_BENC, &len);
    check_str(saved, ==, in);
    tr_free(in);
    tr_free(saved);
    tr_variantFree(&val);

    return 0;
}

static int testBool(void)
{
    tr_variant top;
    int64_t intVal;
    bool boolVal;
    tr_quark const key1 = tr_quark_new("key1", 4);
    tr_quark const key2 = tr_quark_new("key2", 4);
    tr_quark const key3 = tr_quark_new("key3", 4);
    tr_quark const key4 = tr_quark_new("key4", 4);

    tr_variantInitDict(&top, 0);

    tr_variantDictAddBool(&top, key1, false);
    tr_variantDictAddBool(&top, key2, 0);
    tr_variantDictAddInt(&top, key3, true);
    tr_variantDictAddInt(&top, key4, 1);
    check(tr_variantDictFindBool(&top, key1, &boolVal));
    check(!boolVal);
    check(tr_variantDictFindBool(&top, key2, &boolVal));
    check(!boolVal);
    check(tr_variantDictFindBool(&top, key3, &boolVal));
    check(boolVal);
    check(tr_variantDictFindBool(&top, key4, &boolVal));
    check(boolVal);
    check(tr_variantDictFindInt(&top, key1, &intVal));
    check_int(intVal, ==, 0);
    check(tr_variantDictFindInt(&top, key2, &intVal));
    check_int(intVal, ==, 0);
    check(tr_variantDictFindInt(&top, key3, &intVal));
    check_int(intVal, !=, 0);
    check(tr_variantDictFindInt(&top, key4, &intVal));
    check_int(intVal, !=, 0);

    tr_variantFree(&top);
    return 0;
}

static int testParse2(void)
{
    tr_variant top;
    tr_variant top2;
    int64_t intVal;
    char const *strVal;
    double realVal;
    bool boolVal;
    size_t len;
    char *benc;
    char const *end;
    size_t strLen;
    tr_quark const key_bool = tr_quark_new("this-is-a-bool", TR_BAD_SIZE);
    tr_quark const key_real = tr_quark_new("this-is-a-real", TR_BAD_SIZE);
    tr_quark const key_int = tr_quark_new("this-is-an-int", TR_BAD_SIZE);
    tr_quark const key_str = tr_quark_new("this-is-a-string", TR_BAD_SIZE);

    tr_variantInitDict(&top, 0);
    tr_variantDictAddBool(&top, key_bool, true);
    tr_variantDictAddInt(&top, key_int, 1234);
    tr_variantDictAddReal(&top, key_real, 0.5);
    tr_variantDictAddStr(&top, key_str, "this-is-a-string");

    benc = tr_variantToStr(&top, TR_VARIANT_FMT_BENC, &len);
    check_str(
        benc,
        ==,
        "d14:this-is-a-booli1e14:this-is-a-real8:0.50000016:this-is-a-string16:this-is-a-string14:this-is-an-"
        "inti1234ee");
    check_int(tr_variantFromBencFull(&top2, benc, len, NULL, &end), ==, 0);
    check_ptr(end, ==, benc + len);
    check(tr_variantIsDict(&top2));
    check(tr_variantDictFindInt(&top, key_int, &intVal));
    check_int(intVal, ==, 1234);
    check(tr_variantDictFindBool(&top, key_bool, &boolVal));
    check(boolVal);
    check(tr_variantDictFindStr(&top, key_str, &strVal, &strLen));
    check_uint(strLen, ==, 16);
    check_str(strVal, ==, "this-is-a-string");
    check(tr_variantDictFindReal(&top, key_real, &realVal));
    check_int((int)(realVal * 100), ==, 50);

    tr_variantFree(&top2);
    tr_free(benc);
    tr_variantFree(&top);

    return 0;
}

int main(void)
{
    // clang-format off
    static testFunc const tests[] = {
        testInt,
        testStr,
        testParse,
        testJSON,
        testMerge,
        testBool,
        testParse2,
        testStackSmash,
    };
    // clang-format on

    return runTests(tests, NUM_TESTS(tests));
}
