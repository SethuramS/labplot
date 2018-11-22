/***************************************************************************
    File                 : DifferentiationTest.cpp
    Project              : LabPlot
    Description          : Tests for numerical differentiation
    --------------------------------------------------------------------
    Copyright            : (C) 2018 Stefan Gerlach (stefan.gerlach@uni.kn)
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

#include "DifferentiationTest.h"
#include "backend/core/column/Column.h"
#include "backend/worksheet/plots/cartesian/XYDifferentiationCurve.h"

//##############################################################################

void DifferentiationTest::testLinear() {
	// data
	QVector<int> xData = {1,2,3,4};
	QVector<double> yData = {1.,2.,3.,4.};

	//data source columns
	Column xDataColumn("x", AbstractColumn::Integer);
	xDataColumn.replaceInteger(0, xData);

	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	XYDifferentiationCurve differentiationCurve("differentiation");
	differentiationCurve.setXDataColumn(&xDataColumn);
	differentiationCurve.setYDataColumn(&yDataColumn);

	//prepare the differentiation
	XYDifferentiationCurve::DifferentiationData differentiationData = differentiationCurve.differentiationData();
	differentiationCurve.setDifferentiationData(differentiationData);

	//perform the differentiation
	differentiationCurve.recalculate();
	const XYDifferentiationCurve::DifferentiationResult& differentiationResult = differentiationCurve.differentiationResult();

	//check the results
	QCOMPARE(differentiationResult.available, true);
	QCOMPARE(differentiationResult.valid, true);

	const AbstractColumn* resultXDataColumn = differentiationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = differentiationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 4);

	for (int i = 0; i < np; i++)
		QCOMPARE(resultXDataColumn->valueAt(i), (double)i + 1);

	QCOMPARE(resultYDataColumn->valueAt(0), 1.);
	QCOMPARE(resultYDataColumn->valueAt(1), 1.);
	QCOMPARE(resultYDataColumn->valueAt(2), 1.);
	QCOMPARE(resultYDataColumn->valueAt(3), 1.);
}

QTEST_MAIN(DifferentiationTest)
