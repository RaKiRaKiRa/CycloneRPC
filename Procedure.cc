/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-18 20:33
 * Last modified : 2019-10-18 21:08
 * Filename      : Procedure.cc
 * Description   : 
 **********************************************************/

#include "Procedure.h"

template<typename ProcedureType>
template<typename... ParamNameAndTypes>
void Procedure<ProcedureType>::initProcedure(const std::string& name, const json::ValueType& type, ParamNameAndTypes... nameAndTypes)
{
    params_.emplace_back(name, type);
    initProcedure(nameAndTypes...);
}

template<typename ProcedureType>
void Procedure<ProcedureType>::initProcedure(const std::string& name, const json::ValueType& type)
{
    params_.emplace_back(name, type);
}