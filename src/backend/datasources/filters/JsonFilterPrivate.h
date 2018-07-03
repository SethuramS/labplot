#ifndef JSONFILTERPRIVATE_H
#define JSONFILTERPRIVATE_H

#include <QJsonDocument>
#include "QJsonModel.h"
class KFilterDev;
class AbstractDataSource;
class AbstractColumn;

class JsonFilterPrivate {

public:
	JsonFilterPrivate (JsonFilter* owner);

	int checkRow(QJsonValueRef value, int &countCols);
	int parseColumnModes(QJsonValue row, QString rowName = "");
	void setEmptyValue(int column, int row);
	void setValueFromString(int column, int row, QString value);

	int prepareDeviceToRead(QIODevice&);
	int prepareDocumentToRead(const QJsonDocument&);

	void readDataFromDevice(QIODevice& device, AbstractDataSource* = nullptr,
	                        AbstractFileFilter::ImportMode = AbstractFileFilter::Replace, int lines = -1);
	void readDataFromFile(const QString& fileName, AbstractDataSource* = nullptr,
	                      AbstractFileFilter::ImportMode = AbstractFileFilter::Replace, int lines = -1);
	void readDataFromDocument(const QJsonDocument& doc, AbstractDataSource* = nullptr,
	                          AbstractFileFilter::ImportMode = AbstractFileFilter::Replace, int lines = -1);

	void importData(AbstractDataSource* = nullptr, AbstractFileFilter::ImportMode = AbstractFileFilter::Replace,
	                int lines = -1);

	void write(const QString& fileName, AbstractDataSource*);
	QVector<QStringList> preview(const QString& fileName);
	QVector<QStringList> preview(QIODevice& device);
	QVector<QStringList> preview(QJsonDocument& doc);
	QVector<QStringList> preview();

	const JsonFilter* q;
	QJsonModel* model;

	JsonFilter::DataContainerType containerType;
	QJsonValue::Type rowType;
	QVector<int> modelRows;

	QString dateTimeFormat;
	QLocale::Language numberFormat;
	double nanValue;
	bool createIndexEnabled;
	bool parseRowsName;
	QStringList vectorNames;
	QVector<AbstractColumn::ColumnMode> columnModes;

	int startRow;		// start row
	int endRow;			// end row
	int startColumn;	// start column
	int endColumn;		// end column

private:
	int m_actualRows;
	int m_actualCols;
	int m_prepared;
	int m_columnOffset; // indexes the "start column" in the datasource. Data will be imported starting from this column.
	QVector<void*> m_dataContainer; // pointers to the actual data containers (columns).
	QJsonDocument m_preparedDoc; // parsed Json document
};

#endif