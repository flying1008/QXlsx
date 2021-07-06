// xlsxzipreader.cpp

#include "xlsxzipreader_p.h"
#include <QFile>

QT_BEGIN_NAMESPACE_XLSX

ZipReader::ZipReader(const QString &filePath) :
#ifdef UNICODE
    m_reader(OpenZip(reinterpret_cast<const wchar_t*>(filePath.utf16()),nullptr))
#else
    m_reader(OpenZip(filePath.toUtf8().constData(),nullptr))
#endif
{
    init();
}

ZipReader::ZipReader(QIODevice *device)
{
    QFile *file = dynamic_cast<QFile*>(device);
    if(nullptr != file)
    {
        QString strFile = file->fileName();
        file->close();
#ifdef UNICODE
    m_reader = OpenZip(reinterpret_cast<const wchar_t*>(strFile.utf16()),nullptr);
#else
    m_reader = OpenZip(strFile.toUtf8().constData(),nullptr);
#endif
    }
    else
    {
      m_reader = OpenZip(device->readAll().data(),device->size(),nullptr);
    }

    init();
}

ZipReader::~ZipReader()
{
    CloseZip(m_reader);
}

void ZipReader::init()
{
    ZIPENTRY entry;
    GetZipItem(m_reader,-1,&entry);
    int nItems = entry.index;
    for(int zi=0; zi<nItems; zi++)
    {
        GetZipItem(m_reader,zi,&entry);
#ifdef UNICODE
        m_filePaths.append(QString::fromWCharArray(entry.name));
#else
        m_filePaths.append(QString::fromUtf8(entry.name));
#endif
    }
}

bool ZipReader::exists() const
{
    return IsZipHandleU(m_reader);
}

QStringList ZipReader::filePaths() const
{
    return m_filePaths;
}

QByteArray ZipReader::fileData(const QString &fileName) const
{
    ZIPENTRY entry;
    int i=-1;
#ifdef UNICODE
     std::wstring strFile = fileName.toStdWString();
     FindZipItem(m_reader,strFile.c_str(),true,&i,&entry);
#else
     FindZipItem(m_reader,fileName.toUtf8().constData(),true,&i,&entry);
#endif

    if(entry.unc_size < 0 || i<0)
    {
        return QByteArray("");
    }

    char*pBuf = new char[entry.unc_size +1];
    QByteArray byteArray;
    UnzipItem(m_reader,i,pBuf,entry.unc_size);
    byteArray.append(pBuf,entry.unc_size);
    delete []pBuf;
    return byteArray;
}

QT_END_NAMESPACE_XLSX
