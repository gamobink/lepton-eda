/* Lepton EDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2013 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*! \todo STILL NEED to clean up line lengths in aa and tr */
#include <config.h>
#include <version.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) g_object_unref)


/***************** Start of help/about dialog box ********************/

/*! \brief Create the about dialog and show it
 *  \par Function Description
 *  This function creates the about dialog.
 */
void about_dialog (GschemToplevel *w_current)
{
  char *version_string;
  char *logo_file;
  GdkPixbuf *logo;
  GError *error = NULL;

  version_string = g_strdup_printf (_("%s (git: %.7s)"),
                                    PACKAGE_DOTTED_VERSION,
                                    PACKAGE_GIT_COMMIT);

  logo_file = g_strconcat (w_current->toplevel->bitmap_directory,
                           G_DIR_SEPARATOR_S, "gschem-about-logo.png", NULL);

  logo = gdk_pixbuf_new_from_file (logo_file, &error);
  g_free (logo_file);

  if (error != NULL) {
    g_assert (logo == NULL);
    s_log_message ("Could not load image at file: %1$s\n%2$s",
                   logo_file, error->message);
    g_error_free (error);
  }


  GtkWidget* dlg = gtk_about_dialog_new();
  GtkAboutDialog* adlg = GTK_ABOUT_DIALOG (dlg);

  gtk_about_dialog_set_name (adlg, "lepton-schematic");
  gtk_about_dialog_set_version (adlg, version_string);
  gtk_about_dialog_set_logo (adlg, logo);
  gtk_about_dialog_set_comments (adlg, _("Lepton Electronic Design Automation"));

  /*
   * TRANSLATORS: "ChangeLog", AUTHORS and COPYING
   * are a literal filenames; please don't translate them.
  */

  gtk_about_dialog_set_copyright (adlg,
    _("Copyright © 1998-2017 by Ales Hvezda and the respective original authors.\n"
      "Copyright © 2017-2018 Lepton Developers.\n"
      "See AUTHORS and ChangeLog files for details."));

  gtk_about_dialog_set_license (adlg,
    "Lepton EDA is freely distributable under the\n"
    "GNU Public License (GPL) version 2.0 or (at your option) any later version.\n"
    "See the COPYING file for the full text of the license.");


  GtkWidget* ca = gtk_dialog_get_content_area (GTK_DIALOG (dlg));

  GtkWidget* website1 = gtk_label_new (NULL);
  GtkWidget* website2 = gtk_label_new (NULL);
  gtk_label_set_selectable (GTK_LABEL (website1), TRUE);
  gtk_label_set_selectable (GTK_LABEL (website2), TRUE);

  gtk_label_set_markup (GTK_LABEL (website1),
    "<a href='http://github.com/lepton-eda/lepton-eda'>github.com/lepton-eda/lepton-eda</a>" );
  gtk_label_set_markup (GTK_LABEL (website2),
    "<a href='http://geda-project.org'>geda-project.org</a>" );

  gtk_box_pack_start (GTK_BOX (ca), website1, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (ca), website2, FALSE, FALSE, 0);


  gtk_widget_show_all (dlg);
  gtk_dialog_run (GTK_DIALOG (dlg));
  gtk_widget_destroy (dlg);

  g_free (version_string);
  g_object_unref (logo);
}

/***************** End of help/about dialog box *********************/

