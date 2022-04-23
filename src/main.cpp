#include <server.h>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    try
    {
        po::options_description generalOptions{"General"};
        generalOptions.add_options()
        ("help,h", "Help screen")
        ("config", po::value<std::string>()->default_value("config.txt"),"Config file");

        po::options_description fileOptions{"Config File"};
        fileOptions.add_options()
        ("port", po::value<int>()->default_value(9999), "Port")
        ("rooms", po::value<std::string>()->default_value("general"), "List rooms");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, generalOptions), vm);
        if (vm.count("config"))
        {
            std::ifstream in{vm["config"].as<std::string>().c_str()};
            if (in)
                store(parse_config_file(in, fileOptions), vm);
            else
                std::cout << "config file doesn't exist\n";
        }
        notify(vm);

        if (vm.count("help")){
            std::cout << generalOptions << '\n';
            std::cout << fileOptions << std::endl;
        }

        int port;
        std::string rooms;

        if (vm.count("port"))
            port = vm["port"].as<int>();
        else
            port = 9999;
        if (vm.count("rooms"))
            rooms = vm["rooms"].as<std::string>();
        else
            rooms = "general";


        io::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), port);
        Server server(io_context, endpoint, rooms);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }

    return 0;
}