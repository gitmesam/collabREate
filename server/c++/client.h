/*
   collabREate client.h
   Copyright (C) 2018 Chris Eagle <cseagle at gmail d0t com>
   Copyright (C) 2018 Tim Vidas <tvidas at gmail d0t com>

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __CLIENT_H
#define __CLIENT_H

#include <map>
#include <string>
#include <stdint.h>
#include <json-c/json.h>
#include "io.h"
#include "utils.h"

using namespace std;

class ConnectionManager;
class Client;

typedef bool (*ClientMsgHandler)(json_object *obj, Client *c);

/**
 * Client
 * This class is responsible for a single client connection
 * It handles the initial client interaction, then reads
 * incoming client commands and kicks them up to the ConnectionManager
 * which farms the commands out to all interested clients
 * @author Tim Vidas
 * @author Chris Eagle
 * @version 0.4.0, August 2012
 */

class Client {
public:

   Client(ConnectionManager *mgr, NetworkIO *s, uint32_t uid);

   void run();

   /**
    * logs a message to the configured log file (in the ConnectionManager)
    * @param verbosity apply a verbosity level to the msg
    * @param msg the string to log
    */
   void clog(int verbosity, const char *format, ...);

   /**
    * logs a message to the configured log file (in the ConnectionManager)
    * @param verbosity apply a verbosity level to the msg
    * @param msg the string to log
    */
   void clog(int verbosity, const string &msg);

   /**
    * getHash inspector to get the hash value (unique per binary in IDA, generated by IDA) of the client
    * @return the hash
    */
   const string &getHash() {
      return hash;
   }

   /**
    * setHash mutator function to set the hash value (uniqure per binary in IDA, generated by IDA) of the client
    * @param phash the hash value
    */
   void setHash(string phash) {
      hash = phash;
   }

   /**
    * getGpid inspector to get the gpid (global project id unique across server instances) value
    * @return the global pid
    */
   const string &getGpid() {
      return gpid;
   }

   /**
    * getGpid mutator to set the gpid (global project id unique across server instances) value
    * @param gpid the global pid value
    */
   void setGpid(const string &gpid) {
      this->gpid = gpid;
   }

   /**
    * getPid inspector to get the pid (local project id, unigue to this server instance only) value
    * @return pid this local pid
    */
   uint32_t getPid() {
      return pid;
   }

   /**
    * getPid mutator to set the pid (local project id, unigue to this server instance only) value
    * @param p the project pid
    */
   void setPid(uint32_t p) {
//      string msg = "set client pid to ";
//      logln(msg + p, LINFO1);
      pid = p;
   }

   /**
    * getUid inspector to get the user id associated with this server
    * @return the user id
    */
   uint32_t getUid() {
      return uid;
   }

   /**
    * getUid mutator to set the user id associated with this server
    * @param u the userid
    */
   void setUid(uint32_t u) {
      uid = u;
   }

   /**
    * post is the function that actually posts updates to clients (if subscribing)
    * @param msg message being sent
    * @param obj message with associated parameters expressed as a json object
    */
   void post(const char *msg, json_object *obj);

   /**
    * similar to post, but does not check subscription status, and takes command as a arg
    * This function should ONLY be called for message id >= MSG_CONTROL_FIRST
    * because these messages do not contain an updateid
    * @param command the command to send
    * @param obj message with associated parameters expressed as a json object
    */
   void send_data(const char *command, json_object *obj);

   /**
    * sendForkFollow sends a FORKFOLLOW message to the client, this occurs when another
    * user on the project decided to fork, the plugin is expected to give the user the
    * option of joining the new project or not
    * @param fuser the user that initiated the fork
    * @param gpid the global pid of the new project
    * @param lastupdateid the updateid that the project forked at
    * @param desc a description of the newly forked project
    */
   void sendForkFollow(string fuser, string gpid, uint64_t lastupdateid, string desc);

   void send_error_msg(string theerror, const char *type);

   /**
    * send_error sends an error string to the plugin
    * @param theerror this error string to send
    */
   void send_error(string theerror) {
      send_error_msg(theerror, MSG_ERROR);
   }

   /**
    * send_fatal sends an error string to the plugin, this is idential to send_error except
    * for the message type, the intent is that the semantics on the plugin side are different
    * @param theerror this error string to send
    */
   void send_fatal(string theerror) {
      send_error_msg(theerror, MSG_FATAL);
   }

   /**
    * terminate closes the client's connection, removes this client from the connection manager
    */
   void terminate();

   /**
    * dumpStats displace the receive / transmit stats for each command
    */
   string dumpStats();

   /**
    * getPort inspector to get the TCP port number of the connection
    * @return the TCP port
    */
   uint32_t getPeerPort();

   /**
    * getAddr inspector to get the IP address of the connection
    * @return the IP address
    */
   string getPeerAddr();

   /**
    * setReqPub mutator to set the publish status of the session requested perms stored in the client
    * @param p the users req publish status
    */
   void setReqPub(uint64_t p) {
      rpublish = p;
   }

   /**
    * setReqSub mutator to set the subscription status of the session requested perms stored in the client
    * @param s the users req subscribe status
    */
   void setReqSub(uint64_t s) {
      rsubscribe = s;
   }
   /**
    * getReqPub inspector to get the publish status of the session requested perms stored in the client
    * @return the users req publish status
    */
   uint64_t getReqPub() {
      return rpublish;
   }

   /**
    * getReqSub inspector to get the subscription status of the session requested perms stored in the client
    * @return the users req subscribe status
    */
   uint64_t getReqSub() {
      return rsubscribe;
   }
   /**
    * setUserPub mutator to set the publish status of the user stored in the client
    * @param p the users publish status
    */
   void setUserPub(uint64_t p) {
      upublish = p;
   }

   /**
    * setUserSub mutator to set the subscription status of the user stored in the client
    * @param s the users subscribe status
    */
   void setUserSub(uint64_t s) {
      usubscribe = s;
   }
   /**
    * getUserPub inspector to get the publish status of the user stored in the client
    * @return the users publish status
    */
   uint64_t getUserPub() {
      return upublish;
   }

   /**
    * getUserSub inspector to get the subscription status of the user stored in the client
    * @return the users subscribe status
    */
   uint64_t getUserSub() {
      return usubscribe;
   }

   /**
    * setPub mutator to set the effective publish status of the client
    * @param p the publish status
    */
   void setPub(uint64_t p) {
      publish = p;
   }

   /**
    * setSub mutator to set the effective subscription status of the client
    * @param s the subscribe status
    */
   void setSub(uint64_t s) {
      subscribe = s;
   }
   /**
    * getPub inspector to get the effective publish status of the client
    * @return the publish status
    */
   uint64_t getPub() {
      return publish;
   }

   /**
    * getSub inspector to get the effective subscription status of the client
    * @return the subscribe status
    */
   uint64_t getSub() {
      return subscribe;
   }

   /**
    * getUser inspector to get the Username assocaited with the client
    * @return the username
    */
   const string &getUser() {
      return username;
   }

   void setChallenge(const uint8_t *data, uint32_t len);
   const uint8_t *getChallenge(uint32_t &len) {len = CHALLENGE_SIZE; return challenge;};

private:
   /**
    * checkPermissions checks to see if the current client has permissions to perform an operation
    * @param command the command to check permissions on
    * @param permType the permission types to check (publish/subscribe)
    */
   /* These are grouped into 'collabREate' permissions, just so there are less permissions to manage
    * for example all the segment operations (add, del, start/end change, etc) are grouped into
    * 'segment' permissions.
    */
   bool checkPermissions(const char *command, uint64_t permType);
   static void init_handlers();

   NetworkIO *conn;
   string hash;
   string username;

   //effective, combined permissions (project & user & requested), used for checks
   uint64_t publish;
   uint64_t subscribe;
   //the permissions for the user account, read from database
   uint64_t upublish;
   uint64_t usubscribe;
   //the requested permissions sent from the plugin
   uint64_t rpublish;
   uint64_t rsubscribe;

   uint32_t uid;  //user id associated with this connection
   uint32_t pid;

   string gpid;  //project id associated with this connection
   uint8_t challenge[CHALLENGE_SIZE];

   ConnectionManager *cm;

   int stats[2][MAX_COMMAND];

   static map<string,ClientMsgHandler> *handlers;

   static bool msg_project_new_request(json_object *obj, Client *c);
   static bool msg_project_join_request(json_object *obj, Client *c);
   static bool msg_project_rejoin_request(json_object *obj, Client *c);
   static bool msg_project_snapshot_request(json_object *obj, Client *c);
   static bool msg_project_fork_request(json_object *obj, Client *c);
   static bool msg_project_snapfork_request(json_object *obj, Client *c);
   static bool msg_project_leave(json_object *obj, Client *c);
   static bool msg_project_join_reply(json_object *obj, Client *c);
   static bool msg_auth_request(json_object *obj, Client *c);
   static bool msg_project_list(json_object *obj, Client *c);
   static bool msg_send_updates(json_object *obj, Client *c);
   static bool msg_set_req_perms(json_object *obj, Client *c);
   static bool msg_get_req_perms(json_object *obj, Client *c);
   static bool msg_get_proj_perms(json_object *obj, Client *c);
   static bool msg_set_proj_perms(json_object *obj, Client *c);

};

#endif
