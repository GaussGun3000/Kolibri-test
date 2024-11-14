#include "FileProcessor.h"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDataStream>
#include <QString>
#include <string>
#include <QDebug>
#include "openfilemodel.h"

FileProcessor::FileProcessor(QObject *parent) : QObject(parent)
{
    operationMap["OR"] = OR;
    operationMap["AND"] = AND;
    operationMap["XOR"] = XOR;
}



void FileProcessor::startProcessing(const QString &fileMask, bool deleteAfter,
                                    const QString &outputFolder, bool overwrite,
                                    int intervalSeconds, const QString &operation,
                                    QString operationValue)
{
    this->fileMask = fileMask;
    this->deleteAfter = deleteAfter;
    this->outputFolder = outputFolder;
    this->overwrite = overwrite;
    this->operation = operation;
    this->operationValue = operationValue;
    this->intervalSeconds = intervalSeconds;
    this->secondsLeft = intervalSeconds;

    if (intervalSeconds > 0)
    {
        connect(&timer, &QTimer::timeout, this, &FileProcessor::processFiles);
        timer.start(intervalSeconds * 1000);
        connect(&countdownTimer, &QTimer::timeout, this, &FileProcessor::updateCountdown);
        countdownTimer.start(1000);
        emit countdownUpdate(secondsLeft);
    } else {
        processFiles();
    }
}

void FileProcessor::processFiles()
{
    QFileInfo fileInfo(fileMask);
    QString directoryPath = fileInfo.absolutePath();
    QString filePattern = fileInfo.fileName();
    QDir dir(directoryPath);

    if (!dir.exists())
    {
        QMessageBox::critical(nullptr, "Error", "The specified directory does not exist: " + directoryPath);
        return;
    }

    QStringList files = dir.entryList(QStringList() << filePattern, QDir::Files);
    if (files.isEmpty())
    {
        QMessageBox::critical(nullptr, "Error", "No files found matching the pattern: " + filePattern);
        return;
    }

    for (const QString &file : files)
    {
        QString outputPath = outputFolder + "/" + file;

        if (QFile::exists(outputPath) && !overwrite)
        {
            QString baseName = QFileInfo(file).completeBaseName();
            QString suffix = QFileInfo(file).suffix();
            int copyCount = 1;

            while (QFile::exists(outputPath))
                outputPath = outputFolder + "/" + baseName + QString("_copy%1").arg(copyCount++) + "." + suffix;
        }

        applyBinaryOperation(file, outputPath);

        if (deleteAfter)
            QFile::remove(file);
    }
}

void FileProcessor::applyBinaryOperation(const QString &inputFile, const QString &outputFile)
{
    if (!OpenFileModel::instance().openFile(outputFile))
    {
        QMessageBox::warning(nullptr, "File in Use", "The output file \"" + outputFile + "\" is already open in the application.");
        return;
    }

    QFile inFile(inputFile);
    QFile outFile(outputFile);

    if (inFile.open(QIODevice::ReadOnly) && outFile.open(QIODevice::WriteOnly))
    {
        Operation op = operationMap.value(operation);
        int64_t operationValue64 = 0;
        if (operationValue.size() >= 8)
            std::memcpy(&operationValue64, operationValue.data(), 8);
        else
            throw std::invalid_argument("operationValue must be at least 8 bytes.");

        int64_t inputValue;
        QDataStream inStream(&inFile);
        inStream.setByteOrder(QDataStream::LittleEndian);
        QDataStream outStream(&outFile);
        outStream.setByteOrder(QDataStream::LittleEndian);

        while (!inStream.atEnd())
        {
            inStream >> inputValue;
            int64_t resultValue;
            switch (op)
            {
            case OR:
                resultValue = inputValue | operationValue64;
                break;
            case AND:
                resultValue = inputValue & operationValue64;
                break;
            case XOR:
                resultValue = inputValue ^ operationValue64;
                break;
            default:
                throw std::invalid_argument("Unknown operation");
            }
            outStream << resultValue;
        }
    }
    inFile.close();
    outFile.close();
    OpenFileModel::instance().closeFile(outputFile);
}

void FileProcessor::updateCountdown()
{
    if (secondsLeft > 0)
    {
        secondsLeft--;
        emit countdownUpdate(secondsLeft);
    }
    else
    {
        secondsLeft = intervalSeconds;
        emit countdownUpdate(secondsLeft);
    }
}
