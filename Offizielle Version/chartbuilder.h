#ifndef CHARTBUILDER_H
#define CHARTBUILDER_H

#include <QWidget>
#include <QLayout>
#include <QListWidget>
#include <QtCharts/QPieSlice>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLegendMarker>

class ChartBuilder
{
public:
    ChartBuilder();
    static QChartView * createDistributionChart(int office, int homeoffice);
    static QHBoxLayout * createStatWidget();
};

#endif // CHARTBUILDER_H
