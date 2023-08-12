/*
 * This file Copyright (C) 2012-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 */

#include "favicon.h"

#include "util.h" /* gtr_get_host_from_url() */

#include <libtransmission/transmission.h>
#include <libtransmission/web.h> /* tr_webRun() */

#include <glib/gstdio.h> /* g_remove() */
#include <gtk/gtk.h>

#define IMAGE_TYPES 4
static char const* image_types[IMAGE_TYPES] = { "ico", "png", "gif", "jpg" };

struct favicon_data
{
    tr_session* session;
    GFunc func;
    gpointer data;
    char* host;
    char* contents;
    size_t len;
    int type;
};

static char* get_url(char const* host, int image_type)
{
    return g_strdup_printf("http://%s/favicon.%s", host, image_types[image_type]);
}

static char* favicon_get_cache_dir(void)
{
    static char* dir = NULL;

    if (dir == NULL)
    {
        dir = g_build_filename(g_get_user_cache_dir(), "transmission", "favicons", NULL);
        g_mkdir_with_parents(dir, 0777);
    }

    return dir;
}

static char* favicon_get_cache_filename(char const* host)
{
    return g_build_filename(favicon_get_cache_dir(), host, NULL);
}

static void favicon_save_to_cache(char const* host, void const* data, size_t len)
{
    char* filename = favicon_get_cache_filename(host);
    g_file_set_contents(filename, data, len, NULL);
    g_free(filename);
}

static GdkPixbuf* favicon_load_from_cache(char const* host)
{
    char* filename = favicon_get_cache_filename(host);
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_size(filename, 16, 16, NULL);

    if (pixbuf == NULL) /* bad file */
    {
        g_remove(filename);
    }

    g_free(filename);
    return pixbuf;
}

static void favicon_web_done_cb(tr_session*, bool, bool, long, void const*, size_t, void*);

static gboolean favicon_web_done_idle_cb(gpointer vfav)
{
    GdkPixbuf* pixbuf = NULL;
    gboolean finished = FALSE;
    struct favicon_data* fav = vfav;

    if (fav->len > 0) /* we got something... try to make a pixbuf from it */
    {
        favicon_save_to_cache(fav->host, fav->contents, fav->len);
        pixbuf = favicon_load_from_cache(fav->host);
        finished = pixbuf != NULL;
    }

    if (!finished) /* no pixbuf yet... */
    {
        if (++fav->type == IMAGE_TYPES) /* failure */
        {
            finished = TRUE;
        }
        else /* keep trying */
        {
            char* url = get_url(fav->host, fav->type);

            g_free(fav->contents);
            fav->contents = NULL;
            fav->len = 0;

            tr_webRun(fav->session, url, favicon_web_done_cb, fav);
            g_free(url);
        }
    }

    if (finished)
    {
        fav->func(pixbuf, fav->data);
        g_free(fav->host);
        g_free(fav->contents);
        g_free(fav);
    }

    return G_SOURCE_REMOVE;
}

static void favicon_web_done_cb(
    tr_session* session UNUSED,
    bool did_connect UNUSED,
    bool did_timeout UNUSED,
    long code UNUSED,
    void const* data,
    size_t len,
    void* vfav)
{
    struct favicon_data* fav = vfav;
    fav->contents = g_memdup2(data, len);
    fav->len = len;

    gdk_threads_add_idle(favicon_web_done_idle_cb, fav);
}

void gtr_get_favicon(tr_session* session, char const* host, GFunc pixbuf_ready_func, gpointer pixbuf_ready_func_data)
{
    GdkPixbuf* pixbuf = favicon_load_from_cache(host);

    if (pixbuf != NULL)
    {
        pixbuf_ready_func(pixbuf, pixbuf_ready_func_data);
    }
    else
    {
        struct favicon_data* data;
        char* url = get_url(host, 0);

        data = g_new(struct favicon_data, 1);
        data->session = session;
        data->func = pixbuf_ready_func;
        data->data = pixbuf_ready_func_data;
        data->host = g_strdup(host);
        data->type = 0;

        tr_webRun(session, url, favicon_web_done_cb, data);
        g_free(url);
    }
}

void gtr_get_favicon_from_url(tr_session* session, char const* url, GFunc pixbuf_ready_func, gpointer pixbuf_ready_func_data)
{
    char host[1024];
    gtr_get_host_from_url(host, sizeof(host), url);
    gtr_get_favicon(session, host, pixbuf_ready_func, pixbuf_ready_func_data);
}
