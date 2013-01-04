/*
 * See Copyright Notice in qnode.h
 */

#include <stdio.h>
#include "qassert.h"
#include "qactor.h"
#include "qlist.h"
#include "qlog.h"
#include "qluautil.h"
#include "qmalloc.h"
#include "qmsg.h"
#include "qserver.h"
#include "qthread.h"

static int server_handle_wrong_msg(qserver_t *server, qmsg_t *msg) {
  qerror("handle server type %d msg error", msg->type);
  return 0;
}

static int server_handle_spawn_msg(qserver_t *server, qmsg_t *msg) {
  qid_t aid = msg->args.spawn.aid;
  lua_State *state = msg->args.spawn.state;
  qactor_t *actor = qactor_new(aid, state);
  actor->parent = msg->args.spawn.parent;
  msg->args.spawn.actor = actor;
  qmsg_set_undelete(msg);
  msg->tid = qserver_worker_thread();
  qserver_send_mail(msg);
  server->actors[aid] = actor;
  return 0;
}

wmsg_handler wmsg_handlers[] = {
  &server_handle_wrong_msg,     /* wrong */
  &server_handle_wrong_msg,     /* s_start, wrong */
  &server_handle_spawn_msg,     /* spawn */
};
