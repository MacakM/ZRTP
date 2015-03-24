#ifndef USERINFO_H
#define USERINFO_H

#include "Integers.h"
#include <vector>
#include <string>

class UserInfo
{
    typedef std::vector<std::string> supported;

public:
    UserInfo() {}

    supported versions;
    supported hashTypes;
    supported cipherTypes;
    supported authTagTypes;
    supported keyAgreementTypes;
    supported sasTypes;
};

#endif // USERINFO_H
