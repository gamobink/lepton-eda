/* Lepton EDA Schematic Capture
 * Copyright (C) 2018 dmn <graahnul.grom@gmail.com>
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

/*!
 * \file font_select_widget.c
 *
 * \brief Schematic font selection widget
 *
 */

#include "gschem.h"


/* convenience macro - gobject type implementation:
*/
G_DEFINE_TYPE (FontSelectWidget, font_select_widget, GSCHEM_TYPE_BIN);


/* widget's property IDs:
*/
typedef enum
{

  PROP_0,       /* placeholder */
  PROP_TOPLEVEL

} FontSelectWidgetProps;


/* --------------------------------------------------------
 *
 * forward declarations:
 *
 */

static void
font_select_widget_create (FontSelectWidget* widget);

static void
font_select_widget_on_show (GtkWidget* w);

static void
update_font_label (FontSelectWidget* widget, const gchar* font);

static gchar*
get_schematic_font (GschemToplevel* toplevel);

static void
set_schematic_font (GschemToplevel* toplevel, const gchar* font);

static void
save_schematic_font (GschemToplevel* toplevel, EdaConfig* cfg);

static void
on_btn_apply(GtkWidget* btn, gpointer p);

static void
on_btn_save(GtkWidget* btn, gpointer p);

static gchar*
font_sel_get_font (GtkFontSelection* sel);

static void
fontsel_set_font (FontSelectWidget* widget, const gchar* font);

static EdaConfig*
save_settings_dlg (FontSelectWidget* widget);




/* --------------------------------------------------------
 *
 * object construction:
 *
 */

GtkWidget*
font_select_widget_new (GschemToplevel* w_current)
{
  gpointer obj = g_object_new (FONT_SELECT_WIDGET_TYPE,
                               "toplevel", w_current,
                               NULL);
  return GTK_WIDGET (obj);
}




/* --------------------------------------------------------
 *
 * gobject stuff:
 *
 */

static void
get_property (GObject* obj, guint id, GValue* val, GParamSpec* spec)
{
  FontSelectWidget* widget = FONT_SELECT_WIDGET (obj);

  if (id == PROP_TOPLEVEL)
  {
    g_value_set_pointer (val, widget->toplevel_);
  }
  else
  {
    G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, id, spec);
  }
}



static void
set_property (GObject* obj, guint id, const GValue* val, GParamSpec* spec)
{
  FontSelectWidget* widget = FONT_SELECT_WIDGET (obj);

  if (id == PROP_TOPLEVEL)
  {
    gpointer ptr = g_value_get_pointer (val);
    widget->toplevel_ = GSCHEM_TOPLEVEL (ptr);
  }
  else
  {
    G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, id, spec);
  }
}



static void
dispose (GObject* obj)
{
  FontSelectWidgetClass* cls = FONT_SELECT_WIDGET_GET_CLASS (obj);
  GObjectClass* parent_cls = (GObjectClass*) g_type_class_peek_parent (cls);

  parent_cls->dispose (obj);
}



static void
font_select_widget_class_init (FontSelectWidgetClass* cls)
{
  GObjectClass* gcls = G_OBJECT_CLASS (cls);

  gcls->dispose      = &dispose;
  gcls->get_property = &get_property;
  gcls->set_property = &set_property;

  GParamFlags flags = (GParamFlags) (G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
  GParamSpec* spec = g_param_spec_pointer ("toplevel", "", "", flags);
  g_object_class_install_property (gcls, PROP_TOPLEVEL, spec);

  GtkWidgetClass* wcls = GTK_WIDGET_CLASS( cls );

  wcls->show = &font_select_widget_on_show;
}



static void
font_select_widget_init (FontSelectWidget* widget)
{
  font_select_widget_create (widget);
}




/* --------------------------------------------------------
 *
 * implementation:
 *
 */

static void
font_select_widget_create (FontSelectWidget* widget)
{
  GtkWidget* vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (widget), vbox);

  /* GTK font selection widget: */
  widget->font_sel_ = GTK_FONT_SELECTION (gtk_font_selection_new());
  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (widget->font_sel_), TRUE, TRUE, 0);

  /* separator */
  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new(), FALSE, FALSE, 5);

  /* horizontal container */
  GtkWidget* hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);


  /* apply button */
  GtkWidget* btn_apply = gtk_button_new_with_mnemonic (_("_Apply"));
  gtk_box_pack_start (GTK_BOX (hbox), btn_apply, FALSE, FALSE, 3);

  /* save button */
  GtkWidget* btn_save = gtk_button_new_with_mnemonic (_("Sa_ve..."));
  gtk_box_pack_start (GTK_BOX (hbox), btn_save, FALSE, FALSE, 3);

  /* horizontal container for the current schematic font label */
  GtkWidget* hbox_lab = gtk_hbox_new (TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), hbox_lab, TRUE, TRUE, 0);

  /* current font label */
  widget->font_label_ = gtk_label_new (NULL);
  gtk_label_set_selectable(GTK_LABEL (widget->font_label_), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox_lab), widget->font_label_, FALSE, FALSE, 3);

  /* separator */
  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new(), FALSE, FALSE, 5);

  g_signal_connect (G_OBJECT (btn_apply),
                    "clicked",
                    G_CALLBACK (&on_btn_apply),
                    widget);

  g_signal_connect (G_OBJECT (btn_save),
                    "clicked",
                    G_CALLBACK (&on_btn_save),
                    widget);

  gtk_widget_show_all (GTK_WIDGET (widget));

} /* font_select_widget_create() */



static void
font_select_widget_on_show (GtkWidget* w)
{
  FontSelectWidget* widget = FONT_SELECT_WIDGET (w);

  g_return_if_fail (widget != NULL);

  if (widget->toplevel_ == NULL)
    return;

  gchar* font = get_schematic_font (widget->toplevel_);

  update_font_label (widget, font);
  fontsel_set_font (widget, font);

  g_free (font);

  GTK_WIDGET_CLASS (font_select_widget_parent_class)->show (w);
}



/*! \brief Update label that displays current schematic font
 *
 *  \param widget Pointer to a FontSelectWidget
 *  \param font   Font name to display
 */
static void
update_font_label (FontSelectWidget* widget, const gchar* font)
{
  g_return_if_fail (widget != NULL);

  gchar* str = g_strdup_printf (_("<b>Current:</b> %s"), font);
  gtk_label_set_markup (GTK_LABEL (widget->font_label_), str);
  g_free (str);
}



/*! \brief Get current renderer's \a font
 *  \note  Caller must g_free() return value
 *
 *  \param toplevel Current gschem toplevel structure
 *
 *  \return Current schematic font name
 */
static gchar*
get_schematic_font (GschemToplevel* toplevel)
{
  g_return_val_if_fail (toplevel != NULL, NULL);
  g_return_val_if_fail (toplevel->renderer != NULL, NULL);

  gchar* font = NULL;
  g_object_get (toplevel->renderer, "font-name", &font, NULL);

  return font;
}



/*! \brief Use specified \a font to render schematic's text
 *
 *  \param toplevel Current gschem toplevel structure
 *  \param font     Font name
 */
static void
set_schematic_font (GschemToplevel* toplevel, const gchar* font)
{
  g_return_if_fail (toplevel != NULL);
  g_return_if_fail (toplevel->renderer != NULL);

  g_object_set (toplevel->renderer, "font-name", font, NULL);

  GschemPageView* view = gschem_toplevel_get_current_page_view (toplevel);
  gschem_page_view_invalidate_all (view);
}



/*! \brief Save current schematic font to the configuration file
 *
 * *  \param toplevel Current gschem toplevel structure
 * *  \param cfg      Configuration context to save settings to
 */
static void
save_schematic_font (GschemToplevel* toplevel, EdaConfig* cfg)
{
  g_return_if_fail (toplevel != NULL);
  g_return_if_fail (cfg != NULL);

  gchar* font = get_schematic_font (toplevel);

  if (cfg != NULL && font != NULL)
  {
    eda_config_set_string (cfg, "schematic.gui", "font", font);
    eda_config_save (cfg, NULL);
  }

  g_free (font);
}



/*! \brief Select \a font in the GtkFontSelection widget, if possible
 *
 *  \param widget Pointer to a FontSelectWidget
 *  \param font   Font name to select in GtkFontSelection widget
 */
static void
fontsel_set_font (FontSelectWidget* widget, const gchar* font)
{
  g_return_if_fail (widget != NULL);

  gtk_font_selection_set_font_name (widget->font_sel_, font);
}




/* --------------------------------------------------------
 *
 * signal handlers:
 *
 */

static void
on_btn_apply (GtkWidget* btn, gpointer p)
{
  FontSelectWidget* widget = (FontSelectWidget*) p;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (widget->toplevel_ != NULL);

  gchar* font = font_sel_get_font (widget->font_sel_);

  set_schematic_font (widget->toplevel_, font);
  update_font_label (widget, font);

  g_free (font);

} /* on_btn_apply() */



static void
on_btn_save (GtkWidget* btn, gpointer p)
{
  FontSelectWidget* widget = (FontSelectWidget*) p;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (widget->toplevel_ != NULL);

  EdaConfig* cfg = save_settings_dlg (widget);

  if (cfg != NULL)
  {
    save_schematic_font (widget->toplevel_, cfg);
  }

} /* on_btn_save() */




/* --------------------------------------------------------
 *
 * helpers:
 *
 */

/*! \brief Get selected font as a string composed of family and face
 *  \note  Caller must g_free() return value
 *
 *  \param sel Pointer to a GtkFontSelection widget
 *
 *  \return string in the form "family face"
 */
static gchar*
font_sel_get_font (GtkFontSelection* sel)
{
  g_return_val_if_fail (sel != NULL, NULL);

  PangoFontFamily* family = gtk_font_selection_get_family (sel);
  const char* family_name = pango_font_family_get_name (family);

  PangoFontFace* face = gtk_font_selection_get_face (sel);
  const char* face_name = pango_font_face_get_face_name (face);

  return g_strdup_printf ("%s %s", family_name, face_name);
}



/*! \brief Open save settings dialog
 *
 *  \param widget Pointer to a FontSelectWidget
 *
 *  \return configuration context to save settings to or NULL
 */
static EdaConfig*
save_settings_dlg (FontSelectWidget* widget)
{
  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (widget->toplevel_ != NULL, FALSE);

  /* create dialog: */
  GtkWidget* dlg = gtk_dialog_new_with_buttons(
    _("Save configuration"),
    GTK_WINDOW (widget->toplevel_->main_window),
    GTK_DIALOG_MODAL,
    GTK_STOCK_OK,     GTK_RESPONSE_ACCEPT,
    GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
    NULL);

  /* label: */
  GtkWidget* lab = gtk_label_new (_("Save settings to:"));

  /* radio buttons: */
  GtkWidget* btn1 = gtk_radio_button_new_with_label (
    NULL, _("Local configuration file (in current directory)"));
  GtkWidget* btn2 = gtk_radio_button_new_with_label_from_widget(
    GTK_RADIO_BUTTON (btn1), _("User configuration file (geda-user.conf)"));

  /* pack to vbox: */
  GtkWidget* vbox = gtk_vbox_new (TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), lab,  TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), btn1, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), btn2, TRUE, TRUE, 0);

  /* pack vbox to dialog's content area: */
  GtkWidget* ca = gtk_dialog_get_content_area (GTK_DIALOG (dlg));
  gtk_box_pack_start (GTK_BOX (ca), vbox, TRUE, TRUE, 0);

  /* show dialog: */
  gtk_widget_show_all (dlg);
  gint res = gtk_dialog_run (GTK_DIALOG (dlg));


  EdaConfig* ctx = NULL;

  if (res == GTK_RESPONSE_ACCEPT)
  {
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (btn1)))
    {
      gchar* cwd = g_get_current_dir();
      ctx = eda_config_get_context_for_path (cwd);
      g_free (cwd);
    }
    else
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (btn2)))
    {
      ctx = eda_config_get_user_context();
    }
  }

  gtk_widget_destroy (dlg);

  return ctx;

} /* save_settings_dlg() */

