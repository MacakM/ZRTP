#include "parser.h"

Parser::Parser()
{
}

Arguments Parser::getArguments(int argc, char *argv[])
{
    Arguments args;

    if(cmdOptionExists(argv, argv+argc, "--user"))
    {
        args.role = getCmdOption(argv, argv + argc, "--user")[0];
    }

    if(cmdOptionExists(argv, argv+argc, "--listen-ip"))
    {
        args.receiveIp = getCmdOption(argv, argv + argc, "--listen-ip");
    }

    if(cmdOptionExists(argv, argv+argc, "--listen-port"))
    {
        args.receivePort = getCmdOption(argv, argv + argc, "--listen-port");
    }

    if(cmdOptionExists(argv, argv+argc, "--remote-ip"))
    {
        args.sendIp = getCmdOption(argv, argv + argc, "--remote-ip");
    }

    if(cmdOptionExists(argv, argv+argc, "--remote-port"))
    {
        args.sendPort = getCmdOption(argv, argv + argc, "--remote-port");
    }

    return args;
}

char *Parser::getCmdOption(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return NULL;
}

bool Parser::cmdOptionExists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}
