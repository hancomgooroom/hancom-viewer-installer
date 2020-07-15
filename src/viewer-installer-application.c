/* viewer-installer-application.c
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

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "viewer-installer-config.h"
#include "viewer-installer-window.h"
#include "viewer-installer-application.h"

struct _ViewerInstallerApplicationPrivate
{
    GtkWindow      *window;
    GtkCssProvider *provider;
};

G_DEFINE_TYPE_WITH_PRIVATE (ViewerInstallerApplication, viewer_installer_application, GTK_TYPE_APPLICATION)

static void
viewer_installer_application_startup (GApplication *app)
{
    G_APPLICATION_CLASS (viewer_installer_application_parent_class)->startup (app);
}

static void
viewer_installer_application_activate (GApplication *app)
{
    GFile *file;
    ViewerInstallerApplicationPrivate *priv;
    priv = viewer_installer_application_get_instance_private (VIEWER_INSTALLER_APPLICATION(app));

    /* Get the current window or create one if necessary. */
    priv->window = gtk_application_get_active_window (GTK_APPLICATION(app));
    if (priv->window == NULL)
        priv->window = g_object_new (VIEWER_INSTALLER_TYPE_WINDOW,
                               "application", app,
                               "default-width", 750,
                               "default-height", 424,
                               NULL);

    gtk_window_set_position (GTK_WINDOW (priv->window), GTK_WIN_POS_CENTER);
    /* Ask the window manager/compositor to present the window. */
    gtk_window_present (priv->window);

    priv->provider = gtk_css_provider_new ();
    file = g_file_new_for_uri ("resource:///kr/hancom/viewer-installer/style.css");
    gtk_css_provider_load_from_file (priv->provider, file, NULL);
    gtk_style_context_add_provider_for_screen (gdk_screen_get_default(),
                                               GTK_STYLE_PROVIDER (priv->provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
    g_object_unref (file);
}

static void
viewer_installer_application_dispose (GObject *object)
{

    ViewerInstallerApplication *app = VIEWER_INSTALLER_APPLICATION(object);
    ViewerInstallerApplicationPrivate *priv = app->priv;

    if (priv && priv->window != NULL)
    {
        gtk_widget_destroy (GTK_WIDGET(priv->window));
        priv->window = NULL;
    }
#if 1  
    if (priv && priv->provider != NULL)
    {
        g_clear_object (&priv->provider);
        priv->provider = NULL;
    }
#endif
    G_OBJECT_CLASS (viewer_installer_application_parent_class)->dispose (object);
}

static void
viewer_installer_application_finalize (GObject *object)
{
    G_OBJECT_CLASS (viewer_installer_application_parent_class)->finalize (object);
}

static void
viewer_installer_application_init (ViewerInstallerApplication *application)
{
    ViewerInstallerApplicationPrivate *priv;
    priv = viewer_installer_application_get_instance_private (application);
    priv->window = NULL;
}

static void
viewer_installer_application_class_init (ViewerInstallerApplicationClass *class)
{
    G_OBJECT_CLASS (class)->dispose = viewer_installer_application_dispose;
    G_OBJECT_CLASS (class)->finalize = viewer_installer_application_finalize;
    G_APPLICATION_CLASS (class)->activate = viewer_installer_application_activate;
    G_APPLICATION_CLASS (class)->startup = viewer_installer_application_startup;
}

ViewerInstallerApplication*
viewer_installer_application_new (void)
{
  return g_object_new (VIEWER_INSTALLER_TYPE_APPLICATION,
                       "application-id", "kr.hancom.viewer-installer",
                       "flags", G_APPLICATION_HANDLES_OPEN,
                       NULL);
}
