#ifndef NW4R_SND_WAVE_ARCHIVE_H
#define NW4R_SND_WAVE_ARCHIVE_H

#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Util.h>
#include <nw4r/snd/snd_WaveFile.h>
#include <nw4r/ut.h>

namespace nw4r {
namespace snd {
namespace detail {

struct WaveArchive {
    static const u16 VERSION = NW4R_VERSION(1, 0);
    static const u32 SIGNATURE = FOURCC('R', 'W', 'A', 'R');
    static const u32 SIGNATURE_TABLE_BLOCK = FOURCC('T', 'A', 'B', 'L');

    struct DataBlock {
        ut::BinaryBlockHeader blockHeader; //!< // at 0x0
        u8 data[1];                        //!< // at 0x8
    };

    typedef Util::DataRef<WaveFile::FileHeader> waveFileRef;

    struct TableItem {
        waveFileRef waveFileRef; //!< // at 0x0
        u32 waveFileSize;        //!< // at 0x8
    };

    struct TableBlock {
        ut::BinaryBlockHeader blockHeader; //!< // at 0x0
        Util::Table<TableItem> fileTable;  //!< // at 0x8
    };

    struct FileHeader {
        ut::BinaryFileHeader fileHeader; //!< // at 0x0
        u32 tableChunkOffset;            //!< // at 0x10
        u32 tableChunkSize;              //!< // at 0x14
        u32 dataChunkOffset;             //!< // at 0x18
        u32 dataChunkSize;               //!< // at 0x1C
    };
};

class WaveArchiveReader {
public:
    static const u16 SUPPORTED_FILE_VERSION_MIN = NW4R_VERSION(1, 0);
    static const u16 SUPPORTED_FILE_VERSION_MAX = NW4R_VERSION(1, 0);

    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     *
     * \~english @param waveArc Pointer to WaveArchive file
     * \~japanese @param waveArc WaveArchiveファイルへのポインタ
     */
    WaveArchiveReader(const void* waveArc);

    /**
     * \~english @brief Gets the specified WaveFile from a WaveArchive.
     * \~japanese @brief 指定のWaveFileをWaveArchiveから取得する。
     *
     * @param index
     * \~english @return WaveFile header
     * \~japanese @return WaveFileのヘッダ
     */
    const WaveFile::FileHeader* GetWaveFile(int index) const;

private:
    /**
     * \~english @brief Checks the WaveArchive header to make sure it is valid.
     * \~japanese @brief WaveArchiveヘッダが有効かどうかを調べる。
     *
     * \~english @param waveArc Pointer to WaveArchive file
     * \~japanese @param waveArc WaveArchiveファイルへのポインタ
     * \~english @return TRUE if valid
     * \~japanese @return 有効であればTRUE
     */
    bool VerifyFileHeader(const void* waveArc);

private:
    const WaveArchive::TableBlock* mTableBlock;
    const WaveArchive::DataBlock* mDataBlock;
};

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
