// Copyright (c) 2015 Chaobin Zhang. All rights reserved.
// Use of this source code is governed by the BSD license that can be
// found in the LICENSE file.

#ifndef  NINJA_SLAVE_MAIN_H_
#define  NINJA_SLAVE_MAIN_H_

#include <string>
#include <map>

#include "base/basictypes.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "google/protobuf/service.h"
#include "ninja/slave_command_runner.h"
#include "proto/slave_services.pb.h"
#include "rpc/rpc_connection.h"
#include "rpc/service_manager.h"
#include "thread/ninja_thread_delegate.h"

namespace rpc {
class RpcSocketClient;
}

namespace ninja {

class SlaveMain : public NinjaThreadDelegate, public slave::SlaveService {
 public:
  SlaveMain(const std::string& master_ip, uint16 port);
  virtual ~SlaveMain();

  // NinjaThreadDelegate implementations.
  void Init() override;
  void InitAsync() override;
  void CleanUp() override;

  // slave::SlaveService implementations.
  void RunCommand(::google::protobuf::RpcController* controller,
                  const ::slave::RunCommandRequest* request,
                  ::slave::RunCommandResponse* response,
                  ::google::protobuf::Closure* done) override;
  void Finish(::google::protobuf::RpcController* controller,
              const ::slave::FinishRequest* request,
              ::slave::FinishResponse* response,
              ::google::protobuf::Closure* done) override;

 private:
  std::string master_ip_;
  uint16 port_;
  scoped_ptr<rpc::RpcSocketClient> rpc_socket_client_;
  scoped_refptr<SlaveCommandRunner> command_runner_;

  DISALLOW_COPY_AND_ASSIGN(SlaveMain);
};

}  // namespace ninja

#endif  // NINJA_SLAVE_MAIN_H_
