/*
 * This file Copyright (C) 2009-2015 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 */

#ifdef _WIN32
#include <winsock2.h> // FD_SETSIZE
#else
#include <sys/select.h> // FD_SETSIZE
#endif

#include <cassert>

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStyle>
#include <QTabWidget>
#include <QTime>
#include <QTimeEdit>
#include <QTimer>
#include <QVBoxLayout>

#include "ColumnResizer.h"
#include "FreeSpaceLabel.h"
#include "Formatter.h"
#include "Prefs.h"
#include "PrefsDialog.h"
#include "Session.h"
#include "Utils.h"

/***
****
***/

namespace {

class PreferenceWidget {
    static char const *const PREF_KEY;

public:
    explicit PreferenceWidget(QObject *object)
        : m_object(object)
    {}

    template<typename T>
    bool is() const
    {
        return qobject_cast<T *>(m_object) != nullptr;
    }

    template<typename T>
    T const *as() const
    {
        assert(is<T>());
        return static_cast<T const *>(m_object);
    }

    template<typename T>
    T *as()
    {
        assert(is<T>());
        return static_cast<T *>(m_object);
    }

    void setPrefKey(int key)
    {
        m_object->setProperty(PREF_KEY, key);
    }

    int getPrefKey() const
    {
        return m_object->property(PREF_KEY).toInt();
    }

private:
    QObject *const m_object;
};

char const *const PreferenceWidget::PREF_KEY = "pref-key";

int qtDayToTrDay(int day)
{
    switch (day) {
    case Qt::Monday:
        return TR_SCHED_MON;

    case Qt::Tuesday:
        return TR_SCHED_TUES;

    case Qt::Wednesday:
        return TR_SCHED_WED;

    case Qt::Thursday:
        return TR_SCHED_THURS;

    case Qt::Friday:
        return TR_SCHED_FRI;

    case Qt::Saturday:
        return TR_SCHED_SAT;

    case Qt::Sunday:
        return TR_SCHED_SUN;

    default:
        assert(false && "Invalid day of week");
        return 0;
    }
}

QString qtDayName(int day)
{
    switch (day) {
    case Qt::Monday:
        return PrefsDialog::tr("Monday");

    case Qt::Tuesday:
        return PrefsDialog::tr("Tuesday");

    case Qt::Wednesday:
        return PrefsDialog::tr("Wednesday");

    case Qt::Thursday:
        return PrefsDialog::tr("Thursday");

    case Qt::Friday:
        return PrefsDialog::tr("Friday");

    case Qt::Saturday:
        return PrefsDialog::tr("Saturday");

    case Qt::Sunday:
        return PrefsDialog::tr("Sunday");

    default:
        assert(false && "Invalid day of week");
        return QString();
    }
}

} // namespace

bool PrefsDialog::updateWidgetValue(QWidget *widget, int prefKey)
{
    PreferenceWidget prefWidget(widget);

    if (prefWidget.is<QCheckBox>()) {
        prefWidget.as<QCheckBox>()->setChecked(myPrefs.getBool(prefKey));
    } else if (prefWidget.is<QSpinBox>()) {
        prefWidget.as<QSpinBox>()->setValue(myPrefs.getInt(prefKey));
    } else if (prefWidget.is<QDoubleSpinBox>()) {
        prefWidget.as<QDoubleSpinBox>()->setValue(myPrefs.getDouble(prefKey));
    } else if (prefWidget.is<QTimeEdit>()) {
        prefWidget.as<QTimeEdit>()->setTime(QTime(0, 0).addSecs(myPrefs.getInt(prefKey) * 60));
    } else if (prefWidget.is<QLineEdit>()) {
        prefWidget.as<QLineEdit>()->setText(myPrefs.getString(prefKey));
    } else if (prefWidget.is<PathButton>()) {
        prefWidget.as<PathButton>()->setPath(myPrefs.getString(prefKey));
    } else if (prefWidget.is<FreeSpaceLabel>()) {
        prefWidget.as<FreeSpaceLabel>()->setPath(myPrefs.getString(prefKey));
    } else if (prefWidget.is<QPlainTextEdit>()) {
        prefWidget.as<QPlainTextEdit>()->setPlainText(myPrefs.getString(prefKey));
    } else {
        return false;
    }

    return true;
}

void PrefsDialog::linkWidgetToPref(QWidget *widget, int prefKey)
{
    PreferenceWidget prefWidget(widget);

    prefWidget.setPrefKey(prefKey);
    updateWidgetValue(widget, prefKey);
    myWidgets.insert(prefKey, widget);

    if (prefWidget.is<QCheckBox>()) {
        connect(widget, SIGNAL(toggled(bool)), SLOT(checkBoxToggled(bool)));
    } else if (prefWidget.is<QTimeEdit>()) {
        connect(widget, SIGNAL(editingFinished()), SLOT(timeEditingFinished()));
    } else if (prefWidget.is<QLineEdit>()) {
        connect(widget, SIGNAL(editingFinished()), SLOT(lineEditingFinished()));
    } else if (prefWidget.is<PathButton>()) {
        connect(widget, SIGNAL(pathChanged(QString)), SLOT(pathChanged(QString)));
    } else if (prefWidget.is<QAbstractSpinBox>()) {
        connect(widget, SIGNAL(editingFinished()), SLOT(spinBoxEditingFinished()));
    } else if (prefWidget.is<QPlainTextEdit>()) {
        connect(widget, SIGNAL(textChanged()), SLOT(plainTextChanged()));
    }
}

void PrefsDialog::checkBoxToggled(bool checked)
{
    PreferenceWidget const prefWidget(sender());

    if (prefWidget.is<QCheckBox>()) {
        setPref(prefWidget.getPrefKey(), checked);
    }
}

void PrefsDialog::spinBoxEditingFinished()
{
    PreferenceWidget const prefWidget(sender());

    if (prefWidget.is<QDoubleSpinBox>()) {
        setPref(prefWidget.getPrefKey(), prefWidget.as<QDoubleSpinBox>()->value());
    } else if (prefWidget.is<QSpinBox>()) {
        setPref(prefWidget.getPrefKey(), prefWidget.as<QSpinBox>()->value());
    }
}

void PrefsDialog::timeEditingFinished()
{
    PreferenceWidget const prefWidget(sender());

    if (prefWidget.is<QTimeEdit>()) {
        setPref(prefWidget.getPrefKey(), QTime(0, 0).secsTo(prefWidget.as<QTimeEdit>()->time()) / 60);
    }
}

void PrefsDialog::lineEditingFinished()
{
    PreferenceWidget const prefWidget(sender());

    if (prefWidget.is<QLineEdit>()) {
        QLineEdit const *const lineEdit = prefWidget.as<QLineEdit>();

        if (lineEdit->isModified()) {
            setPref(prefWidget.getPrefKey(), lineEdit->text());
        }
    }
}

void PrefsDialog::pathChanged(QString const &path)
{
    PreferenceWidget const prefWidget(sender());

    if (prefWidget.is<PathButton>()) {
        setPref(prefWidget.getPrefKey(), path);
    }
}

void PrefsDialog::plainTextChanged()
{
    PreferenceWidget const prefWidget(sender());

    if (prefWidget.is<QPlainTextEdit>()) {
        QPlainTextEdit const *const plainTextEdit = prefWidget.as<QPlainTextEdit>();

        if (plainTextEdit->document()->isModified()) {
            myPrefs.set(prefWidget.getPrefKey(), plainTextEdit->toPlainText());
            // we avoid using setPref() because the included refreshPref() call would reset the cursor while we're editing
        }
    }
}

/***
****
***/

void PrefsDialog::initRemoteTab()
{
    linkWidgetToPref(ui.enableRpcCheck, Prefs::RPC_ENABLED);
    linkWidgetToPref(ui.rpcPortSpin, Prefs::RPC_PORT);
    linkWidgetToPref(ui.requireRpcAuthCheck, Prefs::RPC_AUTH_REQUIRED);
    linkWidgetToPref(ui.rpcUsernameEdit, Prefs::RPC_USERNAME);
    linkWidgetToPref(ui.rpcPasswordEdit, Prefs::RPC_PASSWORD);
    linkWidgetToPref(ui.enableRpcWhitelistCheck, Prefs::RPC_WHITELIST_ENABLED);
    linkWidgetToPref(ui.rpcWhitelistEdit, Prefs::RPC_WHITELIST);

    myWebWidgets << ui.rpcPortLabel << ui.rpcPortSpin << ui.requireRpcAuthCheck << ui.enableRpcWhitelistCheck;
    myWebAuthWidgets << ui.rpcUsernameLabel << ui.rpcUsernameEdit << ui.rpcPasswordLabel << ui.rpcPasswordEdit;
    myWebWhitelistWidgets << ui.rpcWhitelistLabel << ui.rpcWhitelistEdit;
    myUnsupportedWhenRemote << ui.enableRpcCheck << myWebWidgets << myWebAuthWidgets << myWebWhitelistWidgets;

    connect(ui.openWebClientButton, SIGNAL(clicked()), &mySession, SLOT(launchWebInterface()));
}

/***
****
***/

void PrefsDialog::altSpeedDaysEdited(int i)
{
    int const value = qobject_cast<QComboBox *>(sender())->itemData(i).toInt();
    setPref(Prefs::ALT_SPEED_LIMIT_TIME_DAY, value);
}

void PrefsDialog::initSpeedTab()
{
    QString const speed_K_str = Formatter::unitStr(Formatter::SPEED, Formatter::KB);
    QLocale const locale;

    ui.uploadSpeedLimitSpin->setSuffix(QString::fromLatin1(" %1").arg(speed_K_str));
    ui.downloadSpeedLimitSpin->setSuffix(QString::fromLatin1(" %1").arg(speed_K_str));
    ui.altUploadSpeedLimitSpin->setSuffix(QString::fromLatin1(" %1").arg(speed_K_str));
    ui.altDownloadSpeedLimitSpin->setSuffix(QString::fromLatin1(" %1").arg(speed_K_str));

    ui.altSpeedLimitDaysCombo->addItem(tr("Every Day"), QVariant(TR_SCHED_ALL));
    ui.altSpeedLimitDaysCombo->addItem(tr("Weekdays"), QVariant(TR_SCHED_WEEKDAY));
    ui.altSpeedLimitDaysCombo->addItem(tr("Weekends"), QVariant(TR_SCHED_WEEKEND));
    ui.altSpeedLimitDaysCombo->insertSeparator(ui.altSpeedLimitDaysCombo->count());

    for (int i = locale.firstDayOfWeek(); i <= Qt::Sunday; ++i) {
        ui.altSpeedLimitDaysCombo->addItem(qtDayName(i), qtDayToTrDay(i));
    }

    for (int i = Qt::Monday; i < locale.firstDayOfWeek(); ++i) {
        ui.altSpeedLimitDaysCombo->addItem(qtDayName(i), qtDayToTrDay(i));
    }

    ui.altSpeedLimitDaysCombo->setCurrentIndex(
        ui.altSpeedLimitDaysCombo->findData(myPrefs.getInt(Prefs::ALT_SPEED_LIMIT_TIME_DAY)));

    linkWidgetToPref(ui.uploadSpeedLimitCheck, Prefs::USPEED_ENABLED);
    linkWidgetToPref(ui.uploadSpeedLimitSpin, Prefs::USPEED);
    linkWidgetToPref(ui.downloadSpeedLimitCheck, Prefs::DSPEED_ENABLED);
    linkWidgetToPref(ui.downloadSpeedLimitSpin, Prefs::DSPEED);
    linkWidgetToPref(ui.altUploadSpeedLimitSpin, Prefs::ALT_SPEED_LIMIT_UP);
    linkWidgetToPref(ui.altDownloadSpeedLimitSpin, Prefs::ALT_SPEED_LIMIT_DOWN);
    linkWidgetToPref(ui.altSpeedLimitScheduleCheck, Prefs::ALT_SPEED_LIMIT_TIME_ENABLED);
    linkWidgetToPref(ui.altSpeedLimitStartTimeEdit, Prefs::ALT_SPEED_LIMIT_TIME_BEGIN);
    linkWidgetToPref(ui.altSpeedLimitEndTimeEdit, Prefs::ALT_SPEED_LIMIT_TIME_END);

    mySchedWidgets << ui.altSpeedLimitStartTimeEdit << ui.altSpeedLimitToLabel << ui.altSpeedLimitEndTimeEdit
                   << ui.altSpeedLimitDaysLabel << ui.altSpeedLimitDaysCombo;

    ColumnResizer *cr(new ColumnResizer(this));
    cr->addLayout(ui.speedLimitsSectionLayout);
    cr->addLayout(ui.altSpeedLimitsSectionLayout);
    cr->update();

    connect(ui.altSpeedLimitDaysCombo, SIGNAL(activated(int)), SLOT(altSpeedDaysEdited(int)));
}

/***
****
***/

void PrefsDialog::initDesktopTab()
{
    linkWidgetToPref(ui.showTrayIconCheck, Prefs::SHOW_TRAY_ICON);
    linkWidgetToPref(ui.startMinimizedCheck, Prefs::START_MINIMIZED);
    linkWidgetToPref(ui.notifyOnTorrentAddedCheck, Prefs::SHOW_NOTIFICATION_ON_ADD);
    linkWidgetToPref(ui.notifyOnTorrentCompletedCheck, Prefs::SHOW_NOTIFICATION_ON_COMPLETE);
    linkWidgetToPref(ui.playSoundOnTorrentCompletedCheck, Prefs::COMPLETE_SOUND_ENABLED);
}

/***
****
***/

void PrefsDialog::onPortTested(bool isOpen)
{
    ui.testPeerPortButton->setEnabled(true);
    myWidgets[Prefs::PEER_PORT]->setEnabled(true);
    ui.peerPortStatusLabel->setText(isOpen ? tr("Port is <b>open</b>") : tr("Port is <b>closed</b>"));
}

void PrefsDialog::onPortTest()
{
    ui.peerPortStatusLabel->setText(tr("Testing TCP Port..."));
    ui.testPeerPortButton->setEnabled(false);
    myWidgets[Prefs::PEER_PORT]->setEnabled(false);
    mySession.portTest();
}

void PrefsDialog::initNetworkTab()
{
    ui.torrentPeerLimitSpin->setRange(1, FD_SETSIZE);
    ui.globalPeerLimitSpin->setRange(1, FD_SETSIZE);

    linkWidgetToPref(ui.peerPortSpin, Prefs::PEER_PORT);
    linkWidgetToPref(ui.randomPeerPortCheck, Prefs::PEER_PORT_RANDOM_ON_START);
    linkWidgetToPref(ui.enablePortForwardingCheck, Prefs::PORT_FORWARDING);
    linkWidgetToPref(ui.torrentPeerLimitSpin, Prefs::PEER_LIMIT_TORRENT);
    linkWidgetToPref(ui.globalPeerLimitSpin, Prefs::PEER_LIMIT_GLOBAL);
    linkWidgetToPref(ui.enableUtpCheck, Prefs::UTP_ENABLED);
    linkWidgetToPref(ui.enablePexCheck, Prefs::PEX_ENABLED);
    linkWidgetToPref(ui.enableDhtCheck, Prefs::DHT_ENABLED);
    linkWidgetToPref(ui.enableLpdCheck, Prefs::LPD_ENABLED);
    linkWidgetToPref(ui.defaultTrackersPlainTextEdit, Prefs::DEFAULT_TRACKERS);

    ColumnResizer *cr(new ColumnResizer(this));
    cr->addLayout(ui.incomingPeersSectionLayout);
    cr->addLayout(ui.peerLimitsSectionLayout);
    cr->update();

    connect(ui.testPeerPortButton, SIGNAL(clicked()), SLOT(onPortTest()));
    connect(&mySession, SIGNAL(portTested(bool)), SLOT(onPortTested(bool)));
}

/***
****
***/

void PrefsDialog::onBlocklistDialogDestroyed(QObject *o)
{
    Q_UNUSED(o)

    myBlocklistDialog = nullptr;
}

void PrefsDialog::onUpdateBlocklistCancelled()
{
    disconnect(&mySession, SIGNAL(blocklistUpdated(int)), this, SLOT(onBlocklistUpdated(int)));
    myBlocklistDialog->deleteLater();
}

void PrefsDialog::onBlocklistUpdated(int n)
{
    myBlocklistDialog->setText(tr("<b>Update succeeded!</b><p>Blocklist now has %Ln rule(s).", nullptr, n));
    myBlocklistDialog->setTextFormat(Qt::RichText);
}

void PrefsDialog::onUpdateBlocklistClicked()
{
    myBlocklistDialog = new QMessageBox(
        QMessageBox::Information,
        QString(),
        tr("<b>Update Blocklist</b><p>Getting new blocklist..."),
        QMessageBox::Close,
        this);
    connect(myBlocklistDialog, SIGNAL(rejected()), this, SLOT(onUpdateBlocklistCancelled()));
    connect(&mySession, SIGNAL(blocklistUpdated(int)), this, SLOT(onBlocklistUpdated(int)));
    myBlocklistDialog->show();
    mySession.updateBlocklist();
}

void PrefsDialog::encryptionEdited(int i)
{
    int const value(qobject_cast<QComboBox *>(sender())->itemData(i).toInt());
    setPref(Prefs::ENCRYPTION, value);
}

void PrefsDialog::initPrivacyTab()
{
    ui.encryptionModeCombo->addItem(tr("Allow encryption"), 0);
    ui.encryptionModeCombo->addItem(tr("Prefer encryption"), 1);
    ui.encryptionModeCombo->addItem(tr("Require encryption"), 2);

    linkWidgetToPref(ui.encryptionModeCombo, Prefs::ENCRYPTION);
    linkWidgetToPref(ui.blocklistCheck, Prefs::BLOCKLIST_ENABLED);
    linkWidgetToPref(ui.blocklistEdit, Prefs::BLOCKLIST_URL);
    linkWidgetToPref(ui.autoUpdateBlocklistCheck, Prefs::BLOCKLIST_UPDATES_ENABLED);

    myBlockWidgets << ui.blocklistEdit << ui.blocklistStatusLabel << ui.updateBlocklistButton << ui.autoUpdateBlocklistCheck;

    ColumnResizer *cr(new ColumnResizer(this));
    cr->addLayout(ui.encryptionSectionLayout);
    cr->addLayout(ui.blocklistSectionLayout);
    cr->update();

    connect(ui.encryptionModeCombo, SIGNAL(activated(int)), SLOT(encryptionEdited(int)));
    connect(ui.updateBlocklistButton, SIGNAL(clicked()), SLOT(onUpdateBlocklistClicked()));

    updateBlocklistLabel();
}

/***
****
***/

void PrefsDialog::onIdleLimitChanged()
{
    //: Spin box suffix, "Stop seeding if idle for: [ 5 minutes ]" (includes leading space after the number, if needed)
    QString const unitsSuffix = tr(" minute(s)", nullptr, ui.idleLimitSpin->value());

    if (ui.idleLimitSpin->suffix() != unitsSuffix) {
        ui.idleLimitSpin->setSuffix(unitsSuffix);
    }
}

void PrefsDialog::initSeedingTab()
{
    linkWidgetToPref(ui.ratioLimitCheck, Prefs::RATIO_ENABLED);
    linkWidgetToPref(ui.ratioLimitSpin, Prefs::RATIO);
    linkWidgetToPref(ui.idleLimitCheck, Prefs::IDLE_LIMIT_ENABLED);
    linkWidgetToPref(ui.idleLimitSpin, Prefs::IDLE_LIMIT);

    connect(ui.idleLimitSpin, SIGNAL(valueChanged(int)), SLOT(onIdleLimitChanged()));

    onIdleLimitChanged();
}

void PrefsDialog::onQueueStalledMinutesChanged()
{
    //: Spin box suffix, "Download is inactive if data sharing stopped: [ 5 minutes ago ]" (includes leading space after the
    //: number, if needed)
    QString const unitsSuffix = tr(" minute(s) ago", nullptr, ui.queueStalledMinutesSpin->value());

    if (ui.queueStalledMinutesSpin->suffix() != unitsSuffix) {
        ui.queueStalledMinutesSpin->setSuffix(unitsSuffix);
    }
}

void PrefsDialog::initDownloadingTab()
{
    ui.watchDirButton->setMode(PathButton::DirectoryMode);
    ui.downloadDirButton->setMode(PathButton::DirectoryMode);
    ui.incompleteDirButton->setMode(PathButton::DirectoryMode);
    ui.completionScriptButton->setMode(PathButton::FileMode);

    ui.watchDirButton->setTitle(tr("Select Watch Directory"));
    ui.downloadDirButton->setTitle(tr("Select Destination"));
    ui.incompleteDirButton->setTitle(tr("Select Incomplete Directory"));
    ui.completionScriptButton->setTitle(tr("Select \"Torrent Done\" Script"));

    ui.watchDirStack->setMinimumWidth(200);

    ui.downloadDirFreeSpaceLabel->setSession(mySession);
    ui.downloadDirFreeSpaceLabel->setPath(myPrefs.getString(Prefs::DOWNLOAD_DIR));

    linkWidgetToPref(ui.watchDirCheck, Prefs::DIR_WATCH_ENABLED);
    linkWidgetToPref(ui.watchDirButton, Prefs::DIR_WATCH);
    linkWidgetToPref(ui.watchDirEdit, Prefs::DIR_WATCH);
    linkWidgetToPref(ui.showTorrentOptionsDialogCheck, Prefs::OPTIONS_PROMPT);
    linkWidgetToPref(ui.startAddedTorrentsCheck, Prefs::START);
    linkWidgetToPref(ui.trashTorrentFileCheck, Prefs::TRASH_ORIGINAL);
    linkWidgetToPref(ui.downloadDirButton, Prefs::DOWNLOAD_DIR);
    linkWidgetToPref(ui.downloadDirEdit, Prefs::DOWNLOAD_DIR);
    linkWidgetToPref(ui.downloadDirFreeSpaceLabel, Prefs::DOWNLOAD_DIR);
    linkWidgetToPref(ui.downloadQueueSizeSpin, Prefs::DOWNLOAD_QUEUE_SIZE);
    linkWidgetToPref(ui.queueStalledMinutesSpin, Prefs::QUEUE_STALLED_MINUTES);
    linkWidgetToPref(ui.renamePartialFilesCheck, Prefs::RENAME_PARTIAL_FILES);
    linkWidgetToPref(ui.incompleteDirCheck, Prefs::INCOMPLETE_DIR_ENABLED);
    linkWidgetToPref(ui.incompleteDirButton, Prefs::INCOMPLETE_DIR);
    linkWidgetToPref(ui.incompleteDirEdit, Prefs::INCOMPLETE_DIR);
    linkWidgetToPref(ui.completionScriptCheck, Prefs::SCRIPT_TORRENT_DONE_ENABLED);
    linkWidgetToPref(ui.completionScriptButton, Prefs::SCRIPT_TORRENT_DONE_FILENAME);
    linkWidgetToPref(ui.completionScriptEdit, Prefs::SCRIPT_TORRENT_DONE_FILENAME);

    ColumnResizer *cr(new ColumnResizer(this));
    cr->addLayout(ui.addingSectionLayout);
    cr->addLayout(ui.downloadQueueSectionLayout);
    cr->addLayout(ui.incompleteSectionLayout);
    cr->update();

    connect(ui.queueStalledMinutesSpin, SIGNAL(valueChanged(int)), SLOT(onQueueStalledMinutesChanged()));

    updateDownloadingWidgetsLocality();
    onQueueStalledMinutesChanged();
}

void PrefsDialog::updateDownloadingWidgetsLocality()
{
    ui.watchDirStack->setCurrentWidget(myIsLocal ? static_cast<QWidget *>(ui.watchDirButton) : ui.watchDirEdit);
    ui.downloadDirStack->setCurrentWidget(myIsLocal ? static_cast<QWidget *>(ui.downloadDirButton) : ui.downloadDirEdit);
    ui.incompleteDirStack->setCurrentWidget(myIsLocal ? static_cast<QWidget *>(ui.incompleteDirButton) : ui.incompleteDirEdit);
    ui.completionScriptStack->setCurrentWidget(
        myIsLocal ? static_cast<QWidget *>(ui.completionScriptButton) : ui.completionScriptEdit);

    ui.watchDirStack->setFixedHeight(ui.watchDirStack->currentWidget()->sizeHint().height());
    ui.downloadDirStack->setFixedHeight(ui.downloadDirStack->currentWidget()->sizeHint().height());
    ui.incompleteDirStack->setFixedHeight(ui.incompleteDirStack->currentWidget()->sizeHint().height());
    ui.completionScriptStack->setFixedHeight(ui.completionScriptStack->currentWidget()->sizeHint().height());

    ui.downloadDirLabel->setBuddy(ui.downloadDirStack->currentWidget());
}

/***
****
***/

PrefsDialog::PrefsDialog(Session &session, Prefs &prefs, QWidget *parent)
    : BaseDialog(parent)
    , mySession(session)
    , myPrefs(prefs)
    , myIsServer(session.isServer())
    , myIsLocal(mySession.isLocal())
{
    ui.setupUi(this);

    initSpeedTab();
    initDownloadingTab();
    initSeedingTab();
    initPrivacyTab();
    initNetworkTab();
    initDesktopTab();
    initRemoteTab();

    connect(&mySession, SIGNAL(sessionUpdated()), SLOT(sessionUpdated()));

    QList<int> keys;
    keys << Prefs::RPC_ENABLED << Prefs::ALT_SPEED_LIMIT_ENABLED << Prefs::ALT_SPEED_LIMIT_TIME_ENABLED << Prefs::ENCRYPTION
         << Prefs::BLOCKLIST_ENABLED << Prefs::DIR_WATCH << Prefs::DOWNLOAD_DIR << Prefs::INCOMPLETE_DIR
         << Prefs::INCOMPLETE_DIR_ENABLED << Prefs::SCRIPT_TORRENT_DONE_FILENAME;

    for (int const key : keys) {
        refreshPref(key);
    }

    // if it's a remote session, disable the preferences
    // that don't work in remote sessions
    if (!myIsServer) {
        for (QWidget *const w : myUnsupportedWhenRemote) {
            w->setToolTip(tr("Not supported by remote sessions"));
            w->setEnabled(false);
        }
    }

    adjustSize();
}

PrefsDialog::~PrefsDialog()
{}

void PrefsDialog::setPref(int key, QVariant const &v)
{
    myPrefs.set(key, v);
    refreshPref(key);
}

/***
****
***/

void PrefsDialog::sessionUpdated()
{
    bool const isLocal = mySession.isLocal();

    if (myIsLocal != isLocal) {
        myIsLocal = isLocal;
        updateDownloadingWidgetsLocality();
    }

    updateBlocklistLabel();
}

void PrefsDialog::updateBlocklistLabel()
{
    int const n = mySession.blocklistSize();
    ui.blocklistStatusLabel->setText(tr("<i>Blocklist contains %Ln rule(s)</i>", nullptr, n));
}

void PrefsDialog::refreshPref(int key)
{
    switch (key) {
    case Prefs::RPC_ENABLED:
    case Prefs::RPC_WHITELIST_ENABLED:
    case Prefs::RPC_AUTH_REQUIRED:
        {
            bool const enabled(myPrefs.getBool(Prefs::RPC_ENABLED));
            bool const whitelist(myPrefs.getBool(Prefs::RPC_WHITELIST_ENABLED));
            bool const auth(myPrefs.getBool(Prefs::RPC_AUTH_REQUIRED));

            for (QWidget *const w : myWebWhitelistWidgets) {
                w->setEnabled(enabled && whitelist);
            }

            for (QWidget *const w : myWebAuthWidgets) {
                w->setEnabled(enabled && auth);
            }

            for (QWidget *const w : myWebWidgets) {
                w->setEnabled(enabled);
            }

            break;
        }

    case Prefs::ALT_SPEED_LIMIT_TIME_ENABLED:
        {
            bool const enabled = myPrefs.getBool(key);

            for (QWidget *const w : mySchedWidgets) {
                w->setEnabled(enabled);
            }

            break;
        }

    case Prefs::BLOCKLIST_ENABLED:
        {
            bool const enabled = myPrefs.getBool(key);

            for (QWidget *const w : myBlockWidgets) {
                w->setEnabled(enabled);
            }

            break;
        }

    case Prefs::PEER_PORT:
        ui.peerPortStatusLabel->setText(tr("Status unknown"));
        ui.testPeerPortButton->setEnabled(true);
        break;

    default:
        break;
    }

    key2widget_t::iterator it(myWidgets.find(key));

    if (it != myWidgets.end()) {
        QWidget *w(it.value());

        w->blockSignals(true);

        if (!updateWidgetValue(w, key)) {
            if (key == Prefs::ENCRYPTION) {
                QComboBox *comboBox(qobject_cast<QComboBox *>(w));
                int const index = comboBox->findData(myPrefs.getInt(key));
                comboBox->setCurrentIndex(index);
            }
        }

        w->blockSignals(false);
    }
}
