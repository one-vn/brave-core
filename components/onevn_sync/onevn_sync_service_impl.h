/* Copyright 2016 The OneVN Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ONEVN_COMPONENTS_ONEVN_SYNC_ONEVN_SYNC_SERVICE_IMPL_H_
#define ONEVN_COMPONENTS_ONEVN_SYNC_ONEVN_SYNC_SERVICE_IMPL_H_

#include <map>
#include <memory>
#include <string>

#include "base/macros.h"
#include "base/scoped_observer.h"
#include "base/time/time.h"
#include "onevn/components/onevn_sync/onevn_sync_service.h"
#include "onevn/components/onevn_sync/client/onevn_sync_client.h"
#include "components/prefs/pref_change_registrar.h"

FORWARD_DECLARE_TEST(OneVNSyncServiceTest, BookmarkAdded);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, BookmarkDeleted);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, GetSyncWords);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, GetSeed);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnDeleteDevice);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnDeleteDeviceWhenOneDevice);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnDeleteDeviceWhenSelfDeleted);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnResetSync);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, ClientOnGetInitData);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnGetInitData);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnSaveBookmarksBaseOrder);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnSyncPrefsChanged);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnSyncDebug);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnSyncReadyAlreadyWithSync);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnSyncReadyNewToSync);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, OnGetExistingObjects);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, BackgroundSyncStarted);
FORWARD_DECLARE_TEST(OneVNSyncServiceTest, BackgroundSyncStopped);

class OneVNSyncServiceTest;

namespace base {
class RepeatingTimer;
}

namespace onevn_sync {

class SyncDevices;
class Settings;
class BookmarkChangeProcessor;

namespace prefs {
class Prefs;
}  // namespace prefs

using SendDeviceSyncRecordCallback = base::OnceCallback<void(const int,
                                                           const std::string&,
                                                           const std::string&,
                                                           const std::string&)>;

class OneVNSyncServiceImpl
    : public OneVNSyncService,
      public SyncMessageHandler {
 public:
  explicit OneVNSyncServiceImpl(Profile *profile);
  ~OneVNSyncServiceImpl() override;

  // KeyedService overrides
  void Shutdown() override;

  // OneVNSyncService messages from UI
  void OnSetupSyncHaveCode(const std::string& sync_words,
    const std::string& device_name) override;
  void OnSetupSyncNewToSync(const std::string& device_name) override;
  void OnDeleteDevice(const std::string& device_id) override;
  void OnResetSync() override;
  void GetSyncWords() override;
  std::string GetSeed() override;
  void OnSetSyncEnabled(const bool sync_this_device) override;
  void OnSetSyncBookmarks(const bool sync_bookmarks) override;
  void OnSetSyncBrowsingHistory(const bool sync_browsing_history) override;
  void OnSetSyncSavedSiteSettings(const bool sync_saved_site_settings) override;

  void GetSettingsAndDevices(
      const GetSettingsAndDevicesCallback& callback) override;

  bool IsSyncConfigured();
  bool IsSyncInitialized();

  OneVNSyncClient* GetSyncClient() override;

 private:
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, BookmarkAdded);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, BookmarkDeleted);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, GetSyncWords);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, GetSeed);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnDeleteDevice);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnDeleteDeviceWhenOneDevice);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest,
                           OnDeleteDeviceWhenSelfDeleted);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnResetSync);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, ClientOnGetInitData);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnSaveBookmarksBaseOrder);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnGetInitData);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnSyncPrefsChanged);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnSyncDebug);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnSyncReadyAlreadyWithSync);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnSyncReadyNewToSync);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, OnGetExistingObjects);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, BackgroundSyncStarted);
  FRIEND_TEST_ALL_PREFIXES(::OneVNSyncServiceTest, BackgroundSyncStopped);
  friend class ::OneVNSyncServiceTest;

  // SyncMessageHandler overrides
  void BackgroundSyncStarted(bool startup) override;
  void BackgroundSyncStopped(bool shutdown) override;
  void OnSyncDebug(const std::string& message) override;
  void OnSyncSetupError(const std::string& error) override;
  void OnGetInitData(const std::string& sync_version) override;
  void OnSaveInitData(const Uint8Array& seed,
                      const Uint8Array& device_id) override;
  void OnSyncReady() override;
  void OnGetExistingObjects(const std::string& category_name,
    std::unique_ptr<RecordsList> records,
    const base::Time &last_record_time_stamp,
    const bool is_truncated) override;
  void OnResolvedSyncRecords(const std::string& category_name,
    std::unique_ptr<RecordsList> records) override;
  void OnDeletedSyncUser() override;
  void OnDeleteSyncSiteSettings() override;
  void OnSaveBookmarksBaseOrder(const std::string& order) override;
  void OnSaveBookmarkOrder(const std::string& object_id,
                                const std::string& order) override;
  void OnSyncWordsPrepared(const std::string& words) override;

  void OnResolvedHistorySites(const RecordsList &records);
  void OnResolvedPreferences(const RecordsList &records);
  std::unique_ptr<SyncRecordAndExistingList> PrepareResolvedPreferences(
    const RecordsList& records);

  void OnSyncPrefsChanged(const std::string& pref);

  // Other private methods
  void RequestSyncData();
  void FetchSyncRecords(const bool bookmarks, const bool history,
    const bool preferences, int max_records);

  void SendCreateDevice();
  void SendDeviceSyncRecord(
      const int action,
      const std::string& device_name,
      const std::string& device_id,
      const std::string& object_id);

  void SetUpdateDeleteDeviceName_Ext(
    const std::string& action,
    const std::string& deviceName,
    const std::string& deviceId,
    const std::string& objectId);

  void StartLoop();
  void StopLoop();
  void LoopProc();
  void LoopProcThreadAligned();

  void GetExistingHistoryObjects(
    const RecordsList &records,
    const base::Time &last_record_time_stamp,
    const bool is_truncated);

  void NotifyLogMessage(const std::string& message);
  void NotifySyncSetupError(const std::string& error);
  void NotifySyncStateChanged();
  void NotifyHaveSyncWords(const std::string& sync_words);

  void ResetSyncInternal();

  void SetDeviceName(const std::string& name);

  std::unique_ptr<OneVNSyncClient> sync_client_;

  // True when is in active sync chain
  bool sync_configured_ = false;
  // True if we have received SyncReady from JS lib
  bool sync_initialized_ = false;

  // Prevent two sequential calls OnSetupSyncHaveCode or OnSetupSyncNewToSync
  // while being initializing
  bool initializing_ = false;

  bool reseting_ = false;

  std::string sync_words_;

  std::unique_ptr<onevn_sync::Settings> settings_;

  Profile* profile_;
  std::unique_ptr<onevn_sync::prefs::Prefs> sync_prefs_;

  std::unique_ptr<BookmarkChangeProcessor> bookmark_change_processor_;
  // Moment when FETCH_SYNC_RECORDS was sent,
  // will be saved on GET_EXISTING_OBJECTS to be sure request was processed
  base::Time last_time_fetch_sent_;

  std::unique_ptr<base::RepeatingTimer> timer_;

  // send unsynced records in batches
  base::TimeDelta unsynced_send_interval_;

  // Registrar used to monitor the profile prefs.
  PrefChangeRegistrar profile_pref_change_registrar_;

  DISALLOW_COPY_AND_ASSIGN(OneVNSyncServiceImpl);
};

}  // namespace onevn_sync

#endif  // ONEVN_COMPONENTS_ONEVN_SYNC_ONEVN_SYNC_SERVICE_IMPL_H_