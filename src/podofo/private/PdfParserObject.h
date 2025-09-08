/**
 * SPDX-FileCopyrightText: (C) 2005 Dominik Seichter <domseichter@web.de>
 * SPDX-FileCopyrightText: (C) 2020 Francesco Pretto <ceztko@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PDF_PARSER_OBJECT_H
#define PDF_PARSER_OBJECT_H

#include <podofo/main/PdfObject.h>
#include <podofo/main/PdfTokenizer.h>
#include <podofo/main/PdfEncryptSession.h>

namespace PoDoFo {

/**
 * A PdfParserObject constructs a PdfObject from a PDF file.
 * Parsing starts always at the current file position.
 */
class PdfParserObject : public PdfObject
{
    friend class PdfParser;

private:
    /** Parse the object data from the given file handle starting at
     *  the current position.
     *  \param doc document where to resolve object references
     *  \param device an open reference counted input device which is positioned in
     *                 front of the object which is going to be parsed.
     *  \param buffer buffer to use for parsing to avoid reallocations
     *  \param offset the position in the device from which the object shall be read
     *                 if offset == -1, the object will be read from the current
     *                 position in the file.
     */
    PdfParserObject(PdfDocument& doc, const PdfReference& indirectReference,
        InputStreamDevice& device, ssize_t offset);
    /**
     *  \remarks This constructor is reserved for legacy trailers (preceded by token "trailer")
     */
    PdfParserObject(PdfDocument& doc, InputStreamDevice& device, ssize_t offset);

    PdfParserObject(InputStreamDevice& device, const PdfReference& indirectReference, ssize_t offset);

public:
    /**
     *  \remarks This constructor is for testing usage only
     */
    PdfParserObject(InputStreamDevice& device, ssize_t offset = -1);

protected:
    PdfParserObject(PdfDocument* doc, const PdfReference& indirectReference,
        InputStreamDevice& device, ssize_t offset, bool isLegacyTrailer);

public:
    bool TryUnload() override;

    /** Parse the object header and data block, excluding the stream (if any)
     */
    void ParseData();

    /** Parse the object header and data block and the stream (if any)
     */
    void ParseFull();

    /** Parse the object stream (if any)
     *  \param shallow search /Width for a direct length, not following references
     */
    void ParseStream(bool shallow = false);

    /** Pretend to parse the stream (if any), just setting the stream position past it
     */
    void ParseStreamDryRun();

    /** Gets an offset in which the object beginning is stored in the file.
     *  Note the offset points just after the object identifier ("0 0 obj").
     *
     * \returns an offset in which the object is stored in the source device,
     *     or -1, if the object was created on demand.
     */
    inline ssize_t GetOffset() const { return m_Offset; }

    inline void SetEncrypt(const std::shared_ptr<PdfEncryptSession>& encrypt) { m_Encrypt = encrypt; }

protected:
    PdfReference ReadReference(PdfTokenizer& tokenizer);
    void ParseData(PdfTokenizer& tokenizer);

    /** Returns if this object has a stream object appended.
     *  which has to be parsed.
     *  \returns true if there is a stream
     */
    bool HasStreamToParse() const override;

    void delayedLoad() override;
    void delayedLoadStream() override;
    bool removeStream() override;

    void SetRevised() override;

private:
    PdfParserObject(const PdfParserObject&) = delete;
    PdfParserObject& operator=(const PdfParserObject&) = delete;

    /** Starts reading at the file position m_StreamOffset and interprets all bytes
     *  as contents of the objects stream.
     *  It is assumed that the dictionary has a valid /Length key already.
     *
     *  Called from DelayedLoadStream(). Do not call directly.
     */
    void parseStream(bool shallow, bool dryRun);

    PdfReference readReference(PdfTokenizer& tokenizer);

    void checkReference(PdfTokenizer& tokenizer);

private:
    std::shared_ptr<PdfEncryptSession> m_Encrypt;
    InputStreamDevice* m_device;
    size_t m_Offset;
    size_t m_StreamOffset;
    bool m_isLegacyTrailer;
    bool m_HasStream;
    bool m_IsRevised;         ///< True if the object was irreversibly modified since first read
};

};

#endif // PDF_PARSER_OBJECT_H
