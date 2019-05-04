/* Copyright (c) 2019 The OneVN Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "onevn/app/onevn_main_delegate.h"

#include <memory>
#include <unordered_set>

#include "base/base_switches.h"
#include "base/lazy_instance.h"
#include "base/path_service.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "onevn/app/onevn_command_line_helper.h"
#include "onevn/browser/onevn_content_browser_client.h"
#include "onevn/common/onevn_switches.h"
#include "onevn/common/resource_bundle_helper.h"
#include "onevn/components/onevn_ads/browser/buildflags/buildflags.h"
#include "onevn/renderer/onevn_content_renderer_client.h"
#include "onevn/utility/onevn_content_utility_client.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_paths_internal.h"
#include "chrome/common/chrome_switches.h"
#include "components/autofill/core/common/autofill_features.h"
#include "components/password_manager/core/common/password_manager_features.h"
#include "components/unified_consent/feature.h"
#include "content/public/common/content_features.h"
#include "extensions/common/extension_features.h"
#include "gpu/config/gpu_finch_features.h"
#include "services/network/public/cpp/features.h"
#include "third_party/widevine/cdm/buildflags.h"
#include "ui/base/ui_base_features.h"

#if BUILDFLAG(BUNDLE_WIDEVINE_CDM)
#include "onevn/common/onevn_paths.h"
#endif

#if BUILDFLAG(ONEVN_ADS_ENABLED)
#include "components/dom_distiller/core/dom_distiller_switches.h"
#endif

#if !defined(CHROME_MULTIPLE_DLL_BROWSER)
base::LazyInstance<OneVNContentRendererClient>::DestructorAtExit
    g_onevn_content_renderer_client = LAZY_INSTANCE_INITIALIZER;
base::LazyInstance<OneVNContentUtilityClient>::DestructorAtExit
    g_onevn_content_utility_client = LAZY_INSTANCE_INITIALIZER;
#endif
#if !defined(CHROME_MULTIPLE_DLL_CHILD)
base::LazyInstance<OneVNContentBrowserClient>::DestructorAtExit
    g_onevn_content_browser_client = LAZY_INSTANCE_INITIALIZER;
#endif

OneVNMainDelegate::OneVNMainDelegate()
    : ChromeMainDelegate() {}

OneVNMainDelegate::OneVNMainDelegate(base::TimeTicks exe_entry_point_ticks)
    : ChromeMainDelegate(exe_entry_point_ticks) {}

OneVNMainDelegate::~OneVNMainDelegate() {}

content::ContentBrowserClient*
OneVNMainDelegate::CreateContentBrowserClient() {
#if defined(CHROME_MULTIPLE_DLL_CHILD)
  return NULL;
#else
  if (chrome_content_browser_client_ == nullptr) {
    DCHECK(!chrome_feature_list_creator_);
    chrome_feature_list_creator_ = std::make_unique<ChromeFeatureListCreator>();
    chrome_content_browser_client_ =
        std::make_unique<OneVNContentBrowserClient>(
            chrome_feature_list_creator_.get());
  }
  return chrome_content_browser_client_.get();
#endif
}

content::ContentRendererClient*
OneVNMainDelegate::CreateContentRendererClient() {
#if defined(CHROME_MULTIPLE_DLL_BROWSER)
  return NULL;
#else
  return g_onevn_content_renderer_client.Pointer();
#endif
}

content::ContentUtilityClient*
OneVNMainDelegate::CreateContentUtilityClient() {
#if defined(CHROME_MULTIPLE_DLL_BROWSER)
  return NULL;
#else
  return g_onevn_content_utility_client.Pointer();
#endif
}

void OneVNMainDelegate::PreSandboxStartup() {
  ChromeMainDelegate::PreSandboxStartup();
#if defined(OS_POSIX)
  // Setup NativeMessagingHosts to point to the default Chrome locations
  // because that's where native apps will create them
  base::FilePath chrome_user_data_dir;
  base::FilePath native_messaging_dir;
#if defined(OS_MACOSX)
  base::PathService::Get(base::DIR_APP_DATA, &chrome_user_data_dir);
  chrome_user_data_dir = chrome_user_data_dir.Append("Google/Chrome");
  native_messaging_dir = base::FilePath(FILE_PATH_LITERAL(
      "/Library/Google/Chrome/NativeMessagingHosts"));
#else
  chrome::GetDefaultUserDataDirectory(&chrome_user_data_dir);
  native_messaging_dir = base::FilePath(FILE_PATH_LITERAL(
      "/etc/opt/chrome/native-messaging-hosts"));
#endif  // defined(OS_MACOSX)
  base::PathService::OverrideAndCreateIfNeeded(
      chrome::DIR_USER_NATIVE_MESSAGING,
      chrome_user_data_dir.Append(FILE_PATH_LITERAL("NativeMessagingHosts")),
      false, true);
  base::PathService::OverrideAndCreateIfNeeded(chrome::DIR_NATIVE_MESSAGING,
      native_messaging_dir, false, true);
#endif  // OS_POSIX
  if (onevn::SubprocessNeedsResourceBundle()) {
    onevn::InitializeResourceBundle();
  }
}

bool OneVNMainDelegate::BasicStartupComplete(int* exit_code) {
  OneVNCommandLineHelper command_line(base::CommandLine::ForCurrentProcess());
#if BUILDFLAG(ONEVN_ADS_ENABLED)
  command_line.AppendSwitch(switches::kEnableDomDistiller);
#endif
  command_line.AppendSwitch(switches::kDisableDomainReliability);
  command_line.AppendSwitch(switches::kDisableChromeGoogleURLTrackingClient);
  command_line.AppendSwitch(switches::kNoPings);

  // Enabled features.
  const std::unordered_set<const char*> enabled_features = {
      extensions_features::kNewExtensionUpdaterService.name,
      features::kDesktopPWAWindowing.name,
  };

  // Disabled features.
  const std::unordered_set<const char*> disabled_features = {
      autofill::features::kAutofillSaveCardSignInAfterLocalSave.name,
      autofill::features::kAutofillServerCommunication.name,
      features::kAudioServiceOutOfProcess.name,
      features::kDefaultEnableOopRasterization.name,
      network::features::kNetworkService.name,
      unified_consent::kUnifiedConsent.name,
  };

  command_line.AppendFeatures(enabled_features, disabled_features);


  bool ret = ChromeMainDelegate::BasicStartupComplete(exit_code);

#if BUILDFLAG(BUNDLE_WIDEVINE_CDM)
  // Override chrome::FILE_WIDEVINE_CDM path because we install it in user data
  // dir. Must call after ChromeMainDelegate::BasicStartupComplete() to use
  // chrome paths.
  onevn::OverridePath();
#endif

  return ret;
}