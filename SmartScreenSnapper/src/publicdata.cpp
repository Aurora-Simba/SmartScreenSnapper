#include "publicdata.h"
#include "mainwindow.h"
#include "MyGlobalShortcut/MyGlobalShortCut.h"
#include <QSettings>
#include <QApplication>

int PublicData::activeWindowIndex = -1;
int PublicData::totalWindowCount = 0;
int PublicData::snapMethod = 0;
bool PublicData::isPlaySound = false;
bool PublicData::clickCloseToTray = true;
bool PublicData::ignoreClickCloseToTray = false;
bool PublicData::hotKeyNoWait = true;
bool PublicData::includeCursor = false;
bool PublicData::noBorder = false;
bool PublicData::copyToClipBoardAfterSnap = false;
QString PublicData::gifSavePath = "";
QString PublicData::styleName = "";
QString PublicData::qssPath = "";
QString PublicData::imageExtName[] = {".png", ".jpg", ".bmp"};
ShotTypeItem PublicData::snapTypeItems[SNAPTYPECOUNT] = {};
QHash<ScreenShotHelper::ShotType, QList<MyGlobalShortCut*>> PublicData::hotKey;

PublicData::PublicData()
{

}

QString PublicData::getConfigFilePath()
{
    return QCoreApplication::applicationDirPath() + "/SmartScreenSnapperConfig.ini";
}

void PublicData::readSettings()
{
    QSettings qSettings(getConfigFilePath(), QSettings::IniFormat);

    int size = qSettings.beginReadArray("SnapType");

    for (int i = 0; i < size; i++) {
        qSettings.setArrayIndex(i);
        snapTypeItems[i].shotType = ScreenShotHelper::ShotType(qSettings.value("ShotType", i).toInt());
        snapTypeItems[i].waitTime = qSettings.value("WaitTime", 0).toInt();
        snapTypeItems[i].hotKey = qSettings.value("HotKey", "").toString();
        snapTypeItems[i].isAutoSave = qSettings.value("IsAutoSave", false).toBool();
        snapTypeItems[i].autoSavePath = qSettings.value("AutoSavePath", QApplication::applicationDirPath()).toString();
        snapTypeItems[i].autoSaveExtName = qSettings.value("AutoSaveExtName", imageExtName[0]).toString();
    }
    qSettings.endArray();

    isPlaySound = qSettings.value("Sound/IsPlaySound", false).toBool();
    clickCloseToTray = qSettings.value("Config/ClickCloseToTray", true).toBool();
    hotKeyNoWait = qSettings.value("Config/HotKeyNoWait", true).toBool();
    includeCursor = qSettings.value("Config/IncludeCursor", false).toBool();
    noBorder = qSettings.value("Config/NoBorder", false).toBool();
    snapMethod = qSettings.value("Config/SnapMethod", SnapMethod2).toInt();
    copyToClipBoardAfterSnap = qSettings.value("Config/CopyToClipBoardAfterSnap", false).toBool();
    gifSavePath =  qSettings.value("Tool/GIFSavePath", "").toString();
    styleName =  qSettings.value("Config/StyleName", "").toString();
    qssPath =  qSettings.value("Config/QssPath", "").toString();
}

void PublicData::writeSettings()
{
    QSettings qSettings(getConfigFilePath(), QSettings::IniFormat);

    qSettings.beginWriteArray("SnapType");

    int size = sizeof(snapTypeItems) / sizeof(ShotTypeItem);

    for (int i = 0; i < size; i++) {
        qSettings.setArrayIndex(i);
        qSettings.setValue("WaitTime", snapTypeItems[i].waitTime);
        qSettings.setValue("HotKey", snapTypeItems[i].hotKey);
        qSettings.setValue("IsAutoSave", snapTypeItems[i].isAutoSave);
        qSettings.setValue("AutoSavePath", snapTypeItems[i].autoSavePath);
        qSettings.setValue("AutoSaveExtName", snapTypeItems[i].autoSaveExtName);
    }
    qSettings.endArray();

    qSettings.setValue("Sound/IsPlaySound", isPlaySound);
    qSettings.setValue("Config/ClickCloseToTray", clickCloseToTray);
    qSettings.setValue("Config/HotKeyNoWait", hotKeyNoWait);
    qSettings.setValue("Config/IncludeCursor", includeCursor);
    qSettings.setValue("Config/NoBorder", noBorder);
    qSettings.setValue("Config/SnapMethod", snapMethod);
    qSettings.setValue("Config/CopyToClipBoardAfterSnap", copyToClipBoardAfterSnap);
    qSettings.setValue("Tool/GIFSavePath", gifSavePath);
    qSettings.setValue("Config/StyleName", styleName);
    qSettings.setValue("Config/QssPath", qssPath);
}

void PublicData::registerAllHotKey(QWidget* parent)
{
    QStringList registeredKeyList;    //记录次热键是否被本程序注册，如果注册过了就不再注册了
    for (int i = 0; i < (int)(sizeof(PublicData::snapTypeItems) / sizeof(ShotTypeItem)); i++){
        QList<MyGlobalShortCut*> hotKey;
        QStringList keys = PublicData::snapTypeItems[i].hotKey.split(", ");
        for (int j = 0; j < keys.size(); j++) {
            QString key = keys.at(j);
            if (key == "") continue;
            MyGlobalShortCut* shortcut = NULL;
            if (registeredKeyList.contains(key)) {
                shortcut = new MyGlobalShortCut(key, parent, false);
            } else {
                registeredKeyList.push_back(key);
                shortcut = new MyGlobalShortCut(key, parent, true);
            }
            hotKey.push_back(shortcut);
            QObject::connect(shortcut,
                             SIGNAL(activatedHotKey(ScreenShotHelper::ShotType)),
                             parent,
                             SLOT(hotKeyPressed(ScreenShotHelper::ShotType)));
        }
        PublicData::hotKey.insert(PublicData::snapTypeItems[i].shotType, hotKey);
    }
}

void PublicData::unregisterAllHotKey()
{
    while (!PublicData::hotKey.isEmpty()) {
        QList<MyGlobalShortCut*> hotKey = PublicData::hotKey.begin().value();
        for (int i = 0; i < hotKey.size(); i++){
            hotKey.at(i)->unregisterHotKey();
            delete hotKey.at(i);
        }
        PublicData::hotKey.remove(PublicData::hotKey.begin().key());
    }
}

bool PublicData::applyQss()
{
    if (PublicData::qssPath == "") {
        qApp->setStyleSheet("");
        return true;
    }
    QFile qssFile(PublicData::qssPath);
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen()) {
        qApp->setStyleSheet(QLatin1String(qssFile.readAll()));
        qssFile.close();
        return true;
    }
    return false;
}
