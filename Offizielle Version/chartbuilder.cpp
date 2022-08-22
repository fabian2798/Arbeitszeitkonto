#include "chartbuilder.h"

ChartBuilder::ChartBuilder()
{

}

// creates a Pie Chart for the Distrubtion between Office and Home Office work time
QChartView * ChartBuilder::createDistributionChart(int office, int homeoffice){

    int sum = office + homeoffice;
    QPieSeries *series = new QPieSeries();
    if(homeoffice == 0 and office == 0 ){
        series->append("Büro",  50);
        series->append("Home Office",  50);
        QMessageBox * info = new QMessageBox();
        info->setText("Keine Daten verfügbar.");
        info->setWindowTitle("Fehler!");
        info->open();
    }
    else if(homeoffice == 0){
        series->append("Büro",  100);
        series->append("Home Office",  0);
    }
    else if(office == 0){
        series->append("Büro",  0);
        series->append("Home Office",  100);
    }
    else{
    series->append("Büro",  double(office)/double(sum) * 100);
    series->append("Home Office",  double(homeoffice)/double(sum) * 100);
    }
    QPieSlice *sliceO = series->slices().at(0);
    QPieSlice *sliceHO = series->slices().at(1);
    sliceO->setLabelVisible();
    sliceO->setLabel(QString::number(double(office)/double(sum) * 100) + QString("%"));
    sliceHO->setLabelVisible();
    sliceHO->setLabel(QString::number(double(homeoffice)/double(sum) * 100) + QString("%"));
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

