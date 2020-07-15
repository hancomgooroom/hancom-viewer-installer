/* viewer-installer-window-view-model.c
 *
 * Copyright (C) 2020 Hancom Gooroom <gooroom@hancom.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VIEWER_INSTALLERE_WINDOW_VIEW_MODEL_H
#define VIEWER_INSTALLERE_WINDOW_VIEW_MODEL_H

#include <gtk/gtk.h>
#include <glib-object.h>

#define VIEWER_INSTALLER_TYPE_WINDOW_VIEW_MODEL (viewer_installer_window_view_model_get_type ())

G_DECLARE_DERIVABLE_TYPE (ViewerInstallerWindowViewModel, viewer_installer_window_view_model, VIEWER_INSTALLER, WINDOW_VIEW_MODEL, GObject)

struct _ViewerInstallerWindowViewModelClass
{
    GObjectClass    parent_instance;
    void          (*update_data)        (ViewerInstallerWindowViewModel *self);
};

ViewerInstallerWindowViewModel *viewer_installer_window_view_model_new (void);

gboolean
viewer_installer_window_view_model_download_terminate (ViewerInstallerWindowViewModel *view_model);

gboolean
viewer_installer_window_view_model_install_terminate (ViewerInstallerWindowViewModel *view_model);

gchar*
viewer_installer_window_view_model_get_error (ViewerInstallerWindowViewModel *view_model);

gchar*
viewer_installer_window_view_model_get_package (ViewerInstallerWindowViewModel *view_model);

gchar*
viewer_installer_window_view_model_get_file_name (ViewerInstallerWindowViewModel *view_model);

void
viewer_installer_window_view_model_install (ViewerInstallerWindowViewModel *view_model);

void
viewer_installer_window_view_model_download (ViewerInstallerWindowViewModel *view_model);
#endif /* __VIEWER_INSTALLER_WINDOW_VIEW_MODLE_H */

