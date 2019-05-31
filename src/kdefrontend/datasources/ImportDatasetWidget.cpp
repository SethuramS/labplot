/***************************************************************************
File                 : ImportDatasetWidget.cpp
Project              : LabPlot
Description          : import online dataset widget
--------------------------------------------------------------------
Copyright            : (C) 2019 Kovacs Ferencz (kferike98@gmail.com)

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

#include "ImportDatasetWidget.h"
#include "DatasetMetadataManagerDialog.h"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonObject"
#include "QJsonValue"
#include "QStandardPaths"
#include "QFile"
#include "QDebug"
#include "QTreeWidget"
#include "backend/datasources/DatasetHandler.h"
#include "QMessageBox"
#include "QDir"

ImportDatasetWidget::ImportDatasetWidget(QWidget* parent) : QWidget(parent),
	m_categoryCompleter(new QCompleter),
	m_datasetCompleter(new QCompleter)
{
	QString baseDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
	QString containingDir = ".labplot";
	m_jsonDir=baseDir + QDir::separator() + containingDir + QDir::separator();
	ui.setupUi(this);

	if(!QFile(m_jsonDir + "DatasetCategories.json").exists())
		downloadCategoryFile();
	loadDatasetCategoriesFromJson();

	ui.lwDatasets->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.twCategories->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(ui.twCategories, &QTreeWidget::itemDoubleClicked, this, &ImportDatasetWidget::listDatasetsForSubcategory);
	connect(ui.leSearchDatasets, &QLineEdit::textChanged, this, &ImportDatasetWidget::scrollToDatasetListItem);
	connect(ui.leSearchCategories, &QLineEdit::textChanged, this, &ImportDatasetWidget::scrollToCategoryTreeItem);
	connect(ui.bRefresh, &QPushButton::clicked, this, &ImportDatasetWidget::loadDatasetCategoriesFromJson);
	connect(ui.bNewDataset, &QPushButton::clicked, this, &ImportDatasetWidget::showDatasetMetadataManager);
	connect(ui.lwDatasets, &QListWidget::itemSelectionChanged, [this]() {
		emit datasetSelected();
	});
}

ImportDatasetWidget::~ImportDatasetWidget() {
	if(m_categoryCompleter != nullptr)
		delete m_categoryCompleter;
	if(m_datasetCompleter != nullptr)
		delete m_datasetCompleter;
}

QString ImportDatasetWidget::locateCategoryJsonFile() {
	qDebug() << "Locating category file" << QStandardPaths::locate(QStandardPaths::AppDataLocation, "datasets/DatasetCategories.json");
	return QStandardPaths::locate(QStandardPaths::AppDataLocation, "datasets/DatasetCategories.json");
}

void ImportDatasetWidget::loadDatasetCategoriesFromJson() {
	ui.lwDatasets->clear();
	ui.twCategories->clear();
	m_datasetsMap.clear();

	QString filePath = m_jsonDir + "DatasetCategories.json";
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly)) {
		QJsonDocument document = QJsonDocument::fromJson(file.readAll());

		QJsonArray categoryArray = document.object().value("categories").toArray();

		for(int i = 0 ; i < categoryArray.size(); ++i) {
			QJsonObject currentCategory = categoryArray[i].toObject();
			QString categoryName = currentCategory.value("category_name").toString();
			QTreeWidgetItem* currentCategoryItem= new QTreeWidgetItem(QStringList(categoryName));
			ui.twCategories->addTopLevelItem(currentCategoryItem);

			QJsonArray subcategories = currentCategory.value("subcategories").toArray();
			for(int j = 0; j < subcategories.size(); ++j) {
				QJsonObject currentSubCategory = subcategories[j].toObject();
				QString subcategoryName = currentSubCategory.value("subcategory_name").toString();
				currentCategoryItem->addChild(new QTreeWidgetItem(QStringList(subcategoryName)));

				QJsonArray datasetArray= currentSubCategory.value("datasets").toArray();
				QVector<QString> datasets;
				for(int k = 0; k < datasetArray.size(); ++k) {
					QString datasetName = datasetArray[k].toString();
					m_datasetsMap[categoryName][subcategoryName].push_back(datasetName);
				}
			}
		}

		updateCategoryCompleter();
		file.close();
	} else {
		qDebug("Couldn't open dataset category file");
	}
}

void ImportDatasetWidget::listDatasetsForSubcategory(QTreeWidgetItem *item, int column) {
	if(item->childCount() == 0) {
		if(m_selectedSubcategory.compare(item->text(0)) != 0) {
			m_selectedSubcategory = item->text(0);
			QString categoryName = item->parent()->text(0);

			ui.lwDatasets->clear();
			for(QString dataset :  m_datasetsMap[categoryName][m_selectedSubcategory]) {
				ui.lwDatasets->addItem(new QListWidgetItem(dataset));
			}

			updateDatasetCompleter();
		}
	}
}

void ImportDatasetWidget::updateDatasetCompleter() {
	QStringList datasetList;
	for(int i = 0; i <ui.lwDatasets->count(); ++i) {
		datasetList.append(ui.lwDatasets->item(i)->text());
	}

	if(m_datasetCompleter != nullptr)
		delete m_datasetCompleter;

	if(!datasetList.isEmpty()) {
		m_datasetCompleter = new QCompleter(datasetList);
		m_datasetCompleter->setCompletionMode(QCompleter::PopupCompletion);
		m_datasetCompleter->setCaseSensitivity(Qt::CaseSensitive);
		ui.leSearchDatasets->setCompleter(m_datasetCompleter);
	} else
		ui.leSearchDatasets->setCompleter(nullptr);
}

void ImportDatasetWidget::updateCategoryCompleter() {
	QStringList categoryList;
	for (int i = 0; i < ui.twCategories->topLevelItemCount(); ++i)
		categoryList.append(ui.twCategories->topLevelItem(i)->text(0));

	if(m_categoryCompleter != nullptr)
		delete m_categoryCompleter;

	if(!categoryList.isEmpty()) {
		m_categoryCompleter = new QCompleter(categoryList);
		m_categoryCompleter->setCompletionMode(QCompleter::PopupCompletion);
		m_categoryCompleter->setCaseSensitivity(Qt::CaseSensitive);
		ui.leSearchCategories->setCompleter(m_categoryCompleter);
	} else
		ui.leSearchCategories->setCompleter(nullptr);
}

void ImportDatasetWidget::scrollToCategoryTreeItem(const QString& rootName) {
	int topItemIdx = -1;
	for (int i = 0; i < ui.twCategories->topLevelItemCount(); ++i)
		if (ui.twCategories->topLevelItem(i)->text(0) == rootName) {
			topItemIdx = i;
			break;
		}

	if (topItemIdx >= 0)
		ui.twCategories->scrollToItem(ui.twCategories->topLevelItem(topItemIdx),
									  QAbstractItemView::ScrollHint::PositionAtTop);
}

void ImportDatasetWidget::scrollToDatasetListItem(const QString& rootName) {
	int itemIdx = -1;
	for (int i = 0; i < ui.lwDatasets->count(); ++i)
		if (ui.lwDatasets->item(i)->text() == rootName) {
			itemIdx = i;
			break;
		}

	if (itemIdx >= 0)
		ui.lwDatasets->scrollToItem(ui.lwDatasets->item(itemIdx),
									QAbstractItemView::ScrollHint::PositionAtTop);
}

QString ImportDatasetWidget::getSelectedDataset() {
	if (ui.lwDatasets->selectedItems().count() > 0) {
		return ui.lwDatasets->selectedItems().at(0)->text();
	} else
		return QString();
}

void ImportDatasetWidget::loadDatasetToProcess(DatasetHandler* datasetHandler) {
	QString fileName = getSelectedDataset() + ".json";
	downloadDatasetFile(fileName);

	QString filePath = m_jsonDir + fileName;

	if(QFile::exists(filePath)) {
		datasetHandler->processMetadata(filePath);
	} else  {
		QMessageBox::critical(this, i18n("Can't locate file"), i18n("The metadata file for the choosen dataset can't be located"));
	}
}

AbstractFileFilter* ImportDatasetWidget::currentFileFilter() const {
	return m_currentFilter.get();
}

void ImportDatasetWidget::showDatasetMetadataManager() {

	DatasetMetadataManagerDialog* dlg = new DatasetMetadataManagerDialog(this, m_datasetsMap);

	if (dlg->exec() == QDialog::Accepted) {
		//updateCategoryJson() -- will be implemented - TODO
		QString pathToJson =  m_jsonDir + "DatasetCategories.json";
		QString dirPath = QFileInfo(pathToJson).dir().absolutePath();
		dlg->updateDocument(pathToJson);
		dlg->createNewMetadata(dirPath);
	}
	delete dlg;
}

void ImportDatasetWidget::downloadCategoryFile() {
	QString fileNameOld = QStandardPaths::locate(QStandardPaths::AppDataLocation, "datasets/DatasetCategories.json");
	QString fileNameNew =m_jsonDir + "DatasetCategories.json";
	QFile::copy(fileNameOld, fileNameNew);
}

void ImportDatasetWidget::downloadDatasetFile(const QString &datasetName) {
	QString fileNameOld = QStandardPaths::locate(QStandardPaths::AppDataLocation, QString("datasets") + QDir::separator() + datasetName);
	QString fileNameNew = m_jsonDir + datasetName;
	QFile::copy(fileNameOld, fileNameNew);
}


