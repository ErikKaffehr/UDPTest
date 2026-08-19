#include "datastorage.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <QDebug>
#include <QTimer>
//#include <QDom>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QGuiApplication>
#include <QCursor>


#include <bits/stdc++.h>
DataStorage::DataStorage()
{
    _time_zero = time(NULL);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(&_timer, SIGNAL(timeout()), this, SLOT(fakeData()));
    _time_passed = 0;
    _timer.setInterval(1000);
    _timer.start();

}

DataStorage* DataStorage::_my_instance = nullptr;

DataStorage* DataStorage::getInstance() {
    if (!_my_instance)
        _my_instance = new DataStorage;
    return(_my_instance);
}

void DataStorage::addDataFromBuffer(const char* buffer, int /* length */)
{
    std::vector <std::string> tokens;
    std::stringstream line(buffer);

    std::string intermediate;

    while (getline(line, intermediate, ' ')) {
        tokens.push_back(intermediate);
    }

    int pairs = tokens.size();
    if (pairs % 2 > 0)
       return; // Error condition, consider how to handle

    for (int i = 0; i < pairs; i += 2){
        float value = std::stof(tokens[i+1]);
        _varmap[tokens[i]] = value;
    }
}

float DataStorage::getValueByName(const char *name)
{
    if (_varmap.find(name)  == _varmap.end())
        return 0;
    else
        return _varmap[name];
}
DataPointVector* DataStorage::getDataPointVectorByName(const char *name)
{
    if (_dpv_map.find(name) != _dpv_map.end())
        return _dpv_map[name];
    else
        return 0;
}

void DataStorage::timeout()
{
    _time_passed = time(0);

    //VariableNames vnames = getVariableNames();
    VariableMap::iterator it;

    for (it = _varmap.begin(); it != _varmap.end(); it++)
    {
       // std::cout << it->first    // string (key)
       //           << ':'
       //           << it->second   // string's value
       //           << std::endl;
    }

    //or (int i = 0; i< _varmap.size(); i++)
    for (it = _varmap.begin(); it != _varmap.end(); it++)
    {
        std::string vname = it->first;
        if (_dpv_map.find(vname) == _dpv_map.end())
            _dpv_map[vname] = new DataPointVector;
        DataPointVector *dpv =_dpv_map[vname];
        DataPoint dp(_time_passed, getValueByName(vname.c_str()));
        if (dpv->size() > 24 * 3600) {
            dpv->erase(dpv->begin() , dpv->begin() + 1000);
        }
        dpv->push_back(dp);
    }
}

VariableNames DataStorage::getVariableNames()
{
    VariableMap::iterator it;
    VariableNames variables;
    for (it = _varmap.begin(); it != _varmap.end(); it++)
    {
        std::string vname = it->first;
        variables.push_back(vname);
    }
    return variables;
}

void DataStorage::store()
{
    QDomDocument doc;

    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //calculateHugeMandelbrot();              // lunch time...

    QDomElement top = doc.createElement("myML");
    doc.appendChild(top);
    DPVMap::iterator it;
    //VariableNames variables;
    QDomElement time_stamp = doc.createElement("TimeStamp");
    qlonglong tz = _time_zero;
    time_stamp.setAttribute("TimeZero", tz);
    top.appendChild(time_stamp);
    for (it = _dpv_map.begin(); it != _dpv_map.end(); it++)
    {
        std::string vname = it->first;
        QDomElement dvel = doc.createElement("dvel");
        top.appendChild(dvel);
        dvel.setTagName(vname.c_str());
        DataPointVector *dpv = it->second;

        DataPointVector::iterator it2;
        for (it2 = dpv->begin(); it2 != dpv->end(); it2++) {
            DataPoint dp = *it2;
            QDomElement el = doc.createElement("datapair");

            el.setAttribute("time", dp.time());
            el.setAttribute("value", dp.value());
            dvel.appendChild(el);
        }
    }
    QString xml = doc.toString();
    QGuiApplication::restoreOverrideCursor();

    QString file_name = QFileDialog::getSaveFileName(0, tr("Save File"),
                                                    "/media/ekr/PIExtension/Husbil/recentdata.xml",
                                                    tr("XML (*.xml)"));
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << xml;
    QGuiApplication::restoreOverrideCursor();
}

void DataStorage::restore()
{
    QDomDocument doc("mydocument");
    QString file_name = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                    "/media/ekr/PIExtension/Husbil",
                                                    tr("XML (*.xml *.txt)"));
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    _timer.stop();
    _time_passed = 0;
    _time_zero = time(NULL);
    time_t since_midnight = _time_zero % (24 *3600);
    _time_zero -= since_midnight;
    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            qDebug() << qPrintable(e.tagName()) << '\n'; // the node really is an element.

        }

        QString tag_name = e.tagName();

        //const char* tag_name_c_string = tag_name.toLatin1();
        std::string vname = tag_name.toStdString();
        DataPointVector *pv;// = getDataPointVectorByName(tag_name_c_string);

        if (_dpv_map.find(vname) == _dpv_map.end())
            _dpv_map[vname] = new DataPointVector;
        pv =_dpv_map[vname];

        if (pv) {
            float value = 0;
            //erase contents;
            pv->erase(pv->begin(), pv->end());
            QDomNode n2 = n.firstChild();
            while (!n2.isNull()) {
                QDomElement el2 = n2.toElement();
                time_t time;
                //float value;
                if (!el2.isNull()) {
                    time = el2.attribute("time").toULongLong();

                    //if (time > _time_passed)
                    time += _time_zero;
                    value = el2.attribute("value").toFloat();
                    DataPoint dp( time, value);
                    pv->push_back(dp);
                    //qDebug() << time << value <<'\n';
                }

                n2 = n2.nextSibling();

            }
            //std::string name = tag_name_c_string;
            //Insert last avlue of vector into _var_map, so Plotter can find it there.
            _varmap[vname] = value;
        }
        n = n.nextSibling();
    }

        QGuiApplication::restoreOverrideCursor();

}

void DataStorage::fakeData()
{
    char textbuf[128];
    static int iter = 0;
    float T1 = 20 + 0.01 * iter;
    float T2 = 21 + 0.01 * iter;
    float T3 = 22 + 0.01 * iter;
    iter++;
    sprintf(textbuf, "T1 %f T2 %f T3 %f", T1, T2, T3);
    DataStorage::getInstance()->addDataFromBuffer(textbuf, strlen(textbuf));
}
