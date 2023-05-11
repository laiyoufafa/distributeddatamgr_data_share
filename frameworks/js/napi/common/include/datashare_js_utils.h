/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DATASHARE_JSUTILS_H
#define DATASHARE_JSUTILS_H

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "datashare_template.h"
#include "datashare_value_object.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#define NAPI_ASSERT_ERRCODE(env, assertion, error)                                                          \
do {                                                                                                        \
    if (!(assertion)) {                                                                                     \
        napi_throw_error((env), std::to_string((error)->GetCode()).c_str(), (error)->GetMessage().c_str()); \
        return;                                                                                             \
    }                                                                                                       \
} while (0)

#define NAPI_ASSERT_CALL_ERRCODE(env, assertion, call, retVal) \
    do {                                                       \
        if (!(assertion)) {                                    \
            call;                                              \
            return retVal;                                     \
        }                                                      \
    } while (0)

#define NAPI_ASSERT_CALL_ERRCODE_SYNC(env, assertion, call, error, retVal)                                      \
    do {                                                                                                        \
        if (!(assertion)) {                                                                                     \
            call;                                                                                               \
            napi_throw_error((env), std::to_string((error)->GetCode()).c_str(), (error)->GetMessage().c_str()); \
            return retVal;                                                                                      \
        }                                                                                                       \
    } while (0)

namespace OHOS {
namespace DataShare {
class DataShareJSUtils final {
public:
    static constexpr int32_t DEFAULT_BUF_SIZE = 1024;
    static constexpr int32_t ASYNC_RST_SIZE = 2;
    static constexpr int32_t SYNC_RESULT_ELEMNT_NUM = 2;

    static std::string Convert2String(napi_env env, napi_value jsStr, size_t max = DEFAULT_BUF_SIZE);
    static std::vector<std::string> Convert2StrVector(napi_env env, napi_value value, size_t strMax);
    static std::vector<uint8_t> Convert2U8Vector(napi_env env, napi_value jsValue);
    static std::string ConvertAny2String(napi_env env, const napi_value jsValue);
    static std::string UnwrapStringFromJS(napi_env env, napi_value param, const std::string &defaultValue = "");
    static DataShareValueObject Convert2ValueObject(napi_env env, napi_value value, bool &status);
    static Template Convert2Template(napi_env env, napi_value value);
    static TemplateId Convert2TemplateId(napi_env env, napi_value value);
    static Data Convert2PublishedData(napi_env env, napi_value value);
    static sptr<Ashmem> Convert2Ashmem(napi_env env, napi_value value);

    static napi_value Convert2JSValue(napi_env env, const std::monostate &value = {});
    static napi_value Convert2JSValue(napi_env env, const std::vector<std::string> &value);
    static napi_value Convert2JSValue(napi_env env, const std::string &value);
    static napi_value Convert2JSValue(napi_env env, const std::vector<uint8_t> &value);
    static napi_value Convert2JSValue(napi_env env, int32_t value);
    static napi_value Convert2JSValue(napi_env env, int64_t value);
    static napi_value Convert2JSValue(napi_env env, double value);
    static napi_value Convert2JSValue(napi_env env, bool value);
    static napi_value Convert2JSValue(napi_env env, const std::map<std::string, int> &value);
    template<class... Types>
    static napi_value Convert2JSValue(napi_env env, const std::variant<Types...> &value);
    static napi_value Convert2JSValue(napi_env env, const TemplateId &templateId);
    static napi_value Convert2JSValue(napi_env env, const RdbChangeNode &changeNode);
    static napi_value Convert2JSValue(napi_env env, sptr<Ashmem> &ashmem);
    static napi_value Convert2JSValue(napi_env env, PublishedDataItem &publishedDataItem);
    static napi_value Convert2JSValue(napi_env env, std::vector<PublishedDataItem> &publishedDataItems);
    static napi_value Convert2JSValue(napi_env env, PublishedDataChangeNode &changeNode);
    static napi_value Convert2JSValue(napi_env env, const OperationResult &results);
    static napi_value Convert2JSValue(napi_env env, const std::vector<OperationResult> &results);
    static std::vector<uint8_t> ConvertU8Vector(napi_env env, napi_value jsValue);

    static bool Equals(napi_env env, napi_value value, napi_ref copy);

    static bool UnwrapPublishedDataItem(napi_env env, napi_value value, PublishedDataItem &publishedDataItem);
    static bool UnwrapPublishedDataItemVector(napi_env env, napi_value value,
        std::vector<PublishedDataItem> &publishedDataItems);
    static bool UnwrapTemplatePredicates(napi_env env, napi_value jsPredicates,
        std::vector<PredicateTemplateNode> &predicates);
    static bool UnwrapStringByPropertyName(napi_env env, napi_value jsObject, const char *propertyName,
        std::string &value);
    static bool IsArrayForNapiValue(napi_env env, napi_value param, uint32_t &arraySize);
private:
    template<typename _VTp>
    static napi_value ReadVariant(napi_env env, uint32_t step, uint32_t index, const _VTp &output)
    {
        (void)step;
        (void)index;
        (void)output;
        return Convert2JSValue(env);
    }

    template<typename _VTp, typename _First, typename ..._Rest>
    static napi_value ReadVariant(napi_env env, uint32_t step, uint32_t index, const _VTp &value)
    {
        if (step == index) {
            auto *realValue = std::get_if<_First>(&value);
            if (realValue == nullptr) {
                return nullptr;
            }
            return Convert2JSValue(env, *realValue);
        }
        return ReadVariant<_VTp, _Rest...>(env, step + 1, index, value);
    }
};

template<class... Types>
napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const std::variant<Types...> &value)
{
    return ReadVariant<decltype(value), Types...>(env, 0, value.index(), value);
}
} // namespace DataShare
} // namespace OHOS

#endif // DATASHARE_JSUTILS_H