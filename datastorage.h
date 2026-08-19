#ifndef DATASTORAGE_H
#define DATASTORAGE_H
#include <map>
#include <string>
#include <vector>
#include <QObject>
#include <QTimer>
#include <QtTypes>

#include <time.h>

class DataPoint;

typedef std::map<std::string, float> VariableMap;
typedef std::vector<std::string> VariableNames;
typedef std::vector<float> VariableValues;
typedef std::vector<DataPoint> DataPointVector;
typedef std::map<std::string, DataPointVector*> DPVMap;

class DataPoint
{
public:
    DataPoint(double time, float value){ _time_stamp = time, _variable = value;}
    float value() {return _variable;}
    qulonglong time() { return _time_stamp; }

private:
    float _variable;
    time_t _time_stamp;
   // QTimer qtimer;
};

class DataStorage: public QObject
{
    Q_OBJECT
public:
    static DataStorage *getInstance();
    void addDataFromBuffer(const char *buffer, int);
    float getValueByName(const char *name);
    float getVariableByName(const char* name);
    VariableNames getVariableNames();
    DataPointVector* getDataPointVectorByName(const char* name);
    time_t baseTime(){ return _time_zero; }
private:
    explicit DataStorage();
    static DataStorage *_my_instance;

    VariableMap _varmap;


    //VariableValues getVariableValues();
    DPVMap _dpv_map;
    QTimer _timer;
    time_t _time_passed;
    time_t _time_zero;

public slots:
   void timeout();
   void store();
   void restore();
   void fakeData();

};

#endif // DATASTORAGE_H
