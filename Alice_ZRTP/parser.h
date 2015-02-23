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
}Arguments;

class Parser
{
public:
    Parser();

    static Arguments getArguments(int argc, char* argv[]);

private:
    static char* getCmdOption(char **begin, char **end, const std::string &option);
    static bool cmdOptionExists(char **begin, char **end, const std::string &option);
};

#endif // PARSER_H
