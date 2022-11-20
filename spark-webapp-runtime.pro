TARGET = spark-webapp-runtime
TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
        spark-webapp-runtime

# Update translation files
CONFIG(release, debug|release) {
    system(bash $${PWD}/translate_update.sh)
    system(bash $${PWD}/translate_generation.sh)
}
