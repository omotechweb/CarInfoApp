#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QList>
#include <algorithm>
#include <QPalette>
#include <QStyleFactory>

struct Car {
    QString brand;
    QString model;
    int year;
    QString description;
};

QList<Car> loadCarsFromJson(const QString& filePath) {
    QList<Car> cars;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Dosya açılamadı: %s", qPrintable(filePath));
        return cars;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qWarning("JSON dosyası dizi değil!");
        return cars;
    }

    QJsonArray arr = doc.array();
    for (const QJsonValue& val : arr) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();
        Car car;
        car.brand = obj["brand"].toString();
        car.model = obj["model"].toString();
        car.year = obj["year"].toInt();
        car.description = obj["description"].toString();
        cars.append(car);
    }

    return cars;
}

void populateListWidget(QListWidget* listWidget, const QList<Car>& cars) {
    listWidget->clear();
    for (const Car& car : cars) {
        listWidget->addItem(car.brand + " " + car.model);
    }
}

void applyDarkTheme(QApplication& app) {
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    app.setPalette(darkPalette);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    applyDarkTheme(app);

    QList<Car> cars = loadCarsFromJson("cars.json");

    QWidget window;
    window.setWindowTitle("Araç Bilgilendirme Uygulaması");

    QListWidget* listWidget = new QListWidget;
    QLabel* detailLabel = new QLabel("Bir araç seçiniz.");
    detailLabel->setWordWrap(true);
    detailLabel->setMinimumWidth(300);

    QComboBox* sortCombo = new QComboBox;
    sortCombo->addItem("Yeniden Eskiye");
    sortCombo->addItem("Eskiden Yeniye");
    sortCombo->addItem("A'dan Z'ye (Marka)");
    sortCombo->addItem("Z'den A'ya (Marka)");

    QPushButton* producerButton = new QPushButton("YAPIMCI");
    producerButton->setFixedSize(80, 30);

    QPushButton* googleButton = new QPushButton("Google'da Göster");
    googleButton->setFixedSize(120, 30);
    googleButton->setEnabled(false);  // Başlangıçta pasif

    populateListWidget(listWidget, cars);

    QObject::connect(listWidget, &QListWidget::currentRowChanged, [&](int currentRow){
        if (currentRow < 0 || currentRow >= cars.size()) {
            detailLabel->setText("Bir araç seçiniz.");
            googleButton->setEnabled(false);
            return;
        }
        const Car& car = cars[currentRow];
        QString details = QString("Marka: %1\nModel: %2\nYıl: %3\nAçıklama: %4")
                          .arg(car.brand)
                          .arg(car.model)
                          .arg(car.year)
                          .arg(car.description);
        detailLabel->setText(details);
        googleButton->setEnabled(true);
    });

    QObject::connect(sortCombo, &QComboBox::currentTextChanged, [&](const QString &text){
        QList<Car> sortedCars = cars;

        if (text == "Yeniden Eskiye") {
            std::sort(sortedCars.begin(), sortedCars.end(), [](const Car& a, const Car& b){
                return a.year > b.year;
            });
        } else if (text == "Eskiden Yeniye") {
            std::sort(sortedCars.begin(), sortedCars.end(), [](const Car& a, const Car& b){
                return a.year < b.year;
            });
        } else if (text == "A'dan Z'ye (Marka)") {
            std::sort(sortedCars.begin(), sortedCars.end(), [](const Car& a, const Car& b){
                return a.brand.toLower() < b.brand.toLower();
            });
        } else if (text == "Z'den A'ya (Marka)") {
            std::sort(sortedCars.begin(), sortedCars.end(), [](const Car& a, const Car& b){
                return a.brand.toLower() > b.brand.toLower();
            });
        }

        populateListWidget(listWidget, sortedCars);
        listWidget->setCurrentRow(-1);
        detailLabel->setText("Bir araç seçiniz.");
        googleButton->setEnabled(false);
    });

    QObject::connect(producerButton, &QPushButton::clicked, [&](){
        QDesktopServices::openUrl(QUrl("https://omotech.xyz"));
    });

    QObject::connect(googleButton, &QPushButton::clicked, [&](){
        int currentRow = listWidget->currentRow();
        if (currentRow < 0 || currentRow >= cars.size()) {
            return;
        }
        const Car& car = cars[currentRow];
        QString query = QUrl::toPercentEncoding(car.brand + " " + car.model);
        QString url = "https://www.google.com/search?q=" + query;
        QDesktopServices::openUrl(QUrl(url));
    });

    QHBoxLayout* mainLayout = new QHBoxLayout;

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(sortCombo);
    leftLayout->addWidget(listWidget);

    mainLayout->addLayout(leftLayout);

    // Detay ve Google butonu için layout
    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(detailLabel);
    rightLayout->addWidget(googleButton, 0, Qt::AlignLeft);

    mainLayout->addLayout(rightLayout);

    // Sağ üst köşeye YAPIMCI butonu
    QVBoxLayout* topRightLayout = new QVBoxLayout;
    topRightLayout->addWidget(producerButton, 0, Qt::AlignRight | Qt::AlignTop);
    topRightLayout->addStretch();

    mainLayout->addLayout(topRightLayout);

    window.setLayout(mainLayout);
    window.resize(750, 400);
    window.show();

    return app.exec();
}
