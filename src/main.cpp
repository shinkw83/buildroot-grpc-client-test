#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "device_control.grpc.pb.h"

using ams::device::grpc::DeviceControlService;
using ams::device::grpc::DeviceMessage;
using ams::device::grpc::InitSessionNotify;
using ams::device::grpc::ServerMessage;
using ams::device::grpc::Status;

namespace {
DeviceMessage BuildInitSessionMessage(const std::string& mac_address) {
    DeviceMessage message;
    message.set_message_id("init-session-notify");
    message.set_status(Status::OK);
    InitSessionNotify* payload = message.mutable_init_session_notify();
    payload->set_mac_address(mac_address);
    return message;
}

void PrintServerMessage(const ServerMessage& message) {
    std::cout << "[server] message_id=" << message.message_id() << std::endl;
}
}  // namespace

int main(int argc, char** argv) {
    std::string target = "192.168.0.71:6565";
    std::string mac_address = "b2:a2:2d:c1:be:f3";

    if (argc >= 2) {
        target = argv[1];
    }
    if (argc >= 3) {
        mac_address = argv[2];
    }

    auto channel =
        grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    auto stub = DeviceControlService::NewStub(channel);

    grpc::ClientContext context;
    auto stream = stub->Session(&context);
    if (!stream) {
        std::cerr << "Failed to open session stream to " << target << std::endl;
        return 1;
    }

    DeviceMessage init_message = BuildInitSessionMessage(mac_address);
    if (!stream->Write(init_message)) {
        std::cerr << "Failed to send InitSessionNotify message." << std::endl;
        stream->WritesDone();
        grpc::Status finish_status = stream->Finish();
        return finish_status.ok() ? 0 : 1;
    }

    std::cout << "Sent InitSessionNotify for MAC " << mac_address << " to "
              << target << std::endl;

    //stream->WritesDone();

    while (true) {
      ServerMessage server_message;
      if (!stream->Read(&server_message)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        continue;
      }
      PrintServerMessage(server_message);
      if (server_message.payload_case() == ServerMessage::kGetSystemDateAndTime) {
        DeviceMessage response;
        response.set_message_id(server_message.message_id());
        response.set_status(Status::OK);
        auto payload = response.mutable_get_system_date_and_time();
        payload->set_local_time("2025-12-24 13:45:00");
        payload->set_utc_time("2025-12-24 04:45:00");
        payload->set_time_zone("Asia/Seoul");
        payload->set_ntp_server("time.google.com");
        payload->set_date_time_type(
            ams::device::grpc::DateTimeType::NTP);

        if (!stream->Write(response)) {
          std::cerr << "Failed to send GetSystemDateAndTime response."
                    << std::endl;
          grpc::Status finish_status = stream->Finish();
          return finish_status.ok() ? 0 : 1;
        }
      }
    }

    grpc::Status status = stream->Finish();
    if (!status.ok()) {
        std::cerr << "Session finished with error: " << status.error_message()
                  << std::endl;
        return 1;
    }

    std::cout << "Session finished successfully." << std::endl;
    return 0;
}
