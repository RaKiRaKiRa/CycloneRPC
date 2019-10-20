/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-20 20:21
 * Last modified : 2019-10-20 20:21
 * Filename      : 
 * Description   : 
 **********************************************************/

#include "ProcedureNotify.h"

bool ProcedureNotify::checkParam(const json::Value& request) const
{
//参数分两种情况
//索引数组(TYPE_ARRAY)存储参数  {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 3}
//关联数组(TYPE_OBJECT)存储参数 {"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3}
    json::Value::ConstMemberIterator it = request.findMember("params");
    if(it == request.memberEnd())
    {
        return params_.empty();
    }

    json::Value params = it->value;
    //FIXME : [ || params.getSize() == 0 ] ?
    if(params.getSize() != params_.size())
    {
        return false;
    }

    switch(params.getType())
    {
        //[42, 23]
        case json::TYPE_ARRAY:
            for(size_t i = 0; i < params_.size(); ++i)
            {
                if(params[i].getType() != params_[i].paramType)
                {
                    return false;
                }
            }
            break;
        //{"subtrahend": 23, "minuend": 42}
        case json::TYPE_OBJECT:
            for(size_t i = 0; i < params_.size(); ++i)
            {
                json::Value::MemberIterator iter = params.findMember(params_[i].paramName);
                if(iter == params.memberEnd() || params_[i].paramType != iter->value.getType())
                {
                    return false;
                }
            }
            break;
        default:
            return false;
            break;
    }
    return true;
}