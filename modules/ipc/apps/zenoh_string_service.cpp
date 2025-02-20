//=================================================================================================
// Copyright (C) 2023-2024 HEPHAESTUS Contributors
//=================================================================================================

#include <fmt/core.h>

#include "hephaestus/ipc/program_options.h"
#include "hephaestus/ipc/zenoh/service.h"
#include "hephaestus/ipc/zenoh/session.h"
#include "hephaestus/ipc/zenoh/utils.h"
#include "hephaestus/utils/stack_trace.h"

auto main(int argc, const char* argv[]) -> int {
  heph::utils::StackTrace stack_trace;

  try {
    auto desc =
        heph::cli::ProgramDescription("Simple service for std::string types for both Request and Reply. "
                                      "Don't use for services with different types.");
    heph::ipc::appendIPCProgramOption(desc);
    desc.defineOption<std::string>("value", 'v', "the value to pass the query", "");
    const auto args = std::move(desc).parse(argc, argv);
    const auto value = args.getOption<std::string>("value");

    auto [config, topic_config] = heph::ipc::parseIPCProgramOptions(args);
    auto session = heph::ipc::zenoh::createSession(std::move(config));
    LOG(INFO) << fmt::format("Opening session: {}",
                             heph::ipc::zenoh::toString(session->zenoh_session.info_zid()));

    auto results = heph::ipc::zenoh::callService<std::string, std::string>(*session, topic_config, value);

    std::for_each(results.begin(), results.end(),
                  [](const auto& res) { fmt::println(">> Received ('{}': '{}')", res.topic, res.value); });

    return EXIT_SUCCESS;
  } catch (const std::exception& ex) {
    std::ignore = std::fputs(ex.what(), stderr);
    return EXIT_FAILURE;
  }
}
