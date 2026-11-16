#include <nw4r/ut.h>

namespace nw4r {
namespace ut {

template <typename T> T* TextWriterBase<T>::mFormatBuffer = NULL;

template <typename T>
u32 TextWriterBase<T>::mFormatBufferSize = DEFAULT_FORMAT_BUFFER_SIZE;

template <typename T>
TagProcessorBase<T> TextWriterBase<T>::mDefaultTagProcessor;

template <typename T>
TextWriterBase<T>::TextWriterBase()
    : mCharSpace(0.0f),
      mWidthLimit(NW4R_MATH_FLT_MAX),
      mLineSpace(0.0f),
      mTabWidth(4),
      mDrawFlag(0),
      mTagProcessor(&mDefaultTagProcessor) {}

template <typename T> TextWriterBase<T>::~TextWriterBase() {}

template <typename T> void TextWriterBase<T>::SetLineHeight(f32 height) {
    NW4R_POINTER_ASSERT(this, 114);

    const Font* font = GetFont();
    const int linefeed = font != NULL ? font->GetLineFeed() : 0;
    mLineSpace = height - linefeed * GetScaleV();
}

template <typename T> f32 TextWriterBase<T>::GetLineHeight() const {
    NW4R_POINTER_ASSERT(this, 134);

    const Font* pFont = GetFont();
    int lf = pFont != NULL ? pFont->GetLineFeed() : 0;
    return mLineSpace + GetScaleV() * lf;
}

template <typename T>
f32 TextWriterBase<T>::CalcFormatStringWidth(const T* format, ...) const {
    NW4R_POINTER_ASSERT(this, 167);
    NW4R_POINTER_ASSERT(format, 168);

    Rect rect;
    std::va_list vargs;
    va_start(vargs, format);
    CalcVStringRect(&rect, format, vargs);
    va_end(vargs);
    return rect.GetWidth();
}

template <typename T>
f32 TextWriterBase<T>::CalcFormatStringHeight(const T* format, ...) const {
    NW4R_POINTER_ASSERT(this, 196);
    NW4R_POINTER_ASSERT(format, 197);

    Rect rect;
    std::va_list vargs;
    va_start(vargs, format);
    CalcVStringRect(&rect, format, vargs);
    va_end(vargs);
    return rect.GetHeight();
}

template <typename T>
void TextWriterBase<T>::CalcFormatStringRect(Rect* pRect, const T* format,
                                             ...) const {
    NW4R_POINTER_ASSERT(this, 227);
    NW4R_POINTER_ASSERT(pRect, 228);
    NW4R_POINTER_ASSERT(format, 229);

    std::va_list vargs;
    va_start(vargs, format);
    CalcVStringRect(pRect, format, vargs);
    va_end(vargs);
}

template <typename T>
void TextWriterBase<T>::CalcVStringRect(Rect* pRect, const T* format,
                                        std::va_list args) const {
    NW4R_POINTER_ASSERT(this, 257);
    NW4R_POINTER_ASSERT(pRect, 258);
    NW4R_POINTER_ASSERT(format, 259);

    T* buffer;
    if (mFormatBuffer != NULL) {
        buffer = mFormatBuffer;
    } else {
        buffer = static_cast<T*>(__alloca(mFormatBufferSize));
    }

    int length = VSNPrintf(buffer, mFormatBufferSize, format, args);
    length = Min(length, int(mFormatBufferSize - 1));
    CalcStringRect(pRect, buffer, length);
}

template <typename T>
f32 TextWriterBase<T>::CalcStringWidth(const T* str, int length) const {
    NW4R_POINTER_ASSERT(this, 284);
    NW4R_POINTER_ASSERT(str, 285);
    NW4R_MIN_ASSERT(length, 0, 286);

    Rect rect;
    CalcStringRect(&rect, str, length);
    return rect.GetWidth();
}

template <typename T>
f32 TextWriterBase<T>::CalcStringHeight(const T* str, int length) const {
    NW4R_POINTER_ASSERT(this, 284);
    NW4R_POINTER_ASSERT(str, 285);
    NW4R_MIN_ASSERT(length, 0, 286);

    Rect rect;
    CalcStringRect(&rect, str, length);
    return rect.GetHeight();
}

template <typename T> f32 TextWriterBase<T>::Printf(const T* format, ...) {
    NW4R_POINTER_ASSERT(this, 376);
    NW4R_POINTER_ASSERT(format, 377);

    std::va_list vargs;
    va_start(vargs, format);
    f32 width = VPrintf(format, vargs);
    va_end(vargs);
    return width;
}

template <typename T>
f32 TextWriterBase<T>::VPrintf(const T* format, std::va_list args) {
    NW4R_POINTER_ASSERT(this, 404);
    NW4R_POINTER_ASSERT(format, 405);

    T* pBuffer;

    if (mFormatBuffer != NULL) {
        pBuffer = mFormatBuffer;
    } else {
        pBuffer = static_cast<T*>(__alloca(mFormatBufferSize));
    }

    int length = VSNPrintf(pBuffer, mFormatBufferSize, format, args);
    length = Min(length, int(mFormatBufferSize - 1));
    return Print(pBuffer, length);
}

template <typename T> f32 TextWriterBase<T>::Print(const T* str, int length) {
    NW4R_POINTER_ASSERT(this, 284);
    NW4R_POINTER_ASSERT(str, 285);
    NW4R_MIN_ASSERT(length, 0, 286);

    TextWriterBase<T> myCopy(*this);

    f32 width = myCopy.PrintImpl(str, length, FALSE);
    SetCursor(myCopy.GetCursorX(), myCopy.GetCursorY());

    return width;
}

template <typename T>
f32 TextWriterBase<T>::PrintfMutable(const T* format, ...) {
    NW4R_POINTER_ASSERT(this, 467);
    NW4R_POINTER_ASSERT(format, 468);

    std::va_list vargs;
    va_start(vargs, format);
    f32 width = VPrintfMutable(format, vargs);
    va_end(vargs);
    return width;
}

template <typename T>
f32 TextWriterBase<T>::VPrintfMutable(const T* format, std::va_list args) {
    NW4R_POINTER_ASSERT(this, 500);
    NW4R_POINTER_ASSERT(format, 501);

    T* buffer;

    if (mFormatBuffer != NULL) {
        buffer = mFormatBuffer;
    } else {
        buffer = static_cast<T*>(__alloca(mFormatBufferSize));
    }

    int length = VSNPrintf(buffer, mFormatBufferSize, format, args);
    length = Min(length, int(mFormatBufferSize - 1));
    return PrintMutable(buffer, length);
}

template <typename T>
f32 TextWriterBase<T>::PrintMutable(const T* str, int length) {
    NW4R_POINTER_ASSERT(this, 533);
    NW4R_POINTER_ASSERT(str, 534);
    NW4R_MIN_ASSERT(length, 0, 535);

    return PrintImpl(str, length, TRUE);
}

template <typename T>
f32 TextWriterBase<T>::CalcLineWidth(const T* pStr, int len) {
    NW4R_POINTER_ASSERT(this, 568);
    NW4R_POINTER_ASSERT(str, 569);
    NW4R_MIN_ASSERT(length, 0, 570);

    Rect rect;
    TextWriterBase<T> myCopy(*this);

    myCopy.SetCursor(0.0f, 0.0f);
    myCopy.CalcLineRectImpl(&rect, &pStr, len);

    return rect.GetWidth();
}

template <typename T>
bool TextWriterBase<T>::CalcLineRectImpl(Rect* pRect, const T** pStr, int len) {
    NW4R_POINTER_ASSERT(this, 600);
    NW4R_POINTER_ASSERT(pRect, 601);
    NW4R_POINTER_ASSERT(pStr, 602);
    NW4R_POINTER_ASSERT(*pStr, 603);
    NW4R_MIN_ASSERT(length, 0, 604);

    const T* str = *pStr;
    const T* end = str + len;
    bool bUseLimit = mWidthLimit < NW4R_MATH_FLT_MAX;

    PrintContext<T> context = {
        this, // writer
        str,  // str
    };

    f32 x = 0.0f;
    bool bCharSpace = FALSE;
    bool bOverLimit = FALSE;

    const T* prevStreamPos = NULL;
    Rect prevRect;

    NW4R_POINTER_ASSERT(GetFont(), 619);
    CharStrmReader reader = GetFont()->GetCharStrmReader();

    pRect->left = 0.0f;
    pRect->right = 0.0f;
    pRect->top = Min(0.0f, GetLineHeight());
    pRect->bottom = Max(0.0f, GetLineHeight());
    prevRect = *pRect;

    reader.Set(str);
    prevStreamPos = NULL;

    u16 code = reader.Next();

    while (static_cast<const T*>(reader.GetCurrentPos()) <= end) {
        if (code < ' ') {
            Rect rect(x, 0.0f, 0.0f, 0.0f);
            context.str = static_cast<const T*>(reader.GetCurrentPos());
            context.flags = bCharSpace ? 0 : PrintContext<T>::FLAGS_CHARSPACE;

            SetCursorX(x);

            if (bUseLimit && code != '\n' && prevStreamPos != NULL) {
                PrintContext<T> context2 = context;
                TextWriterBase<T> myCopy(*this);

                Rect rect;
                context2.writer = &myCopy;
                mTagProcessor->CalcRect(&rect, code, &context2);

                if (rect.GetWidth() > 0.0f &&
                    myCopy.GetCursorX() - context.x > mWidthLimit) {
                    bOverLimit = TRUE;
                    code = '\n';
                    reader.Set(prevStreamPos);
                    continue;
                }
            }

            typename TagProcessorType::Operation oper =
                mTagProcessor->CalcRect(&rect, code, &context);

            NW4R_POINTER_ASSERT(context.str, 670);
            reader.Set(context.str);

            pRect->left = Min(pRect->left, rect.left);
            pRect->top = Min(pRect->top, rect.top);
            pRect->right = Max(pRect->right, rect.right);
            pRect->bottom = Max(pRect->bottom, rect.bottom);

            x = GetCursorX();

            if (oper == TagProcessorType::OPERATION_END_DRAW) {
                *pStr += len;
                return FALSE;
            }

            if (oper == TagProcessorType::OPERATION_NO_CHAR_SPACE) {
                bCharSpace = FALSE;
            } else if (oper == TagProcessorType::OPERATION_CHAR_SPACE) {
                bCharSpace = TRUE;
            } else if (oper == TagProcessorType::OPERATION_NEXT_LINE) {
                break;
            }
        } else {
            f32 dx = 0.0f;

            if (bCharSpace) {
                dx += GetCharSpace();
            }

            if (IsWidthFixed()) {
                dx += GetFixedWidth();
            } else {
                dx += GetFont()->GetCharWidth(code) * GetScaleH();
            }

            if (bUseLimit && prevStreamPos != NULL && x + dx > mWidthLimit) {
                bOverLimit = TRUE;
                code = '\n';
                reader.Set(prevStreamPos);
                continue;
            }

            x += dx;
            pRect->left = Min(pRect->left, x);
            pRect->right = Max(pRect->right, x);

            bCharSpace = TRUE;
        }

        if (bUseLimit) {
            prevStreamPos = static_cast<const T*>(reader.GetCurrentPos());
        }

        code = reader.Next();
    }

    *pStr = static_cast<const T*>(reader.GetCurrentPos());
    return bOverLimit;
}

template <typename T>
void TextWriterBase<T>::CalcStringRectImpl(Rect* pRect, const T* str,
                                           int length) {
    NW4R_POINTER_ASSERT(this, 769);
    NW4R_POINTER_ASSERT(pRect, 770);
    NW4R_POINTER_ASSERT(str, 771);
    NW4R_MIN_ASSERT(length, 0, 772);

    const T* pEnd = str + length;
    int remain = length;

    pRect->left = 0.0f;
    pRect->right = 0.0f;
    pRect->top = 0.0f;
    pRect->bottom = 0.0f;

    SetCursor(0.0f, 0.0f);

    do {
        Rect rect;
        CalcLineRectImpl(&rect, &str, remain);
        remain = pEnd - str;

        pRect->left = Min(pRect->left, rect.left);
        pRect->top = Min(pRect->top, rect.top);
        pRect->right = Max(pRect->right, rect.right);
        pRect->bottom = Max(pRect->bottom, rect.bottom);
    } while (remain > 0);
}

template <typename T>
f32 TextWriterBase<T>::PrintImpl(const T* str, int length, bool bMutable) {
    NW4R_POINTER_ASSERT(this, 819);
    NW4R_POINTER_ASSERT(str, 820);
    NW4R_POINTER_ASSERT(GetFont(), 821);
    NW4R_MIN_ASSERT(length, 0, 822);

    f32 cursorX = GetCursorX();
    f32 cursorY = GetCursorY();

    bool bUseLimit = mWidthLimit < NW4R_MATH_FLT_MAX;

    f32 orgCursorX = cursorX;
    f32 orgCursorY = cursorY;

    f32 cursorXAdj = 0.0f;
    f32 cursorYAdj = 0.0f;

    bool bCharSpace = FALSE;

    const T* prevStreamPos = str;
    const T* pPrevNewLine = str;

    f32 textWidth = AdjustCursor(&cursorX, &cursorY, str, length);

    cursorXAdj = orgCursorX - GetCursorX();
    cursorYAdj = orgCursorY - GetCursorY();

    PrintContext<T> context = {
        this,    // writer
        str,     // str
        cursorX, // x
        cursorY  // y
    };

    CharStrmReader reader = GetFont()->GetCharStrmReader();
    reader.Set(str);

    typename TagProcessorType::Operation oper;
    u16 code = reader.Next();

    while (static_cast<const T*>(reader.GetCurrentPos()) - str <= length) {
        if (code < ' ') {
            context.str = static_cast<const T*>(reader.GetCurrentPos());
            context.flags = bCharSpace ? 0 : PrintContext<T>::FLAGS_CHARSPACE;

            if (bUseLimit && code != '\n' && prevStreamPos != pPrevNewLine) {
                PrintContext<T> context2 = context;
                TextWriterBase<T> myCopy(*this);
                Rect rect;

                context2.writer = &myCopy;
                oper = mTagProcessor->CalcRect(&rect, code, &context2);

                if (rect.GetWidth() > 0.0f &&
                    myCopy.GetCursorX() - context.x > mWidthLimit) {
                    code = '\n';
                    reader.Set(prevStreamPos);
                    continue;
                }
            }

            oper = mTagProcessor->Process(code, &context);
            if (oper == TagProcessorType::OPERATION_NEXT_LINE) {
                NW4R_POINTER_ASSERT(context.str, 881);
                if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_TEXT,
                                  DRAWFLAG_ALIGN_TEXT_CENTER)) {

                    int remain = length - (context.str - str);
                    f32 width = CalcLineWidth(context.str, remain);

                    f32 offset = (textWidth - width) / 2.0f;
                    SetCursorX(context.x + offset);
                } else if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_TEXT,
                                         DRAWFLAG_ALIGN_TEXT_RIGHT)) {

                    int remain = length - (context.str - str);
                    f32 width = CalcLineWidth(context.str, remain);

                    f32 offset = textWidth - width;
                    SetCursorX(context.x + offset);
                } else {
                    f32 width = GetCursorX() - context.x;
                    textWidth = Max(textWidth, width);
                    SetCursorX(context.x);
                }

                if (bUseLimit) {
                    pPrevNewLine =
                        static_cast<const T*>(reader.GetCurrentPos());
                }

                bCharSpace = FALSE;
            } else if (oper == TagProcessorType::OPERATION_NO_CHAR_SPACE) {
                bCharSpace = FALSE;
            } else if (oper == TagProcessorType::OPERATION_CHAR_SPACE) {
                bCharSpace = TRUE;
            } else if (oper == TagProcessorType::OPERATION_END_DRAW) {
                break;
            }

            NW4R_POINTER_ASSERT(context.str, 926);
            reader.Set(context.str);
        } else {
            f32 baseY = GetCursorY();
            if (bUseLimit && prevStreamPos != pPrevNewLine) {
                f32 baseX = GetCursorX();
                f32 space = bCharSpace ? GetCharSpace() : 0.0f;

                f32 width = IsWidthFixed()
                                ? GetFixedWidth()
                                : GetFont()->GetCharWidth(code) * GetScaleH();

                if (baseX - cursorX + space + width > mWidthLimit) {
                    code = '\n';
                    reader.Set(prevStreamPos);
                    continue;
                }
            }

            if (bCharSpace) {
                MoveCursorX(GetCharSpace());
            }

            bCharSpace = TRUE;

            f32 adj = -GetFont()->GetBaselinePos() * GetScaleV();
            MoveCursorY(adj);
            CharWriter::Print(code);
            SetCursorY(baseY);
        }

        if (bUseLimit) {
            prevStreamPos = static_cast<const T*>(reader.GetCurrentPos());
        }

        code = reader.Next();
    }

    f32 width = GetCursorX() - context.x;
    textWidth = Max(textWidth, width);

    if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_CENTER) ||
        IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_TOP)) {
        SetCursorY(orgCursorY);
    } else {
        if (bMutable) {
            if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V,
                              DRAWFLAG_ALIGN_V_BASELINE)) {
                SetCursorY(GetCursorY() - GetFontAscent());
            }
        } else {
            MoveCursorY(cursorYAdj);
        }
    }

    return textWidth;
}

template <typename T>
f32 TextWriterBase<T>::AdjustCursor(f32* xOrigin, f32* yOrigin, const T* str,
                                    int length) {
    f32 textWidth = 0.0f;
    f32 textHeight = 0.0f;

    if (!IsDrawFlagSet(DRAWFLAG_MASK_ALL, DRAWFLAG_MASK_ALIGN_V) &&
        !IsDrawFlagSet(DRAWFLAG_MASK_ALL, 0)) {

        Rect rect;
        CalcStringRect(&rect, str, length);

        textWidth = rect.left + rect.right;
        textHeight = rect.top + rect.bottom;
    }

    if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_H, DRAWFLAG_ALIGN_H_CENTER)) {
        *xOrigin -= textWidth / 2;
    } else if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_H, DRAWFLAG_ALIGN_H_RIGHT)) {
        *xOrigin -= textWidth;
    }

    if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_CENTER)) {
        *yOrigin -= textHeight / 2;
    } else if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V, DRAWFLAG_ALIGN_V_TOP)) {
        *yOrigin -= textHeight;
    }

    if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_TEXT, DRAWFLAG_ALIGN_TEXT_CENTER)) {
        SetCursorX(*xOrigin + (textWidth - CalcLineWidth(str, length)) / 2);
    } else if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_TEXT,
                             DRAWFLAG_ALIGN_TEXT_RIGHT)) {
        SetCursorX(*xOrigin + (textWidth - CalcLineWidth(str, length)));
    } else {
        SetCursorX(*xOrigin);
    }

    if (IsDrawFlagSet(DRAWFLAG_MASK_ALIGN_V,
                      DRAWFLAG_ALIGN_V_CENTER | DRAWFLAG_ALIGN_V_TOP)) {
        SetCursorY(*yOrigin);
    } else {
        SetCursorY(*yOrigin + GetFontAscent());
    }

    return textWidth;
}

template <typename T>
void TextWriterBase<T>::CalcStringRect(Rect* pRect, const T* pStr,
                                       int len) const {
    TextWriterBase<T> myCopy(*this);
    myCopy.CalcStringRectImpl(pRect, pStr, len);
}

template struct TextWriterBase<char>;
template struct TextWriterBase<wchar_t>;

} // namespace ut
} // namespace nw4r
