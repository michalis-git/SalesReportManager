#ifndef DECOMPRESSER_H
#define DECOMPRESSER_H

#include <QString>



class Decompresser
{
    QString m_decompressedFilePath;
    bool readCompressedData(const QString &compressedFilePath, QByteArray &compressedData);
    bool writeDecompressedData(const QString &decompressedFilePath, QByteArray &data);
public:
    Decompresser(const QString &compressedFilepath, const QString &decompressedDirpath);

    const QString &decompressedFilePath() const;
};

#endif // DECOMPRESSER_H
