#include "TestObj.h"
#include <string> 
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#include <windows.h>
using namespace std;

TestObj::TestObj()
{
    Name = GetNewGUID().c_str();
    Value = GetNewGUID().c_str();
}

string TestObj::GetNewGUID()
{
    UUID uuid = { 0 };
    string guid;

    // Create uuid or load from a string by UuidFromString() function
    UuidCreate(&uuid);

    // If you want to convert uuid to string, use UuidToString() function
    RPC_CSTR szUuid = NULL;
    if (UuidToStringA(&uuid, &szUuid) == RPC_S_OK)
    {
        guid = (char*)szUuid;
        RpcStringFreeA(&szUuid);
    }

    return guid;
}