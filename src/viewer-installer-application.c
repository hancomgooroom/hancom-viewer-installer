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
#include <glib/gi18n.h>
#include <gdk/gdkx.h>

#include "define.h"
#include "utils.h"
#include "viewer-installer-config.h"
#include "viewer-installer-window.h"
#include "viewer-installer-application.h"

struct _ViewerInstallerApplicationPrivate
{
    gchar          *msg;
    GtkWidget      *dialog;

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
    ViewerInstallerApplicationPrivate *priv;
    priv = viewer_installer_application_get_instance_private (VIEWER_INSTALLER_APPLICATION(app));

#ifdef USE_HANCOM_TOOLKIT
    GNetworkMonitor *monitor = g_network_monitor_get_default();
    gboolean is_connected = g_network_monitor_get_network_available (monitor);

    g_usleep (G_USEC_PER_SEC * 2);

    if (!is_connected)
    {
        priv->msg = g_strdup (_("Network is not active"));
        priv->dialog = gtk_message_dialog_new  (NULL,
                                          GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_ERROR,
                                          GTK_BUTTONS_OK,
                                          NULL);
        gtk_window_set_title (GTK_WINDOW (priv->dialog), _("HancomToolkit"));
        gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (priv->dialog), "%s", priv->msg);
        gtk_window_set_keep_above (GTK_WINDOW (priv->dialog), TRUE);

        gtk_dialog_run (GTK_DIALOG (priv->dialog));

        g_application_quit (G_APPLICATION (app));
        return;
    }

    gchar **args;
    g_autofree gchar *command;

    command = g_strdup_printf ("pkexec %s/%s/%s %s", LIBDIR, GETTEXT_PACKAGE, VIEWER_SCRIPT, TOOLKIT_NAME);

    args = g_strsplit (command, " ", -1);

    g_spawn_sync (NULL, args, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, NULL, NULL);

    g_strfreev (args);

    g_spawn_sync (NULL, args, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, NULL, NULL);

    if (!check_package (TOOLKIT_NAME))
    {
        priv->msg = g_strdup (_("Package is not installed properly.\nRestart is required."));
        priv->dialog = gtk_message_dialog_new  (NULL,
                                          GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_ERROR,
                                          GTK_BUTTONS_OK,
                                          NULL);
        gtk_window_set_title (GTK_WINDOW (priv->dialog), _("HancomToolkit"));
        gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (priv->dialog), "%s", priv->msg);
        gtk_window_set_keep_above (GTK_WINDOW (priv->dialog), TRUE);

        gtk_dialog_run (GTK_DIALOG (priv->dialog));

        g_application_quit (G_APPLICATION (app));
        return;
    }

    g_spawn_command_line_sync (TOOLKIT_NAME, NULL, NULL, NULL, NULL);
#else
    GFile *file;
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
#endif
}

static void
viewer_installer_application_dispose (GObject *object)
{
    ViewerInstallerApplication *app = VIEWER_INSTALLER_APPLICATION(object);
    ViewerInstallerApplicationPrivate *priv = app->priv;

    if (priv && priv->dialog != NULL)
    {
        gtk_widget_destroy (priv->dialog);
        priv->dialog = NULL;
    }

    if (priv && priv->msg != NULL)
    {
        g_free (priv->msg);
    }

    if (priv && priv->window != NULL)
    {
        gtk_widget_destroy (GTK_WIDGET(priv->window));
        priv->window = NULL;
    }

    if (priv && priv->provider != NULL)
    {
        g_clear_object (&priv->provider);
        priv->provider = NULL;
    }

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
    priv->provider = NULL;
    priv->dialog = NULL;
    priv->msg = NULL;
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
