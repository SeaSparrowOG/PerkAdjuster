#include <hooks.h>
#include <iniParser.h>
#include <papyrusAPI.h>

#include <spdlog/sinks/basic_file_sink.h>

namespace logger = SKSE::log;

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");

    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);

    //Pattern
    spdlog::set_pattern("[%l]: %v");
}

void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
    switch (a_message->type) {
    case SKSE::MessagingInterface::kDataLoaded: {
        
        auto manager = Hooks::DescriptionManager::GetSingleton();

        if (!manager) {

            SKSE::log::critical("Failed to get manager singleton, plugin will NOT be loaded.");
            break;
        }
        manager->InstallHook();

        //Note, this doesn't load anything. It applies changes based on INI configurations.
        INIParser::loadINIs();
        SKSE::log::info("Finished parsing available INIs.");

        SKSE::GetPapyrusInterface()->Register(PapyrusAPI::Bind);
        SKSE::log::info("Finished binding Papyrus functions.");

        SKSE::log::info("Finished pre-game startup.");
        break;
    }

    default:
        break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SetupLog();

    SKSE::Init(skse);


    auto messaging = SKSE::GetMessagingInterface();
    messaging->RegisterListener(MessageHandler);

    return true;
}