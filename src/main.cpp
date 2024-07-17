#include <spdlog/sinks/basic_file_sink.h>
#include "hooks.h"

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");

    auto pluginName = Version::NAME;
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

    spdlog::set_default_logger(std::move(loggerPtr));
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);
#endif

    //Pattern
    spdlog::set_pattern("%v");
}

void MessageHandler(SKSE::MessagingInterface::Message* a_message) {
    switch (a_message->type) {
    case SKSE::MessagingInterface::kDataLoaded:
        Hooks::Get_Description_Hook::Install();
        break;
    default:
        break;
    }
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v;
    v.PluginVersion({ Version::MAJOR, Version::MINOR, Version::PATCH });
    v.PluginName(Version::NAME);
    v.AuthorName(Version::PROJECT_AUTHOR);
    v.UsesAddressLibrary();
    v.UsesUpdatedStructs();
    v.CompatibleVersions({ SKSE::RUNTIME_LATEST });
    return v;
    }();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse) {
    SetupLog();
    _loggerInfo("Starting up {}.", Version::NAME);
    _loggerInfo("Plugin Version: {}.", Version::VERSION);
    _loggerInfo("-------------------------------------------------------------------------------------");
    SKSE::Init(a_skse);

    auto messaging = SKSE::GetMessagingInterface();
    messaging->RegisterListener(MessageHandler);
    return true;
}