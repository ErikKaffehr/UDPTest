#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>

class MyLineEdit : public QWidget
{
    Q_OBJECT
public:
    explicit MyLineEdit(QWidget *parent = nullptr);
    QLabel *_label;
    QLineEdit *line_edit;
public slots:
signals:
};

#endif // MYLINEEDIT_H
