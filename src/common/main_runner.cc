// Copyright (c) 2015 Chaobin Zhang. All rights reserved.
// Use of this source code is governed by the BSD license that can be
// found in the LICENSE file.

#include "common/main_runner.h"

#include <string>

#include "base/command_line.h"
#include "base/run_loop.h"
#include "base/strings/string_number_conversions.h"
#include "master/master_main_runner.h"
#include "rpc/rpc_options.h"
#include "slave/slave_main_runner.h"
#include "thread/ninja_thread_impl.h"

namespace {
const int kMinPort = 1024;
const int kMaxPort = 65535;
}  // namespace

namespace common {

MainRunner::~MainRunner() {
  rpc_thread_->Stop();
}

bool MainRunner::InitFromManifest(const std::string& input_file,
                                  std::string* error) {
  BuildConfig config;
  ninja_builder_.reset(new ninja::NinjaBuilder(config));
  return ninja_builder_->InitFromManifest(input_file, error);
}

void MainRunner::CreateThreads() {
  main_thread_.reset(
      new NinjaThreadImpl(NinjaThread::MAIN, base::MessageLoop::current()));
  rpc_thread_.reset(new NinjaThreadImpl(NinjaThread::RPC));
}

void MainRunner::Run() {
  base::Thread::Options options(base::MessageLoop::TYPE_IO, 0);
  rpc_thread_->StartWithOptions(options);
  base::RunLoop run_loop;
  run_loop.Run();
}

// static
MainRunner* MainRunner::Create() {
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();

  uint32 port = rpc::kDefaultPort;
  if (command_line->HasSwitch(switches::kPort)) {
    std::string port_str = command_line->GetSwitchValueASCII(switches::kPort);
    DCHECK(!port_str.empty());
    base::StringToUint(port_str, &port);
    CHECK(port >= kMinPort && port <= kMaxPort)
        << "Port should be in range [" << kMinPort << ", " << kMaxPort << "].";
  }

  // Setup whether rpc thread running as master or slave.
  if (command_line->HasSwitch(switches::kMaster)) {
    LOG(INFO) << "Running as slave.";
    std::string master = command_line->GetSwitchValueASCII(switches::kMaster);
    DCHECK(!master.empty());
    return new SlaveMainRunner(master, port);
  } else {
    LOG(INFO) << "Running as master.";
    std::string bind_ip = rpc::kDefaultBindIP;
    if (command_line->HasSwitch(switches::kBindIP)) {
      bind_ip = command_line->GetSwitchValueASCII(switches::kBindIP);
      DCHECK(!bind_ip.empty());
    }
    return new MasterMainRunner(bind_ip, port);
  }
}

}  // namespace common
