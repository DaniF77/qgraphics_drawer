#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QXmlStreamReader>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct XmlItem {
        QString name;
        QList<XmlItem> children;  // Здесь обязательно QList<XmlItem>
    };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QList<XmlItem> parseXml(QXmlStreamReader &xml);
    void buildGraphicsTree(const XmlItem &item, int x, int &y, int indent);

private slots:
    void openXmlAndShowTree();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
};

#endif // MAINWINDOW_H
