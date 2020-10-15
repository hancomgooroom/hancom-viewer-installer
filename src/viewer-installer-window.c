/* viewer-installer-window.c
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

#include <glib/gi18n.h>

#include "utils.h"
#include "define.h"
#include "viewer-installer-config.h"
#include "viewer-installer-window.h"
#include "viewer-installer-window-view-model.h"

struct _ViewerInstallerWindow
{
    GtkApplicationWindow  parent_instance;
};

typedef struct
{
    ViewerInstallerWindowViewModel *view_model;

    /* Template widgets */
    GtkImage            *main_image;

    GtkStack            *bar_stack;
    GtkWidget           *header_bar;
    GtkWidget           *start_bar;
    GtkWidget           *install_bar;
    GtkWidget           *end_bar;

    GtkLabel            *status_label;
    GtkLabel            *update_label;
    GtkLabel            *error_label;

    GtkButton           *install_button;
    GtkButton           *installing_button;
    GtkButton           *close_button;

    GtkProgressBar      *install_progressbar;
} ViewerInstallerWindowPrivate;


G_DEFINE_TYPE_WITH_PRIVATE (ViewerInstallerWindow, viewer_installer_window, GTK_TYPE_APPLICATION_WINDOW)

static void viewer_installer_window_button_close_clicked (GtkWidget *button, ViewerInstallerWindow *win)
{
    gtk_widget_destroy (GTK_WIDGET (win));
}

static void
viewer_installer_window_button_install_clicked (GtkWidget *button, ViewerInstallerWindow *win)
{
    g_return_if_fail (VIEWER_INSTALLER_WINDOW(win));

    ViewerInstallerWindowPrivate *priv = viewer_installer_window_get_instance_private (win);
    viewer_installer_window_view_model_download(priv->view_model);
}

static gboolean
viewer_installer_window_start_install (gpointer user_data)
{
    ViewerInstallerWindow *win = user_data;
    ViewerInstallerWindowPrivate *priv;

    priv = viewer_installer_window_get_instance_private (win);

    viewer_installer_window_view_model_install(priv->view_model);

    return G_SOURCE_REMOVE;
}

static void
viewer_installer_window_notify_status (GObject *object,
                                       GParamSpec *pspec,
                                       gpointer data)
{
    guint val;
    guint *status;

    ViewerInstallerWindow *win = data;
    ViewerInstallerWindowPrivate *priv;

    g_return_if_fail (VIEWER_INSTALLER_WINDOW(data));

    priv = viewer_installer_window_get_instance_private (win);

    g_object_get (object, "status", &status, NULL);

    val = GPOINTER_TO_UINT (status);

    switch (val)
    {
        case STATUS_DOWNLOADING :
        {
            gchar *txt = g_strdup (_("Downloading Hangul 2020 Viewer Beta"));
            gtk_label_set_text (priv->status_label, txt);
            gtk_stack_set_visible_child (GTK_STACK (priv->bar_stack), priv->install_bar);
            gtk_header_bar_set_show_close_button (GTK_HEADER_BAR(priv->header_bar), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET(priv->installing_button), FALSE);
            gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->install_progressbar), 0);
            break;
        }
        case STATUS_DOWNLOADED :
        {
            gchar *txt = g_strdup (_("Downloaded"));
            gtk_label_set_text (priv->status_label, txt);
            viewer_installer_window_view_model_download_terminate (priv->view_model);
            g_timeout_add (1000, viewer_installer_window_start_install, data);
            break;
        }
        case STATUS_INSTALLING:
        {
            gchar *txt = g_strdup (_("Installing Hangul 2020 Viewer Beta"));
            gtk_label_set_text (priv->status_label, txt);
            break;
        }
        case STATUS_INSTALLED:
        {
            gchar *txt;
            gchar *package;
            viewer_installer_window_view_model_install_terminate (priv->view_model);
            package = viewer_installer_window_view_model_get_package (priv->view_model);

            if (check_package(package))
            {
                txt = g_strdup (_("The installation of Hangul 2020 Viewer Beta is complete"));
            }
            else
            {
                txt = g_strdup (_("The Installation of Hangul 2020 Viewer Beta is failed"));
            }

            gtk_label_set_text (priv->error_label, txt);
            gtk_stack_set_visible_child (GTK_STACK (priv->bar_stack), priv->end_bar);
            gtk_header_bar_set_show_close_button (GTK_HEADER_BAR(priv->header_bar), TRUE);
            g_object_freeze_notify (object);
            break;
        }
        case STATUS_ERROR :
        {
            gchar *error;
            error = viewer_installer_window_view_model_get_error (priv->view_model);
            gtk_label_set_text (priv->error_label, error);
            gtk_stack_set_visible_child (GTK_STACK (priv->bar_stack), priv->end_bar);
            gtk_header_bar_set_show_close_button (GTK_HEADER_BAR(priv->header_bar), TRUE);
            g_object_freeze_notify (object);
        }
        default:
            break;
    }
}

static void
viewer_installer_window_notify_progress (GObject *object,
                                         GParamSpec *pspec,
                                         gpointer data)
{
    gdouble val;
    guint *progress;

    ViewerInstallerWindow *win = data;
    ViewerInstallerWindowPrivate *priv;

    g_return_if_fail (VIEWER_INSTALLER_WINDOW(data));

    priv = viewer_installer_window_get_instance_private (win);

    g_object_get (object, "progress", &progress, NULL);

    val = GPOINTER_TO_UINT (progress) / 100.f;

    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->install_progressbar), val);
}

static void
viewer_installer_window_constructed (GObject *self)
{
    G_OBJECT_CLASS (viewer_installer_window_parent_class)->constructed (self);
}

static void
viewer_installer_window_dispose (GObject *self)
{
    ViewerInstallerWindow *win;
    ViewerInstallerWindowPrivate *priv;

    g_return_if_fail (self != NULL);

    win = VIEWER_INSTALLER_WINDOW (self);
    priv = viewer_installer_window_get_instance_private (win);

    if (priv->view_model)
    {
        g_object_unref (priv->view_model);
        priv->view_model = NULL;
    }

    G_OBJECT_CLASS (viewer_installer_window_parent_class)->dispose (self);
}

static void
viewer_installer_window_finalize (GObject *self)
{
    G_OBJECT_CLASS (viewer_installer_window_parent_class)->finalize (self);
}

static void
viewer_installer_window_class_init (ViewerInstallerWindowClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    oclass->dispose = viewer_installer_window_dispose;
    oclass->finalize = viewer_installer_window_finalize;
    oclass->constructed = viewer_installer_window_constructed;

    gtk_widget_class_set_template_from_resource (widget_class, "/kr/hancom/viewer-installer/viewer-installer-window.ui");
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, main_image);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, bar_stack);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, header_bar);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, start_bar);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, install_bar);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, end_bar);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, update_label);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, status_label);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, error_label);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, install_button);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, installing_button);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, close_button);
    gtk_widget_class_bind_template_child_private (widget_class, ViewerInstallerWindow, install_progressbar);
}

static void
viewer_installer_window_init (ViewerInstallerWindow *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));

    ViewerInstallerWindow *win;
    win = VIEWER_INSTALLER_WINDOW (self);
    ViewerInstallerWindowPrivate *priv = viewer_installer_window_get_instance_private (win);

    GdkPixbuf *pixbuf;
    pixbuf = gdk_pixbuf_new_from_resource ("/kr/hancom/viewer-installer/viewer-installer.svg", NULL);
    gtk_image_set_from_pixbuf (priv->main_image, pixbuf);

    priv->view_model = viewer_installer_window_view_model_new ();

    //viewer_installer_window_check_package (self);
 
    g_signal_connect (priv->close_button, "clicked",
              G_CALLBACK (viewer_installer_window_button_close_clicked), self);
    g_signal_connect (priv->install_button, "clicked",
              G_CALLBACK (viewer_installer_window_button_install_clicked), self);
    g_signal_connect (priv->view_model, "notify::status",
              G_CALLBACK (viewer_installer_window_notify_status), self);
    g_signal_connect (priv->view_model, "notify::progress",
              G_CALLBACK (viewer_installer_window_notify_progress), self);
}
