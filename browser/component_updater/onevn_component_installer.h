/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ONEVN_COMPONENTS_ONEVN_SHIELDS_BROWSER_ONEVN_COMPONENT_INSTALLER_H_
#define ONEVN_COMPONENTS_ONEVN_SHIELDS_BROWSER_ONEVN_COMPONENT_INSTALLER_H_

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "base/bind.h"
#include "base/callback.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "components/component_updater/component_installer.h"
#include "components/update_client/update_client.h"

namespace base {
class FilePath;
}  // namespace base

using ReadyCallback = base::Callback<void(const base::FilePath&,
    const std::string& manifest)>;

namespace onevn {

class OneVNComponentInstallerPolicy :
    public component_updater::ComponentInstallerPolicy {
 public:
  explicit OneVNComponentInstallerPolicy(const std::string& name,
      const std::string& base64_public_key,
      const ReadyCallback& ready_callback);

  ~OneVNComponentInstallerPolicy() override;

 private:
  // The following methods override ComponentInstallerPolicy
  bool VerifyInstallation(const base::DictionaryValue& manifest,
      const base::FilePath& install_dir) const override;
  bool SupportsGroupPolicyEnabledComponentUpdates() const override;
  bool RequiresNetworkEncryption() const override;
  update_client::CrxInstaller::Result OnCustomInstall(
      const base::DictionaryValue& manifest,
      const base::FilePath& install_dir) override;
  void OnCustomUninstall() override;
  void ComponentReady(
      const base::Version& version,
      const base::FilePath& install_dir,
      std::unique_ptr<base::DictionaryValue> manifest) override;
  base::FilePath GetRelativeInstallDir() const override;
  void GetHash(std::vector<uint8_t>* hash) const override;
  std::string GetName() const override;
  std::vector<std::string> GetMimeTypes() const override;
  update_client::InstallerAttributes GetInstallerAttributes() const override;

  std::string name_;
  std::string base64_public_key_;
  std::string public_key_;
  ReadyCallback ready_callback_;

  DISALLOW_COPY_AND_ASSIGN(OneVNComponentInstallerPolicy);
};

void RegisterComponent(component_updater::ComponentUpdateService* cus,
    const std::string& name,
    const std::string& base64_public_key,
    const base::Closure& registered_callback,
    const ReadyCallback& ready_callback);

}  // namespace onevn

#endif  // ONEVN_COMPONENTS_ONEVN_SHIELDS_BROWSER_ONEVN_COMPONENT_INSTALLER_H_