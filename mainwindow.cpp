#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QXmlStreamReader>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scene(nullptr)
{
    ui->setupUi(this);
    connect(ui->openXmlButton, &QPushButton::clicked, this, &MainWindow::openXmlAndShowTree);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (scene) delete scene;
}

QList<MainWindow::XmlItem> MainWindow::parseXml(QXmlStreamReader &xml)
{
    QList<XmlItem> childrenList;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == "item") {
            XmlItem child;
            child.name = xml.attributes().value("name").toString();
            child.children = parseXml(xml); // Рекурсивно получаем дочерние элементы (список)
            childrenList.append(child);
        } else if (xml.isEndElement() && xml.name() == "item") {
            break;
        }
    }

    return childrenList;
}



void MainWindow::buildGraphicsTree(const XmlItem &item, int x, int &y, int indent)
{
    int nodeX = x + indent * 120; // горизонтальный сдвиг с отступом
    int nodeY = y;

    // Создаем текстовый элемент узла
    QGraphicsTextItem *textItem = new QGraphicsTextItem(item.name);
    textItem->setPos(nodeX, nodeY);

    // Получаем размер текста
    QRectF textRect = textItem->boundingRect();

    // Создаем прямоугольник с небольшим отступом (padding)
    qreal padding = 6;
    QGraphicsRectItem *rectItem = new QGraphicsRectItem(
        nodeX - padding,
        nodeY - padding / 2,
        textRect.width() + 2 * padding,
        textRect.height() + padding);

    // Настраиваем стиль прямоугольника (например, черная рамка, белый фон)
    rectItem->setPen(QPen(Qt::black));
    rectItem->setBrush(QBrush(Qt::white));

    // Добавляем прямоугольник в сцену до текста (чтобы текст был сверху)
    scene->addItem(rectItem);
    scene->addItem(textItem);

    // Позиция центра текста справа, откуда идут линии к детям
    QPointF parentLineStart = textItem->pos() + QPointF(textRect.width(), textRect.height()/2);

    y += 40; // шаг по вертикали для следующего узла

    for (const XmlItem &child : item.children) {
        // Позиция начала линии у ребенка — слева от текста ребенка
        QPointF childPos(x + (indent + 1) * 120, y + 10);
        QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(parentLineStart, childPos));
        scene->addItem(line);

        // Рекурсивно рисуем ребенка
        buildGraphicsTree(child, x, y, indent + 1);
    }
}


void MainWindow::openXmlAndShowTree()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть XML", "", "XML Files (*.xml)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QXmlStreamReader xml(&file);

    XmlItem root;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == "item") {
            root.name = xml.attributes().value("name").toString();
            root.children = parseXml(xml);
            break; // корневой элемент прочитан, дальше можно выходить
        }
    }

    file.close();

    if (scene)
        delete scene;
    scene = new QGraphicsScene(this);

    int yPos = 10;
    int xPos = 20;
    int indent = 0;

    buildGraphicsTree(root, xPos, yPos, indent);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}
