/***************************************************************************
    File                 : ImportProjectDialog.cpp
    Project              : LabPlot
    Description          : import project dialog
    --------------------------------------------------------------------
    Copyright            : (C) 2017 Alexander Semke (alexander.semke@web.de)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "ImportProjectDialog.h"
#include "backend/core/AspectTreeModel.h"
#include "backend/core/Project.h"
#include "kdefrontend/MainWin.h"
#include "commonfrontend/widgets/TreeViewComboBox.h"

#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressBar>
#include <QStatusBar>

#include <KLocale>
#include <KWindowConfig>

/*!
    \class ImportProjectDialog
    \brief Dialog for importing project files.

	\ingroup kdefrontend
 */
ImportProjectDialog::ImportProjectDialog(MainWin* parent, ProjectType type) : QDialog(parent),
	m_mainWin(parent),
	m_projectType(type),
	m_aspectTreeModel(new AspectTreeModel(parent->project()) ){

	QVBoxLayout* vLayout = new QVBoxLayout(this);

	//main widget
	QWidget* mainWidget = new QWidget(this);
	ui.setupUi(mainWidget);
	vLayout->addWidget(mainWidget);

	ui.bOpen->setIcon( QIcon::fromTheme("document-open") );

	m_cbAddTo = new TreeViewComboBox(ui.gbImportTo);
	m_cbAddTo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	ui.gbImportTo->layout()->addWidget(m_cbAddTo);

	QList<const char*> list;
	list << "Folder";
	m_cbAddTo->setTopLevelClasses(list);
	m_aspectTreeModel->setSelectableAspects(list);
	m_cbAddTo->setModel(m_aspectTreeModel);

	m_bNewFolder = new QPushButton(ui.gbImportTo);
	m_bNewFolder->setIcon(QIcon::fromTheme("list-add"));
	ui.gbImportTo->layout()->addWidget(m_bNewFolder);

	//dialog buttons
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false); //ok is only available if some project objects were selected
	vLayout->addWidget(m_buttonBox);

	//Signals/Slots
	connect(ui.leFileName, SIGNAL(textChanged(QString)), SLOT(fileNameChanged(QString)));
	connect(ui.bOpen, SIGNAL(clicked()), this, SLOT (selectFile()));
	connect(m_bNewFolder, SIGNAL(clicked()), this, SLOT(newFolder()));
	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QString title;
	QString lastImportedFile;
	switch (m_projectType) {
	case (ProjectLabPlot):
		title = i18n("Import LabPlot Project");
		lastImportedFile = QLatin1String("LastImportedLabPlotProject");
		break;
	case (ProjectOrigin):
		title = i18n("Import Origin Project");
		lastImportedFile = QLatin1String("LastImportedOriginProject");
		break;
	}

	//dialog title and icon
	setWindowTitle(title);
	setWindowIcon(QIcon::fromTheme("document-import"));

	//restore saved settings
	KConfigGroup conf(KSharedConfig::openConfig(), "ImportProjectDialog");
	KWindowConfig::restoreWindowSize(windowHandle(), conf);
	ui.leFileName->setText(conf.readEntry(lastImportedFile, ""));
}

ImportProjectDialog::~ImportProjectDialog() {
	//save current settings
	KConfigGroup conf(KSharedConfig::openConfig(), "ImportProjectDialog");
	KWindowConfig::saveWindowSize(windowHandle(), conf);

	QString lastImportedFile;
	switch (m_projectType) {
	case (ProjectLabPlot):
		lastImportedFile = QLatin1String("LastImportedLabPlotProject");
		break;
	case (ProjectOrigin):
		lastImportedFile = QLatin1String("LastImportedOriginProject");
		break;
	}

	conf.writeEntry(lastImportedFile, ui.leFileName->text());
}

void ImportProjectDialog::setCurrentFolder(const Folder* folder) {
	m_cbAddTo->setCurrentModelIndex(m_aspectTreeModel->modelIndexOfAspect(folder));
}

void ImportProjectDialog::importTo(QStatusBar* statusBar) const {
	DEBUG("ImportProjectDialog::import()");

	//TODO: determine the folder where the project objects have to be imported into
	QString folderPath;

	//show a progress bar in the status bar
	QProgressBar* progressBar = new QProgressBar();
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);

	statusBar->clearMessage();
	statusBar->addWidget(progressBar, 1);

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents(QEventLoop::AllEvents, 100);

	QTime timer;
	timer.start();

	//TODO ProjectParser* projectParser = nullptr;
// 	switch (m_projectType) {
// 	case (ProjectLabPlot)
// 		projectParser = new LabPlotProjectParser()
// 		break;
// 	case (ProjectOrigin):
// 		projectParser = new OriginProjectParser()
// 		break;
// 	}

	//import the selected project objects into the specified folder
	//connect(projectParser, SIGNAL(completed(int)), progressBar, SLOT(setValue(int)));
	//projectParser->importTo(folderPath);
	statusBar->showMessage( i18n("Project data imported in %1 seconds.", (float)timer.elapsed()/1000) );

	QApplication::restoreOverrideCursor();
	statusBar->removeWidget(progressBar);
}

void ImportProjectDialog::refreshPreview() {
	QString project = ui.leFileName->text();

	//show some general information about the project (version, creation time, author, etc.)
	//TODO
	QString info;// = projectParser->info(project);
	ui.lProjectInfo->setText(info);

	//show the content of the project
	//TODO
}


//##############################################################################
//#################################  SLOTS  ####################################
//##############################################################################
void ImportProjectDialog::selectionChanged() {
	//TODO:
	//determine the dependent objects and select/deselect them too

	//Ok-button is only available if some project objects were selected
	bool objectsSelected = false; //TODO
	m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(objectsSelected);
}

/*!
	opens a file dialog and lets the user select the project file.
*/
void ImportProjectDialog::selectFile() {
	KConfigGroup conf(KSharedConfig::openConfig(), "ImportProjectWidget");

	QString title;
	QString lastDir;
	QString supportedFormats;
	switch (m_projectType) {
	case (ProjectLabPlot):
		title = i18n("Open LabPlot Project");
		lastDir = conf.readEntry(QLatin1String("LastImportLabPlotProjectDir"), "");
		supportedFormats = i18n("LabPlot Projects (*.lml *.lml.gz *.lml.bz2 *.lml.xz *.LML *.LML.GZ *.LML.BZ2 *.LML.XZ)");
		break;
	case (ProjectOrigin):
		title = i18n("Open Origin Project");
		lastDir = conf.readEntry(QLatin1String("LastImportOriginPlojecttDir"), "");
		supportedFormats = i18n("Origin Projects (*.opj *.OPJ)");
		break;
	}

	QString path = QFileDialog::getOpenFileName(this, title, lastDir, supportedFormats);
	if (path.isEmpty())
		return; //cancel was clicked in the file-dialog

	int pos = path.lastIndexOf(QDir::separator());
	if (pos != -1) {
		QString newDir = path.left(pos);
		if (newDir != lastDir)
			conf.writeEntry(lastDir, newDir);
	}

	ui.leFileName->setText(path);
	refreshPreview();
}

void ImportProjectDialog::fileNameChanged(const QString& name) {
	QString fileName = name;
#ifndef HAVE_WINDOWS
	// make relative path
	if ( !fileName.isEmpty() && fileName.left(1) != QDir::separator())
		fileName = QDir::homePath() + QDir::separator() + fileName;
#endif

	bool fileExists = QFile::exists(fileName);
	if (fileExists)
		ui.leFileName->setStyleSheet("");
	else
		ui.leFileName->setStyleSheet("QLineEdit{background:red;}");

	if (!fileExists) {
		//file doesn't exist -> delete the content preview that is still potentially
		//available from the previously selected file
		ui.tvPreview->setModel(nullptr);
		m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		return;
	}

	refreshPreview();
}

void ImportProjectDialog::newFolder() {
	QString path = ui.leFileName->text();
	QString name = path.right( path.length()-path.lastIndexOf(QDir::separator())-1 );

	bool ok;
	QInputDialog* dlg = new QInputDialog(this);
	name = dlg->getText(this, i18n("Add new folder"), i18n("Folder name:"), QLineEdit::Normal, name, &ok);
	if (ok) {
		Folder* folder = new Folder(name);
		m_mainWin->addAspectToProject(folder);
		m_cbAddTo->setCurrentModelIndex(m_mainWin->model()->modelIndexOfAspect(folder));
	}

	delete dlg;
}
