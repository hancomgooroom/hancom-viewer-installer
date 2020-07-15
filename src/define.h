typedef enum
{
  STATUS_NORMAL = 0,
  STATUS_DOWNLOADING,
  STATUS_DOWNLOADED,
  STATUS_INSTALLING,
  STATUS_INSTALLED,
  STATUS_CANCEL,
  STATUS_ERROR,
  N_STATUS
} InstallStatus;

#define VIEWER_NAME "hoffice-hwpviewer"
#define VIEWER_CHECK "hancom-viewer-check"
#define VIEWER_SCRIPT "hancom-viewer-install"
#define VIEWER_REFERER  "https://www.sample.com"
#define VIEWER_INSTALL_URL "https://cdn.sample.com"
