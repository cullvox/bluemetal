#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>

#include "Logger.h"

namespace bl {


Logger::Logger()
{
    auto async_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>("logs/latest.txt");
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_st>();

    dist_sink->add_sink(async_file_sink);
    dist_sink->add_sink(console_sink);

}

Logger::~Logger()
{

}

}