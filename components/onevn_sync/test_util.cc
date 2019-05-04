/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "onevn/components/onevn_sync/test_util.h"

#include "base/files/file_util.h"
#include "base/strings/utf_string_conversions.h"
#include "onevn/components/onevn_sync/onevn_sync_service_factory.h"
#include "onevn/components/onevn_sync/client/bookmark_node.h"
#include "onevn/components/onevn_sync/tools.h"
#include "onevn/components/onevn_sync/values_conv.h"
#include "chrome/browser/prefs/browser_prefs.h"
#include "chrome/test/base/testing_profile.h"
#include "chrome/test/base/testing_profile_manager.h"
#include "components/bookmarks/browser/bookmark_model.h"
#include "components/bookmarks/test/test_bookmark_client.h"
#include "components/sync_preferences/pref_service_mock_factory.h"
#include "components/sync_preferences/testing_pref_service_syncable.h"

namespace {

using namespace bookmarks;

void AddPermanentNode(BookmarkPermanentNodeList* extra_nodes, int64_t id,
      const std::string& title) {
  auto node = std::make_unique<onevn_sync::OneVNBookmarkPermanentNode>(id);
  node->set_type(bookmarks::BookmarkNode::FOLDER);
  node->set_visible(false);
  node->SetTitle(base::UTF8ToUTF16(title));
  extra_nodes->push_back(std::move(node));
}

}  // namespace

namespace onevn_sync {

MockOneVNSyncClient::MockOneVNSyncClient() {}

MockOneVNSyncClient::~MockOneVNSyncClient() {}

std::unique_ptr<Profile> CreateOneVNSyncProfile(const base::FilePath& path) {
  OneVNSyncServiceFactory::GetInstance();

  sync_preferences::PrefServiceMockFactory factory;
  auto registry = base::MakeRefCounted<user_prefs::PrefRegistrySyncable>();
  std::unique_ptr<sync_preferences::PrefServiceSyncable> prefs(
      factory.CreateSyncable(registry.get()));
  RegisterUserProfilePrefs(registry.get());

  TestingProfile::Builder profile_builder;
  profile_builder.SetPrefService(std::move(prefs));
  profile_builder.SetPath(path);
  return profile_builder.Build();
}

std::unique_ptr<KeyedService> BuildFakeBookmarkModelForTests(
    content::BrowserContext* context) {
  // Don't need context, unless we have more than one profile
  using namespace bookmarks;
  std::unique_ptr<TestBookmarkClient> client(new TestBookmarkClient());
  BookmarkPermanentNodeList extra_nodes;

  // These hard-coded titles cannot be changed
  AddPermanentNode(&extra_nodes, 0xDE1E7ED40DE, "Deleted Bookmarks");
  AddPermanentNode(&extra_nodes, 0x9E7D17640DE, "Pending Bookmarks");

  client->SetExtraNodesToLoad(std::move(extra_nodes));
  std::unique_ptr<BookmarkModel> model(
      TestBookmarkClient::CreateModelWithClient(std::move(client)));
  return model;
}

SyncRecordPtr SimpleBookmarkSyncRecord(
    const int action,
    const std::string& object_id,
    const std::string& location,
    const std::string& title,
    const std::string& order,
    const std::string& parent_object_id) {
  auto record = std::make_unique<onevn_sync::jslib::SyncRecord>();
  record->action = ConvertEnum<onevn_sync::jslib::SyncRecord::Action>(action,
    onevn_sync::jslib::SyncRecord::Action::A_MIN,
    onevn_sync::jslib::SyncRecord::Action::A_MAX,
    onevn_sync::jslib::SyncRecord::Action::A_INVALID);

  record->deviceId = "3";
  record->objectId = object_id.empty() ? tools::GenerateObjectId() : object_id;
  record->objectData = "bookmark";

  record->syncTimestamp = base::Time::Now();

  auto bookmark = std::make_unique<onevn_sync::jslib::Bookmark>();

  bookmark->isFolder = false;
  // empty parentFolderObjectId means child of some permanent node
  bookmark->parentFolderObjectId = parent_object_id;
  bookmark->hideInToolbar = true;
  bookmark->order = order;

  bookmark->site.location = location;
  bookmark->site.title = title;

  record->SetBookmark(std::move(bookmark));

  return record;
}

SyncRecordPtr SimpleFolderSyncRecord(
    const int action,
    const std::string& title,
    const std::string& order,
    const std::string& parent_object_id,
    const bool hide_in_toolbar,
    const std::string& custom_title) {
  auto record = std::make_unique<onevn_sync::jslib::SyncRecord>();
  record->action = ConvertEnum<onevn_sync::jslib::SyncRecord::Action>(action,
    onevn_sync::jslib::SyncRecord::Action::A_MIN,
    onevn_sync::jslib::SyncRecord::Action::A_MAX,
    onevn_sync::jslib::SyncRecord::Action::A_INVALID);

  record->deviceId = "3";
  record->objectId = tools::GenerateObjectId();
  record->objectData = "bookmark";

  record->syncTimestamp = base::Time::Now();

  auto bookmark = std::make_unique<onevn_sync::jslib::Bookmark>();

  bookmark->isFolder = true;
  bookmark->parentFolderObjectId = parent_object_id;
  bookmark->hideInToolbar = hide_in_toolbar;
  bookmark->order = order;

  bookmark->site.title = title;
  bookmark->site.customTitle = custom_title;

  record->SetBookmark(std::move(bookmark));

  return record;
}

SyncRecordPtr SimpleDeviceRecord(
    const int action,
    const std::string& device_id,
    const std::string& name) {
  auto record = std::make_unique<onevn_sync::jslib::SyncRecord>();
  record->action = ConvertEnum<onevn_sync::jslib::SyncRecord::Action>(action,
    onevn_sync::jslib::SyncRecord::Action::A_MIN,
    onevn_sync::jslib::SyncRecord::Action::A_MAX,
    onevn_sync::jslib::SyncRecord::Action::A_INVALID);
  record->deviceId = device_id;
  record->objectId = tools::GenerateObjectId();
  record->objectData = "device";
  record->syncTimestamp = base::Time::Now();

  auto device = std::make_unique<onevn_sync::jslib::Device>();
  device->name = name;
  record->SetDevice(std::move(device));

  return record;
}

}  // namespace