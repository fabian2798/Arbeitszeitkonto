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
#include <QMessageBox>

class ChartBuilder
{
public:
    ChartBuilder();
    static QChartView * createDistributionChart(int office, int homeoffice);
    static QHBoxLayout * createStatWidget();
    static void getDistribution(int * test1, int * test2, QString bm, QString by, QString em, QString ey);
};

#endif // CHARTBUILDER_H
