#ifndef ONEVN_CONTENT_BROWSER_WEBUI_ONEVN_SHARED_RESOURCES_DATA_SOURCE_H_
#define ONEVN_CONTENT_BROWSER_WEBUI_ONEVN_SHARED_RESOURCES_DATA_SOURCE_H_

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/single_thread_task_runner.h"
#include "content/public/browser/url_data_source.h"

namespace onevn_content {

using namespace content;

// A DataSource for chrome://onevn-resources/ URLs.
class OneVNSharedResourcesDataSource : public URLDataSource {
 public:
  OneVNSharedResourcesDataSource();
  ~OneVNSharedResourcesDataSource() override;

  // URLDataSource implementation.
  std::string GetSource() const override;
  void StartDataRequest(
      const std::string& path,
      const ResourceRequestInfo::WebContentsGetter& wc_getter,
      const URLDataSource::GotDataCallback& callback) override;
  bool AllowCaching() const override;
  std::string GetMimeType(const std::string& path) const override;
  bool ShouldServeMimeTypeAsContentTypeHeader() const override;
  scoped_refptr<base::SingleThreadTaskRunner> TaskRunnerForRequestPath(
      const std::string& path) const override;
  std::string GetAccessControlAllowOriginForOrigin(
      const std::string& origin) const override;
  bool IsGzipped(const std::string& path) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(OneVNSharedResourcesDataSource);
};

}  // namespace onevn_content

#endif