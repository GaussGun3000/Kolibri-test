#ifndef OPENFILEMODEL_H
#define OPENFILEMODEL_H
#include <QSet>
#include <QString>

class OpenFileModel
{
public:
    static OpenFileModel& instance()
    {
        static OpenFileModel instance;
        return instance;
    }

    OpenFileModel(const OpenFileModel&) = delete;
    OpenFileModel& operator=(const OpenFileModel&) = delete;

    bool openFile(const QString &filePath)
    {
        if (openFiles.contains(filePath))
            return false;
        openFiles.insert(filePath);
        return true;
    }

    void closeFile(const QString &filePath)
    {
        openFiles.remove(filePath);
    }

    bool isFileOpen(const QString &filePath) const
    {
        return openFiles.contains(filePath);
    }

private:
    OpenFileModel() {}
    QSet<QString> openFiles;
};

#endif // OPENFILEMODEL_H
