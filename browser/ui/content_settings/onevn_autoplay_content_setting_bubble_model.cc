/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "onevn/browser/ui/content_settings/onevn_autoplay_content_setting_bubble_model.h"

#include "base/strings/utf_string_conversions.h"
#include "onevn/browser/onevn_browser_process_impl.h"
#include "onevn/browser/ui/onevn_browser_content_setting_bubble_model_delegate.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "onevn/grit/onevn_generated_resources.h"
#include "chrome/grit/generated_resources.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_utils.h"
#include "components/url_formatter/elide_url.h"
#include "ui/base/l10n/l10n_util.h"

using content_settings::SettingInfo;
using content_settings::SettingSource;
using content_settings::SETTING_SOURCE_USER;
using content_settings::SETTING_SOURCE_NONE;

OnevnAutoplayContentSettingBubbleModel::OnevnAutoplayContentSettingBubbleModel(
    Delegate* delegate,
    WebContents* web_contents)
    : ContentSettingSimpleBubbleModel(delegate,
                                      web_contents,
                                      CONTENT_SETTINGS_TYPE_AUTOPLAY),
      block_setting_(CONTENT_SETTING_BLOCK) {
  SetTitle();
  SetRadioGroup();
}

OnevnAutoplayContentSettingBubbleModel::~OnevnAutoplayContentSettingBubbleModel() {}

void OnevnAutoplayContentSettingBubbleModel::CommitChanges() {
  if (settings_changed()) {
    ContentSetting setting = selected_item() == kAllowButtonIndex
                                 ? CONTENT_SETTING_ALLOW
                                 : block_setting_;
    SetNarrowestContentSetting(setting);
  }
}

bool OnevnAutoplayContentSettingBubbleModel::settings_changed() const {
  return selected_item() != bubble_content().radio_group.default_item;
}

void OnevnAutoplayContentSettingBubbleModel::SetTitle() {
  set_title(l10n_util::GetStringUTF16(IDS_BLOCKED_AUTOPLAY_TITLE));
}

void OnevnAutoplayContentSettingBubbleModel::SetRadioGroup() {
  GURL url = web_contents()->GetURL();
  base::string16 display_host = url_formatter::FormatUrlForSecurityDisplay(url);
  if (display_host.empty())
    display_host = base::ASCIIToUTF16(url.spec());

  TabSpecificContentSettings* content_settings =
      TabSpecificContentSettings::FromWebContents(web_contents());
  bool allowed = !content_settings->IsContentBlocked(content_type());
  DCHECK(!allowed || content_settings->IsContentAllowed(content_type()));

  RadioGroup radio_group;
  radio_group.url = url;
  base::string16 radio_allow_label =
    l10n_util::GetStringFUTF16(IDS_BLOCKED_AUTOPLAY_UNBLOCK, display_host);
  base::string16 radio_block_label =
    l10n_util::GetStringUTF16(IDS_BLOCKED_AUTOPLAY_NO_ACTION);
  radio_group.radio_items.push_back(radio_allow_label);
  radio_group.radio_items.push_back(radio_block_label);

  ContentSetting setting;
  SettingSource setting_source = SETTING_SOURCE_NONE;

  SettingInfo info;
  HostContentSettingsMap* map =
    HostContentSettingsMapFactory::GetForProfile(GetProfile());
  std::unique_ptr<base::Value> value =
    map->GetWebsiteSetting(url, url, content_type(), std::string(), &info);
  setting = content_settings::ValueToContentSetting(value.get());
  setting_source = info.source;

  if (setting == CONTENT_SETTING_ALLOW) {
    radio_group.default_item = kAllowButtonIndex;
    // |block_setting_| is already set to |CONTENT_SETTING_BLOCK|.
  } else {
    radio_group.default_item = 1;
    block_setting_ = setting;
  }

  // Prevent creation of content settings for illegal urls like about:blank
  bool is_valid = map->CanSetNarrowestContentSetting(url, url, content_type());
  radio_group.user_managed = is_valid && setting_source == SETTING_SOURCE_USER;
  set_radio_group(radio_group);
}

void OnevnAutoplayContentSettingBubbleModel::SetNarrowestContentSetting(
    ContentSetting setting) {
  if (!GetProfile())
    return;

  auto* map = HostContentSettingsMapFactory::GetForProfile(GetProfile());
  map->SetNarrowestContentSetting(bubble_content().radio_group.url,
                                  bubble_content().radio_group.url,
                                  content_type(), setting);
}
