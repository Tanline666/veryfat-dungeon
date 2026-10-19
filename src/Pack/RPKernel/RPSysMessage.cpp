#include <Pack/RPKernel.h>

/**
 * \~english @brief Constructor
 *
 * \~english @param pBinary Binary file data
 * \~english @param pHeap Heap to use for allocations
 */
RPSysMessage::RPSysMessage(const void* pBinary, EGG::Heap* /* pHeap */)
    : EGG::MsgRes(pBinary) {}

/**
 * \~english @brief Destructor
 */
RPSysMessage::~RPSysMessage() {}

/**
 * \~english @brief Gets the text for the specified message
 *
 * \~english @param group Group ID
 * \~english @param idx Message index
 */
const wchar_t* RPSysMessage::GetMsg(u32 group, u32 idx) {
    return getMsg(group, idx);
}

/**
 * \~english @brief Gets the info block for the specified message
 *
 * \~english @param group Group ID
 * \~english @param idx Message index
 */
const RPSysMessage::Entry* RPSysMessage::GetMsgEntry(u32 group, u32 idx) {
    return static_cast<const Entry*>(getMsgEntry(group, idx));
}

/**
 * \~english @brief Gets the attributes for the specified message
 *
 * \~english @param group Group ID
 * \~english @param idx Message index
 */
u32 RPSysMessage::GetAttribute(u32 group, u32 idx) {
    return GetMsgEntry(group, idx)->mAttribute;
}

/**
 * \~english @brief Gets the scale of the specified message
 * @details The decimal scale is obtained by dividing this value by 100.
 *
 * \~english @param group Group ID
 * \~english @param idx Message index
 */
u16 RPSysMessage::GetScale(u32 group, u32 idx) {
    return GetMsgEntry(group, idx)->scale;
}

/**
 * \~english @brief Gets the character spacing of the specified message
 *
 * \~english @param group Group ID
 * \~english @param idx Message index
 */
u8 RPSysMessage::GetCharSpace(u32 group, u32 idx) {
    return GetMsgEntry(group, idx)->charSpace;
}
