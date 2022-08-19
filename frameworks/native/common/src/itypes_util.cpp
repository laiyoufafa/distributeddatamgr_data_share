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

#include "itypes_util.h"
#include "datashare_log.h"

namespace OHOS::DataShare {
bool ITypesUtil::Marshalling(const DataSharePredicates &predicates, Parcel &parcel)
{
    LOG_DEBUG("Marshalling DataSharePredicates Start");
    const std::list<OperationItem> &operations = predicates.GetOperationList();
    if (!parcel.WriteInt32(operations.size())) {
        LOG_ERROR("predicate write size failed");
        return false;
    }
    for (auto &it : operations) {
        if (!Marshalling(it, parcel)) {
            LOG_ERROR("predicate write OperationItem failed");
            return false;
        }
    }
    if (!parcel.WriteString(predicates.GetWhereClause())) {
        LOG_ERROR("predicate write whereClause failed");
        return false;
    }
    if (!parcel.WriteStringVector(predicates.GetWhereArgs())) {
        LOG_ERROR("predicate write whereArgs failed");
        return false;
    }
    if (!parcel.WriteString(predicates.GetOrder())) {
        LOG_ERROR("predicate write order failed");
        return false;
    }
    if (!parcel.WriteInt64(static_cast<int64_t>(predicates.GetSettingMode()))) {
        LOG_ERROR("predicate write settingMode failed");
        return false;
    }
    return true;
}

bool ITypesUtil::Unmarshalling(Parcel &parcel, DataSharePredicates &predicates)
{
    LOG_DEBUG("Unmarshalling DataSharePredicates Start");
    std::list<OperationItem> operations {};
    std::string whereClause = "";
    std::vector<std::string> whereArgs;
    std::string order = "";
    int64_t mode = INVALID_MODE;
    size_t size = static_cast<size_t>(parcel.ReadInt32());
    if (static_cast<int32_t>(size) < 0) {
        LOG_ERROR("predicate read listSize failed");
        return false;
    }
    if ((size > parcel.GetReadableBytes()) || (operations.max_size() < size)) {
        LOG_ERROR("Read operations failed, size : %{public}zu", size);
        return false;
    }
    operations.clear();
    for (size_t i = 0; i < size; i++) {
        OperationItem listitem {};
        if (!Unmarshalling(parcel, listitem)) {
            LOG_ERROR("operations read OperationItem failed");
            return false;
        }
        operations.push_back(listitem);
    }
    if (!parcel.ReadString(whereClause)) {
        LOG_ERROR("predicate read whereClause failed");
        return false;
    }
    if (!parcel.ReadStringVector(&whereArgs)) {
        LOG_ERROR("predicate read whereArgs failed");
        return false;
    }
    if (!parcel.ReadString(order)) {
        LOG_ERROR("predicate read order failed");
        return false;
    }
    if (!parcel.ReadInt64(mode)) {
        LOG_ERROR("predicate read mode failed");
        return false;
    }
    DataSharePredicates tmpPredicates(operations);
    tmpPredicates.SetWhereClause(whereClause);
    tmpPredicates.SetWhereArgs(whereArgs);
    tmpPredicates.SetOrder(order);
    tmpPredicates.SetSettingMode(static_cast<SettingMode>(mode));
    predicates = tmpPredicates;
    return true;
}

bool ITypesUtil::Marshalling(const DataShareValuesBucket &valuesBucket, Parcel &parcel)
{
    if (!parcel.WriteInt32(valuesBucket.valuesMap.size())) {
        LOG_ERROR("valuesBucket write size failed");
        return false;
    }
    for (auto &it : valuesBucket.valuesMap) {
        if (!parcel.WriteString(it.first)) {
            LOG_ERROR("valuesBucket write first failed");
            return false;
        }
        if (!Marshalling(it.second, parcel)) {
            LOG_ERROR("valuesBucket write second failed");
            return false;
        }
    }
    return true;
}

bool ITypesUtil::Unmarshalling(Parcel &parcel, DataShareValuesBucket &valuesBucket)
{
    int len = parcel.ReadInt32();
    if (len < 0) {
        LOG_ERROR("valuesBucket read mapSize failed");
        return false;
    }
    size_t size = static_cast<size_t>(len);
    if ((size > parcel.GetReadableBytes()) || (valuesBucket.valuesMap.max_size() < size)) {
        LOG_ERROR("Read valuesMap failed, size : %{public}zu", size);
        return false;
    }
    valuesBucket.valuesMap.clear();
    for (size_t i = 0; i < size; i++) {
        std::string key = parcel.ReadString();
        DataShareValueObject value {};
        if (!Unmarshalling(parcel, value)) {
            LOG_ERROR("valuesBucket read value failed");
            return false;
        }
        valuesBucket.valuesMap.insert(std::make_pair(key, value));
    }
    return true;
}

bool ITypesUtil::Marshalling(const OperationItem &operationItem, Parcel &parcel)
{
    if (!parcel.WriteInt64(static_cast<int64_t>(operationItem.operation))) {
        LOG_ERROR("predicate write operation failed");
        return false;
    }
    if (!parcel.WriteInt64(static_cast<int64_t>(operationItem.parameterCount))) {
        LOG_ERROR("predicate write parameterCount failed");
        return false;
    }
    if (!Marshalling(operationItem.para1, parcel)) {
        LOG_ERROR("predicate write para1 failed");
        return false;
    }
    if (!Marshalling(operationItem.para2, parcel)) {
        LOG_ERROR("predicate write para2 failed");
        return false;
    }
    if (!Marshalling(operationItem.para3, parcel)) {
        LOG_ERROR("predicate write para3 failed");
        return false;
    }
    return true;
}

bool ITypesUtil::Unmarshalling(Parcel &parcel, OperationItem &operationItem)
{
    operationItem.operation = static_cast<OperationType>(parcel.ReadInt64());
    if (operationItem.operation < OperationType::INVALID_OPERATION) {
        LOG_ERROR("operationItem read operation failed");
        return false;
    }
    operationItem.parameterCount = static_cast<ParameterCount>(parcel.ReadInt64());
    if (operationItem.parameterCount < ParameterCount::ZERO_COUNT ||
        operationItem.parameterCount >= ParameterCount::INVALID_COUNT) {
        LOG_ERROR("operationItem read parameterCount failed");
        return false;
    }
    if (!Unmarshalling(parcel, operationItem.para1)) {
        LOG_ERROR("operationItem read para1 failed");
        return false;
    }
    if (!Unmarshalling(parcel, operationItem.para2)) {
        LOG_ERROR("operationItem read para2 failed");
        return false;
    }
    if (!Unmarshalling(parcel, operationItem.para3)) {
        LOG_ERROR("operationItem read para3 failed");
        return false;
    }
    return true;
}

bool ITypesUtil::Marshalling(const DataSharePredicatesObject &predicatesObject, Parcel &parcel)
{
    if (!parcel.WriteInt16((int16_t)predicatesObject.GetType())) {
        LOG_ERROR("predicatesObject write type failed");
        return false;
    }
    switch (predicatesObject.GetType()) {
        case DataSharePredicatesObjectType::TYPE_INT: {
            if (!parcel.WriteInt32(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteInt32 failed");
                return false;
            }
            break;
        }
        case DataSharePredicatesObjectType::TYPE_LONG: {
            if (!parcel.WriteInt64(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteInt64 failed");
                return false;
            }
            break;
        }
        case DataSharePredicatesObjectType::TYPE_DOUBLE: {
            if (!parcel.WriteDouble(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteDouble failed");
                return false;
            }
            break;
        }
        case DataSharePredicatesObjectType::TYPE_STRING: {
            if (!parcel.WriteString(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteString failed");
                return false;
            }
            break;
        }
        case DataSharePredicatesObjectType::TYPE_BOOL: {
            if (!parcel.WriteBool(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteBool failed");
                return false;
            }
            break;
        }
        case DataSharePredicatesObjectType::TYPE_INT_VECTOR: {
            if (!parcel.WriteInt32Vector(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteInt32Vector failed");
                return false;
            }
            break;
        }
        case DataSharePredicatesObjectType::TYPE_LONG_VECTOR: {
            if (!parcel.WriteInt64Vector(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteInt64Vector failed");
                return false;
            }
            break;
        }
        case DataSharePredicatesObjectType::TYPE_DOUBLE_VECTOR: {
            if (!parcel.WriteDoubleVector(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteDoubleVector failed");
                return false;
            }
            break;
        }
        case DataSharePredicatesObjectType::TYPE_STRING_VECTOR: {
            if (!parcel.WriteStringVector(predicatesObject)) {
                LOG_ERROR("predicatesObject WriteStringVector failed");
                return false;
            }
            break;
        }
        default:
            break;
    }
    return true;
}

bool ITypesUtil::Unmarshalling(Parcel &parcel, DataSharePredicatesObject &predicatesObject)
{
    int16_t type = parcel.ReadInt16();
    if (type < (int16_t)DataSharePredicatesObjectType::TYPE_NULL) {
        LOG_ERROR("predicatesObject read type failed");
        return false;
    }
    predicatesObject.type = static_cast<DataSharePredicatesObjectType>(type);
    switch (predicatesObject.type) {
        case DataSharePredicatesObjectType::TYPE_INT: {
            predicatesObject.value = parcel.ReadInt32();
            break;
        }
        case DataSharePredicatesObjectType::TYPE_LONG: {
            predicatesObject.value = parcel.ReadInt64();
            break;
        }
        case DataSharePredicatesObjectType::TYPE_DOUBLE: {
            predicatesObject.value = parcel.ReadDouble();
            break;
        }
        case DataSharePredicatesObjectType::TYPE_STRING: {
            predicatesObject.value = parcel.ReadString();
            break;
        }
        case DataSharePredicatesObjectType::TYPE_BOOL: {
            predicatesObject.value = parcel.ReadBool();
            break;
        }
        case DataSharePredicatesObjectType::TYPE_INT_VECTOR: {
            std::vector<int> intval {};
            if (!parcel.ReadInt32Vector(&intval)) {
                LOG_ERROR("predicatesObject ReadInt32Vector value failed");
                return false;
            }
            predicatesObject.value = intval;
            break;
        }
        case DataSharePredicatesObjectType::TYPE_LONG_VECTOR: {
            std::vector<int64_t> int64val {};
            if (!parcel.ReadInt64Vector(&int64val)) {
                LOG_ERROR("predicatesObject ReadInt64Vector value failed");
                return false;
            }
            predicatesObject.value = int64val;
            break;
        }
        case DataSharePredicatesObjectType::TYPE_DOUBLE_VECTOR: {
            std::vector<double> doubleval {};
            if (!parcel.ReadDoubleVector(&doubleval)) {
                LOG_ERROR("predicatesObject ReadDoubleVector value failed");
                return false;
            }
            predicatesObject.value = doubleval;
            break;
        }
        case DataSharePredicatesObjectType::TYPE_STRING_VECTOR: {
            std::vector<std::string> stringval {};
            if (!parcel.ReadStringVector(&stringval)) {
                LOG_ERROR("predicatesObject ReadDoubReadStringVectorleVector value failed");
                return false;
            }
            predicatesObject.value = stringval;
            break;
        }
        default:
            break;
    }
    return true;
}

bool ITypesUtil::Marshalling(const DataShareValueObject &valueObject, Parcel &parcel)
{
    if (!parcel.WriteInt16((int16_t)valueObject.type)) {
        LOG_ERROR("valueObject write type failed");
        return false;
    }
    switch (valueObject.type) {
        case DataShareValueObjectType::TYPE_INT: {
            if (!parcel.WriteInt64(std::get<int64_t>(valueObject.value))) {
                LOG_ERROR("valueObject WriteInt64 failed");
                return false;
            }
            break;
        }
        case DataShareValueObjectType::TYPE_DOUBLE: {
            if (!parcel.WriteDouble(std::get<double>(valueObject.value))) {
                LOG_ERROR("valueObject WriteDouble failed");
                return false;
            }
            break;
        }
        case DataShareValueObjectType::TYPE_STRING: {
            if (!parcel.WriteString(std::get<std::string>(valueObject.value))) {
                LOG_ERROR("valueObject WriteString failed");
                return false;
            }
            break;
        }
        case DataShareValueObjectType::TYPE_BLOB: {
            if (!parcel.WriteUInt8Vector(std::get<std::vector<uint8_t>>(valueObject.value))) {
                LOG_ERROR("valueObject WriteUInt8Vector failed");
                return false;
            }
            break;
        }
        case DataShareValueObjectType::TYPE_BOOL: {
            if (!parcel.WriteBool(std::get<bool>(valueObject.value))) {
                LOG_ERROR("valueObject WriteBool failed");
                return false;
            }
            break;
        }
        default:
            break;
    }
    return true;
}

bool ITypesUtil::Unmarshalling(Parcel &parcel, DataShareValueObject &valueObject)
{
    int16_t type = parcel.ReadInt16();
    if (type < (int16_t)DataShareValueObjectType::TYPE_NULL) {
        LOG_ERROR("valueObject read type failed");
        return false;
    }
    valueObject.type = static_cast<DataShareValueObjectType>(type);
    switch (valueObject.type) {
        case DataShareValueObjectType::TYPE_INT: {
            valueObject.value = parcel.ReadInt64();
            break;
        }
        case DataShareValueObjectType::TYPE_DOUBLE: {
            valueObject.value = parcel.ReadDouble();
            break;
        }
        case DataShareValueObjectType::TYPE_STRING: {
            valueObject.value = parcel.ReadString();
            break;
        }
        case DataShareValueObjectType::TYPE_BLOB: {
            std::vector<uint8_t> val;
            if (!parcel.ReadUInt8Vector(&val)) {
                LOG_ERROR("valueObject ReadUInt8Vector value failed");
                return false;
            }
            valueObject.value = val;
            break;
        }
        case DataShareValueObjectType::TYPE_BOOL: {
            valueObject.value = parcel.ReadBool();
            break;
        }
        default:
            break;
    }
    return true;
}
} // namespace OHOS::DistributedKv
