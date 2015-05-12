#ifndef PARSER_H
#define PARSER_H

#include <algorithm>
#include <iostream>

typedef struct
{
    char role;
    char *receiveIp;
    char *receivePort;
    char *sendIp;
    char *sendPort;
    char *packetDelay;
    char *packetLoss;
    char *testing;
}Arguments;

/**
 * Class that loads information from command line and returns loaded arguments.
 */
class Parser
{
public:
    /**
     * Constructor of Parser.
     */
    Parser() {}

    static Arguments getArguments(int argc, char* argv[]);

private:
    /**
     * Returns chosen command line option.
     *
     * @param begin     beginning of command line arguments
     * @param end       ending of command line arguments
     * @param option    searched option
     *
     * @return          value of searched option
     */
    static char* getCmdOption(char **begin, char **end, const std::string &option);

    /**
     * Returns whether the chosen command line option exists.
     *
     * @param begin     beginning of command line arguments
     * @param end       ending of command line arguments
     * @param option    searched option
     *
     * @return          true = searched option exists, false = otherwise
     */
    static bool cmdOptionExists(char **begin, char **end, const std::string &option);
};

#endif // PARSER_H
