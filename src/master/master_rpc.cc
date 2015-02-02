// Copyright (c) 2015 Chaobin Zhang. All rights reserved.
// Use of this source code is governed by the BSD license that can be
// found in the LICENSE file.

#include "master/master_rpc.h"

#include "proto/slave_services.pb.h"
#include "master/master_main_runner.h"
#include "rpc/rpc_connection.h"
#include "thread/ninja_thread.h"

namespace master {

MasterRPC::MasterRPC(const std::string& bind_ip,
                     uint16 port,
                     scoped_refptr<MasterMainRunner> master_main_runner)
    : bind_ip_(bind_ip),
      port_(port),
      master_main_runner_(master_main_runner) {
  NinjaThread::SetDelegate(NinjaThread::RPC, this);
}

MasterRPC::~MasterRPC() {
  NinjaThread::SetDelegate(NinjaThread::RPC, NULL);
}

void MasterRPC::Init() {
}

void MasterRPC::InitAsync() {
  rpc_socket_server_.reset(new rpc::RpcSocketServer(bind_ip_, port_));
  rpc_socket_server_->AddObserver(this);
}

void MasterRPC::CleanUp() {
  rpc_socket_server_->RemoveObserver(this);
  rpc_socket_server_.reset(NULL);
}

void MasterRPC::OnConnect(rpc::RpcConnection* connection) {
}

}  // namespace master
