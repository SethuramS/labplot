#ifndef JSONFILTER_H
#define JSONFILTER_H

#include "backend/datasources/filters/AbstractFileFilter.h"
#include "backend/core/AbstractColumn.h"
#include <QAbstractItemModel>
#include <QJsonValue>

class QStringList;
class QIODevice;
class QJsonDocument;
class QJsonModel;
class JsonFilterPrivate;

class JsonFilter : public AbstractFileFilter {
	Q_OBJECT

public:
	enum DataContainerType {Array, Object};

	JsonFilter();
	~JsonFilter() override;

	static QStringList dataTypes();
	static QStringList dataRowTypes();

	// read data from any device
	void readDataFromDevice(QIODevice& device, AbstractDataSource*,
	                        AbstractFileFilter::ImportMode = AbstractFileFilter::Replace, int lines = -1);
	// overloaded function to read from file
	QVector<QStringList> readDataFromFile(const QString& fileName, AbstractDataSource* = nullptr,
	                                      AbstractFileFilter::ImportMode = AbstractFileFilter::Replace, int lines = -1) override;
	void write(const QString& fileName, AbstractDataSource*) override;

	QVector<QStringList> preview(const QString& fileName);
	QVector<QStringList> preview(QIODevice& device);
	QVector<QStringList> preview(QJsonDocument& doc);

	void loadFilterSettings(const QString&) override;
	void saveFilterSettings(const QString&) const override;

	void setDataRowType(const QJsonValue::Type);
	QJsonValue::Type dataRowType() const;
	void setModelRows(const QVector<int>);
	QVector<int> modelRows() const;

	void setDateTimeFormat(const QString&);
	QString dateTimeFormat() const;
	void setNumberFormat(QLocale::Language);
	QLocale::Language numberFormat() const;
	void setNaNValueToZero(const bool);
	bool NaNValueToZeroEnabled() const;
    void setCreateIndexEnabled(const bool);
	void setParseRowsName(const bool);

	void setVectorNames(const QString&);
	QStringList vectorNames() const;
	QVector<AbstractColumn::ColumnMode> columnModes();

	void setStartRow(const int);
	int startRow() const;
	void setEndRow(const int);
	int endRow() const;
	void setStartColumn(const int);
	int startColumn() const;
	void setEndColumn(const int);
	int endColumn() const;

	void save(QXmlStreamWriter*) const override;
	bool load(XmlStreamReader*) override;

private:
	std::unique_ptr<JsonFilterPrivate> const d;
	friend class JsonFilterPrivate;
};

#endif