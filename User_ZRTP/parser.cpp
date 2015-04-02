#include "parser.h"

Arguments Parser::getArguments(int argc, char *argv[])
{
    Arguments args;

    if(cmdOptionExists(argv, argv+argc, "--user"))
    {
        args.role = getCmdOption(argv, argv + argc, "--user")[0];
    }
    else
    {
        args.role = 1;
    }

    if(cmdOptionExists(argv, argv+argc, "--listen-ip"))
    {
        args.receiveIp = getCmdOption(argv, argv + argc, "--listen-ip");
    }
    else
    {
        args.receiveIp = NULL;
    }

    if(cmdOptionExists(argv, argv+argc, "--listen-port"))
    {
        args.receivePort = getCmdOption(argv, argv + argc, "--listen-port");
    }
    else
    {
        args.receivePort = NULL;
    }

    if(cmdOptionExists(argv, argv+argc, "--remote-ip"))
    {
        args.sendIp = getCmdOption(argv, argv + argc, "--remote-ip");
    }
    else
    {
        args.sendIp = NULL;
    }

    if(cmdOptionExists(argv, argv+argc, "--remote-port"))
    {
        args.sendPort = getCmdOption(argv, argv + argc, "--remote-port");
    }
    else
    {
        args.sendPort = NULL;
    }

    if(cmdOptionExists(argv, argv+argc, "--packet-delay"))
    {
        args.packetDelay = getCmdOption(argv, argv + argc, "--packet-delay");
    }
    else
    {
        args.packetDelay = 0;
    }

    if(cmdOptionExists(argv, argv+argc, "--packet-loss"))
    {
        args.packetLoss = getCmdOption(argv, argv + argc, "--packet-loss");
    }
    else
    {
        args.packetLoss = 0;
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
