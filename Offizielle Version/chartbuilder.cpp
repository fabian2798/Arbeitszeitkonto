#include "chartbuilder.h"

ChartBuilder::ChartBuilder()
{

}

// creates a Pie Chart for the Distrubtion between Office and Home Office work time
QChartView * ChartBuilder::createDistributionChart(int office, int homeoffice){
    QPieSeries *series = new QPieSeries();
    series->append("Büro", office);
    series->append("Home Office", homeoffice);
    QPieSlice *sliceO = series->slices().at(0);
    QPieSlice *sliceHO = series->slices().at(1);
    sliceO->setLabelVisible();
    sliceO->setLabel(QString::number(office) + QString("%"));
    sliceHO->setLabelVisible();
    sliceHO->setLabel(QString::number(homeoffice) + QString("%"));
    sliceO->setBrush(Qt::blue);
    sliceHO->setBrush(Qt::green);
    QChart *chart = new QChart();
    chart->addSeries(series);

    chart->legend()->setVisible(true);
    QList<QLegendMarker*> test = chart->legend()->markers();
    test.at(0)->setLabel("Office");
    test.at(1)->setLabel("Home Office");
    chart->setTitle("Büro / Home Office Distribution");
    QChartView *chartView = new QChartView(chart);
    return chartView;
}


QHBoxLayout * ChartBuilder::createStatWidget(){
    QHBoxLayout * newLayout = new QHBoxLayout();
    QListWidget * month_list = new QListWidget();
    month_list->setMaximumWidth(400);
    newLayout->addWidget(month_list);
    newLayout->addWidget(createDistributionChart(60, 40));
    return newLayout;
}
