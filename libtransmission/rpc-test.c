/*
 * This file Copyright (C) 2013-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 */

#include "transmission.h"
#include "rpcimpl.h"
#include "utils.h"
#include "variant.h"

#include "libtransmission-test.h"

static int test_list(void)
{
    size_t len;
    int64_t i;
    char const *str;
    tr_variant top;

    tr_rpc_parse_list_str(&top, "12", TR_BAD_SIZE);
    check(tr_variantIsInt(&top));
    check(tr_variantGetInt(&top, &i));
    check_int(i, ==, 12);
    tr_variantFree(&top);

    tr_rpc_parse_list_str(&top, "12", 1);
    check(tr_variantIsInt(&top));
    check(tr_variantGetInt(&top, &i));
    check_int(i, ==, 1);
    tr_variantFree(&top);

    tr_rpc_parse_list_str(&top, "6,7", TR_BAD_SIZE);
    check(tr_variantIsList(&top));
    check_uint(tr_variantListSize(&top), ==, 2);
    check(tr_variantGetInt(tr_variantListChild(&top, 0), &i));
    check_int(i, ==, 6);
    check(tr_variantGetInt(tr_variantListChild(&top, 1), &i));
    check_int(i, ==, 7);
    tr_variantFree(&top);

    tr_rpc_parse_list_str(&top, "asdf", TR_BAD_SIZE);
    check(tr_variantIsString(&top));
    check(tr_variantGetStr(&top, &str, &len));
    check_uint(len, ==, 4);
    check_str(str, ==, "asdf");
    tr_variantFree(&top);

    tr_rpc_parse_list_str(&top, "1,3-5", TR_BAD_SIZE);
    check(tr_variantIsList(&top));
    check_uint(tr_variantListSize(&top), ==, 4);
    check(tr_variantGetInt(tr_variantListChild(&top, 0), &i));
    check_int(i, ==, 1);
    check(tr_variantGetInt(tr_variantListChild(&top, 1), &i));
    check_int(i, ==, 3);
    check(tr_variantGetInt(tr_variantListChild(&top, 2), &i));
    check_int(i, ==, 4);
    check(tr_variantGetInt(tr_variantListChild(&top, 3), &i));
    check_int(i, ==, 5);
    tr_variantFree(&top);

    return 0;
}

/***
****
***/

static void rpc_response_func(tr_session *session UNUSED, tr_variant *response, void *setme)
{
    *(tr_variant *)setme = *response;
    tr_variantInitBool(response, false);
}

static int test_session_get_and_set(void)
{
    tr_session *session;
    tr_variant request;
    tr_variant response;
    tr_variant *args;
    tr_torrent *tor;

    session = libttest_session_init(NULL);
    tor = libttest_zero_torrent_init(session);
    check_ptr(tor, !=, NULL);

    tr_variantInitDict(&request, 1);
    tr_variantDictAddStr(&request, TR_KEY_method, "session-get");
    tr_rpc_request_exec_json(session, &request, rpc_response_func, &response);
    tr_variantFree(&request);

    check(tr_variantIsDict(&response));
    check(tr_variantDictFindDict(&response, TR_KEY_arguments, &args));
    check_ptr(tr_variantDictFind(args, TR_KEY_alt_speed_down), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_alt_speed_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_alt_speed_time_begin), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_alt_speed_time_day), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_alt_speed_time_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_alt_speed_time_end), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_alt_speed_up), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_blocklist_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_blocklist_size), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_blocklist_url), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_cache_size_mb), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_config_dir), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_dht_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_download_dir), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_download_dir_free_space), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_download_queue_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_download_queue_size), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_encryption), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_idle_seeding_limit), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_idle_seeding_limit_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_incomplete_dir), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_incomplete_dir_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_lpd_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_peer_limit_global), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_peer_limit_per_torrent), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_peer_port), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_peer_port_random_on_start), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_pex_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_port_forwarding_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_queue_stalled_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_queue_stalled_minutes), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_rename_partial_files), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_rpc_version), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_rpc_version_minimum), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_script_torrent_done_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_script_torrent_done_filename), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_seed_queue_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_seed_queue_size), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_seedRatioLimit), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_seedRatioLimited), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_speed_limit_down), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_speed_limit_down_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_speed_limit_up), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_speed_limit_up_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_start_added_torrents), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_trash_original_torrent_files), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_units), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_utp_enabled), !=, NULL);
    check_ptr(tr_variantDictFind(args, TR_KEY_version), !=, NULL);
    tr_variantFree(&response);

    /* cleanup */
    tr_torrentRemove(tor, false, NULL);
    libttest_session_close(session);
    return 0;
}

/***
****
***/

int main(void)
{
    testFunc const tests[] = {
        test_list,
        test_session_get_and_set,
    };

    return runTests(tests, NUM_TESTS(tests));
}
