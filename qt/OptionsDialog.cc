/*
 * This file Copyright (C) 2009-2015 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 */

#include <QFileInfo>
#include <QPushButton>

#include <libtransmission/transmission.h>
#include <libtransmission/utils.h> /* mime64 */
#include <libtransmission/variant.h>

#include "AddData.h"
#include "FreeSpaceLabel.h"
#include "OptionsDialog.h"
#include "Prefs.h"
#include "Session.h"
#include "Torrent.h"
#include "Utils.h"

/***
****
***/

OptionsDialog::OptionsDialog(Session &session, Prefs const &prefs, AddData const &addme, QWidget *parent)
    : BaseDialog(parent)
    , mySession(session)
    , myAdd(addme)
    , myIsLocal(mySession.isLocal())
    , myHaveInfo(false)
    , myVerifyButton(new QPushButton(tr("&Verify Local Data"), this))
    , myVerifyFile(nullptr)
    , myVerifyHash(QCryptographicHash::Sha1)
    , myEditTimer(this)
{
    ui.setupUi(this);

    QString title;

    if (myAdd.type == AddData::FILENAME) {
        title = tr("Open Torrent from File");
    } else {
        title = tr("Open Torrent from URL or Magnet Link");
    }

    setWindowTitle(title);

    myEditTimer.setInterval(2000);
    myEditTimer.setSingleShot(true);
    connect(&myEditTimer, SIGNAL(timeout()), this, SLOT(onDestinationChanged()));

    if (myAdd.type == AddData::FILENAME) {
        ui.sourceStack->setCurrentWidget(ui.sourceButton);
        ui.sourceButton->setMode(PathButton::FileMode);
        ui.sourceButton->setTitle(tr("Open Torrent"));
        ui.sourceButton->setNameFilter(tr("Torrent Files (*.torrent);;All Files (*.*)"));
        ui.sourceButton->setPath(myAdd.filename);
        connect(ui.sourceButton, SIGNAL(pathChanged(QString)), this, SLOT(onSourceChanged()));
    } else {
        ui.sourceStack->setCurrentWidget(ui.sourceEdit);
        ui.sourceEdit->setText(myAdd.readableName());
        ui.sourceEdit->selectAll();
        connect(ui.sourceEdit, SIGNAL(editingFinished()), this, SLOT(onSourceChanged()));
    }

    ui.sourceStack->setFixedHeight(ui.sourceStack->currentWidget()->sizeHint().height());
    ui.sourceLabel->setBuddy(ui.sourceStack->currentWidget());

    QFontMetrics const fontMetrics(font());
    int const width = fontMetrics.size(0, QString::fromUtf8("This is a pretty long torrent filename indeed.torrent")).width();
    ui.sourceStack->setMinimumWidth(width);

    QString const downloadDir(Utils::removeTrailingDirSeparator(prefs.getString(Prefs::DOWNLOAD_DIR)));
    ui.freeSpaceLabel->setSession(mySession);
    ui.freeSpaceLabel->setPath(downloadDir);

    ui.destinationButton->setMode(PathButton::DirectoryMode);
    ui.destinationButton->setTitle(tr("Select Destination"));
    ui.destinationButton->setPath(downloadDir);
    ui.destinationEdit->setText(downloadDir);

    if (myIsLocal) {
        myLocalDestination = downloadDir;
    }

    connect(ui.destinationButton, SIGNAL(pathChanged(QString)), this, SLOT(onDestinationChanged()));
    connect(ui.destinationEdit, SIGNAL(textEdited(QString)), &myEditTimer, SLOT(start()));
    connect(ui.destinationEdit, SIGNAL(editingFinished()), this, SLOT(onDestinationChanged()));

    ui.filesView->setEditable(false);

    ui.priorityCombo->addItem(tr("High"), TR_PRI_HIGH);
    ui.priorityCombo->addItem(tr("Normal"), TR_PRI_NORMAL);
    ui.priorityCombo->addItem(tr("Low"), TR_PRI_LOW);
    ui.priorityCombo->setCurrentIndex(1); // Normal

    ui.dialogButtons->addButton(myVerifyButton, QDialogButtonBox::ActionRole);
    connect(myVerifyButton, SIGNAL(clicked(bool)), this, SLOT(onVerify()));

    ui.startCheck->setChecked(prefs.getBool(Prefs::START));
    ui.trashCheck->setChecked(prefs.getBool(Prefs::TRASH_ORIGINAL));

    connect(ui.dialogButtons, SIGNAL(rejected()), this, SLOT(deleteLater()));
    connect(ui.dialogButtons, SIGNAL(accepted()), this, SLOT(onAccepted()));

    connect(ui.filesView, SIGNAL(priorityChanged(QSet<int>, int)), this, SLOT(onPriorityChanged(QSet<int>, int)));
    connect(ui.filesView, SIGNAL(wantedChanged(QSet<int>, bool)), this, SLOT(onWantedChanged(QSet<int>, bool)));

    connect(&myVerifyTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    connect(&mySession, SIGNAL(sessionUpdated()), SLOT(onSessionUpdated()));

    updateWidgetsLocality();
    reload();
}

OptionsDialog::~OptionsDialog()
{
    clearInfo();
}

/***
****
***/

void OptionsDialog::clearInfo()
{
    if (myHaveInfo) {
        tr_metainfoFree(&myInfo);
    }

    myHaveInfo = false;
    myFiles.clear();
}

void OptionsDialog::reload()
{
    clearInfo();
    clearVerify();

    tr_ctor *ctor = tr_ctorNew(nullptr);

    switch (myAdd.type) {
    case AddData::MAGNET:
        tr_ctorSetMetainfoFromMagnetLink(ctor, myAdd.magnet.toUtf8().constData());
        break;

    case AddData::FILENAME:
        tr_ctorSetMetainfoFromFile(ctor, myAdd.filename.toUtf8().constData());
        break;

    case AddData::METAINFO:
        tr_ctorSetMetainfo(ctor, reinterpret_cast<quint8 const *>(myAdd.metainfo.constData()), myAdd.metainfo.size());
        break;

    default:
        break;
    }

    int const err = tr_torrentParse(ctor, &myInfo);
    myHaveInfo = err == 0;
    tr_ctorFree(ctor);

    ui.filesView->clear();
    myFiles.clear();
    myPriorities.clear();
    myWanted.clear();

    bool const haveFilesToShow = myHaveInfo && myInfo.fileCount > 0;

    ui.filesView->setVisible(haveFilesToShow);
    myVerifyButton->setEnabled(haveFilesToShow);
    layout()->setSizeConstraint(haveFilesToShow ? QLayout::SetDefaultConstraint : QLayout::SetFixedSize);

    if (myHaveInfo) {
        myPriorities.insert(0, myInfo.fileCount, TR_PRI_NORMAL);
        myWanted.insert(0, myInfo.fileCount, true);

        for (tr_file_index_t i = 0; i < myInfo.fileCount; ++i) {
            TorrentFile file;
            file.index = i;
            file.priority = myPriorities[i];
            file.wanted = myWanted[i];
            file.size = myInfo.files[i].length;
            file.have = 0;
            file.filename = QString::fromUtf8(myInfo.files[i].name);
            myFiles.append(file);
        }
    }

    ui.filesView->update(myFiles);
}

void OptionsDialog::updateWidgetsLocality()
{
    ui.destinationStack->setCurrentWidget(myIsLocal ? static_cast<QWidget *>(ui.destinationButton) : ui.destinationEdit);
    ui.destinationStack->setFixedHeight(ui.destinationStack->currentWidget()->sizeHint().height());
    ui.destinationLabel->setBuddy(ui.destinationStack->currentWidget());

    // hide the % done when non-local, since we've no way of knowing
    (ui.filesView->*(myIsLocal ? &QTreeView::showColumn : &QTreeView::hideColumn))(2);

    myVerifyButton->setVisible(myIsLocal);
}

void OptionsDialog::onSessionUpdated()
{
    bool const isLocal = mySession.isLocal();

    if (myIsLocal != isLocal) {
        myIsLocal = isLocal;
        updateWidgetsLocality();
    }
}

void OptionsDialog::onPriorityChanged(QSet<int> const &fileIndices, int priority)
{
    for (int const i : fileIndices) {
        myPriorities[i] = priority;
    }
}

void OptionsDialog::onWantedChanged(QSet<int> const &fileIndices, bool isWanted)
{
    for (int const i : fileIndices) {
        myWanted[i] = isWanted;
    }
}

void OptionsDialog::onAccepted()
{
    // rpc spec section 3.4 "adding a torrent"

    tr_variant args;
    tr_variantInitDict(&args, 10);
    QString downloadDir;

    // "download-dir"
    if (ui.destinationStack->currentWidget() == ui.destinationButton) {
        downloadDir = myLocalDestination.absolutePath();
    } else {
        downloadDir = ui.destinationEdit->text();
    }

    tr_variantDictAddStr(&args, TR_KEY_download_dir, downloadDir.toUtf8().constData());

    // paused
    tr_variantDictAddBool(&args, TR_KEY_paused, !ui.startCheck->isChecked());

    // priority
    int const index = ui.priorityCombo->currentIndex();
    int const priority = ui.priorityCombo->itemData(index).toInt();
    tr_variantDictAddInt(&args, TR_KEY_bandwidthPriority, priority);

    // files-unwanted
    int count = myWanted.count(false);

    if (count > 0) {
        tr_variant *l = tr_variantDictAddList(&args, TR_KEY_files_unwanted, count);

        for (int i = 0, n = myWanted.size(); i < n; ++i) {
            if (myWanted.at(i) == false) {
                tr_variantListAddInt(l, i);
            }
        }
    }

    // priority-low
    count = myPriorities.count(TR_PRI_LOW);

    if (count > 0) {
        tr_variant *l = tr_variantDictAddList(&args, TR_KEY_priority_low, count);

        for (int i = 0, n = myPriorities.size(); i < n; ++i) {
            if (myPriorities.at(i) == TR_PRI_LOW) {
                tr_variantListAddInt(l, i);
            }
        }
    }

    // priority-high
    count = myPriorities.count(TR_PRI_HIGH);

    if (count > 0) {
        tr_variant *l = tr_variantDictAddList(&args, TR_KEY_priority_high, count);

        for (int i = 0, n = myPriorities.size(); i < n; ++i) {
            if (myPriorities.at(i) == TR_PRI_HIGH) {
                tr_variantListAddInt(l, i);
            }
        }
    }

    mySession.addTorrent(myAdd, &args, ui.trashCheck->isChecked());

    deleteLater();
}

void OptionsDialog::onSourceChanged()
{
    if (ui.sourceStack->currentWidget() == ui.sourceButton) {
        myAdd.set(ui.sourceButton->path());
    } else {
        myAdd.set(ui.sourceEdit->text());
    }

    reload();
}

void OptionsDialog::onDestinationChanged()
{
    if (ui.destinationStack->currentWidget() == ui.destinationButton) {
        myLocalDestination = ui.destinationButton->path();
        ui.freeSpaceLabel->setPath(myLocalDestination.absolutePath());
    } else {
        ui.freeSpaceLabel->setPath(ui.destinationEdit->text());
    }
}

/***
****
****  VERIFY
****
***/

void OptionsDialog::clearVerify()
{
    myVerifyHash.reset();
    myVerifyFile.close();
    myVerifyFilePos = 0;
    myVerifyFlags.clear();
    myVerifyFileIndex = 0;
    myVerifyPieceIndex = 0;
    myVerifyPiecePos = 0;
    myVerifyTimer.stop();

    for (TorrentFile &f : myFiles) {
        f.have = 0;
    }

    ui.filesView->update(myFiles);
}

void OptionsDialog::onVerify()
{
    clearVerify();
    myVerifyFlags.insert(0, myInfo.pieceCount, false);
    myVerifyTimer.setSingleShot(false);
    myVerifyTimer.start(0);
}

namespace {

uint64_t getPieceSize(tr_info const *info, tr_piece_index_t pieceIndex)
{
    if (pieceIndex != info->pieceCount - 1) {
        return info->pieceSize;
    }

    return info->totalSize % info->pieceSize;
}

} // namespace

void OptionsDialog::onTimeout()
{
    if (myFiles.isEmpty()) {
        myVerifyTimer.stop();
        return;
    }

    tr_file const *file = &myInfo.files[myVerifyFileIndex];

    if (myVerifyFilePos == 0 && !myVerifyFile.isOpen()) {
        QFileInfo const fileInfo(myLocalDestination, QString::fromUtf8(file->name));
        myVerifyFile.setFileName(fileInfo.absoluteFilePath());
        myVerifyFile.open(QIODevice::ReadOnly);
    }

    int64_t leftInPiece = getPieceSize(&myInfo, myVerifyPieceIndex) - myVerifyPiecePos;
    int64_t leftInFile = file->length - myVerifyFilePos;
    int64_t bytesThisPass = qMin(leftInFile, leftInPiece);
    bytesThisPass = qMin(bytesThisPass, static_cast<int64_t>(sizeof(myVerifyBuf)));

    if (myVerifyFile.isOpen() && myVerifyFile.seek(myVerifyFilePos)) {
        int64_t numRead = myVerifyFile.read(myVerifyBuf, bytesThisPass);

        if (numRead == bytesThisPass) {
            myVerifyHash.addData(myVerifyBuf, numRead);
        }
    }

    leftInPiece -= bytesThisPass;
    leftInFile -= bytesThisPass;
    myVerifyPiecePos += bytesThisPass;
    myVerifyFilePos += bytesThisPass;

    myVerifyBins[myVerifyFileIndex] += bytesThisPass;

    if (leftInPiece == 0) {
        QByteArray const result(myVerifyHash.result());
        bool const matches = memcmp(result.constData(), myInfo.pieces[myVerifyPieceIndex].hash, SHA_DIGEST_LENGTH) == 0;
        myVerifyFlags[myVerifyPieceIndex] = matches;
        myVerifyPiecePos = 0;
        ++myVerifyPieceIndex;
        myVerifyHash.reset();

        FileList changedFiles;

        if (matches) {
            for (auto i = myVerifyBins.begin(), end = myVerifyBins.end(); i != end; ++i) {
                TorrentFile &f(myFiles[i.key()]);
                f.have += i.value();
                changedFiles.append(f);
            }
        }

        ui.filesView->update(changedFiles);
        myVerifyBins.clear();
    }

    if (leftInFile == 0) {
        myVerifyFile.close();
        ++myVerifyFileIndex;
        myVerifyFilePos = 0;
    }

    bool done = myVerifyPieceIndex >= myInfo.pieceCount;

    if (done) {
        uint64_t have = 0;

        for (TorrentFile const &f : myFiles) {
            have += f.have;
        }

        if (have == 0) // everything failed
        {
            // did the user accidentally specify the child directory instead of the parent?
            QStringList const tokens = QString::fromUtf8(file->name).split(QLatin1Char('/'));

            if (!tokens.empty() && myLocalDestination.dirName() == tokens.at(0)) {
                // move up one directory and try again
                myLocalDestination.cdUp();
                onVerify();
                done = false;
            }
        }
    }

    if (done) {
        myVerifyTimer.stop();
    }
}
