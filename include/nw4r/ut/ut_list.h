#ifndef NW4R_UT_LIST_H
#define NW4R_UT_LIST_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ut {

/**
 * @~english @brief Bidirectional list structure.
 * @~japanese @brief 双方向リストのストラクト。
 */
struct List {
    void* headObject; //!< @~english Pointer to the object linked to the start
                      //!< of the list @~japanese
                      //!< リストの先頭に繋がれているオブジェクトへのポインタ \~
                      //!< // at 0x0
    void* tailObject; //!< @~english Pointer to the object linked to the end
                      //!< of the list @~japanese
                      //!< リストの後尾に繋がれているオブジェクトへのポインタ \~
                      //!< // at 0x4
    u16 numObjects;   //!< @~english Number of linked objects in the list
                    //!< @~japanese リストに繋げれているオブジェクトの個数 \~ //
                    //!< at 0x8
    u16 offset; //!< \~ // at 0xA
};

struct Link {
    void* prevObject; // at 0x0
    void* nextObject; // at 0x4
};

/**
 * @~english @brief Initializes the specified List object.
 * @~japanese @brief 指定されたListオブジェクトを初期化する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 * @~english @param offset Offset of the Link object within the List
 * @~japanese @param offset List内でのLinkオブジェクトのオフセット
 */
void List_Init(List* list, u16 offset);

/**
 * @~english @brief Inserts an object at the end of the list.
 * @~japanese @brief リストの後尾にオブジェクトを挿入する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 * @~english @param object Pointer to the object to be inserted
 * @~japanese @param object 挿入したいオブジェクトへのポインタ
 */
void List_Append(List* list, void* object);
/**
 * @~english @brief Inserts an object at the start of the list.
 * @~japanese @brief リストの先頭にオブジェクトを挿入する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 * @~english @param object Pointer to the object to be inserted
 * @~japanese @param object 挿入したいオブジェクトへのポインタ
 */
void List_Prepend(List* list, void* object);

/**
 * @~english @brief Inserts an object in front of the target.
 * @~japanese @brief オブジェクトをtargetの前に挿入する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 * @~english @param target Pointer to the object in the specified location
 * @~japanese @param target 挿入したい位置にあるオブジェクトへのポインタ
 * @~english @param object Pointer to the object to be inserted
 * @~japanese @param object 挿入したいオブジェクトへのポインタ
 */
void List_Insert(List* list, void* target, void* object);
/**
 * @~english @brief Deletes the specified object from the list.
 * @~japanese @brief 指定されたオブジェクトをリストから削除する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 * @~english @param object Pointer to the object to be deleted
 * @~japanese @param 削除したいオブジェクトへのポインタ
 */
void List_Remove(List* list, void* object);

/**
 * @~english @brief Gets the next object following the specified object.
 * @~japanese @brief 指定したオブジェクトの次にオブジェクトを取得する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 * @~english @param object Pointer to the specified object
 * @~japanese @param object 指定したオブジェクトへのポインタ
 * @~english @return If object is NULL, headObject is returned
 * @~japanese @return objectがNULLであれば、headObjectを返す
 */
void* List_GetNext(const List* list, const void* object);
inline const void* List_GetNextConst(const List* list, const void* object) {
    return List_GetNext(list, object);
}

/**
 * @~english @brief Gets the object behind the specified object.
 * @~japanese @brief 指定したオブジェクトの後ろにオブジェクトを取得する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 * @~english @param object Pointer to the specified object
 * @~japanese @param object 指定したオブジェクトへのポインタ
 * @~english @return If object is NULL, tailObject is returned
 * @~japanese @return objectがNULLであれば、tailObjectを返す
 */
void* List_GetPrev(const List* list, const void* object);
inline const void* List_GetPrevConst(const List* list, const void* object) {
    return List_GetPrev(list, object);
}

/**
 * @~english @brief Gets the Nth object in the list.
 * @~japanese @brief リストのN番目のオブジェクトを取得する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 * @~english @param index Index number of the object
 * @~japanese @param index オブジェクトのインデックス番号
 */
void* List_GetNth(const List* list, u16 index);
inline const void* List_GetNthConst(const List* list, u16 index) {
    return List_GetNth(list, index);
}

/**
 * @~english @brief Gets headObject.
 * @~japanese @brief headObjectを取得する。
 *
 * @~english @param list Pointer to List object
 * @~japanese @param list Listオブジェクトへのポインタ
 */
inline void* List_GetFirst(const List* list) {
    return List_GetNext(list, NULL);
}
inline const void* List_GetFirstConst(const List* list) {
    return List_GetFirst(list);
}

inline void* List_GetLast(const List* list) {
    return List_GetPrev(list, NULL);
}
inline const void* List_GetLastConst(const List* list) {
    return List_GetLast(list);
}

inline u16 List_GetSize(const List* list) {
    return list->numObjects;
}

/******************************************************************************
 *
 * Macros
 *
 ******************************************************************************/
/**
 * @brief Declares a member Link.
 */
#define NW4R_UT_LIST_LINK_DECL() nw4r::ut::Link link

/**
 * @brief Initializes a List object for use with the specified type.
 *
 * @~english @param LIST Reference to list
 * @~english @param T List element type
 */
#define NW4R_UT_LIST_INIT(LIST, T)                                             \
    nw4r::ut::List_Init(&(LIST), offsetof(T, link))

/**
 * @brief Gets the underlying Link within the specified object.
 *
 * @~english @param LIST Reference to list
 * @~english @param OBJ Pointer to list object
 */
#define NW4R_UT_LIST_GET_LINK(LIST, OBJ)                                       \
    reinterpret_cast<nw4r::ut::Link*>((u8*)(OBJ) + (LIST).offset)

/**
 * @brief List for-each macro.
 *
 * @~english @param TYPE Element type
 * @~english @param NAME Element name
 * @~english @param LIST Reference to list
 * @~english @param ... Statement(s) to execute
 */
#define NW4R_UT_LIST_FOREACH(TYPE, NAME, LIST, ...)                            \
    {                                                                          \
        TYPE* NAME = NULL;                                                     \
                                                                               \
        while ((NAME = static_cast<TYPE*>(                                     \
                    nw4r::ut::List_GetNext(&(LIST), NAME))) != NULL) {         \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

/**
 * @brief List for-each macro (reverse order).
 *
 * @~english @param TYPE Element type
 * @~english @param NAME Element name
 * @~english @param LIST Reference to list
 * @~english @param ... Statement(s) to execute
 */
#define NW4R_UT_LIST_FOREACH_REV(TYPE, NAME, LIST, ...)                        \
    {                                                                          \
        TYPE* NAME = NULL;                                                     \
                                                                               \
        while ((NAME = static_cast<TYPE*>(                                     \
                    nw4r::ut::List_GetPrev(&(LIST), NAME))) != NULL) {         \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

/**
 * @brief List for-each macro, with robust iteration.
 *
 * @~english @param TYPE Element type
 * @~english @param NAME Element name
 * @~english @param LIST Reference to list
 * @~english @param ... Statement(s) to execute
 */
#define NW4R_UT_LIST_FOREACH_SAFE(TYPE, NAME, LIST, ...)                       \
    {                                                                          \
        TYPE* NAME;                                                            \
        TYPE* __next__;                                                        \
                                                                               \
        for (NAME = static_cast<TYPE*>(nw4r::ut::List_GetFirst(&(LIST)));      \
             NAME != NULL; NAME = __next__) {                                  \
                                                                               \
            __next__ =                                                         \
                static_cast<TYPE*>(nw4r::ut::List_GetNext(&(LIST), NAME));     \
                                                                               \
            __VA_ARGS__;                                                       \
        }                                                                      \
    }

} // namespace ut
} // namespace nw4r

#endif
