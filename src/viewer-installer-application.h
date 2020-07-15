/* viewer-installer-application.h
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

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define VIEWER_INSTALLER_TYPE_APPLICATION (viewer_installer_application_get_type ())
#define VIEWER_INSTALLER_APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VIEWER_INSTALLER_TYPE_APPLICATION, ViewerInstallerApplication))

typedef struct _ViewerInstallerApplication           ViewerInstallerApplication;
typedef struct _ViewerInstallerApplicationClass      ViewerInstallerApplicationClass;
typedef struct _ViewerInstallerApplicationPrivate    ViewerInstallerApplicationPrivate;

struct _ViewerInstallerApplication
{
  GtkApplication parent;
  ViewerInstallerApplicationPrivate *priv;
};

struct _ViewerInstallerApplicationClass
{
  GtkApplicationClass parent_class;
};

GType                               viewer_installer_application_get_type        (void);
ViewerInstallerApplication         *viewer_installer_application_new             (void);

G_END_DECLS
