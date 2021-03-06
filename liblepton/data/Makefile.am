theme=hicolor
xdgdatadir=@XDGDATADIR@
themedir=$(xdgdatadir)/icons/$(theme)

mime_icons = \
	application-x-lepton-sch2pcb-project \
	application-x-lepton-schematic        \
	application-x-lepton-symbol

mime_icon_files = \
	$(mime_icons:%=%-16.png) \
	$(mime_icons:%=%-22.png) \
	$(mime_icons:%=%-24.png) \
	$(mime_icons:%=%-32.png) \
	$(mime_icons:%=%-48.png) \
	$(mime_icons:%=%.svg)

mime_icon_sources = \
	$(mime_icons:%=%-16.svg) \
	$(mime_icons:%=%-22.svg) \
	$(mime_icons:%=%-32.svg) \
	$(mime_icons:%=%-48.svg)

theme_icons = \
	$(mime_icon_files:%=mimetypes,%)

mimedir = $(xdgdatadir)/mime
xmldir = $(mimedir)/packages

xml_in_files = liblepton.xml.in
xml_DATA = $(xml_in_files:.xml.in=.xml)

application_in_files = \
	x-lepton-sch2pcb-project.desktop.in \
	x-lepton-schematic.desktop.in \
	x-lepton-symbol.desktop.in

if ENABLE_KDE_DESKTOP_DATA
kdedatadir=@KDEDATADIR@
mimelnkdir = $(kdedatadir)/mimelnk
applicationdir = $(mimelnkdir)/application
application_DATA = $(application_in_files:.desktop.in=.desktop)
endif ENABLE_KDE_DESKTOP_DATA

install_icon_exec = \
	$(ICON_THEME_INSTALLER) \
	-t $(theme) \
	-s $(srcdir) \
	-d "x$(DESTDIR)" \
	-b $(themedir) \
	-m "$(mkinstalldirs)" \
	-x "$(INSTALL_DATA)"

DOMAIN=$(LIBLEPTON_GETTEXT_DOMAIN)

@DESKTOP_I18N_LANGS_RULE@
@DESKTOP_I18N_DESKTOP_RULE@
@DESKTOP_I18N_XML_RULE@

install-data-local:
	$(install_icon_exec) -i $(theme_icons)

install-data-hook:
if ENABLE_UPDATE_XDG_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
endif

uninstall-hook:
if ENABLE_UPDATE_XDG_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
endif
	$(install_icon_exec) -u $(theme_icons)

MAINTAINERCLEANFILES = *.log core FILE *~ Makefile.in
DISTCLEANFILES = $(xml_DATA) $(application_DATA)
EXTRA_DIST = $(mime_icon_files) $(mime_icon_sources) \
             $(xml_in_files) $(application_in_files)

