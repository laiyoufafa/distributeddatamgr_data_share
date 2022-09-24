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

#ifndef DATASHARESERVICE_DATA_SHARE_MANAGER_IMPL_H
#define DATASHARESERVICE_DATA_SHARE_MANAGER_IMPL_H

#include <map>
#include <memory>
#include <mutex>

#include "ikvstore_data_service.h"
#include "data_share_manager.h"
#include "data_share_types.h"
#include "idata_share_service.h"
#include "iremote_object.h"
#include "refbase.h"

namespace OHOS::DistributedKv {
class KvStoreDataServiceProxy;
}

namespace OHOS::DataShare {
class DataShareService;
class DataShareServiceProxy;
class DataShareManagerImpl {
public:
    static DataShareManagerImpl &GetInstance();
    std::shared_ptr<IDataShareService> GetDataShareService();


private:
    DataShareManagerImpl();
    ~DataShareManagerImpl();
    sptr<IDataShareService> GetDataShareServiceProxy();
    void ResetServiceHandle();
    static sptr<DistributedKv::IKvStoreDataService> GetDistributedDataManager();

    std::mutex mutex_;
    sptr<DistributedKv::IKvStoreDataService> dataMgrService_;
    std::shared_ptr<IDataShareService> dataShareService_;
    std::string bundleName_;
    static constexpr int GET_SA_RETRY_TIMES = 3;
    static constexpr int RETRY_INTERVAL = 1;
    static constexpr int WAIT_TIME = 2;
};
} // namespace OHOS::DataShare
#endif
