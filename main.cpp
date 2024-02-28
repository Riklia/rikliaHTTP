#include "TCPServer.h"
#include "ServerConfiguration.h"
#include "GlobalConfigurations.h"
#include "HTTPResponse.h"
#include "HTTPServer.h"
#include <string>
#include <thread>

int main()
{
	std::string configDir = "configs";
	auto globalConfig = GlobalConfigurations::instance();
	globalConfig->setConfigurationDir(configDir);
	const std::string configFilePath = configDir + "/riklia.conf";
	auto serversConfig = ServerConfig::parseConfigurationFile(configFilePath);

	std::map<std::string, std::thread> serverThreads;
	for (const auto& configPair : serversConfig) {
		auto server = HTTPServer(configPair.second);
		auto threadFunc = [server = std::move(server)]() mutable {
			server.start();
		};

		serverThreads[configPair.first] = std::thread(std::move(threadFunc));
	}

	for (auto& pair : serverThreads) {
		pair.second.join();
	}
}