// Copyright (c) 2015 Cesanta Software Limited
// All rights reserved

#include <gc.h>
#include "mongoose.h"

struct Root
{
  struct mg_mgr *mgr;
};

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  if (ev == MG_EV_HTTP_REQUEST) {
    if(mg_vcmp(&hm->uri, "/hmm") == 0)
    {
      return;
    }
    mg_serve_http(nc, (struct http_message *) ev_data, s_http_server_opts);

    if(mg_vcmp(&hm->uri, "/abc/") == 0)
    {
      nc->mgr->user_data = NULL;
    }
  }
}

void verbose_mg_mgr_free(struct mg_mgr *mgr)
{
  printf("Freeing Mgr\n");
  mg_mgr_free(mgr);
}

int main(void) {
  struct Root *root = gc_root(sizeof(struct Root));

  root->mgr = gc_alloc(sizeof(struct mg_mgr));

  mg_mgr_init(root->mgr, NULL);
  gc_reattach(root->mgr, verbose_mg_mgr_free);

  root->mgr->user_data = root;
  struct mg_connection *nc = mg_bind(root->mgr, s_http_port, ev_handler);

  // Set up HTTP server parameters
  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.document_root = ".";  // Serve current directory
  s_http_server_opts.dav_document_root = ".";  // Allow access via WebDav
  s_http_server_opts.enable_directory_listing = "yes";

  printf("Starting web server on port %s\n", s_http_port);
  for (;;) {
    mg_mgr_poll(root->mgr, 1000);
    gc_collect();

    if(root->mgr->user_data == NULL)
    {
      break;
    }
  }

  memset(root, 0, sizeof(struct Root));

  gc_collect();

  return 0;
}
