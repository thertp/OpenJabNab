TEMPLATE = subdirs
SUBDIRS = auth locate record rfid msgall annuaire stats
target.path = $$[QT_INSTALL_LIBS]/openjabnab/plugins
INSTALLS += target
