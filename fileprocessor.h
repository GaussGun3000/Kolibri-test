#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QObject>
#include <QTimer>
#include <QDir>
#include <QMap>

class FileProcessor : public QObject {
    Q_OBJECT
public:
    explicit FileProcessor(QObject *parent = nullptr);

    Q_INVOKABLE void startProcessing(const QString &fileMask, bool deleteAfter,
                                     const QString &outputFolder, bool overwrite,
                                     int intervalSeconds, const QString &operation,
                                     QString operationValue);
    enum Operation {
        OR,
        AND,
        XOR
    };

signals:
    void countdownUpdate(int secondsLeft);

private slots:
    void processFiles();
    void updateCountdown();

private:
    QMap<QString, Operation> operationMap;
    QTimer timer;
    QTimer countdownTimer;
    QString fileMask;
    bool deleteAfter;
    QString outputFolder;
    bool overwrite;
    QString operation;
    QString operationValue;
    int intervalSeconds;
    int secondsLeft;

    void applyBinaryOperation(const QString &inputFile, const QString &outputFile);


};

#endif // FILEPROCESSOR_H
