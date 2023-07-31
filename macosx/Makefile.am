EXTRA_DIST = \
    AboutWindow.xib \
    AboutWindowController.h \
    AboutWindowController.m \
    AddMagnetWindowController.h \
    AddMagnetWindowController.m \
    AddWindowController.h \
    AddWindowController.m \
    BadgeView.h \
    BadgeView.m \
    Badger.h \
    Badger.m \
    BlocklistDownloader.h \
    BlocklistDownloader.m \
    BlocklistDownloaderViewController.h \
    BlocklistDownloaderViewController.m \
    BlocklistScheduler.h \
    BlocklistScheduler.m \
    BlocklistStatusWindow.xib \
    BonjourController.h \
    BonjourController.m \
    ButtonToolbarItem.h \
    ButtonToolbarItem.m \
    CMakeLists.txt \
    ColorTextField.h \
    ColorTextField.m \
    Controller.h \
    Controller.m \
    CreatorWindowController.h \
    CreatorWindowController.m \
    Credits.rtf \
    Defaults.plist \
    DragOverlayView.h \
    DragOverlayView.m \
    DragOverlayWindow.h \
    DragOverlayWindow.m \
    ExpandedPathToIconTransformer.h \
    ExpandedPathToIconTransformer.m \
    ExpandedPathToPathTransformer.h \
    ExpandedPathToPathTransformer.m \
    FileListNode.h \
    FileListNode.m \
    FileNameCell.h \
    FileNameCell.m \
    FileOutlineController.h \
    FileOutlineController.m \
    FileOutlineView.h \
    FileOutlineView.m \
    FilePriorityCell.h \
    FilePriorityCell.m \
    FileRenameSheetController.h \
    FileRenameSheetController.m \
    FileRenameSheetController.xib \
    FilterBar.xib \
    FilterBarController.h \
    FilterBarController.m \
    FilterBarView.h \
    FilterBarView.m \
    FilterButton.h \
    FilterButton.m \
    GlobalOptionsPopoverViewController.h \
    GlobalOptionsPopoverViewController.m \
    GroupToolbarItem.h \
    GroupToolbarItem.m \
    GroupsController.h \
    GroupsController.m \
    GroupsPrefsController.h \
    GroupsPrefsController.m \
    Images \
    Info.plist \
    Info.plist.in \
    InfoActivityViewController.h \
    InfoActivityViewController.m \
    InfoFileView.xib \
    InfoFileViewController.h \
    InfoFileViewController.m \
    InfoGeneralViewController.h \
    InfoGeneralViewController.m \
    InfoOptionsViewController.h \
    InfoOptionsViewController.m \
    InfoPeersView.xib \
    InfoPeersViewController.h \
    InfoPeersViewController.m \
    InfoTabButtonBack.h \
    InfoTabButtonBack.m \
    InfoTabButtonCell.h \
    InfoTabButtonCell.m \
    InfoTextField.h \
    InfoTextField.m \
    InfoTrackersView.xib \
    InfoTrackersViewController.h \
    InfoTrackersViewController.m \
    InfoViewController.h \
    InfoWindow.xib \
    InfoWindowController.h \
    InfoWindowController.m \
    MessageWindow.xib \
    MessageWindowController.h \
    MessageWindowController.m \
    NSApplicationAdditions.h \
    NSApplicationAdditions.m \
    NSImageAdditions.h \
    NSImageAdditions.m \
    NSMutableArrayAdditions.h \
    NSMutableArrayAdditions.m \
    NSStringAdditions.h \
    NSStringAdditions.m \
    PeerProgressIndicatorCell.h \
    PeerProgressIndicatorCell.m \
    PeerTableView.h \
    PeerTableView.m \
    PiecesView.h \
    PiecesView.m \
    PortChecker.h \
    PortChecker.m \
    PredicateEditorRowTemplateAny.h \
    PredicateEditorRowTemplateAny.m \
    PrefsController.h \
    PrefsController.m \
    PrefsWindow.h \
    PrefsWindow.m \
    ProgressGradients.h \
    ProgressGradients.m \
    QuickLookPlugin \
    ShareToolbarItem.h \
    ShareToolbarItem.m \
    ShareTorrentFileHelper.h \
    ShareTorrentFileHelper.m \
    Sparkle.framework \
    StatsWindow.xib \
    StatsWindowController.h \
    StatsWindowController.m \
    StatusBar.xib \
    StatusBarController.h \
    StatusBarController.m \
    StatusBarView.h \
    StatusBarView.m \
    ToolbarSegmentedCell.h \
    ToolbarSegmentedCell.m \
    Torrent.h \
    Torrent.m \
    TorrentCell.h \
    TorrentCell.m \
    TorrentGroup.h \
    TorrentGroup.m \
    TorrentTableView.h \
    TorrentTableView.m \
    TrackerCell.h \
    TrackerCell.m \
    TrackerNode.h \
    TrackerNode.m \
    TrackerTableView.h \
    TrackerTableView.m \
    TransmissionHelp \
    Transmission_Prefix.pch \
    URLSheetWindow.xib \
    URLSheetWindowController.h \
    URLSheetWindowController.m \
    VDKQueue \
    WebSeedTableView.h \
    WebSeedTableView.m \
    da.lproj \
    de.lproj \
    en.lproj \
    es.lproj \
    fr.lproj \
    it.lproj \
    main.m \
    nl.lproj \
    pt_PT.lproj \
    ru.lproj \
    sparkle_dsa_pub.pem \
    tr.lproj

if BUILD_MAC
# Use Xcode do make the actual build
all:
	@(cd .. && xcodebuild -alltargets -activeconfiguration | grep -v "^$$")
clean-local:
	@(cd .. && xcodebuild -alltargets -activeconfiguration clean | grep -v "^$$")
endif

define PACKAGE_RULE1
	$(RM) tmp "TransmissionOG $(VERSION_PACKAGE)" \
	  TransmissionOG-$(VERSION_PACKAGE).dmg
	mkdir -p tmp
	cp -R macosx/TransmissionOG.app tmp/
endef
define PACKAGE_RULE2
	mv tmp "TransmissionOG $(VERSION_PACKAGE)"
	hdiutil create -format UDZO -imagekey zlib-level=9 -srcfolder \
	  "TransmissionOG $(VERSION_PACKAGE)" TransmissionOG-$(VERSION_PACKAGE).dmg
	rm -rf "TransmissionOG $(VERSION_PACKAGE)"
endef

package:
	$(PACKAGE_RULE1)
	$(PACKAGE_RULE2)

package-release:
	$(PACKAGE_RULE1)
	strip -S tmp/TransmissionOG.app/Contents/MacOS/TransmissionOG
	$(PACKAGE_RULE2)
