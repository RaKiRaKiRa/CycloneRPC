/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-26 16:50
 * Last modified : 2019-11-06 03:07
 * Filename      : StubException.h
 * Description   : 
 **********************************************************/
#ifndef CYCLONERPC_STUBEXCEPTION_H
#define CYCLONERPC_STUBEXCEPTION_H
#include <exception>

class StubException: std::exception
{
public:
    explicit StubException(const char* msg):
        msg_(msg)
    {}

    const char* what() const noexcept
    {
        return msg_;
    }
private:
    const char* msg_;
};

inline void stubExpect(bool result, const char* errMsg)
{
    if(!result)
    {
        throw StubException(errMsg);
    }
}

#endif