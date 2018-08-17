/*
   collabREate debug_mgr.cpp
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

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include "utils.h"
#include "proj_info.h"
#include "client.h"
#include "debug_mgr.h"
#include "projectmap.h"
#include "clientset.h"

using namespace std;

/**
 * DebugConnectionManager
 * This class is responsible for routing incoming packets to all
 * interested clients
 * @author Tim Vidas
 * @author Chris Eagle
 * @version 0.4.0, August 2012
 */


DebugConnectionManager::DebugConnectionManager(json_object *conf) : ConnectionManagerBase(conf, true) {
   basicmodepid = 500;
   sem_init(&pidLock, 0, 1);
}

DebugConnectionManager::~DebugConnectionManager() {
}

/**
 * authenticate authenticates a user (for use in database mode)
 * bacially this is standard CHAP with HMAC (md5)
 * @param user the user to authenticate
 * @param challenge the randomly generated challenge send to the plugin
 * @param response the calculated response from the plugin to check 
 * @return the user id of an authenticated user, or INVALID_USER
 */
int DebugConnectionManager::authenticate(Client *c, const char *user, const uint8_t *challenge, uint32_t clen, const uint8_t *response, uint32_t rlen) {
   //always authenticate in debug mode
   c->setPub(FULL_PERMISSIONS);
   c->setSub(FULL_PERMISSIONS);
   c->setUserPub(FULL_PERMISSIONS);
   c->setUserSub(FULL_PERMISSIONS);
   c->setReqPub(FULL_PERMISSIONS);
   c->setReqSub(FULL_PERMISSIONS);
   return BASIC_USER;
}

/**
 * post both queues a newly received update to be sent to other clients and (if in DB mode)
 * archives the udpate in the database so that future clients can receive it 
 * @param src the client that made the update
 * @param cmd the 'command' that was performed (comment, rename, etc)
 * @param data the 'data' portion of the command (the comment text, etc)
 */
void DebugConnectionManager::post(Client *src, const char * cmd, json_object *obj) {
   const char *json = json_object_to_json_string(obj);
   fprintf(stderr, "%s\n", json);
   json_object_put(obj);
}

/**
 * sendLatestUpdates sends updates from LastUpdate to current 
 * it is expected that the client has already joined a project before calling this function
 * it is expected that the client has already received updates from 0 - lastUpdate 
 * this function is typically called when a user is re-joining a project that they had previously worked on
 * @param c the client requesting updates 
 * @param lastUpdate the last update the client received 
 */
void DebugConnectionManager::sendLatestUpdates(Client *c, uint64_t lastUpdate) {
   c->send_error("Server is in debug mode, updates to date are not stored");
}

/**
 * getProjectInfo gets information related to a local project
 * @param pid the local pid of a project to get info on
 * @return a  project info object for the provided pid
 */
ProjectInfo *DebugConnectionManager::getProjectInfo(int pid) {
   return NULL;
}

/**
 * getProjectList generates a list of projects on this server, each list (vector) item is 
 * actually a pinfo (project info) object, the list does NOT contain all projects, but
 * only contains projects relevant to the binary that is currently loaded in IDA
 * @param phash the IDA generated hash that is unique among the analysis files
 * @return a vector of project info objects for the provided phash
 */
vector<ProjectInfo*> *DebugConnectionManager::getProjectList(const string &phash) {
   return new vector<ProjectInfo*>;
}

/**
 * joinProject joings a particular client to a project so that it can participate in collabREation 
 * @param c the client attempting to join 
 * @param lpid the local project id of the project on this server 
 * @return 0 on success, negative value on failure
 */
int DebugConnectionManager::joinProject(Client *c, int lpid) {
   return 0;
}

/**
 * snapProject adds a snapshop for a project, this does not change the client's 
 * current project, nor copy any updates, it simply marks a point-in-time (updateid wise)
 * this point-in-time can later be used as a project fork point if desired 
 * @param c the client invoking the snapshot
 * @param lastupdateid the point-in-time the client wishes to save in the snapshot
 * @param desc a user provided description of the snapshot
 * @return the snapshotid on success, -1 on failure
 */
int DebugConnectionManager::snapProject(Client *c, uint64_t lastupdateid, const string &desc) {
   c->send_error("Server is in debug mode, snapshots cannot be made");
   return -1;
}


/**
 * forkProject  forks a project - creats new project and copies all updates to point to the new project,
 * publish and subscribe values are inherited
 * @param c client object invoking the fork
 * @param lastupdateid the updateid value the fork is to occur at
 * @param desc user provided description of the fork
 * @return the new projectid on success, -1 on failure
 */

int DebugConnectionManager::forkProject(Client *c, uint64_t lastupdateid, const string &desc) {
   c->send_error("Server is in debug mode, forking is not available");
   return -1;
}


/**
 * forkProject  forks a project - creats new project and copies all updates to point to the new project
 * @param c client object invoking the fork
 * @param lastupdateid the updateid value the fork is to occur at
 * @param desc user provided description of the fork
 * @param pub specified publish permissions
 * @param sub specified subscribe permissions
 * @return the new projectid on success, -1 on failure
 */
int DebugConnectionManager::forkProject(Client *c, uint64_t lastupdateid, const string &desc, uint64_t pub, uint64_t sub) {
   c->send_error("Server is in debug mode, forking is not available");
   return -1;
}

/**
 * sendForkFollows sends a special "follow fork" message to all clients working on
 * a project that has been forked, this allows the user to decide if they would like
 * to continue to work on the existing project, or change to the newly created project
 * @param originator the client that instigated the fork
 * @param oldlpid the local pid of the original project
 * @param lastupdateid the last update processed prior to fork (if your database is different you can't change to the new project)
 * @param desc the description of the new project, so the user can make a more educated descision
 */
void DebugConnectionManager::sendForkFollows(Client *originator, int oldlpid, uint64_t lastupdateid, const string &desc) {
   originator->send_error("Server is in debug mode, follow forking is not available");
}


/**
 * snapforkProject -  this is a special version of forkProject that is designed to work
 * on snapshots (instead of existing projects) this works exactly like forkProject, execpt
 * updates are copied from the 'parent' of the snapshot instead of the client's currently 
 * associated project, also updates are copied until the lastupdateid from the snapshot, 
 * not from the plugin (last received update is stored in the idb)
 * @param c client invoking the snapforkProject
 * @param spid the pid of the project that is being snapshotted
 * @param desc the user provided description for the snapshot
 * @return the new project id on success, -1 on failure
 */

int DebugConnectionManager::snapforkProject(Client *c, int spid, const string &desc, uint64_t pub, uint64_t sub) {
   c->send_error("Server is in debug mode, forking snapshots is not available");
   return -1;
}

/**
 * migrateProject adds a project to the database  
 * fairly similar to addProject
 * @param owner the uid to be the owner of the new project
 * @param gpid unique global id for the incoming project
 * @param hash unique hash for the binary file originally generated by IDA
 * @param desc user provided description of the project 
 * @param pub the publish permissions for the project
 * @param sub the subscribe permissions for the project
 * @return the new project id on success, -1 on failure
 */

int DebugConnectionManager::migrateProject(const char *owner, const string &gpid, const string &hash, const string &desc, uint64_t pub, uint64_t sub) {
   ::logln("migrating in debug mode doesn't make sense!", LERROR);
   return -1;
}

/**
 * addProject adds a project to the database and reflector (or merely a reflector in non-DB mode) 
 * @param c cliend invoking the addProject
 * @param hash unique hash for the binary file originally generated by IDA
 * @param desc user provided description of the project 
 * @param pub the publish permissions for the project
 * @param sub the subscribe permissions for the project
 * @return the new project id on success, -1 on failure
 */

int DebugConnectionManager::addProject(Client *c, const string &hash, const string &desc, uint64_t pub, uint64_t sub) {
   return basicmodepid++;
}

/**
 * lpid2gpid converts an lpid (pid local to a particular server instance) 
 * to a gpid (which is unique across all projects on all servers)
 * @param lpid the local pid for this particular server 
 * @return the glocabl pid
 */
string DebugConnectionManager::lpid2gpid(int lpid) {
   return string();
}