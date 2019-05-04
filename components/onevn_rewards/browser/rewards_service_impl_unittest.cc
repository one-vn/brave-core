/* Copyright (c) 2019 The OneVN Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <map>

#include "base/files/scoped_temp_dir.h"
#include "onevn/components/onevn_rewards/browser/wallet_properties.h"
#include "onevn/components/onevn_rewards/browser/rewards_service_factory.h"
#include "onevn/components/onevn_rewards/browser/rewards_service_impl.h"
#include "onevn/components/onevn_rewards/browser/rewards_service_observer.h"
#include "onevn/components/onevn_rewards/browser/test_util.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/test/test_browser_thread_bundle.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- onevn_unit_tests --filter=RewardsServiceTest.*

namespace onevn_rewards {

using ::testing::_;

class MockRewardsServiceObserver : public RewardsServiceObserver {
 public:
  MockRewardsServiceObserver() {}
  MOCK_METHOD2(OnWalletInitialized, void(RewardsService*, uint32_t));
  MOCK_METHOD3(OnWalletProperties, void(RewardsService*,
      int,
      std::unique_ptr<onevn_rewards::WalletProperties>));
  MOCK_METHOD3(OnGrant,
      void(RewardsService*, unsigned int, onevn_rewards::Grant));
  MOCK_METHOD3(OnGrantCaptcha,
      void(RewardsService*, std::string, std::string));
  MOCK_METHOD4(OnRecoverWallet, void(RewardsService*,
                                     unsigned int,
                                     double,
                                     std::vector<onevn_rewards::Grant>));
  MOCK_METHOD3(OnGrantFinish,
      void(RewardsService*, unsigned int, onevn_rewards::Grant));
  MOCK_METHOD1(OnContentSiteUpdated, void(RewardsService*));
  MOCK_METHOD3(OnExcludedSitesChanged, void(RewardsService*,
                                            std::string,
                                            bool));
  MOCK_METHOD5(OnReconcileComplete, void(RewardsService*,
                                         unsigned int,
                                         const std::string&,
                                         const std::string&,
                                         const std::string&));
  MOCK_METHOD2(OnGetRecurringTips,
      void(RewardsService*, const onevn_rewards::ContentSiteList&));
  MOCK_METHOD2(OnPublisherBanner,
      void(RewardsService*, const onevn_rewards::PublisherBanner));
  MOCK_METHOD4(OnPanelPublisherInfo,
      void(RewardsService*, int, ledger::PublisherInfo*, uint64_t));
};

class RewardsServiceTest : public testing::Test {
 public:
  RewardsServiceTest() {}
  ~RewardsServiceTest() override {}

 protected:
  void SetUp() override {
    ASSERT_TRUE(temp_dir_.CreateUniqueTempDir());
    profile_ = CreateOneVNRewardsProfile(temp_dir_.GetPath());
    ASSERT_TRUE(profile_.get() != NULL);
    rewards_service_ = static_cast<RewardsServiceImpl*>(
        RewardsServiceFactory::GetForProfile(profile()));
    ASSERT_TRUE(RewardsServiceFactory::GetInstance() != NULL);
    ASSERT_TRUE(rewards_service() != NULL);
    observer_.reset(new MockRewardsServiceObserver);
    rewards_service_->AddObserver(observer_.get());
  }

  void TearDown() override {
    rewards_service_->RemoveObserver(observer_.get());
    profile_.reset();
  }

  Profile* profile() { return profile_.get(); }
  RewardsServiceImpl* rewards_service() { return rewards_service_; }
  MockRewardsServiceObserver* observer() { return observer_.get(); }

 private:
  // Need this as a very first member to run tests in UI thread
  // When this is set, class should not install any other MessageLoops, like
  // base::test::ScopedTaskEnvironment
  content::TestBrowserThreadBundle thread_bundle_;
  std::unique_ptr<Profile> profile_;
  RewardsServiceImpl* rewards_service_;
  std::unique_ptr<MockRewardsServiceObserver> observer_;
  base::ScopedTempDir temp_dir_;
};

TEST_F(RewardsServiceTest, OnWalletProperties) {
  // We always need to call observer as we report errors back even when we have
  // null pointer
  EXPECT_CALL(*observer(), OnWalletProperties(_, 1, _)).Times(1);
  rewards_service()->OnWalletProperties(ledger::Result::LEDGER_ERROR, nullptr);
}

// add test for strange entries

}  // namespace onevn_rewards